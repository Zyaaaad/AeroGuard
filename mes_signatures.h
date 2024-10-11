#ifndef MES_SIGNATURES_H
#define MES_SIGNATURES_H

#include "mes_structures.h"

// Initialisation d'un drone
void initDrone(Drone *d, int id, float x, float y, float z, float vitesse);

// Initialisation de la carte
void initCarte(Carte *carte, const char *imagePath);

// Déplacement d'un drone
void deplacerDrone(Drone *d, float dx, float dy, float dz, Carte *carte);

// Prise de photo par un drone
void photographier(Drone *d, Carte *carte);

// Détection de drones malveillants
void detecterDronesMalveillants(Drone *drones, int nombreDrones);

// Sauvegarde de la carte
void sauvegarderCarte(Carte *carte, const char *outputPath);

// Libération de la mémoire allouée pour la carte
void libererCarte(Carte *carte);

void lireScenario(char *fichierScenario, Drone *drones, int *nombreDrones, Carte *carte);

// Initialisation
#endif