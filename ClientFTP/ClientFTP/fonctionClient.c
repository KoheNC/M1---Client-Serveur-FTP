#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "reseauClient.h"
#define SUCCESS 1
#define FAILED 0

extern unsigned int port_data;
extern long startValue;     //position du curseur pour un fichier en DL/UP
extern int sock_local;      //utilisé dans la commande PORT
char *parametre;            // Nom du fichier à envoyer / recevoir


/****************************
       realloc_propre
****************************/
/* On ne déclare par les fonctions STATIC dans le header !!!!! */
 static char* realloc_propre(char** buff, size_t* size)
{
//    size_t new_size = (size_t)(*size * 1.5);
    size_t new_size = (size_t)(*size + 1);
    char*  tmp      = realloc(*buff, new_size);

    if(tmp != NULL)
    {
        *buff = tmp;
        *size = new_size;
    }
    else
    {
        fprintf(stderr, "error realloc failed: %s\n", strerror(ENOMEM));
        free(*buff);
        *buff = NULL;
    }

    return *buff;
}


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


/****************************
       formaterPort
****************************/
void formaterPort()
{
    int port = 0;
    int poidsFort = 0;
    int poidsFaible = 0;
    size_t nombre = 0;
    char poidsFort_chaine[4] = {0};
    char poidsFaible_chaine[4] = {0};
    char ip[INET_ADDRSTRLEN];

    struct sockaddr_in adresse_client;
    socklen_t peer_longueur;

    //Il faut pré-calculer la taille de la structure avant d'utiliser la fonction getpeername()
    peer_longueur = sizeof(struct sockaddr_in);

    if(getpeername(sock_local, (struct sockaddr*) &adresse_client, &peer_longueur)!=0){
        perror("Erreur lors de la récupération des infos client....getpeername failed \n");
        exit(EXIT_FAILURE);
    }

    inet_ntop(AF_INET,&(adresse_client.sin_addr),ip,INET_ADDRSTRLEN);

    port = atoi(parametre);
    poidsFort = port/256;
    poidsFaible = port - (poidsFort*256);

    sprintf(poidsFort_chaine,"%i",poidsFort);
    sprintf(poidsFaible_chaine,"%i",poidsFaible);

    nombre = INET_ADDRSTRLEN + strlen(poidsFort_chaine) + strlen(poidsFaible_chaine) +3;
    parametre = realloc_propre(&parametre,&nombre);

    strcpy(parametre,ip);
    strcat(parametre,",");
    strcat(parametre,poidsFort_chaine);
    strcat(parametre,",");
    strcat(parametre,poidsFaible_chaine);
    remplacer_str(parametre,".",",",0);

    printf("le parametre est=%s\n",parametre);
}


