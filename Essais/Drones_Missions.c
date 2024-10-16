#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DRONES 10

typedef struct {
    float x, y, z; // Position du drone
    float v;       // Vitesse du drone
    int id;        // Identifiant du drone
    int leader;    // Indique si c'est le leader
} Drone;

typedef enum { MER, LITTORAL, VILLE, CAMPAGNE } ZoneType;

typedef struct {
    Drone drones[MAX_DRONES];
    int num_drones;
} Fleet;

void init_drone(Drone* d, int id, float x, float y, float z, float v) {
    d->id = id;
    d->x = x;
    d->y = y;
    d->z = z;
    d->v = v;
    d->leader = 0;  // Par défaut, le drone n'est pas leader
    printf("Drone %d initialisé à la position (%.2f, %.2f, %.2f) avec vitesse %.2f\n", id, x, y, z, v);
}

void add_drone(Fleet* f, Drone d) {
    if (f->num_drones < MAX_DRONES) {
        f->drones[f->num_drones] = d;
        f->num_drones++;
    }
}

void move_drone(Drone* d, float dx, float dy, float dz) {
    d->x += dx;
    d->y += dy;
    d->z += dz;
    printf("Drone %d déplacé à la position (%.2f, %.2f, %.2f)\n", d->id, d->x, d->y, d->z);
}

void define_zone(Drone* d, ZoneType zone) {
    switch (zone) {
        case MER:
            printf("Drone %d est dans une zone maritime\n", d->id);
            break;
        case LITTORAL:
            printf("Drone %d est dans une zone littorale\n", d->id);
            break;
        case VILLE:
            printf("Drone %d est dans une zone urbaine\n", d->id);
            break;
        case CAMPAGNE:
            printf("Drone %d est dans une zone rurale\n", d->id);
            break;
    }
}

void capture_image(Drone* d, char* filename, float resolution, float altitude) {
    printf("Drone %d capture une image avec résolution %.2f à l'altitude %.2f\n", d->id, resolution, altitude);
    FILE *f = fopen(filename, "w");
    fprintf(f, "Image capturée par drone %d à la position (%.2f, %.2f, %.2f)\n", d->id, d->x, d->y, d->z);
    fclose(f);
}

void check_enclave(Drone* d, float xMin, float yMin, float xMax, float yMax) {
    if (d->x > xMin && d->x < xMax && d->y > yMin && d->y < yMax) {
        printf("Drone %d est dans l'enclave : (%.2f, %.2f)\n", d->id, d->x, d->y);
    } else {
        printf("Drone %d est hors de l'enclave\n", d->id);
    }
}

void designate_leader(Fleet* f) {
    int leader_id = rand() % f->num_drones;  // Choix aléatoire du leader
    f->drones[leader_id].leader = 1;
    printf("Drone %d désigné leader\n", f->drones[leader_id].id);
}

void update_fleet(Fleet* f) {
    for (int i = 0; i < f->num_drones; i++) {
        Drone* d = &f->drones[i];
        move_drone(d, 1.0, 0.0, 0.0);  // Exemple de déplacement
        if (d->leader) {
            printf("Drone %d est en train de coordonner les autres drones.\n", d->id);
        }
    }
}

void init_zone(float xMin, float yMin, float xMax, float yMax) {
    printf("Zone initialisée : (%.2f, %.2f) à (%.2f, %.2f)\n", xMin, yMin, xMax, yMax);
}

void change_trajectory(Drone* d, float x, float y, float z) {
    printf("Changement de trajectoire pour le drone %d vers la position (%.2f, %.2f, %.2f)\n", d->id, x, y, z);
    d->x = x;
    d->y = y;
    d->z = z;
}

int main() {
    Fleet fleet = { .num_drones = 0 };
    
    // Initialisation des drones
    Drone d1, d2, d3;
    init_drone(&d1, 1, 0.0, 0.0, 0.0, 1.0);
    add_drone(&fleet, d1);
    
    init_drone(&d2, 2, 10.0, 0.0, 0.0, 1.0);
    add_drone(&fleet, d2);
    
    init_drone(&d3, 3, 20.0, 0.0, 0.0, 1.0);
    add_drone(&fleet, d3);
    
    // Désignation d'un leader
    designate_leader(&fleet);

    // Définition d'une zone
    init_zone(0.0, 0.0, 100.0, 100.0);
    
    // Mise à jour de la flotte
    update_fleet(&fleet);

    // Capture d'images
    capture_image(&d1, "image_d1.txt", 1080.0, d1.z);
    capture_image(&d2, "image_d2.txt", 1080.0, d2.z);

    // Vérification des enclaves
    check_enclave(&d1, 0.0, 0.0, 50.0, 50.0);
    check_enclave(&d2, 0.0, 0.0, 50.0, 50.0);
    
    // Changement de trajectoire
    change_trajectory(&d1, 5.0, 5.0, 5.0);

    // Mise à jour de la flotte après changement de trajectoire
    update_fleet(&fleet);

    return 0;
}
