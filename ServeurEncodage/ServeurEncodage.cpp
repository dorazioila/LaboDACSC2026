#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <mysql.h>
#include "TCP.h"
#include "OBEP.h"

void HandlerSIGINT(int s);
void TraitementConnexion(int sService, MYSQL* connexion);
void* FctThreadClient(void* p);

int sEcoute;

#define NB_THREADS_POOL 5
#define TAILLE_FILE_ATTENTE 20

int socketsAcceptees[TAILLE_FILE_ATTENTE];
int indiceEcriture = 0;
int indiceLecture = 0;

pthread_mutex_t mutexSocketsAcceptees = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condSocketsAcceptees = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Erreur...\nUSAGE : Serveur portServeur\n");
        exit(1);
    }

    for (int i = 0; i < TAILLE_FILE_ATTENTE; i++) {
        socketsAcceptees[i] = -1;
    }

    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = HandlerSIGINT;
    if (sigaction(SIGINT, &A, NULL) == -1) {
        perror("Erreur de sigaction");
        exit(1);
    }


    pthread_t th;
    for (int i = 0; i < NB_THREADS_POOL; i++) {
        pthread_create(&th, NULL, FctThreadClient, NULL);
    }

    // Socket d'écoute
    if ((sEcoute = ServerSocket(atoi(argv[1]))) == -1) {
        perror("Erreur de ServerSocket");
        exit(1);
    }

    int sService;
    char ipClient[50];
    printf("Démarrage du serveur d'encodage sur le port %s...\n", argv[1]);

    while (1) {
        printf("Attente d'une connexion...\n");
        if ((sService = Accept(sEcoute, ipClient)) == -1) {
            perror("Erreur de Accept");
            close(sEcoute);
            OBEP_Close();
            exit(1);
        }
        printf("Connexion acceptée : IP=%s socket=%d\n", ipClient, sService);

        pthread_mutex_lock(&mutexSocketsAcceptees);
        socketsAcceptees[indiceEcriture] = sService;
        indiceEcriture = (indiceEcriture + 1) % TAILLE_FILE_ATTENTE;
        pthread_mutex_unlock(&mutexSocketsAcceptees);

        pthread_cond_signal(&condSocketsAcceptees);
    }

    return 0;
}

void* FctThreadClient(void* p) {
    int sService;

    MYSQL* connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion, "localhost", "Student", "PassStudent1_", "PourStudent", 0, NULL, 0) == NULL) {
        fprintf(stderr, "\t[THREAD %p] Erreur MySQL: %s\n", (void*)pthread_self(), mysql_error(connexion));
        pthread_exit(NULL);
    }

    while (1) {
        pthread_mutex_lock(&mutexSocketsAcceptees);
        while (socketsAcceptees[indiceLecture] == -1) {
            pthread_cond_wait(&condSocketsAcceptees, &mutexSocketsAcceptees);
        }

        sService = socketsAcceptees[indiceLecture];
        socketsAcceptees[indiceLecture] = -1;
        indiceLecture = (indiceLecture + 1) % TAILLE_FILE_ATTENTE;
        pthread_mutex_unlock(&mutexSocketsAcceptees);

        printf("\t[THREAD %p] Prise en charge de la socket %d\n", (void*)pthread_self(), sService);
        TraitementConnexion(sService, connexion);
    }

    mysql_close(connexion);
    pthread_exit(NULL);
}

void TraitementConnexion(int sService, MYSQL* connexion) {
    OBEP_MESSAGE requete, reponse;
    int nbLus, nbEcrits;
    bool onContinue = true;

    while (onContinue) {
        printf("\t[THREAD %p] Attente requête...\n", (void*)pthread_self());

        if ((nbLus = Receive(sService, (char*)&requete)) < 0) {
            perror("Erreur de Receive");
            close(sService);
            return;
        }

        if (nbLus == 0) {
            printf("\t[THREAD %p] Client déconnecté (socket %d).\n", (void*)pthread_self(), sService);
            close(sService);
            return;
        }

        onContinue = OBEP(&requete, &reponse, connexion);

        if ((nbEcrits = Send(sService, (char*)&reponse, sizeof(OBEP_MESSAGE))) < 0) {
            perror("Erreur de Send");
            close(sService);
            return;
        }

        if (!onContinue) {
            printf("\t[THREAD %p] Fin de connexion sur la socket %d\n", (void*)pthread_self(), sService);
            close(sService);
        }
    }
}

void HandlerSIGINT(int s) {
    printf("\nArrêt du serveur.\n");
    close(sEcoute);
    OBEP_Close();
    exit(0);
}