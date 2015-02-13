#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>

#define TRUE 1
#define FALSE 0
#define BLOCK 4096

#include "fonctionClient.h"
#include "reseauClient.h"
#include "socketClient.h"

extern unsigned int port_data;
extern int port_temporaire;

int main () {
    char *message;
    int choix = TRUE;
//    /// ---------------ZONE DE TEST ----------------------------------
//    /// COMMENT GERER LES BITS !!!!!!
//    unsigned int i = 62000;
//    unsigned char carac[2];
//
//    carac[0] = (unsigned char)((i >> 8)& 0x00FF);  /// EN SUPPOSANT QUE LE MSB EST EN 1ER !!!!/* MSB (3) en tete (0) */
//    carac[1] = (unsigned char)((i >> 0) & 0x00FF);
//
//
//    unsigned int reception = 0;
//
//    reception |=(carac[0] << (1*8));
//    reception |=(carac[1] << (0*8));
//
//    printf("numero=%i\n",reception);

//char *msg = "lol.salut.c'est.cool.ici...lolol";
//
//remplacer_str(msg,".salut",",",0);
//
//printf("Le message est =%s\n",msg);
//
//
//
//
//return 1;
/// ---------------FIN ZONE DE TEST ----------------------------------

    initialisation_client();

    printf("\nBienvenue dans le client  ! \n\n");
    connectionUserName();

    while(choix > 0)
    {
        fflush(stdin);
        printf("Que voulez-vous faire ?\n");
        printf("   1 : Changer le port du canal de data \n");
        printf("   2 : Télécharger un fichier depuis (RETR)\n");
        printf("   3 : Envoyer un fichier vers le serveur (STOR)\n");
        printf("   4 : Réponse NOOP de la part du serveur \n");
        printf("   5 : Modifier le type de représentation des données (actuellement type IMAGE (Binaire))\n");
        printf("   6 : Changer le mode de transfert \n");
        printf("   7 : Changer le type de structure de fichier : (F)ichier, (R)ecording - Enregistrement- (P)ages \n");
        printf("   9 : Quitter l'application \n");

        printf("\nChoix :\n\n");
        printf("FTP>");

        scanf("%d",&choix);

        switch(choix)
        {
            case 1:
                printf("Veuillez indiquer le nouveau numéro de port: \n");
                preparation_Commande("PORT");

                if(port_temporaire !=0 ){
                    message = receptionMessage();
                    printf("\n[Info Serveur] : %s\n",message);

                    if(strstr(message,"200") != NULL){
                        port_data = port_temporaire;
                        printf("\nLe port du canal de donnée a été changé à %i\n",port_data);
                    }
                }
                break;

            case 2:
                printf("Entrez votre nom de fichier : \n");
                preparation_Commande("RETR");
                receptionFichierBinaire();
                break;

            case 3:
                printf("Veuillez saisir le nom du fichier à transférer :\n");
                preparation_Commande("STOR");
                printf("\n[Info] : %s\n",receptionMessage());
                break;

            case 4:
                preparation_Commande("NOOP");
                printf("\n[Info Serveur] : %s\n",receptionMessage());
                break;

            case 5:
                printf("Veuillez saisir le type de représentation de données :\n");
                preparation_Commande("TYPE");
                printf("\n[Info Serveur] : %s\n\n",receptionMessage());
                break;

            case 6:
                printf("Veuillez saisir le mode de transfert :\n");
                preparation_Commande("MODE");
                printf("\n[Info Serveur] : %s\n\n",receptionMessage());
                break;

            case 7:
                printf("Veuillez saisir le type de structure de fichier : (F)ichier, (R)ecording - Enregistrement- (P)ages \n");
                preparation_Commande("STRU");
                printf("\n[Info Serveur] : %s\n\n",receptionMessage());
                break;

            case 9:
                preparation_Commande("QUIT");
                choix = 0;
                printf("\n[Info Serveur] : %s\n", receptionMessage());
                printf ("\nMerci d'avoir utilisé les applications FTP développées par Mr. LERAYER, Mr. MOUTARDE et Mme KRIMI !\n");
                finSocketLocal();
                break;


            default:
                printf("\nERREUR : Votre choix est incorrect !!!\n\n");
        }
    }
	printf ("Fin du client\n\n");
	return 0;
}

