#ifndef OBEP_H
#define OBEP_H

#include <mysql.h>
#include <stdbool.h>

bool TraiterRequeteOBEP(char* trameRequete, char* trameReponse, MYSQL* connexion);

#endif