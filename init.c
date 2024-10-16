#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Inclure la bibliothèque stb_image_write.h
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"

#define MAX_DRONES 10
#define IMAGE_WIDTH 512
#define IMAGE_HEIGHT 512

// Définition d'une structure pour un drone
typedef struct {
    int id;
    float x, y; // Coordonnées
    float coverage; // Couverture
} Drone;

Drone drones[MAX_DRONES]; // Tableau global des drones
int drone_count = 0; // Compteur de drones actifs

// Fonction pour initialiser les drones
void spread_drones_simple(int num_drones, float xmin, float ymin, float xmax, float ymax) {
    float grid_width = (xmax - xmin) / (float)ceil(sqrt(num_drones));
    float grid_height = (ymax - ymin) / (float)ceil(sqrt(num_drones));
    
    int drone_id = 0;

    for (float x = xmin; x < xmax && drone_id < num_drones; x += grid_width) {
        for (float y = ymin; y < ymax && drone_id < num_drones; y += grid_height) {
            // Initialiser le drone
            drones[drone_id].id = drone_id + 1;
            drones[drone_id].x = x + grid_width / 2; // Centrer le drone
            drones[drone_id].y = y + grid_height / 2; // Centrer le drone
            drones[drone_id].coverage = grid_width; // Définir la couverture (largeur de la zone)

            drone_id++;
        }
    }

    drone_count = drone_id; // Mettre à jour le compteur de drones
}

// Tableau de couleurs pour les zones de couverture
unsigned char colors[MAX_DRONES][3] = {
    {255, 0, 0},    // Rouge
    {0, 255, 0},    // Vert
    {0, 0, 255},    // Bleu
    {255, 255, 0},  // Jaune
    {255, 0, 255},  // Magenta
    {0, 255, 255},  // Cyan
    {192, 192, 192}, // Gris
    {128, 0, 128},   // Violet
    {180, 128, 180}  // Autre couleur
};

// Fonction pour dessiner les drones et leur couverture sur une image
void draw_drones_on_image(const char* filename) {
    // Créer un tableau de pixels pour l'image (RGB)
    unsigned char *image = (unsigned char *)malloc(IMAGE_WIDTH * IMAGE_HEIGHT * 3);
    // Initialiser l'image à noir
    for (int i = 0; i < IMAGE_WIDTH * IMAGE_HEIGHT * 3; i++) {
        image[i] = 0; // Noir
    }

    int drone_radius = 8; // Rayon du drone
    int coverage_size = 40; // Taille de la zone de couverture (carré)

    // Dessiner chaque drone et sa couverture
    for (int i = 0; i < drone_count; i++) {
        int x_pos = (int)drones[i].x; // Convertir la position x
        int y_pos = (int)drones[i].y; // Convertir la position y

        // Vérifiez si les coordonnées sont dans les limites de l'image
        if (x_pos >= 0 && x_pos < IMAGE_WIDTH && y_pos >= 0 && y_pos < IMAGE_HEIGHT) {
            // Dessiner un drone en blanc
            for (int dy = -drone_radius; dy <= drone_radius; dy++) {
                for (int dx = -drone_radius; dx <= drone_radius; dx++) {
                    if (dx * dx + dy * dy <= drone_radius * drone_radius) { // Vérifie si dans le cercle
                        int px = x_pos + dx;
                        int py = y_pos + dy;
                        // Vérifiez les limites de l'image
                        if (px >= 0 && px < IMAGE_WIDTH && py >= 0 && py < IMAGE_HEIGHT) {
                            // Appliquer la couleur blanche pour le drone
                            image[(py * IMAGE_WIDTH + px) * 3 + 0] = 255; // Rouge
                            image[(py * IMAGE_WIDTH + px) * 3 + 1] = 255; // Vert
                            image[(py * IMAGE_WIDTH + px) * 3 + 2] = 255; // Bleu
                        }
                    }
                }
            }

            // Dessiner la zone de couverture (carrée)
            for (int dy = -coverage_size / 2; dy <= coverage_size / 2; dy++) {
                for (int dx = -coverage_size / 2; dx <= coverage_size / 2; dx++) {
                    int px = x_pos + dx;
                    int py = y_pos + dy;
                    // Vérifiez les limites de l'image
                    if (px >= 0 && px < IMAGE_WIDTH && py >= 0 && py < IMAGE_HEIGHT) {
                        // Appliquer la couleur unique pour la couverture
                        image[(py * IMAGE_WIDTH + px) * 3 + 0] = colors[i % MAX_DRONES][0]; // Rouge
                        image[(py * IMAGE_WIDTH + px) * 3 + 1] = colors[i % MAX_DRONES][1]; // Vert
                        image[(py * IMAGE_WIDTH + px) * 3 + 2] = colors[i % MAX_DRONES][2]; // Bleu
                    }
                }
            }
        }
    }

    // Sauvegarder l'image en format PNG
    stbi_write_png(filename, IMAGE_WIDTH, IMAGE_HEIGHT, 3, image, IMAGE_WIDTH * 3);

    // Libérer la mémoire
    free(image);
}

// Fonction principale
int main() {
    // Initialiser les drones
    spread_drones_simple(9, 0.0, 0.0, 512.0, 512.0);

    // Dessiner les drones et leur couverture sur une image et sauvegarder
    draw_drones_on_image("drones_with_square_coverage.png");

    printf("Image générée : drones_with_square_coverage.png\n");
    return 0;
}
