#include "OBEP.h"
#include <stdio.h>
#include <string.h>

bool TraiterRequeteOBEP(char* trameRequete, char* trameReponse, MYSQL* connexion) {
    char commande[50];
    // Extraction du premier champ (la commande)
    char* token = strtok(trameRequete, "#");
    if (token == NULL) return false;
    strcpy(commande, token);

    // --- CAS : OBTENIR LA LISTE DES AUTEURS ---
    if (strcmp(commande, "GET_AUTHORS") == 0) {
        MYSQL_RES* result;
        MYSQL_ROW row;

        if (mysql_query(connexion, "SELECT last_name, first_name FROM authors")) {
            sprintf(trameReponse, "GET_AUTHORS#KO#0\n");
            return true;
        }

        result = mysql_store_result(connexion);
        int nbAuteurs = mysql_num_rows(result);

        // Construction du début de la trame : TYPE#STATUS#NB_ELEMENTS
        char temp[1024];
        sprintf(trameReponse, "GET_AUTHORS#OK#%d", nbAuteurs);

        // Concaténation des auteurs : #NOM1#PRENOM1#NOM2#PRENOM2...
        while ((row = mysql_fetch_row(result))) {
            sprintf(temp, "#%s#%s", row[0], row[1]);
            strcat(trameReponse, temp);
        }

        strcat(trameReponse, "\n"); // Fin de trame
        mysql_free_result(result);
        return true;
    }

    // --- CAS : LOGOUT ---
    if (strcmp(commande, "LOGOUT") == 0) {
        sprintf(trameReponse, "LOGOUT#OK\n");
        return false; // Met fin à la boucle dans TraitementConnexion
    }

    sprintf(trameReponse, "ERROR#UNKNOWN_COMMAND\n");
    return true;
}