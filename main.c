#include <stdio.h>
#include <stdlib.h>

#include "mes_fonctions.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

//________________________________________________________________________________________________________________________________

int main()
{

    // Charger une image PNG avec stb_image
    int largeur, hauteur, comp;
    unsigned char *image = stbi_load("carte_ecole.png", &largeur, &hauteur, &comp, 0);
    if (!image)
    {
        printf("Erreur : Impossible de charger l'image.\n");
        return -1;
    }
    printf("Image chargée avec succès: Largeur = %d, Hauteur = %d, Composants = %d\n", largeur, hauteur, comp);
    //_______________________________________________________________________________________________________________________

    // Création d'un drone et initialisation de sa position, vitesse et statut
    Drone drone1;
    initDrone(&drone1, 1, 0.0, 0.0, 0.0, 1.0, 1); // Drone initialisé comme actif

    // Affichage de la position initiale et du statut
    afficherPosition(&drone1);

    // Tentative de déplacement du drone
    deplacerDrone(&drone1, 1.0, 1.0, 0.5); // Déplacement en (1, 1, 0.5)

    // Affichage après déplacement
    afficherPosition(&drone1);
    // Désactivation du drone
    changerStatus(&drone1, INACTIF);
    printf("\nDrone désactivé.\n");

    // Tentative de déplacement avec drone inactif
    deplacerDrone(&drone1, 0.5, -0.5, 1.0); // Déplacement en (0.5, -0.5, 1)

    // Chemin vers l'image PNG que tu veux afficher
    const char *chemin_image = "carte_ecole.png"; // Remplace par le chemin complet si nécessaire

    // Utiliser la commande 'start' pour ouvrir l'image avec l'application par défaut
    char commande[256];
    snprintf(commande, sizeof(commande), "start %s", chemin_image);
    system(commande);

    return 0;
}