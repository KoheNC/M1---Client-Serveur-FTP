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

#include "socketClient.h"
#include "reseauClient.h"

int sockClient_listen; // Socket pour écouter en attendant la connexion du serveur pour la data
int sockServeur_data; //Socket créé quand on accepte la connexion depuis le serveur pour la data
int sock_local; //Socket créé quand on se connecte au serveur pour le canal de commande
unsigned int port_data = 1025; //Port par défaut utilisé pour la data


/***************************
   finSocketData
***************************/
void finSocketData()
{
    close(sockServeur_data);
    close(sockClient_listen);
    sockServeur_data = 0;
    sockClient_listen = 0;
}


/***************************
   finSocketLocal
***************************/
void finSocketLocal()
{
    close(sock_local);
    sock_local = 0;
}


/******************************
    newSocketData_listen
******************************/
void newSocketData_listen()
{
    struct sockaddr_in client, paramClient;
    socklen_t longueur;

    sockClient_listen = socket(AF_INET, SOCK_STREAM,0);
    if (sockClient_listen == -1)
        perror("Impossible de créer le socket :");

    paramClient.sin_family = AF_INET;
    paramClient.sin_addr.s_addr = INADDR_ANY;
    paramClient.sin_port = htons(port_data);

    /* Bind */
    if(bind(sockClient_listen,(struct sockaddr *)&paramClient,sizeof(paramClient)) < 0){
        perror("Le bind a échoué :");
        exit(EXIT_FAILURE);
    }

    /* On écoute */
    listen(sockClient_listen,1);

    longueur = sizeof(struct sockaddr_in);
    if((sockServeur_data = accept(sockClient_listen,(struct sockaddr *)&client,&longueur)) == 1){
        perror("Erreur lors de l'accept :");
        exit(EXIT_FAILURE);
    }
}


/******************************
    initialisation_client
******************************/
void initialisation_client()
{
    char nom_addr_srv [81],port [6];
    int port_srv;
    struct addrinfo hints, *infoServeur;

    printf("'Il s'agit de TCP \n");

    /*Demande de l'IP de serveur*/
    puts ("Nom ou IP Serveur :");
    memset (nom_addr_srv,0,sizeof(nom_addr_srv));
    if (scanf("%s",nom_addr_srv)==0)
    {
		perror("Erreur de lecture du nom ou de l'adresse");
		exit(EXIT_FAILURE);
    }

    /* Demande du port d'écoute du serveur
     * puis conversion du port en decimal --> tableau int-->char
     */
    puts("port d'écoute du serveur :");
    memset(&port_srv,0,sizeof(port_srv));
    if (scanf ("%d",&port_srv)==0)
    {
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
    if(getaddrinfo (nom_addr_srv,port,&hints,&infoServeur)!=0)//2 derniers struct // renvoyer une autre structure ou liste chaine addr_onfo+nom+ort
    {
        perror("Erreur getaddri");
        exit (EXIT_FAILURE);
    }

    /*Création de socket*/
    if ((sock_local=socket (infoServeur->ai_family,infoServeur->ai_socktype,infoServeur->ai_protocol))==-1)
    {
        perror("Erreur création du socket");
        exit(EXIT_FAILURE);
    }

	/* Si mode TCP */
    /* Connexion au serveur */
    if (connect (sock_local, infoServeur->ai_addr, infoServeur->ai_addrlen) == -1) //Connexion au client
    {
        perror ("Erreur de connect");
        close (sock_local);
        exit (EXIT_FAILURE);
    }
    printf("\n[Info Serveur] : %s",receptionMessage());
}
