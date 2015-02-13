#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include "outils.h"
#include "socketServeur.h"
#include "fonctionReseau.h"

#define TRUE 1
#define FALSE 0
#define TAILLE_BLOC 4096
#define SUCCESS 1
#define FAILED 0
#define PORT_RESERVE 10
#define PORT_DYNAMIQUE 11
#define PORT_INVALIDE 12
#define WRONG_ARG 50


extern Commande *commandeRecue; //Reçoit la structure commande après avoir été analysé et découpé
extern int sock_local; //Sert à créer et bind la socket du serveur
extern int sockClient; //Reçoit le descripteur de socket après qu'on ait accept le client
extern int sockClient_data;
extern unsigned int port_data; //Port utilisé pour la data par défaut

long startValue = 0; //position du curseur pour un fichier en DL/UP
//socklen_t longueur;
char blocClient[TAILLE_BLOC] = {0};


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

    while(!fin) {
		/* on cherche dans le tampon courant */
		while((finBloc > debutBloc) && (blocClient[debutBloc]!='\0')){
			message[index++] = blocClient[debutBloc++];
		}
		/* on a trouve ? */
		if ((index > 0) && (blocClient[debutBloc]=='\0')) {
			//message[index++] = '\n';
			message[index] = '\0';
			debutBloc++;
			fin = TRUE;
			return strdup(message);
		}else{
			/* il faut en lire plus de données en provenance de la socket*/
			debutBloc = 0;
			nbreOctet = recv(sockClient, blocClient, TAILLE_BLOC, 0);

			if (nbreOctet < 0) {
				perror("Reception, erreur de recv.");
				close(sock_local);
                exit(EXIT_FAILURE);
			}else if(nbreOctet == 0) {
				fprintf(stderr, "Erreur de reception : Le serveur a fermé la connexion.\n");
				return NULL;
			}else {
				finBloc = nbreOctet;
			}
		}
	}
	return NULL;
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
        nbreOctet = send(sockClient,monMessage,tailleBuffer,0);
        if (nbreOctet < 0) {
				perror("Reception, erreur de send : ");
				close(sockClient);
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
   emissionFichierBinaire
***************************/
int emissionFichierBinaire()
{
    int fd; //fileDescriptor de type INT
    long sizeFichier = 0;
    int nbBloc = 0;
    int plusPetit = 0;
    int plusGrand = 0;
    char buffer[TAILLE_BLOC];
    int tailleEnvoyee = 0;
    int nbreOctetBufferise = 0;
    long tailleToSend = 0;

    printf("Nom du fichier à Transmettre: %s\n\n",commandeRecue->parametre);

    /* Test de l'existence du fichier */
    if((fd = open (commandeRecue->parametre, O_RDONLY)) == -1){
        emissionMessage("550 - Fichier non trouvé sur le serveur....\n");
        return FAILED;

    }else{
        sizeFichier = tailleFichier (commandeRecue->parametre);
        printf("Taille du fichier à transmettre : %ld octets\n",sizeFichier);

        if(startValue != 0){
            if(startValue == sizeFichier){
                emissionMessage("551 - Vous avez déjà le fichier en entier. Supprimez-le si vous voulez le re-télécharger !\n");
                startValue = 0;
                return FAILED;
            }else{
                /* On déplace la tête de lecture du fichier */
                if(lseek(fd,(off_t)startValue,SEEK_CUR) == -1){
                    emissionMessage("451 - Service interrompu. Erreur lors du déplacement de la tête de lecture dans le fichier...\n");
                    return FAILED;
                }else{
                    emissionMessage("150 - Statut de fichier vérifié : il existe sur le serveur. Ouverture du canal de données en cours....\n");
                }
            }
        }else{
            emissionMessage("150 - Statut de fichier vérifié : il existe sur le serveur. Ouverture du canal de données en cours....\n");
        }
    }

    /* Ouverture du socket pour les données */
    newSocketData();

    /* On gère la taille des données à envoyer pour la boucle while */
    if(startValue == 0){
        tailleToSend = sizeFichier;
    }else{
        tailleToSend = sizeFichier - startValue;
    }

    /* On envoie la taille du fichier qu'on avait récupérée */
    if (send(sockClient_data,&sizeFichier,sizeof(int),0)==-1){
        perror("Erreur de send");
        close(sockClient_data);
        exit(EXIT_FAILURE);
    }

    while (tailleEnvoyee<tailleToSend) {
        fflush(stdout);
        memset(buffer,0,sizeof(buffer));
        nbreOctetBufferise = read (fd, buffer, TAILLE_BLOC);

        if (send (sockClient_data, buffer, nbreOctetBufferise, 0) == -1) {
            printf("erreur de send\n");
            perror ("Erreur envoi fichier : ");
            close (fd);
            close (sockClient_data);
            exit (EXIT_FAILURE);
        }else{
            nbBloc += 1;
            tailleEnvoyee += nbreOctetBufferise;
            if(nbreOctetBufferise>plusGrand){
                plusGrand = nbreOctetBufferise;
            }else{
                plusPetit = nbreOctetBufferise;
            }
        }
    }
    emissionMessage("226 - Transfert effectué. Fermeture du canal de data...");
    printf("\nTaille du lot transmis : %i \n", tailleEnvoyee);
    printf("Nombre de blocs envoyés : %i \n", nbBloc);

    if(plusPetit == 0){
        printf("Taille du seul bloc envoyé : %i \n", plusGrand);
    }else{
        printf("Taille des %i premiers blocs envoyés : %i \n", nbBloc-1, plusGrand);
        printf("Taille du dernier bloc envoyé : %i \n\n", plusPetit);
    }
    startValue = 0;
    close (fd);
    finSocketClient_data();
    return SUCCESS;
}


/***************************
   changeDataPort
***************************/
int changeDataPort(char *newPort)
{
    int nouveauPort = 0;
    int nb = 3;
    int index = 0;
    int poidsFort = 0;
    int poidsFaible = 0;
    char *position = NULL;
    char poidsFort_chaine[10] = {0};
    char poidsFaible_chaine[5] = {0};

    remplacer_str(newPort,",",".",&nb);

    position = strchr(newPort,',');
    if(position == NULL){
        return WRONG_ARG;

    }else{
        while(newPort[index] != ',')
            index++;

        strncpy(poidsFort_chaine,&newPort[index+1],10);
        position = strchr(poidsFort_chaine,'\0');
        if(position == NULL){
            poidsFort_chaine[10] = '\0';
            }

        position = strchr(poidsFort_chaine,',');

        if(position == NULL){
            return WRONG_ARG;
        }else{
            index = 0;
            while(poidsFort_chaine[index] != ',')
                    index++;

            strncpy(poidsFaible_chaine,&poidsFort_chaine[index+1],5);
            /* On change la virgule de poidsFort_chaine par un \0 */
            *position = '\0';

            position = strchr(poidsFaible_chaine,'\0');
            if(position == NULL){
                poidsFort_chaine[5] = '\0';
                }

            poidsFort = atoi(poidsFort_chaine);
            poidsFaible = atoi(poidsFaible_chaine);

            nouveauPort = poidsFort*256 + poidsFaible;

            if((nouveauPort<1024) && (nouveauPort>0)){
                    printf("ERREUR : Le numéro de port indiqué est réservé pour les \"Well Known Ports\"\n");
                    printf("Impossible de l'utiliser comme port de data \n");
                    printf("Retour au menu...\n\n");
                    return FAILED;
                }else if((nouveauPort>49151) && (nouveauPort < 65536)){
                    printf("ERREUR : Le numéro de port indiqué est réservé pour les ports dynamiques et/ou privés\n");
                    printf("Ils sont réservés pour les applications clientes lorsqu'une connexion est initiée ! \n");
                    printf("Retour au menu...\n\n");
                    return FAILED;
                }else if((nouveauPort<0) || (nouveauPort>65535 || (nouveauPort == 0))){
                    printf("ERREUR : Le numéro de port indiqué est invalide ! \n");
                    printf("Retour au menu...\n\n");
                    return FAILED;
                }else{
                    port_data = nouveauPort;
                    printf("Le port de data a été changé avec succès à %i\n",nouveauPort);
                    return SUCCESS;
                }
            }
        }
}
