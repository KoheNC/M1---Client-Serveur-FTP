#ifndef OUTILS_SERVEUR
#define OUTILS_SERVEUR

/************************************************************************************
    Description : Le champ instruction contient la commande à exécuter.
        Le champ parametre contient le paramètre associé à la commande si besoin.
        Le champ commande_FTP est à 1 s'il s'agit d'une commande FTP, sinon 0
        (ex : pour une commande système)
************************************************************************************/

typedef struct {
    char *instruction;
    char *parametre;
    int commande_FTP;
}Commande;


/************************************************************************************
    Nom fonction : my_strdup(const char* chaine)

    =====================================
    Description : Copie la structure 'Commande' entrée en paramètre en faisant
        un malloc et un memcpy, puis renvoie l'adresse de la nouvelle chaîne.
    =====================================
    Différence avec strdup : strdup() renvoie un pointeur sur une chaîne de caractères
        my_strdup renvoie un pointeur sur une structure 'Commande' !!!!
************************************************************************************/

Commande* my_strdup(const Commande* chaine);


/**************************************************************************************
    Nom fonction : tailleFichier(char *chemin_fichier)

    =====================================
    Description : Renvoie un int indiquant la taille du fichier passé en paramètre.

**************************************************************************************/

long tailleFichier (char *chemin_fichier);


/************************************************************************************
    Nom fonction : analyseCommande(char *donneeRecue)

    =====================================
    Description : Analyse la donnée reçue en paramètre.
        Renvoie une structure 'Commande' avec les champs
        remplis et alloués avec malloc()
************************************************************************************/

Commande * analyseCommande(char *donneeRecue);

///////////////////////////////////////////////////
void vider_buffer();

///************************************************************************************
//    Nom fonction : serveurShutdown()
//
//    =====================================
//    Description : Eteint proprement le serveur en fermant le socket d'écoute des
//        connexion entrantes pour les nouveaux clients.
//        Puis affiche un message de sortie.
//************************************************************************************/
//
//void serveurShutdown(int signal);

void authentification();

/* ------------------------------------------------------------------------
  Nom        : <remplacer>
  Creation   : Thierry Husson, Sept 2010
  But        : Cherche et remplace une sous-chaine par une autre
  Parametres :
        str     : Chaine de caractères dans laquelle on fouille
        oldstr  : Sous-chaine recherchée
        newstr  : Sous-chaine de remplacement
        count   : Pointeur vers un entier ou NULL si optionnel
          Entrée: Nombre de remplacements maximum à faire.
                  Si < 1 ou NULL, fait des remplacements illimités
          Sortie: Nombre de remplacements faits ou -1 si erreur
  Retour     : Pointeur vers une nouvelle chaine de caractères
               avec les remplacements faits ou NULL si erreur
  Remarques  :
     - Tient compte des majuscules/minuscules (case sensitive)
       Autrement, remplacez les fonctions "strstr" par "strcasestr"
     - Fait systématiquement une copie de la chaine en entrée
       comme résultat de la fonction (pointeur sur nouvelle chaine)
------------------------------------------------------------------------ */

char* remplacer_str(const char *str, const char *oldstr, const char *newstr, int *count);

#endif