/******************
   tailleFichier
******************/
long tailleFichier (char *chemin_fichier)
{
    FILE *fichier;
    long taille =0;

    fichier = fopen (chemin_fichier, "ab");
    if (fichier)
    {
        fseek (fichier, 0, SEEK_END);
        taille = ftell (fichier);
        fclose (fichier);
    }

    return taille;
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
       lire
******************/
int lire(char *chaine, int longueur)
{
    char *positionEntree = NULL;

    fputs("\nFTP>",stdout);
    fflush(stdout);

    if (fgets(chaine, longueur, stdin) != NULL)
        {
        positionEntree = strchr(chaine, '\n');
        if (positionEntree != NULL)
            *positionEntree = '\0';
        else
            vider_buffer();
        return 1;
        }
    else
        {
        vider_buffer();
        printf("Erreur de saisie du message \n");
        return 0;
        }
}


/******************
       lireSTDIN
******************/
char *lireSTDIN()
{
  int index, carac;
//  char *temp = NULL;
  char *temp = malloc(sizeof(char));
  char *buffer = malloc(sizeof(char));
  size_t nombre = 0;

  if(buffer) {
    for(index = 0; ; index++) {

      if((carac = getchar()) == '\n'){
      //if(((carac = getchar()) == '\n') || (carac == ' ')){
            break;
      }

      if(isprint(carac)){
        buffer[index] = carac;

         /* +3 car on pense au \0 et au CRLF qui doit être implémenté pour respecter la RFC FTP */
//        if( (temp = realloc(buffer, (index +3) * sizeof(char))) ){
            nombre = index+3;
            temp = realloc_propre(&buffer,&nombre);
            buffer = temp;
//        }else {
//          free(temp);
//          free(buffer);
//          return 0;
//        }
      }
    }
    buffer[index] = 0;
  }
  return buffer;
}


/******************
       lireParametre
******************/
char * lireParametre()
{
    char *parametre;
    char *positionEntree;

    fflush(stdout);
    vider_buffer();

    fputs("\nFTP>",stdout);
    parametre = lireSTDIN();

    if(parametre != NULL){
        positionEntree = strchr(parametre, ' ');

        /* Si on trouve l'espace, on le remplace par \0 pour former l'instruction ou le parametre */
        if (positionEntree != NULL){
            *positionEntree = '\0';
        } else {
            positionEntree = strchr(parametre, '\0');
            if (positionEntree != NULL){
                *positionEntree = '\0';
            } else {
                vider_buffer();
            }
        }
        return parametre;
    } else {
        perror("Erreur de saisie du message :");
        vider_buffer();
        return (char*)FAILED;
    }
}


/****************************
       formatNomUser
****************************/
char *formatNomUser(char *nomUser)
{
    char *positionEntree = strchr(nomUser, ' ');
    if(positionEntree != NULL){
        *positionEntree = '\0';
        printf("OK formatNomUser\n");
    }
    return nomUser;
}


/****************************
       preparation_Commande
****************************/
void preparation_Commande(char *instruction)
{
    int envoiAutorise = 1;
    int isREST = 0;
    int fd;
    long taille = 0;
    size_t nombre;
    char *positionEntree;
    char *paramTemp;
//    char *instruction_copiee = calloc((strlen(instruction)+2),sizeof(instruction_copiee));
    char *instruction_copiee = calloc((strlen(instruction)+5),sizeof(instruction_copiee));
    // +5 pour le long utilisé dans REST

    strcpy(instruction_copiee,instruction);
    /* Si l'instruction reçu n'est pas QUIT ou NOOP, on demande le paramètre à l'utilisateur
     * -->parametre contiendra un pointeur sur une zone mémoire allouée et contenant le parametre
     * Si l'instruction reçue est QUIT ou NOOP, on alloue une zone mémoire de taille 3 pour
     * parametre, pour pouvoir placer le CRLF en plus du \0
     */
    if((strcmp(instruction,"QUIT") != 0) && (strcmp(instruction,"NOOP") != 0)){
        parametre = lireParametre();
    }

    if(strcmp(instruction,"PORT") == 0){
        envoiAutorise = isPortValide(parametre);
        if(envoiAutorise == 1) {
            formaterPort();
        }
    }

    if(strcmp(instruction,"USER") == 0){
        parametre = formatNomUser(parametre);
    }

    /* On vérifie que le fichier existe ou non pour savoir si on envoie une commande RETR ou REST */
    if(strcmp(instruction,"RETR") == 0){

        if ((fd = open(parametre,O_RDONLY)) != -1) {
            close(fd);
            taille = tailleFichier(parametre);
            printf("\n[Info Client] : Taille du fichier local=%ld\n",taille);

            instruction_copiee = remplacer_str(instruction_copiee,"RETR","REST",0);
            /* Remplace la valeur du paramètre par la taille du fichier en local */
            startValue = taille; // Modification de la position du curseur pour un fichier en DL/UP
            paramTemp = calloc(strlen(parametre)+3,sizeof(paramTemp));
            strcpy(paramTemp,parametre);
            sprintf(parametre,"%ld",taille);

            printf("\n[Info Client] : Le fichier existe en partie ou en entier...\n\n");
        }
    }

    do{
        //on rajoute un espace avant le \0 de la 1ere chaine instruction
        positionEntree = strchr(instruction_copiee, '\0');

        if (positionEntree != NULL){
            *positionEntree = ' ';
            *((positionEntree)+1) = '\0';
        } else {
            perror("Erreur, pas de caractère \0 dans l'instruction !\n");
        }

        // Dans le paramètre, on décale le \0 de fin de 2 cases pour pouvoir insérer le CRLF juste avant, puis on concatène
        positionEntree = strchr(parametre, '\0');
        *positionEntree = '\r';
        *((positionEntree)+1) = '\n';
        *((positionEntree)+2) = '\0';

        /* instruction_copiee va recevoir la commande complète par concaténation puis être envoyée
         * Il faut réallouer instruction_copiee à la bonne taille du coup sinon buffer Overflow */
        nombre = strlen(instruction_copiee)+strlen(parametre)+1;
        instruction_copiee = realloc_propre(&instruction_copiee,&nombre);
        strcat(instruction_copiee,parametre);

        /* Envoie de la commmande à l'hôte distant. Si la commande contient l'instruction REST, on le remplace par RETR et on réenvoie */
        if(envoiAutorise == 1){
            emissionMessage(instruction_copiee);
            isREST = 0;

            if(strstr(instruction_copiee,"REST")!= NULL){
                printf("[Info Serveur] : %s",receptionMessage());
                /* On réalloue proprement notre intruction copiée pour qu'elle puisse recevoir la commande */
                instruction_copiee = calloc(7,sizeof(instruction_copiee));
                strcpy(instruction_copiee,"RETR");
                parametre = paramTemp;
                isREST = 1;
            }
        }else{
            printf("Les ports valides vont de 1024 à 49151 inclus\n");
            printf("Retour au menu...\n\n");
        }

    } while (isREST == 1);
}


/****************************
       connectionUserName
****************************/
void connectionUserName()
{
    int isUserOK = 0;
    char *donneeRecue;
        printf("Veuillez saisir votre nom d'utilisateur :\n");
        preparation_Commande("USER");

    do {
        fflush(stdin);

        donneeRecue = receptionMessage();

        printf("\n[Info Serveur] : %s\n\n",donneeRecue);
        if(strstr(donneeRecue,"230") != NULL){
            isUserOK = 1;
        }
        else if((strstr(donneeRecue,"331") != NULL) || (strstr(donneeRecue,"332") != NULL) ){
            printf("Veuillez saisir votre mot de passe :\n");
            preparation_Commande("PASS");
        }
        else{
            printf("Veuillez saisir votre nom d'utilisateur :\n");
            preparation_Commande("USER");
        }
    }while(isUserOK == 0);
}
