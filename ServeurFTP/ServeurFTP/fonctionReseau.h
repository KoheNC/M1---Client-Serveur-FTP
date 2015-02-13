#ifndef FONCTION_RESEAU
#define FONCTION_RESEAU

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


/**************************************************************************************
    Nom fonction : emissionFichierBinaire()

    =====================================
    Description : Ouvre un nouveau socket pour la transmission de la data avec
        newSocketData(char *port_data).
        Transmet le fichier en mode binaire.
        Puis ferme le socket de data.

**************************************************************************************/

int emissionFichierBinaire();


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
    Nom fonction : changeDataPort(char *newPort)

    =====================================
    Description : Change le port utilisé pour la data --> port_data
        Avec l'argument indiqué en paramètre.
        Conversion du char en int, puis vérifie que le port indiqué est valide.

**************************************************************************************/

int changeDataPort(char *newPort);

#endif
