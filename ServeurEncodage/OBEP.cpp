#include "OBEP.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool OBEP(OBEP_MESSAGE* requete, OBEP_MESSAGE* reponse, MYSQL* connexion) {
    // Par défaut, la réponse conserve le même type que la requête
    memset(reponse, 0, sizeof(OBEP_MESSAGE));
    reponse->type = requete->type;

    switch (requete->type) {

        case LOGIN: {
            char login[50], password[50];
            sscanf(requete->data, "%[^;];%s", login, password);

            char sql[256];
            sprintf(sql, "SELECT id FROM employees WHERE login='%s' AND password='%s';", login, password);

            if (mysql_query(connexion, sql) == 0) {
                MYSQL_RES* res = mysql_store_result(connexion);
                if (res && mysql_num_rows(res) > 0) {
                    reponse->status = 0; // Succès
                } else {
                    reponse->status = -1;
                }
                if (res) mysql_free_result(res);
            } else {
                reponse->status = -1;
            }
            return true;
        }

        case LOGOUT: {
            reponse->status = 0;
            return false; 
        }

        case GET_AUTHORS: {
            char sql[] = "SELECT id, last_name, first_name FROM authors;";
            if (mysql_query(connexion, sql) == 0) {
                MYSQL_RES* res = mysql_store_result(connexion);
                if (res) {
                    MYSQL_ROW row;
                    reponse->data[0] = '\0';
                    while ((row = mysql_fetch_row(res)) != NULL) {
                        char temp[128];
                        sprintf(temp, "%s;%s;%s#", row[0], row[1], row[2]);
                        strcat(reponse->data, temp);
                    }
                    mysql_free_result(res);
                    reponse->status = 0;
                }
            } else {
                reponse->status = -1;
            }
            return true;
        }

        case GET_SUBJECTS: {
            char sql[] = "SELECT id, name FROM subjects;";
            if (mysql_query(connexion, sql) == 0) {
                MYSQL_RES* res = mysql_store_result(connexion);
                if (res) {
                    MYSQL_ROW row;
                    reponse->data[0] = '\0';
                    while ((row = mysql_fetch_row(res)) != NULL) {
                        char temp[128];
                        sprintf(temp, "%s;%s#", row[0], row[1]);
                        strcat(reponse->data, temp);
                    }
                    mysql_free_result(res);
                    reponse->status = 0;
                }
            } else {
                reponse->status = -1;
            }
            return true;
        }

        case ADD_AUTHOR: {
            char lastName[50], firstName[50];
            sscanf(requete->data, "%[^;];%s", lastName, firstName);

            char sqlCheck[256];
            sprintf(sqlCheck, "SELECT id FROM authors WHERE last_name='%s' AND first_name='%s';", lastName, firstName);
            mysql_query(connexion, sqlCheck);
            MYSQL_RES* res = mysql_store_result(connexion);

            if (res && mysql_num_rows(res) > 0) {
                reponse->status = -1; // Auteur existe déjà
                mysql_free_result(res);
            } else {
                if (res) mysql_free_result(res);
                char sqlInsert[256];
                sprintf(sqlInsert, "INSERT INTO authors (last_name, first_name) VALUES ('%s', '%s');", lastName, firstName);
                if (mysql_query(connexion, sqlInsert) == 0) {
                    reponse->status = 0;
                    reponse->id = (int)mysql_insert_id(connexion);
                } else {
                    reponse->status = -1;
                }
            }
            return true;
        }

        case ADD_SUBJECT: {
            char name[50];
            strcpy(name, requete->data);

            char sqlCheck[256];
            sprintf(sqlCheck, "SELECT id FROM subjects WHERE name='%s';", name);
            mysql_query(connexion, sqlCheck);
            MYSQL_RES* res = mysql_store_result(connexion);

            if (res && mysql_num_rows(res) > 0) {
                reponse->status = -1;
                mysql_free_result(res);
            } else {
                if (res) mysql_free_result(res);
                char sqlInsert[256];
                sprintf(sqlInsert, "INSERT INTO subjects (name) VALUES ('%s');", name);
                if (mysql_query(connexion, sqlInsert) == 0) {
                    reponse->status = 0;
                    reponse->id = (int)mysql_insert_id(connexion);
                } else {
                    reponse->status = -1;
                }
            }
            return true;
        }

        case ADD_BOOK: {
            int authorId, subjectId, pageCount, stockQuantity, publishYear;
            float price;
            char title[100], isbn[20];

            sscanf(requete->data, "%d;%d;%[^;];%[^;];%d;%d;%f;%d", 
                   &authorId, &subjectId, title, isbn, &pageCount, &stockQuantity, &price, &publishYear);

            char sqlInsert[512];
            sprintf(sqlInsert, "INSERT INTO books (author_id, subject_id, title, isbn, page_count, stock_quantity, price, publish_year) "
                               "VALUES (%d, %d, '%s', '%s', %d, %d, %.2f, %d);",
                    authorId, subjectId, title, isbn, pageCount, stockQuantity, price, publishYear);

            if (mysql_query(connexion, sqlInsert) == 0) {
                reponse->status = 0;
                reponse->id = (int)mysql_insert_id(connexion);
            } else {
                reponse->status = -1;
            }
            return true;
        }

        default:
            reponse->status = -1;
            return true;
    }
}

void OBEP_Close() {
   
}