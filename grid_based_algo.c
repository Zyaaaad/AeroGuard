#include <stdio.h>
#include <math.h>
#include <stdbool.h>

#define GRID_SIZE 10
#define DRONE_COUNT 4
#define VMAX 5.0

typedef struct {
    float x, y, z;
    float communicationRange;
    float coverageArea;
    int is_active;
} Drone;

Drone drones[DRONE_COUNT];
int grid[GRID_SIZE][GRID_SIZE];

// Limites de la région
float xMin = 0.0, xMax = 10.0;
float yMin = 0.0, yMax = 10.0;
float zMin = 0.0, zMax = 20.0;

// Fonction pour vérifier si une position est dans les limites de la zone
bool isWithinBounds(float x, float y, float z) {
    return (x >= xMin && x <= xMax && y >= yMin && y <= yMax && z >= zMin && z <= zMax);
}

// Fonction pour détecter une collision avec un autre drone
bool isCollision(float x, float y, float z, int id) {
    for (int i = 0; i < DRONE_COUNT; i++) {
        if (i != id && drones[i].is_active) {
            float dx = x - drones[i].x;
            float dy = y - drones[i].y;
            float dz = z - drones[i].z;
            float distance = sqrt(dx * dx + dy * dy + dz * dz);
            if (distance < 1.0) {  // Collision détectée si distance inférieure à 1 unité
                return true;
            }
        }
    }
    return false;
}

// Fonction pour optimiser les positions des autres drones
void optimizeDronePositions(int id) {
    for (int i = 0; i < DRONE_COUNT; i++) {
        if (i != id && drones[i].is_active) {
            float offset = drones[id].communicationRange / 2;
            if (drones[i].x > drones[id].x) {
                drones[i].x += offset;
            } else {
                drones[i].x -= offset;
            }

            if (drones[i].y > drones[id].y) {
                drones[i].y += offset;
            } else {
                drones[i].y -= offset;
            }
        }
    }
}

// Fonction de déplacement avec gestion des collisions et optimisation de couverture
float move(int id, float x, float y, float z) {
    Drone *d = &drones[id];

    // Vérification des limites de la zone
    if (!isWithinBounds(x, y, z)) {
        printf("Le déplacement est hors des limites de la zone.\n");
        return -1;
    }

    // Vérification des collisions
    if (isCollision(x, y, z, id)) {
        printf("Collision détectée avec un autre drone.\n");
        return -1;
    }

    // Calculer la distance du déplacement
    float dx = x - d->x;
    float dy = y - d->y;
    float dz = z - d->z;
    float distance = sqrt(dx * dx + dy * dy + dz * dz);

    // Déplacer le drone à la nouvelle position
    d->x = x;
    d->y = y;
    d->z = z;

    // Optimiser les positions des autres drones
    optimizeDronePositions(id);

    // Retourner le temps nécessaire pour le déplacement
    return distance / VMAX;
}

// Fonction pour afficher la position des drones
void displayDrones() {
    for (int i = 0; i < DRONE_COUNT; i++) {
        if (drones[i].is_active) {
            printf("Drone %d:\n", i + 1);
            printf("  Position (x=%.2f, y=%.2f, z=%.2f)\n", drones[i].x, drones[i].y, drones[i].z);
            printf("  Communication Range = %.2f meters\n", drones[i].communicationRange);
            printf("  Coverage Area = %.2f m^2\n", drones[i].coverageArea);
            printf("  Active Status = %s\n", drones[i].is_active ? "Active" : "Inactive");
        }
    }
}

// Fonction pour initialiser les drones
void initDrones() {
    drones[0] = (Drone){1.0, 1.0, 10.0, 2.0, 4.0, 1};
    drones[1] = (Drone){1.0, 3.0, 10.0, 2.0, 4.0, 1};
    drones[2] = (Drone){3.0, 1.0, 10.0, 2.0, 4.0, 1};
    drones[3] = (Drone){3.0, 3.0, 10.0, 2.0, 4.0, 1};
}

// Fonction pour placer les drones dans la grille
void placeDronesInGrid() {
    for (int i = 0; i < DRONE_COUNT; i++) {
        if (drones[i].is_active) {
            int gx = (int) drones[i].x;
            int gy = (int) drones[i].y;
            if (gx < GRID_SIZE && gy < GRID_SIZE) {
                grid[gx][gy] = 1;
            }
        }
    }
}

// Fonction pour afficher la grille
void displayGrid() {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
}

int main() {
    // Initialisation
    initDrones();
    
    // Afficher les positions initiales
    printf("Coordonnées initiales des drones :\n");
    displayDrones();
    
    // Déplacer un drone
    float t = move(0, 1.0, 3.0, 10.0);
    if (t >= 0) {
        printf("\nLe drone a été déplacé. Temps nécessaire: %.2f secondes.\n", t);
    }
    
    // Afficher les nouvelles positions
    printf("\nCoordonnées des drones après déplacement :\n");
    displayDrones();

    // Placer les drones dans la grille
    placeDronesInGrid();
    printf("\nGrille de couverture des drones :\n");
    displayGrid();

    return 0;
}
