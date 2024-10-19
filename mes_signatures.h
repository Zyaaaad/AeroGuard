#include "mes_structures.h"
#include<stdbool.h>

#ifndef MES_SIGNATURES_H
#define MES_SIGNATURES_H

// Fonction d'affichage des informations des drones
void print_drone_infos();
void print_neighbors();
void print_move_action(int drone_id, float new_x, float new_y);

// Fonctions de gestion des déplacements
void adjustDronesPosition(int fixed_drone_id, float new_x, float new_y, float xmin, float ymin, float xmax, float ymax); // Réajuste la position des drones après le déplacement
float move(int id, float x, float y, float z);
void speed(int id, float vx, float vy, float vz , float t);

// Fonctions de gestion des voisins (détection et mise à jour)
void detect_neighbors();
bool isCollision(float x, float y, float z, int id);
int within_range(int drone_id, int target_id);

// Fonctions de gestion des destructions
void destruct_drone(int drone_id, float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res);
void adjust_drones_after_destruction(float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res);
void update_neighbors_for_destroyed_drones(int destroyed_drone_id);

// Fonctions de gestion des collisions
void avoid_collisions();

// Fonctions de gestion des drones (initialisation et suppression)
void spread_drones(int num_drones, float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res);
void optimizeDronePositions(int id);
void remove_drone(int index);

#endif
