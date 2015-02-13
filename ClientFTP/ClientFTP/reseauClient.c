#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#define TRUE 1
#define FALSE 0
#define TAILLE_BLOC 4096
#define SUCCESS 1
#define FAILED 0
#define PORT_RESERVE 10
#define PORT_DYNAMIQUE 11
#define PORT_INVALIDE 12

#include "fonctionClient.h"
#include "reseauClient.h"
#include "socketClient.h"

extern int sock_local; //Socket créé quand on se connecte au serveur pour le canal de commande
extern int sockServeur_data; //Socket créé quand on accepte la connexion depuis le serveur pour la data
int port_temporaire; // On sauvegarde le numéro de port s'il est valide, en attendant la réponse du serveur
long startValue = 0; //position du curseur pour un fichier en DL/UP

char blocServeur[TAILLE_BLOC] = {0}; //Variable recevant les messages de la part du serveur
extern char *parametre; // Paramètre de la commande déclaré dans fonctionClient.c


/***********************
   receptionMessage()
***********************/
char *receptionMessage()
{
	char message[TAILLE_BLOC];
	int index = 0;
	int fin = FALSE;
	int nbreOctet = 0;
	int debutBloc =0;
	int finBloc = 0;

    memset(blocServeur,0,TAILLE_BLOC);

    while(!fin) {
		/* on cherche dans le tampon courant */
		while((finBloc > debutBloc) && (blocServeur[debutBloc]!='\0'))
		{
			message[index++] = blocServeur[debutBloc++];
		}
		/* on a trouve ? */
		if ((index > 0) && (blocServeur[debutBloc]=='\0')) {
			//message[index++] = '\n';
			message[index] = '\0';
			debutBloc++;
			fin = TRUE;
			return strdup(message);

		} else {
			/* il faut en lire plus de données en provenance de la socket*/
			debutBloc = 0;
			nbreOctet = recv(sock_local, blocServeur, TAILLE_BLOC, 0);

			if (nbreOctet < 0) {
				perror("Reception, erreur de recv.");
				close(sock_local);
                exit(EXIT_FAILURE);
			} else if(nbreOctet == 0) {
				fprintf(stderr, "Erreur de reception : Le serveur a fermé la connexion.\n");
				return NULL;
			} else {
				finBloc = nbreOctet;
			}
		}
	}
	return NULL;
}


/******************************
    receptionFichierBinaire
******************************/
int receptionFichierBinaire ()
{
    int fd;
    int tailleFichier = 0;
    int tailleBlocRecu = 0;
    int tailleLotRecu = 0;
    int plusPetit = 0;
    int plusGrand = 0;
    int nbreOctetBufferise = 0;
    int tailleBuffer=TAILLE_BLOC;
    unsigned int nbBloc = 0;
    char *message;
    char bufferReception [TAILLE_BLOC] = {0};

    message = receptionMessage();
    printf("\n[Info Serveur] : %s\n",message);
    if(strstr(message,"150") == NULL){
        startValue = 0;
        return FAILED;
    }

    newSocketData_listen();

    /* On récupère la taille du fichier */
    recv (sockServeur_data, &tailleFichier, sizeof(int), 0);
    printf("Taille totale du fichier à recevoir (théorique) : %i \n",tailleFichier);

    /* On enlève les caractères \r\n contenus dans parametre sinon le nom de fichier sera erroné */
    *((parametre)+(strlen(parametre)-2)) = '\0';

    if(startValue == 0){
        if((fd = open (parametre, O_WRONLY | O_CREAT | O_TRUNC,0666)) == -1){
            printf("Problème avec le fichier \n\n");
            fflush(stdout);
            perror("Erreur ouverture fichier :");
            exit (EXIT_FAILURE);
        }
    }else{
        if((fd = open (parametre, O_WRONLY | O_APPEND)) == -1){
            printf("Problème avec le fichier \n\n");
            fflush(stdout);
            perror("Erreur ouverture fichier :");
            exit (EXIT_FAILURE);
        }

        /* startValue -1 car on enlève le EOF */
        if(lseek(fd,(off_t)startValue-1,SEEK_CUR) == -1){
            perror("Erreur : Problème lors du déplacement du curseur dans le fichier. Abandon... : ");
            return FAILED;
        }else{
            printf("Le curseur de lecture du fichier a été déplacé\n\n");
            }

        tailleBlocRecu = startValue;
    }

    while(tailleBlocRecu<tailleFichier){
        if((nbreOctetBufferise = read (sockServeur_data, bufferReception, tailleBuffer)) < 0)
            perror("Erreur lors de la lecteur de numbre d'octet dans le socket :");

        if((write (fd, bufferReception, nbreOctetBufferise)) != nbreOctetBufferise)
            perror("Erreur lors de l'écriture du fichier :");

        nbBloc += 1;
        tailleBlocRecu += nbreOctetBufferise;
        tailleLotRecu += nbreOctetBufferise;

        if((tailleFichier-tailleBlocRecu)< TAILLE_BLOC)
            tailleBuffer = tailleFichier - tailleBlocRecu;

        if(nbreOctetBufferise>plusGrand){
                plusGrand = nbreOctetBufferise;
            }else{
                plusPetit = nbreOctetBufferise;
            }
    }

    printf("Taille du lot reçu : %i \n", tailleLotRecu);
    printf("\nNombre de blocs reçus : %i \n", nbBloc);

    if(plusPetit == 0){
        printf("Taille du seul bloc reçu : %i \n", plusGrand);
    }else{
        printf("Taille des %i premiers blocs reçus : %i \n", nbBloc-1, plusGrand);
        printf("Taille du dernier bloc reçu : %i \n", plusPetit);
    }

    startValue = 0;
    close (fd);
    printf("\n[Info Serveur] : %s\n\n",receptionMessage());
    finSocketData();
    return SUCCESS;
}


