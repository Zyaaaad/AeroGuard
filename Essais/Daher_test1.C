#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stb_image.h"

#define MAX_DRONES 10
#define ZOOM_LEVELS 3
#define TILE_SIZE 256  // Taille d'une tuile en pixels

typedef struct {
    float x, y, z; // Position du drone
    float v;       // Vitesse du drone
    int id;        // Identifiant du drone
    int leader;    // Indique si c'est le leader
} Drone;

typedef enum { HIGH, MEDIUM, LOW } Altitude; // Altitudes (1000m, 500m, 300m)

typedef struct {
    Drone drones[MAX_DRONES];
    int num_drones;
    char* map_tiles[ZOOM_LEVELS]; // Chemins vers les tuiles de carte
} Fleet;

// Charge une image à partir d'un fichier
unsigned char* load_image(const char* filename, int* width, int* height, int* channels) {
    return stbi_load(filename, width, height, channels, 0);
}

void init_drone(Drone* d, int id, float x, float y, float z, float v) {
    d->id = id;
    d->x = x;
    d->y = y;
    d->z = z;
    d->v = v;
    d->leader = 0;
}

void add_drone(Fleet* f, Drone d) {
    if (f->num_drones < MAX_DRONES) {
        f->drones[f->num_drones] = d;
        f->num_drones++;
    }
}

void set_map_tiles(Fleet* f, char* high, char* medium, char* low) {
    f->map_tiles[HIGH] = high;
    f->map_tiles[MEDIUM] = medium;
    f->map_tiles[LOW] = low;
}

void display_map(Fleet* f, Altitude alt) {
    int width, height, channels;
    unsigned char* img_data = load_image(f->map_tiles[alt], &width, &height, &channels);
    if (img_data) {
        printf("Affichage de la carte à une altitude de %d m (taille: %dx%d)\n", (alt == HIGH) ? 1000 : (alt == MEDIUM) ? 500 : 300, width, height);
        // Logique d'affichage de l'image ici
        stbi_image_free(img_data);
    } else {
        printf("Erreur de chargement de l'image: %s\n", f->map_tiles[alt]);
    }
}

void assign_tiles_to_drones(Fleet* f) {
    // Chaque drone surveille une sous-zone (tuile)
    for (int i = 0; i < f->num_drones; i++) {
        Drone* d = &f->drones[i];
        float tile_x = (d->x / TILE_SIZE) * TILE_SIZE; // Calculer la tuile à partir de la position
        float tile_y = (d->y / TILE_SIZE) * TILE_SIZE; // Calculer la tuile à partir de la position
        printf("Drone %d surveille la tuile à (%.2f, %.2f)\n", d->id, tile_x, tile_y);
    }
}

int main() {
    Fleet fleet = { .num_drones = 0 };

    // Configuration des tuiles de carte pour différentes altitudes
    set_map_tiles(&fleet, "high_zoom_tile.png", "medium_zoom_tile.png", "low_zoom_tile.png");

    // Initialisation des drones
    for (int i = 0; i < 3; i++) {
        Drone d;
        init_drone(&d, i + 1, i * TILE_SIZE, i * TILE_SIZE, 1000.0f, 1.0f); // Drones à 1000m
        add_drone(&fleet, d);
    }

    // Affichage de la carte à une altitude donnée
    display_map(&fleet, HIGH);
    display_map(&fleet, MEDIUM);
    display_map(&fleet, LOW);

    // Assignation des tuiles aux drones
    assign_tiles_to_drones(&fleet);

    return 0;
}
