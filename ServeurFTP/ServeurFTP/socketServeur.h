#ifndef SOCKET_SERVEUR
#define SOCKET_SERVEUR

/**************************************************************************************
    Nom fonction : finSocketClient_data()

    =====================================
    Description : Ferme le socket de data vers le client.
        Remet l'int du descripteur à 0.

**************************************************************************************/

void finSocketClient_data();


/**************************************************************************************
    Nom fonction : finSocketClient()

    =====================================
    Description : Ferme le socket de commande avec le client.
        Remet l'int du descripteur à 0.
        Remet le port de data par défaut à 1025.

**************************************************************************************/

void finSocketClient();


/**************************************************************************************
    Nom fonction : finSocketLocal()

    =====================================
    Description : Ferme le socket local sur lequel on écoute.
        Remet l'int du descripteur à 0.

**************************************************************************************/

void finSocketLocal();


/**************************************************************************************
    Nom fonction : newSocketData()

    =====================================
    Description : Crée un nouveau socket et se connecte à l'adresse du client avec le
        port indiqué en paramètre. Si le paramètre est NULL, un port par défaut est
        indiqué.

**************************************************************************************/

void newSocketData();


/**************************************************************************************
    Nom fonction : attenteClient()

    =====================================
    Description : Attend l'arrivée d'un nouveau client, puis stocke le descripteur
        de socket dans sockClient après que sa connexion ait été acceptée.

**************************************************************************************/

void attenteClient();


/**************************************************************************************
    Nom fonction : initialisation_serveur()

    =====================================
    Description : Crée une socket et la bind à l'adresse locale du serveur et le port
        entrés en paramètres. Puis écoute sur le socket créé.

**************************************************************************************/

void initialisation_serveur();


/**************************************************************************************
    Nom fonction : infoNewClient(int sock_client)

    =====================================
    Description : Affiche l'IP et le port du client passé en paramètre

**************************************************************************************/

void infoNewClient(int sock_client);

#endif
