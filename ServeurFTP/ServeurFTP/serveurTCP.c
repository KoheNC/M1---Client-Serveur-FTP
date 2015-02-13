#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include "fonctionReseau.h"
#include "outils.h"
#include "socketServeur.h"

#define TRUE 1
#define FALSE 0
#define BLOCK 4096
#define SUCCESS 1
#define FAILED 0

#define PORT_RESERVE 10
#define PORT_DYNAMIQUE 11
#define PORT_INVALIDE 12
#define WRONG_ARG 50

int run_serveur = 1;
Commande *commandeRecue; //Reçoit la structure commande après avoir été analysé et découpé
extern long startValue;

///A AMELIORER, APRES INTERRUPTION, LE SERVEUR REPREND SON ACTIVITE (A CAUSE DE SA_RESTART ?)
/// PEUT-ETRE LE ACCEPT NE GERE PAS LES ERREURS, A VOIR !

/********************
      serveurShutdown
********************/
void serveurShutdown(int signal)
{
    if(signal == SIGINT){
        run_serveur = 0;
        finSocketLocal(); // Fermeture du socket local d'écoute
        printf("Serveur tué, au revoir !\n\n");
    }
}


/********************
      Main
********************/
int main ()
{
    int run_client = 1;
    int result = 0;
    char *donneeRecue;

    /// ARMEMENT DE LA COMMANDE CTRL+C POUR FERMER PROPREMENT LE SERVEUR SI ON A CETTE INTERRUPTION
//    struct sigaction CtrlC, oldAction;
//
//    CtrlC.sa_handler = serveurShutdown;
//    sigemptyset(&CtrlC.sa_mask);
//    CtrlC.sa_flags = SA_RESTART;
//    sigaction(SIGINT, &CtrlC, &oldAction);

    // Création du socket et bind local
    initialisation_serveur();

    while(run_serveur == 1){
        attenteClient();
        run_client = TRUE;

        /* Les USER acceptés sont inscrits en dur dans la fonction authentification() */
        authentification();

        while(run_client == TRUE){
            printf("\nAttente d'un message de la part du client\n");

            // Réception et analyse de la commande
            donneeRecue = receptionMessage();
            commandeRecue = analyseCommande(donneeRecue);

            if(strcmp(commandeRecue->instruction,"PORT") == 0)
            {
                        /// IL FAUT EMETTRE ICI UN MESSAGE !!!!!!! ET CONF LE CLIENT POUR QU'IL LE RECOIVE OU BIEN DANS LE MAIN !
                result = changeDataPort(commandeRecue->parametre);

                if(result == WRONG_ARG) {
                    emissionMessage("501 - Les arguments entrés pour le changement de port sont incorrects. Abandon...\n");
                }

                else if(result != SUCCESS){
                    emissionMessage("501 - Port invalide. Abandon...\n");
                    printf("L'argument entré pour changer le port est incorrect");
                    printf("Les ports valides vont de 1024 à 49151 inclus\n");
                }
                else{
                    emissionMessage("200 - Changement de port data accepté !");
                }
            }
            else if(strcmp(commandeRecue->instruction,"RETR") == 0)
            {
                emissionFichierBinaire();
            }
            else if(strcmp(commandeRecue->instruction,"STOR") == 0)
            {
                emissionMessage("502 - Commande STOR non implémentée. (aucune action entreprise)\n");
            }
            else if(strcmp(commandeRecue->instruction,"NOOP") == 0)
            {
                emissionMessage("200 - Commande NOOP OK !\n");
            }
            else if(strcmp(commandeRecue->instruction,"TYPE") == 0)
            {
                emissionMessage("502 - Commande TYPE non implémentée. (aucune action entreprise)");
            }
            else if(strcmp(commandeRecue->instruction,"MODE") == 0)
            {
                emissionMessage("502 - Commande MODE non implémentée. (aucune action entreprise)");
            }
            else if(strcmp(commandeRecue->instruction,"STRU") == 0)
            {
                emissionMessage("502 - Commande STRU non implémentée. (aucune action entreprise)");
            }
            else if(strcmp(commandeRecue->instruction,"REST") == 0)
            {
                startValue = strtol(commandeRecue->parametre,NULL,10);
                emissionMessage("350 - Position de reprise acceptée !\n");
            }
            else if(strcmp(commandeRecue->instruction,"QUIT") == 0)
            {
                emissionMessage("221 - Déconnexion imminente\n");
                finSocketClient();
                printf ("Le client s'est déconnecté \n");
                run_client = FALSE;
            }
            else
            {
                emissionMessage("500 - Commande non reconnue (aucune action entreprise)...\n");
            }

            //free(donneeRecue);
            //free(commandeRecue);
        }
    }
	return SUCCESS;
}
