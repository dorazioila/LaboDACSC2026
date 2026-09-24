#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "TCP.h"

typedef struct
{
    char nom[20];
    int age;
    float poids;
} PERSONNE;

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("Erreur...\n");
        printf("USAGE : ServeurTest portServeur\n");
        exit(1);
    }

    int sServer;
    if ((sServer = ServerSocket(atoi(argv[1]))) == -1)
    {
        perror("Erreur de ServerSocket");
        exit(1);
    }

    printf("Attente d'une connexion...\n");

    int sService;
    if ((sService = Accept(sServer, NULL)) == -1)
    {
        perror("Erreur de Accept");
        close(sServer);
        exit(1);
    }

    printf("Connexion acceptee !\n");

    // ***** 1. Réception texte pur **************************************
    char buffer[100];
    int nbLus;
    if ((nbLus = Receive(sService, buffer)) < 0)
    {
        perror("Erreur de Receive");
        close(sService);
        close(sServer);
        exit(1);
    }
    printf("NbLus = %d\n", nbLus);
    printf("Lu = --%s--\n", buffer);

    // ***** 2. Envoi de texte pur ***************************************
    char texte[80];
    sprintf(texte, "Je vais bien merci ;) !");
    int nbEcrits;
    if ((nbEcrits = Send(sService, texte, strlen(texte))) < 0)
    {
        perror("Erreur de Send");
        close(sService);
        close(sServer);
        exit(1);
    }
    printf("NbEcrits = %d\n", nbEcrits);
    printf("Ecrit = --%s--\n", texte);

    // ***** 3. Réception d'une structure ********************************
    PERSONNE p;
    if ((nbLus = Receive(sService, (char*)&p)) < 0)
    {
        perror("Erreur de Receive");
        close(sService);
        close(sServer);
        exit(1);
    }
    printf("NbLus = %d\n", nbLus);
    printf("Lu = --%s--%d--%f--\n", p.nom, p.age, p.poids);

    // ***** 4. Envoi d'une structure *************************************
    strcpy(p.nom, "charlet");
    p.age = 54;
    p.poids = 71.98f;
    if ((nbEcrits = Send(sService, (char*)&p, sizeof(PERSONNE))) < 0)
    {
        perror("Erreur de Send");
        close(sService);
        close(sServer);
        exit(1);
    }
    printf("NbEcrits = %d\n", nbEcrits);
    printf("Ecrit = --%s--%d--%f--\n", p.nom, p.age, p.poids);

    // Fermeture des sockets
    close(sService);
    close(sServer);

    return 0;
}