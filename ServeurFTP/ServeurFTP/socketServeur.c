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

int sockClient_data = 0;
int sock_local; //Sert à créer et bind la socket du serveur
int sockClient; //Reçoit le descripteur de socket après qu'on ait accept le client
unsigned int port_data = 1025; //Port utilisé pour la data par défaut
socklen_t longueur;


/***************************
   finSocketClient_data
***************************/
void finSocketClient_data()
{
    close (sockClient_data);
    sockClient_data = 0;
}


/***************************
   finSocketClient
***************************/
void finSocketClient()
{
    close (sockClient);
    sockClient = 0;
    port_data = 1025;   // On reset le port data par défaut à la déconnexion du client
}


/***************************
   finSocketLocal
***************************/
void finSocketLocal()
{
    close(sock_local);
    sock_local = 0;
}


/***************************
        newSocketData
***************************/
void newSocketData()
{
    char nom_addr_srv[INET_ADDRSTRLEN];
    char port[6];
    int index = 0;
    int connexionOK = FALSE;
    struct addrinfo hints, *infoServeur;
    struct sockaddr_in adresse_client;
    socklen_t peer_longueur;

    //Il faut pré-calculer la taille de la structure avant d'utiliser la fonction getpeername()
    peer_longueur = sizeof(struct sockaddr_in);
    if(getpeername(sockClient, (struct sockaddr*) &adresse_client, &peer_longueur)!=0) {
        perror("Erreur lors de la récupération des infos client....getpeername failed \n");
        exit(EXIT_FAILURE);
    }
    inet_ntop(AF_INET,&(adresse_client.sin_addr),nom_addr_srv,INET_ADDRSTRLEN);
    sprintf(port,"%d",port_data);

    //printf("Adresse IP récupérée depuis le socket_cmd du client : %s \n", nom_addr_srv);
    printf("Port du canal de data client : %s \n", port);

    /* Préparation de la structure addrinfo, on initialise hints avec des 0 et la taille de hints
     * Puis appel à la fonction getaddrinfo
     */
    memset (&hints,0,sizeof (hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    if(getaddrinfo (nom_addr_srv,port,&hints,&infoServeur)!=0) {
        perror("Erreur getaddri");
        exit (EXIT_FAILURE);
    }

    /*Création de socket*/
    if ((sockClient_data=socket (infoServeur->ai_family,infoServeur->ai_socktype,infoServeur->ai_protocol))==-1) {
        perror("Erreur création du socket");
        exit(EXIT_FAILURE);
    }

    /* On tempo un peu si la connexion ne marche pas dès le début */
    while((index<6) && (connexionOK == FALSE)){
        if (connect (sockClient_data, infoServeur->ai_addr, infoServeur->ai_addrlen) == -1){
            index += 0.5;
            sleep(0.5);
        }else{
            connexionOK = TRUE;
        }
    }

    if(connexionOK != TRUE){
        perror ("Erreur de connect");
        close (sockClient_data);
        exit (EXIT_FAILURE);
    }
}


/****************
   infoNewClient
****************/
void infoNewClient(int sock_client)
{
    struct sockaddr_in adresse_client;
    socklen_t peer_longueur;
    char ip[INET_ADDRSTRLEN];

    //Il faut pré-calculer la taille de la structure avant d'utiliser la fonction getpeername()
    peer_longueur = sizeof(struct sockaddr_in);

    if(getpeername(sock_client, (struct sockaddr*) &adresse_client, &peer_longueur)!=0){
        perror("Erreur lors de la récupération des infos client....getpeername failed \n");
        exit(EXIT_FAILURE);
    }

    inet_ntop(AF_INET,&(adresse_client.sin_addr),ip,INET_ADDRSTRLEN);
    printf("Nouveau client accepté ! \n");
    printf("IP Client = %s, Port côté client = %u \n\n",
           ip,
           ntohs(adresse_client.sin_port));
}


/*******************
   attenteClient
*******************/
void attenteClient()
{
    struct sockaddr client; //structure qui reçoit les info client
    longueur = sizeof(struct sockaddr_in);

    printf("\nAttente Client \n\n");
    if((sockClient=accept(sock_local,&client,&longueur))==-1){
        perror("Erreur de accept");
        close(sock_local);
        exit(EXIT_FAILURE);
    }else {
        infoNewClient(sockClient);
        emissionMessage("220 - Serveur FTP actif. Session ouverte.... \n");
    }
}


/***************************
   initialisation_serveur
***************************/
void initialisation_serveur()
{
    char nom_addr_srv [81],port [6];
    int port_srv;
    struct addrinfo hints, *infoServeur;

    memset (nom_addr_srv,0,sizeof(nom_addr_srv));
    memset(&port_srv,0,sizeof(port_srv));

    printf("Il s'agit de TCP \n");

	 /*Demande de l'IP de serveur*/
    puts ("Entrez l'IP du Serveur :");
    if (scanf("%s",nom_addr_srv)==0){
		perror("Erreur de lecture du nom ou de l'adresse");
		exit(EXIT_FAILURE);
    }

    /* Demande du port d'écoute du serveur
     * puis conversion du port en decimal --> tableau int-->char
     */
    puts("Entrez le port d'écoute du serveur :");
    if (scanf ("%d",&port_srv)==0){
        printf("Erreur de lecture du port\n");
        exit (EXIT_FAILURE);
    }
    sprintf(port,"%d",port_srv);

    /* Préparation de la structure addrinfo, on initialise hints avec des 0 et la taille de hints
     * Puis appel à la fonction getaddrinfo
     */
    memset (&hints,0,sizeof (hints));
    hints.ai_family=AF_INET;
    hints.ai_socktype=SOCK_STREAM;
    if(getaddrinfo (nom_addr_srv,port,&hints,&infoServeur)!=0){
        perror("Erreur getaddri");
        exit (EXIT_FAILURE);
    }

    /*Création et bind du socket*/
    if ((sock_local=socket (infoServeur->ai_family,infoServeur->ai_socktype,infoServeur->ai_protocol))==-1){
        perror("Erreur création du socket");
        exit(EXIT_FAILURE);
    }
    if (bind(sock_local,infoServeur->ai_addr,infoServeur->ai_addrlen)==-1){
        perror("Erreur de bind");
        close(sock_local);
        exit(EXIT_FAILURE);
    }

    /* Attente d'un client */
    if(listen(sock_local,5)== -1){
        perror ("Erreur de listen");
        close(EXIT_FAILURE);
    }
}
