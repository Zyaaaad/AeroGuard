#include <stdio.h>
#include <stdlib.h>

#include "mes_signatures.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//________________________________________________________________________________________________________________________________

int main()
{
    // Charger une image PNG avec stb_image
    int largeur, hauteur, comp;
    unsigned char *image = stbi_load("./Cartes/CarteParis.png", &largeur, &hauteur, &comp, 0);
    if (!image)
    {
        printf("Erreur : Impossible de charger l'image.\n");
        return -1;
    }
    printf("Image chargée avec succès: Largeur = %d, Hauteur = %d, Composants = %d\n", largeur, hauteur, comp);
    //_______________________________________________________________________________________________________________________

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
    

    // Tentative de déplacement avec drone inactif
    deplacerDrone(&drone1, 0.5, -0.5, 1.0); // Déplacement en (0.5, -0.5, 1)

    // Chemin vers l'image PNG que tu veux afficher
    //const char *chemin_image = "carte_ecole.png"; // Remplace par le chemin complet si nécessaire

    // Utiliser la commande 'start' pour ouvrir l'image avec l'application par défaut
    //char commande[256];
    //snprintf(commande, sizeof(commande), "start %s", chemin_image);
    //system(commande);*/

    return 0;
}