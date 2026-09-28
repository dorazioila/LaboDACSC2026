#include "TCP.h"

int ServerSocket(int port){//socket,bind,listen 
	int sSocket;
	struct sockaddr_in adresse;
	
	if ((sSocket = socket(AF_INET, SOCK_STREAM,0))== -1){
		perror("Erreur de socket()");
		return -1;
	}
	
	memset(&adresse,0,sizeof(struct sockaddr_in));
	adresse.sin_family = AF_INET;
	adresse.sin_port = htons((unsigned short)port);
	adresse.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if(bind(sSocket,(struct sockaddr*)&adresse,sizeof(struct sockaddr_in))==-1){
		close(sSocket);
		return -1;
	}
	if(listen(sSocket,5)==-1){
		close(sSocket);
		return -1;
	}
	return sSocket;
}

int Accept(int sEcoute, char *ipClient){
	int sService;
	struct sockaddr_in adrClient;
	socklen_t tailleAdr =sizeof(struct sockaddr_in);

	sService = accept(sEcoute, (struct sockaddr*)&adrClient, &tailleAdr);
    if (sService == -1){
        return -1;
    }
    
    if (ipClient != NULL){
        strcpy(ipClient, inet_ntoa(adrClient.sin_addr));
    }

    return sService;
}

int ClientSocket(char* ipServeur,int portServeur){
	int sSocket;
	struct sockaddr_in adresse;
	if((sSocket = socket(AF_INET,SOCK_STREAM,0))==-1){
		return -1;
	}
	memset(&adresse, 0, sizeof(struct sockaddr_in));
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons((unsigned short)portServeur);
    adresse.sin_addr.s_addr = inet_addr(ipServeur);

    if (connect(sSocket, (struct sockaddr*)&adresse, sizeof(struct sockaddr_in)) == -1){
        close(sSocket);
        return -1;
    }
    return sSocket;
}

int Send(int sSocket, char* data, int taille){
    int totalEcrit = 0;
    int nbEcrits;
    while (totalEcrit < taille){
        nbEcrits = write(sSocket, data + totalEcrit, taille - totalEcrit);
        if (nbEcrits <= 0) return -1;
        totalEcrit += nbEcrits;
    }

    char fin = '\n';
    if (write(sSocket, &fin, 1) <= 0) return -1;

    return totalEcrit + 1;
}
   

int Receive(int sSocket, char* data){
    char c;
    int nbLus;
    int i = 0;
    char finDeTrame = '\n'; 
    while (i < TAILLE_MAX_DATA - 1){
        nbLus = read(sSocket, &c, 1);

        if (nbLus < 0){
            return -1; 
        }
        if (nbLus == 0){
            break; 
        }
        if (c == finDeTrame){
            break;
        } 
        data[i] = c;
        i++;
    }
    data[i] = '\0';
    return i; 
}