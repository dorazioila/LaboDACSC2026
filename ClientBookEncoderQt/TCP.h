#ifndef TCP_H
#define TCP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int ServerSocket(int port);
int Accept(int sEcoute, char *ipClient);
int ClientSocket(char* ipServeur, int portServeur);
int Send(int sSocket, char* data, int taille);
int ReceiveLine(int sSocket, char* buffer, int maxSize);

#endif