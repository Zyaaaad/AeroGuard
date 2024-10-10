// mes_signatures.h

#ifndef MES_SIGNATURES_H
#define MES_SIGNATURES_H

#include "mes_structures.h"

// Initialisation d'un drone
void initDrone(Drone *d, int id, float x, float y, float z, float vitesse);

// Déplacement d'un drone
void deplacerDrone(Drone *d, float dx, float dy, float dz);

// Prise de photo par un drone
void photographier(Drone *d, Carte *carte);

// Détection de drones malveillants
void detecterDronesMalveillants(Drone *drones, int nombreDrones);

// Analyser la zone et photographier
void analyserZone(Drone *drones, int nombreDrones, Carte *carte);

// Chargement de la carte depuis un fichier
void chargerCarte(Carte *carte, char *fichierCarte);

// Initialisation de la carte
void initCarte(Carte *carte, int largeur, int hauteur);

// Affichage de la carte
void afficherCarte(Carte *carte);

// Lecture du scénario depuis un fichier
void lireScenario(char *fichierScenario, Drone *drones, int *nombreDrones);

#endif
