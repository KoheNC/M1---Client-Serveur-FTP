#ifndef FONCTION_RESEAU
#define FONCTION_RESEAU

/**************************************************************************************
    Nom fonction : tailleFichier(char *chemin_fichier)

    =====================================
    Description : Renvoie un int indiquant la taille
        du fichier passé en paramètre

**************************************************************************************/


long tailleFichier (char *chemin_fichier);

/**************************************************************************************
    Nom fonction : vider_buffer()

    =====================================
    Description : Vide le buffer pour éviter
        les saisies erronnées

**************************************************************************************/

void vider_buffer();

/**************************************************************************************
    Nom fonction : lire(char *chaine, int longueur)

    =====================================
    Description : Permet de saisir une chaîne au clavier, AVEC UN NOMBRE MAXIMUM DE
        CARACTERES, définit par la taille de l'argument 'chaîne', puis remplace le '\n'
        saisi avec fgets() par un '\0'
**************************************************************************************/

int lire(char *chaine, int longueur);


/**************************************************************************************
    Nom fonction : lireParametre()

    =====================================
    Description : Saisit une chaîne au clavier, de taille INDEFINIE.Puis remplace
        le 1er espace trouvé par un caractère de fin \0.
        Puis la fonction renvoie la chaîne.
    =====================================
    IMPORTANT : Comme la fonction remplace le 1er espace trouvé par un \0, si l'argument
        entré par l'utilisateur contient un ou des espace(s), il(s) est(sont) supprimé(s).
        Dans le cas où l'argument est un chemin absolu, on suppose qu'il NE CONTIENT PAS d'ESPACE.

**************************************************************************************/

char* lireParametre();


/**************************************************************************************
    Nom fonction : preparation_Commande(char *instruction, char *message)

    =====================================
    Description : Prépare la requête devant être envoyée au serveur par le canal de
        commande. L'argument @instruction entré est le début de la requête.
        - L'utilisateur saisi l'argument si besoin
        - Les caractères CRLF (Carrier Return Line Feed --> \r\n) sont insérés à la fin
            de la chaîne. Puis le caractère \0 est inséré pour finir la chaîne.
        - Des traitements intermédiaires sont effectués si nécessaire
        - Puis l'instruction est envoyé si le flag envoiAutorise le permet.

**************************************************************************************/

void preparation_Commande(char *instruction);


/**************************************************************************************
    Nom fonction : lireSTDIN()

    =====================================
    Description : Récupère les caractères contenus dans le buffer STDIN, puis les
        insère dans un buffer, qui est automatiquement réalloué pour chaque caractère
        supplémentaire trouvé dans STDIN.
        Quand le caractère \n est trouvé, il est remplacé par \0.
        Le buffer est toujours réalloué avec le nombre de caractères trouvés
            + 2. On garde +2 pour pouvoir insérer le CRLF.
        La fonction renvoie un pointeur vers le buffer contenant toute la chaîne.
    =====================================
    IMPORTANT : On suppose que l'argument saisi par l'utilisateur NE PEUT PAS contenir d'ESPACE.
        Dans le cas où l'argument est un chemin absolu, on suppose qu'il NE CONTIENT PAS d'ESPACE.

**************************************************************************************/

char *lireSTDIN();


/**************************************************************************************
    Nom fonction : formatNomUser(char *nomUser)

    =====================================
    Description : Analyse la chaîne fournie en argument et si un espace est trouvé, il
        est alors remplacé par un caractère de fin \0

**************************************************************************************/

char *formatNomUser(char *nomUser);


/**************************************************************************************
    Nom fonction : connectionUserName()

    =====================================
    Description : Demande à l'utilisateur de saisir son login, qui est alors envoyé au
        serveur. Le serveur peut répondre par un 220 et alors on affiche le menu. Il
        peut également répondre par un 331/332 et l'utilisateur doit alors saisir son
        mot de passe. Enfin le serveur peut indiquer que le couple login/mdp est
        incorrect et alors on redemande le login à l'utilisateur

**************************************************************************************/

void connectionUserName();


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
