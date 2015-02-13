#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "fonctionReseau.h"
#include "outils.h"

#define TRUE 1
#define FALSE 0

extern Commande *commandeRecue; //Reçoit la structure commande après avoir été analysé et découpé

/****************************
       remplacer_str
****************************/
char* remplacer_str(const char *str, const char *oldstr, const char *newstr, int *count)
{
   const char *tmp = str;
   char *result;
   int   found = 0;
   int   length, reslen;
   int   oldlen = strlen(oldstr);
   int   newlen = strlen(newstr);
   int   limit = (count != NULL && *count > 0) ? *count : -1;

   /* Compte le nombre de fois que la chaine originale est trouvée */
   while ((tmp = strstr(tmp, oldstr)) != NULL && found != limit)
      found++, tmp += oldlen; /* Reprend après la chaine trouvée */

   /* Calcule l'espace mémoire nécessaire pour la nouvelle chaine */
   length = strlen(str) + found * (newlen - oldlen);
   if ( (result = (char *)malloc(length+2)) == NULL) {
      fprintf(stderr, "Not enough memory\n");
      found = -1;
   } else {
      tmp = str;
      limit = found; /* Compteur à rebours */
      reslen = 0;  /* longueur en cours du résultat */
      /* Pour chaque sous-chaine trouvée, on met celle de remplacement */
      while ((limit-- > 0) && (tmp = strstr(tmp, oldstr)) != NULL) {
         length = (tmp - str); /* Nombre de caractères inchangés */
         strncpy(result + reslen, str, length); /* Copie partie inchangée */
         strcpy(result + (reslen += length), newstr); /* Ajoute newstr */
         reslen += newlen;
         tmp += oldlen;
         str = tmp;
      }
      strcpy(result + reslen, str); /* Ajoute fin de la chaine inchangée */
   }
   if (count != NULL) *count = found;
   return result;
}


/******************
   tailleFichier
******************/
long tailleFichier (char *chemin_fichier)
{
    FILE *fichier;
    long taille =0;

    fichier = fopen (chemin_fichier, "rb");
    if (fichier){
        fseek (fichier, 0, SEEK_END);
        taille = ftell (fichier);
        fclose (fichier);
    }

    return taille;
}


/********************
      my_strdup
********************/
/* N'est plus utilisé */
/* En plus c'est pas bon */
Commande* my_strdup(const Commande* chaine)
{
    size_t longueur;
    Commande *retour = NULL;

    if(chaine)
    {
        longueur = sizeof(chaine);
        if (!(retour = malloc(longueur)))
            return(NULL);
        if (retour)
            memcpy(retour,chaine,longueur);
        else
            retour = NULL;
    }
    return retour;
}


/********************
   analyseCommande
********************/
Commande * analyseCommande( char *donneeRecue)
{
    Commande *cmd;
    Commande *cmdRetour;
    char *donneeAnalysee;
    char *position;
    int index = 0;

    donneeAnalysee = donneeRecue;
    cmd = (Commande *)malloc(sizeof(Commande));
    cmd->commande_FTP = (int)malloc(sizeof(int));
    cmd->commande_FTP = 1;

    while(donneeAnalysee[index] != ' ')
        index++;

    cmd->instruction = malloc(sizeof(cmd->instruction)*index);
    strncpy(cmd->instruction,donneeRecue,index);
    *((cmd->instruction)+index) = '\0';

    cmd->parametre = malloc(sizeof(cmd->parametre)*((strlen(donneeAnalysee)-index+1)) );

    strcpy(cmd->parametre,&donneeAnalysee[index+1]);
    position = strchr(cmd->parametre, '\r');
    *position = '\0';

    if((strcmp(cmd->instruction,"ls") == 0) ||
       (strcmp(cmd->instruction,"pwd") == 0) ||
       (strcmp(cmd->instruction,"cd") == 0) ||
       (strcmp(cmd->instruction,"help") == 0))
       {
        cmd->commande_FTP = 0;
        }else{
            cmd->commande_FTP = 1;
        }
    // On réalloue à la bonne taille = taille de l'instruction + taille du paramètre
    // + taille de commande_FTP (2 octets car c'est un INT) + 2 pour les \0 de instruction et paramètre
    cmdRetour = realloc((Commande *)cmd,strlen(cmd->instruction)+strlen(cmd->parametre)+4);

    return cmdRetour;
}


/******************
    vider_buffer
******************/
void vider_buffer()
{
    int c = 0;
    while (c != '\n' && c != EOF)
        c = getchar();
}


/******************
    authentification
******************/
/* Note: A optimiser, ça serait bien de lire dans un tableau contenant les login/pwd pour simplifier le traitement */
void authentification()
{
    char *nomUser1 = "ADRIEN";
    char *nomUser2 = "KEVIN";
    char *passUser2 = "KEVIN";
    int isUserOK = 0;
    char *donneeRecue;

    do {
        donneeRecue = receptionMessage();
        commandeRecue = analyseCommande(donneeRecue);

        if(strcmp(commandeRecue->parametre,nomUser1) == 0){
            printf("OK, utilisateur authentifié...\n");
            isUserOK = 1;
            emissionMessage("230 - Session ouverte....");

        }else if(strcmp(commandeRecue->parametre,nomUser2) == 0) {
            printf("Connexion d'un utilisateur nécessitant un mot de passe \n");
            emissionMessage("331 - Mot de passe demandé....");
            donneeRecue = receptionMessage();
            commandeRecue = analyseCommande(donneeRecue);

            if(strcmp(commandeRecue->parametre,passUser2) == 0) {
                printf("OK, utilisateur authentifié...\n");
                isUserOK = 1;
                emissionMessage("230 - Session ouverte....");
            }else{
                emissionMessage("530 - Echec de l'ouverture de session, mot de passe erroné....");
            }

        }else{
            emissionMessage("530 - Echec de l'ouverture de session, utilisateur non reconnu....");
        }
    }while(isUserOK != 1);
}
