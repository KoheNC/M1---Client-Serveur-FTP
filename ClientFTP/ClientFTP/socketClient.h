#ifndef SOCKET_CLIENT
#define SOCKET_CLIENT

/**************************************************************************************
    Nom fonction : finSocketData()

    =====================================
    Description : Ferme le socket de data vers le serveur.
        Remet l'int du descripteur à 0.
        Fermé également le socket d'écoute data sockClient_listen.
        Remet l'int du decripteur à 0.

**************************************************************************************/

void finSocketData();


/**************************************************************************************
    Nom fonction : finSocketLocal()

    =====================================
    Description : Ferme le socket local sur lequel on écoute.
        Remet l'int du descripteur à 0.

**************************************************************************************/

void finSocketLocal();


///**************************************************************************************
//    Nom fonction : finSocketClient()
//
//    =====================================
//    Description : Ferme le socket de commande avec le client.
//        Remet l'int du descripteur à 0.
//
//**************************************************************************************/
//
//void finSocketClient();


/**************************************************************************************
    Nom fonction : initialisationClient()

    =====================================
    Description : Initialise le client en créant un socket
        et en se connectant au serveur.

**************************************************************************************/


void initialisation_client();

/**************************************************************************************
    Nom fonction : newSocketData_listen()

    =====================================
    Description : Ouvre un socket en écoute, pour recevoir la connexion du serveur.
        A la connexion du serveur, le descripteur du socket de data est initialisé.

**************************************************************************************/

void newSocketData_listen();

#endif