/***************************
   emissionMessage
***************************/
int emissionMessage(char *message)
{
    char *monMessage = message;
    int tailleMessage = 0;
    int tailleEnvoyee = 0;
    int nbreOctet = 0;
    int tailleBuffer = TAILLE_BLOC;

    if(strchr(monMessage, '\0') == NULL){
        printf("Erreur : Le caractère de fin \\0 n'a pas été trouvé ! Abandon de l'envoi du message");
        exit(EXIT_FAILURE);
    }

    tailleMessage = strlen(monMessage) + 1;

    if(tailleMessage<TAILLE_BLOC){
        tailleBuffer = tailleMessage;
        }

    while(tailleEnvoyee < tailleMessage){
        nbreOctet = send(sock_local,monMessage,tailleBuffer,0);
        if (nbreOctet < 0) {
				perror("Reception, erreur de send : ");
				close(sock_local);
                exit(EXIT_FAILURE);
			} else if(nbreOctet == 0) {
				fprintf(stderr, "Erreur d'émission : L'hôte distant a fermé la connexion.\n");
				return FAILED;
			} else {
				tailleEnvoyee += nbreOctet;
				 if((tailleMessage-tailleEnvoyee)< TAILLE_BLOC){
                    tailleBuffer = tailleMessage - tailleEnvoyee;
				 }
			}
    }
    return SUCCESS;
}


/***************************
   isPortValide
***************************/
int isPortValide(char *newPort)
{
    int nouveauPort = atoi(newPort);

    if((nouveauPort<1024) && (nouveauPort>0)){
        printf("\nERREUR : Le numéro de port indiqué est réservé pour les \"Well Known Ports\"\n");
        printf("Impossible de l'utiliser comme port de data \n");
        port_temporaire = 0;
        return PORT_RESERVE;
    }else if((nouveauPort>49151) && (nouveauPort < 65536)){
        printf("\nERREUR : Le numéro de port indiqué est réservé pour les ports dynamiques et/ou privés\n");
        printf("Ils sont réservés pour les applications clientes lorsqu'une connexion est initiée ! \n");
        port_temporaire = 0;
        return PORT_DYNAMIQUE;
    }else if((nouveauPort<0) || (nouveauPort>65535 || (nouveauPort == 0))){
        printf("\nERREUR : Le numéro de port indiqué est invalide ! \n");
        port_temporaire = 0;
        return PORT_INVALIDE;
    }else{
        port_temporaire = nouveauPort;
        return SUCCESS;
    }
}



