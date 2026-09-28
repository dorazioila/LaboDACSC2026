#ifndef OBEP_H
#define OBEP_H
#include <mysql.h>

#define LOGIN        1
#define LOGOUT       2
#define GET_AUTHORS  3
#define GET_SUBJECTS 4
#define ADD_AUTHOR   5
#define ADD_SUBJECT  6
#define ADD_BOOK     7

typedef struct {
    int type;        
    int status;      
    int id;         
    char data[512];
    } OBEP_MESSAGE; 

bool OBEP(OBEP_MESSAGE* requete, OBEP_MESSAGE* reponse, MYSQL* connexion);
void OBEP_Close();

#endif