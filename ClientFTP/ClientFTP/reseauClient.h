#ifndef RESEAU_CLIENT
#define RESEAU_CLIENT


/**************************************************************************************
    Nom fonction : receptionFichierBinaire (int socket_serveur)

    =====================================
    Description : Ouvre un nouveau socket pour la data grâce à newSocketData_listen().
        Puis reçoit un fichier binaire et le reconstruit.
        Puis ferme le socket de data créé pour le transfert.

**************************************************************************************/

int receptionFichierBinaire ();


/**************************************************************************************
    Nom fonction : emissionMessage()

    =====================================
    Description : Envoie le message passé en paramètre, par bloc dont la taille est
        définie par TAILLE_BLOC.
        Si l'(les) envoi(s) a(ont) réussi(s), la fonction renvoie SUCCESS.
        Si l'(les) envoi(s) a(ont) échoué(s), la fonction s'arrête par un exit().
        Si l'(les) envoi(s) a(ont) renvoie(nt) 0, la fonction renvoie FAILED.

**************************************************************************************/

int emissionMessage(char *message);


/**************************************************************************************
    Nom fonction : isPortValide(char *newPort)

    =====================================
    Description : Vérifie que le port indiqué est valide.
        S'il l'est, la VG port_temporaire prend la valeur du paramètre casté en int.
        S'il ne l'est pas, la VG port_temporaire est mise à 0.

**************************************************************************************/

int isPortValide(char *newPort);


/**************************************************************************************
    Nom fonction : receptionMessage()

    =====================================
    Description : Reçoit des données depuis le socket et les stocke dans un char *
        que la fonction renvoie. Tant qu'on a pas reçu le caractère de fin '\0',
        on continue de recevoir dans le char *, on écoute à nouveau le socket
        si besoin en continuant à remplir, et quand on a '\0', on le remplace
        par '\n' et la case suivante du tableau est remplie avec '\0'

**************************************************************************************/

char *receptionMessage();

#endif
