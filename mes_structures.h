// mes_structures.h

#ifndef MES_STRUCTURES_H
#define MES_STRUCTURES_H

typedef struct
{
    float x, y, z; // Position dans l'espace
} Position;

typedef struct
{
    int endommage;        // 0 = OK, 1 = Endommagé
    int enMission;        // 0 = Pas en mission, 1 = En mission
    int droneMalveillant; // 0 = Normal, 1 = Intrus
} State;

typedef struct
{
    int id;
    Position pos;
    State etat;
    float vitesse;    // Vitesse de déplacement
    float resolution; // Résolution de photographie
} Drone;

typedef struct
{
    int largeur;
    int hauteur;
    char **carte; // Matrice représentant la carte
} Carte;

#endif
