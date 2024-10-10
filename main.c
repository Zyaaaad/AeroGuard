// main.c

#include <stdio.h>
#include <stdlib.h>
#include "mes_structures.h"
#include "mes_signatures.h"

#define MAX_DRONES 100 // Limite maximale de drones

int main()
{
    Drone drones[MAX_DRONES]; // Tableau pour les drones
    int nombreDrones = 0;     // Compteur de drones initialisés
    Carte carte;

    // Initialisation de la carte avec une largeur et hauteur (ex: 30x30)
    initCarte(&carte, 30, 30);

    printf("Lecture du scénario...\n");
    lireScenario("scenario.txt", drones, &nombreDrones);

    // Afficher la carte après l'exécution
    afficherCarte(&carte);

    // Libérer la mémoire allouée pour la carte
    for (int i = 0; i < carte.hauteur; i++)
    {
        free(carte.carte[i]);
    }
    free(carte.carte);

    return 0;
}
