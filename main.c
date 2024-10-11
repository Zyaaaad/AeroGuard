#include <stdio.h>
#include <stdlib.h>
#include "mes_structures.h"
#include "mes_signatures.h"

int main()
{
    Drone drones[10]; // Tableau pour un maximum de 10 drones
    Carte carte;
    int nombreDrones = 0;

    // Initialiser la carte avec une image (par exemple, une carte vierge noire)
    initCarte(&carte, "./Cartes/CarteParis.png");

    // Lire le fichier de scénario
    lireScenario("scenario.txt", drones, &nombreDrones, &carte);

    // Sauvegarder l'état final de la carte révélée
    sauvegarderCarte(&carte, "carte_revelee.png");

    // Libérer la mémoire de la carte
    libererCarte(&carte);

    return 0;
}
