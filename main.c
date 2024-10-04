#include <stdio.h>
#include <stdlib.h>

#include "mes_signatures.h"

//________________________________________________________________________________________________________________________________

int main()
{
    // Charger la carte
    int largeur, hauteur, comp;
    load_image("./Cartes/CarteParis.png", &largeur, &hauteur, &comp);
    // Création d'un drone et initialisation de sa position, vitesse et statut
    Drone drone1;
    // Drone initialisé comme actif
    initDrone(&drone1, 1, 0.0, 0.0, 0.0, 1.0, ACTIF); 

    // Affichage du statut
    afficherStatut(&drone1);
    // Affichage de la position initiale
    afficherPosition(&drone1);

    // Tentative de déplacement du drone
    deplacerDrone(&drone1, 1.0, 1.0, 0.5); // Déplacement en (1, 1, 0.5)
    afficherPosition(&drone1);

    // Désactivation du drone
    changerStatus(&drone1, INACTIF);
    deplacerDrone(&drone1, 0.5, -0.5, 1.0); // Déplacement en (0.5, -0.5, 1)



    // Chemin vers l'image PNG que tu veux afficher
    //const char *chemin_image = "carte_ecole.png"; // Remplace par le chemin complet si nécessaire

    // Utiliser la commande 'start' pour ouvrir l'image avec l'application par défaut
    //char commande[256];
    //snprintf(commande, sizeof(commande), "start %s", chemin_image);
    //system(commande);*/

    return 0;
}