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

#define TAILLE_FILE_ATTENTE 20

int socketsAcceptees[TAILLE_FILE_ATTENTE];
int indiceEcriture = 0;
int indiceLecture = 0;

pthread_mutex_t mutexSocketsAcceptees = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condSocketsAcceptees = PTHREAD_COND_INITIALIZER;

int main(int argc, char* argv[]) {
    int port = 50000;
    int nbThreads = 5;    FILE* f = fopen("serveur.conf", "r");
    if (f != NULL) {
        char ligne[100];
        while (fgets(ligne, sizeof(ligne), f)) {
            sscanf(ligne, "PORT_ENCODING=%d", &port);
            sscanf(ligne, "NB_THREADS_POOL=%d", &nbThreads);
        }
        fclose(f);
        printf("Configuration chargée : Port = %d, Threads Pool = %d\n", port, nbThreads);
    } else {
        printf("Fichier serveur.conf introuvable, utilisation des valeurs par défaut.\n");
    }    for (int i = 0; i < TAILLE_FILE_ATTENTE; i++) {
        socketsAcceptees[i] = -1;
    }    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = HandlerSIGINT;
    if (sigaction(SIGINT, &A, NULL) == -1) {
        perror("Erreur de sigaction");
        exit(1);
    }    pthread_t* thPool = (pthread_t*)malloc(sizeof(pthread_t) * nbThreads);
    for (int i = 0; i < nbThreads; i++) {
        pthread_create(&thPool[i], NULL, FctThreadClient, NULL);
    }    if ((sEcoute = ServerSocket(port)) == -1) {
        perror("Erreur de ServerSocket");
        free(thPool);
        exit(1);
    }

    int sService;
    char ipClient[50];
    printf("Démarrage du serveur d'encodage sur le port %d...\n", port);

    while (1) {
        printf("Attente d'une connexion...\n");
        if ((sService = Accept(sEcoute, ipClient)) == -1) {
            perror("Erreur de Accept");
            close(sEcoute);
            OBEP_Close();
            free(thPool);
            exit(1);
        }
        printf("Connexion acceptée : IP=%s socket=%d\n", ipClient, sService);

        pthread_mutex_lock(&mutexSocketsAcceptees);
        socketsAcceptees[indiceEcriture] = sService;
        indiceEcriture = (indiceEcriture + 1) % TAILLE_FILE_ATTENTE;
        pthread_mutex_unlock(&mutexSocketsAcceptees);

        pthread_cond_signal(&condSocketsAcceptees);
    }

    free(thPool);
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
        printf("\t[THREAD %p] Attente requête...\n", (void*)pthread_self());        nbLus = Receive(sService, (char*)&requete, sizeof(OBEP_MESSAGE));

        if (nbLus <= 0) {
            printf("\t[THREAD %p] Client déconnecté ou erreur de lecture (socket %d).\n", (void*)pthread_self(), sService);
            close(sService);
            return;
        }

        onContinue = OBEP(&requete, &reponse, connexion);        if ((nbEcrits = Send(sService, (char*)&reponse, sizeof(OBEP_MESSAGE))) < 0) {
            perror("Erreur de Send");
            close(sService);
            return;
        }

        if (!onContinue) {
            printf("\t[THREAD %p] Fin de session OBEP (LOGOUT) sur la socket %d\n", (void*)pthread_self(), sService);
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