#ifndef MES_STRUCTURES_H
#define MES_STRUCTURES_H

#define MAX_DRONES 100
typedef struct
{
    float x, y, z; // Coordonnées
    float v;       // Vitesse
    int id;
    int leader;                // Indicateur de leader
    int zoom_level;            // Niveau de zoom correspondant à l'altitude
    unsigned char *image_data; // Données d'image
    int img_width, img_height, img_channels;
    int destroyed; // Indicateur de destruction du drone
    int active;    // Statut du drone : actif ou détruit
} Drone;

typedef struct
{
    Drone drones[MAX_DRONES];
    int num_drones;
} Fleet;

typedef struct
{
    float min_lat;
    float max_lat;
    float min_lon;
    float max_lon;
} Area;

#endif // MES_STRUCTURES_H
