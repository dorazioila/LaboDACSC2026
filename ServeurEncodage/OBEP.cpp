#include "OBEP.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

bool TraiterRequeteOBEP(char* trameRequete, char* trameReponse, MYSQL* connexion) {
    char commande[50];

    // Extraction de la commande (premier jeton avant le #)
    char* token = strtok(trameRequete, "#");
    if (token == NULL) return false;
    strcpy(commande, token);

    // --- CAS : LOGIN ---
    if (strcmp(commande, "LOGIN") == 0) {
        char* user = strtok(NULL, "#");
        char* password = strtok(NULL, "#");

        if (user != NULL && password != NULL) {
            char requeteSQL[256];
            sprintf(requeteSQL, "SELECT * FROM employees WHERE login='%s' AND password='%s'", user, password);

            if (mysql_query(connexion, requeteSQL) == 0) {
                MYSQL_RES* result = mysql_store_result(connexion);
                if (result) {
                    if (mysql_num_rows(result) > 0) {
                        sprintf(trameReponse, "LOGIN#OK\n");
                        mysql_free_result(result);
                        return true;
                    }
                    mysql_free_result(result);
                }
            }
        }
        sprintf(trameReponse, "LOGIN#KO\n");
        return true;
    }

    // --- CAS : GET_AUTHORS ---
    if (strcmp(commande, "GET_AUTHORS") == 0) {
        if (mysql_query(connexion, "SELECT last_name, first_name FROM authors")) {
            sprintf(trameReponse, "GET_AUTHORS#KO#0\n");
            return true;
        }

        MYSQL_RES* result = mysql_store_result(connexion);
        int nbAuteurs = mysql_num_rows(result);

        char temp[256];
        sprintf(trameReponse, "GET_AUTHORS#OK#%d", nbAuteurs);

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            sprintf(temp, "#%s#%s", row[0], row[1]);
            strcat(trameReponse, temp);
        }

        strcat(trameReponse, "\n");
        mysql_free_result(result);
        return true;
    }

    // --- CAS : GET_SUBJECTS (AJOUTÉ) ---
    if (strcmp(commande, "GET_SUBJECTS") == 0) {
        // Remplacez 'name' par le nom exact de votre colonne si différente dans la table 'subjects'
        if (mysql_query(connexion, "SELECT name FROM subjects")) {
            sprintf(trameReponse, "GET_SUBJECTS#KO#0\n");
            return true;
        }

        MYSQL_RES* result = mysql_store_result(connexion);
        int nbSujets = result ? mysql_num_rows(result) : 0;

        char temp[256];
        sprintf(trameReponse, "GET_SUBJECTS#OK#%d", nbSujets);

        MYSQL_ROW row;
        while ((row = mysql_fetch_row(result))) {
            sprintf(temp, "#%s", row[0]);
            strcat(trameReponse, temp);
        }

        strcat(trameReponse, "\n");
        mysql_free_result(result);
        return true;
    }

    // --- CAS : ADD_AUTHOR ---
    if (strcmp(commande, "ADD_AUTHOR") == 0) {
        char* nom = strtok(NULL, "#");
        char* prenom = strtok(NULL, "#");

        if (nom != NULL && prenom != NULL) {
            char requeteSQL[256];
            sprintf(requeteSQL, "INSERT INTO authors (last_name, first_name) VALUES ('%s', '%s')", nom, prenom);

            if (mysql_query(connexion, requeteSQL) == 0) {
                sprintf(trameReponse, "ADD_AUTHOR#OK\n");
                return true;
            }
        }
        sprintf(trameReponse, "ADD_AUTHOR#KO\n");
        return true;
    }

    // --- CAS : ADD_SUBJECT ---
    if (strcmp(commande, "ADD_SUBJECT") == 0) {
        char* name = strtok(NULL, "#");

        if (name != NULL) {
            char requeteSQL[256];
            sprintf(requeteSQL, "INSERT INTO subjects (name) VALUES ('%s')", name);

            if (mysql_query(connexion, requeteSQL) == 0) {
                sprintf(trameReponse, "ADD_SUBJECT#OK\n");
                return true;
            }
        }
        sprintf(trameReponse, "ADD_SUBJECT#KO\n");
        return true;
    }

    // --- CAS : ADD_BOOK ---
    if (strcmp(commande, "ADD_BOOK") == 0) {
        char* titre = strtok(NULL, "#");
        char* authorStr = strtok(NULL, "#"); 
        char* subjectStr = strtok(NULL, "#"); 
        char* isbn = strtok(NULL, "#");
        char* pageCount = strtok(NULL, "#");
        char* publishYear = strtok(NULL, "#");
        char* price = strtok(NULL, "#");
        char* stockQuantity = strtok(NULL, "#");

        if (titre && authorStr && subjectStr && isbn) {
            // 1. Conversion de la virgule en point pour le prix
            if (price != NULL) {
                char* pVirgule = strchr(price, ',');
                if (pVirgule) *pVirgule = '.';
            }

            int authorId = 0;
            int subjectId = 0;
            char requeteSQL[512];

            // 2. Recherche de l'author_id
            char authorCopy[256];
            strcpy(authorCopy, authorStr);
            char* nom = strtok(authorCopy, " ");
            char* prenom = strtok(NULL, " ");

            if (nom && prenom) {
                sprintf(requeteSQL, "SELECT id FROM authors WHERE last_name='%s' AND first_name='%s'", nom, prenom);
            } else {
                sprintf(requeteSQL, "SELECT id FROM authors WHERE last_name='%s'", authorStr);
            }

            if (mysql_query(connexion, requeteSQL) == 0) {
                MYSQL_RES* res = mysql_store_result(connexion);
                if (res) {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    if (row) authorId = atoi(row[0]);
                    mysql_free_result(res);
                }
            }

            // 3. Recherche du subject_id
            sprintf(requeteSQL, "SELECT id FROM subjects WHERE name='%s'", subjectStr);
            if (mysql_query(connexion, requeteSQL) == 0) {
                MYSQL_RES* res = mysql_store_result(connexion);
                if (res) {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    if (row) subjectId = atoi(row[0]);
                    mysql_free_result(res);
                }
            }

            // 4. Insertion dans 'books' avec 'stock_quantity'
            sprintf(requeteSQL, 
                "INSERT INTO books (title, author_id, subject_id, isbn, page_count, publish_year, price, stock_quantity) "
                "VALUES ('%s', %d, %d, '%s', %d, %d, %.2f, %d)",
                titre, authorId, subjectId, isbn, 
                pageCount ? atoi(pageCount) : 0, 
                publishYear ? atoi(publishYear) : 0, 
                price ? atof(price) : 0.0, 
                stockQuantity ? atoi(stockQuantity) : 0);

            printf("\t[DEBUG SQL] %s\n", requeteSQL);

            if (mysql_query(connexion, requeteSQL) == 0) {
                int newId = (int)mysql_insert_id(connexion);
                sprintf(trameReponse, "ADD_BOOK#OK#%d\n", newId);
                return true;
            } else {
                printf("\t[Erreur MySQL] %s\n", mysql_error(connexion));
            }
        }
        sprintf(trameReponse, "ADD_BOOK#KO\n");
        return true;
    }

    // --- CAS : LOGOUT ---
    if (strcmp(commande, "LOGOUT") == 0) {
        sprintf(trameReponse, "LOGOUT#OK\n");
        return false; // Clôture la session côté serveur
    }

    sprintf(trameReponse, "ERROR#UNKNOWN_COMMAND\n");
    return true;
}