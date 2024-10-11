#ifndef MES_STRUCTURES_H
#define MES_STRUCTURES_H

// Structure pour la position d'un drone
typedef struct
{
    float x, y, z;
} Position;

// Structure pour la carte
typedef struct
{
    int largeur;
    int hauteur;
    unsigned char *image;    // Image originale chargée
    unsigned char *revealed; // Carte révélée au fur et à mesure
} Carte;

// Structure pour le drone
typedef struct
{
    int id;
    Position pos;  // Position du drone
    float vitesse; // Vitesse du drone
    int endommage; // 0 = OK, 1 = Endommagé
    int enMission; // 0 = Pas en mission, 1 = En mission
} Drone;

#endif
