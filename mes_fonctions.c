#include <stdio.h>
#include <math.h>
#include "mes_structures.h"
#include "mes_signatures.h"
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"

#define MAX_DRONES 30
#define MAX_NEIGHBORS 10

#define VMAX 50
#define alpha 0.05

#define MAX_LINE_LENGTH 100

Drone drones[MAX_DRONES];
int drone_count = 0;

// === Fonctions d'affichage des informations des drones ===

void print_drone_infos()
{
    printf("\n=== Informations des drones ===\n");

    if (drone_count > 0)
    {
        printf("Zone de couverture par drone = %.3f m²\n", drones[0].coverage);
        printf("Portée de communication = %.3f mètres\n", drones[0].communication_range);
    }

    for (int i = 0; i < drone_count; i++)
    {
        printf("\nDrone %d :\n", drones[i].id);
        printf("  Position (x=%.3f, y=%.3f, z=%.3f)\n", drones[i].x, drones[i].y, drones[i].z);
        printf("  Vitesse (vx=%.3f, vy=%.3f, vz=%.3f)\n", drones[i].vx, drones[i].vy, drones[i].vz);
        printf("  Statut : %s\n", drones[i].is_active ? "Actif" : "Détruit");
    }
    printf("\n===============================\n");
}

void print_neighbors()
{
    printf("\n=== Voisins des drones ===\n");
    for (int i = 0; i < drone_count; i++)
    {
        printf("Drone %d : ", drones[i].id);
        if (drones[i].neighbor_count == 0)
        {
            printf("Pas de voisins.\n");
        }
        else
        {
            printf("Voisins : ");
            for (int j = 0; j < drones[i].neighbor_count; j++)
            {
                printf("%d ", drones[i].neighbors[j]);
            }
            printf("\n");
        }
    }
    printf("===============================\n");
}

void print_move_action(int drone_id, float new_x, float new_y)
{
    printf("\nDrone %d déplacé à la nouvelle position (x=%.3f, y=%.3f)\n", drone_id, new_x, new_y);
    printf("\nRéajustement des autres drones...\n");
}

void print_speed_action(int drone_id, float vx, float vy, float vz,float delta_t)
{
    printf("\nDrone %d se déplace avec un vecteur vitesse (vx=%.3f, vy=%.3f, vz=%.3f)\n", drone_id, vx, vy, vz);
    printf("\nAjustement des autres drones...\n");
}

// Fonction pour détecter une collision avec un autre drone
bool isCollision(float x, float y, float z, int id) {
    for (int i = 0; i < drone_count; i++) {
        if (i != id && drones[i].is_active) {
            float dx = x - drones[i].x;
            float dy = y - drones[i].y;
            float distance = sqrt(dx * dx + dy * dy);
             printf("Distance entre Drone %d et Drone %d = %.3f\n", id+1 , drones[i].id, distance);
            if (distance < 0.1) {
                return true; // Collision détectée
            }
        }
    }
    return false;
}


// === Fonctions de gestion des déplacements ===

float move(int id, float x, float y, float z)
{
    // Vérifier s'il y a une collision avec un autre drone
    if (isCollision(x, y, drones[id - 1].z, id )) {
        printf("Collision détectée avec un autre drone.\n");
        return 0; 
    }
    else{
        Drone *d = &drones[id];
        float dx = x - d->x;
        float dy = y - d->y;
        float dz = z - d->z;
        float distance = sqrt(dx * dx + dy * dy + dz * dz);

        d->x = x;
        d->y = y;
        d->z = z;

        float t = distance / VMAX;
        return t;
    }
}

// Fonction pour optimiser les positions des autres drones
void optimizeDronePositions(int id) {
    for (int i = 0; i < drone_count; i++) {
        if (i != id && drones[i].is_active) {
            // Ajustement basé sur la portée de communication
            float offset = drones[id].communication_range / 2;
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
        printf("Drone %d ajusté à (x=%.3f, y=%.3f, z=%.3f)\n", drones[i].id, drones[i].x, drones[i].y, drones[i].z);

    }
}


void adjustDronesPosition(int fixed_drone_id, float new_x, float new_y, float xmin, float ymin, float xmax, float ymax)
{
    
    if ((fixed_drone_id < 0) || (fixed_drone_id >= drone_count) || (!drones[fixed_drone_id - 1].is_active)) {
        printf("Error: ID Invalid ou drone non-actif.\n");
        return ; 
    }

    // Vérifier si les coordonnées sont dans les limites
    if ((new_x < xmin)|| (new_x > xmax) || (new_y < ymin)|| (new_y > ymax) || drones[fixed_drone_id-1].z < 0) {
        printf("Error: Destination en dehors de la zone!.\n");
        return ; 
    }
    // Déplacer le drone à la nouvelle position
    if(move(fixed_drone_id - 1, new_x, new_y, drones[fixed_drone_id - 1].z)){
        optimizeDronePositions(fixed_drone_id - 1);
    }

}

float* speed(int id, float vx, float vy, float vz , float t){
    //Verifier l ID
    
    if ((id < 0) || (id >= drone_count) || (!drones[id-1].is_active)) {
        printf("Error: Invalid drone ID or drone is not active.\n");
        return NULL; // Return NULL to indicate an error
    }

    //Recuperer le drone par son id
    Drone *d = &drones[id-1];
    int x0, y0, z0;

    x0 = d->x ;
    y0 = d->y;
    z0 = d->z;

    // Tester si la vitesse deppaser la vitesse maximal
    if (sqrt(vx*vx + vy*vy + vz*vz) > VMAX){
        //Au lieu de laisser la vitesse deppasser la VMAX on l'associe
        float facteur = VMAX / sqrt(vx*vx + vy*vy + vz*vz);
        vx *= facteur;
        vy *= facteur;
        vz *= facteur;
    }

    //Mettre à jour la position du drone selon la vitesse et le temps
    float new_x = vx * t + d->x;
    float new_y= vy * t + d->y;
    float new_z = vz * t + d->z;

    d->x = new_x;
    d->y = new_y;
    d->z = new_z;

    d->vx += vx;
    d->vy += vy;
    d->vz += vz;

    // Allocate memory to store the new coordinates
    float* newco = (float*)malloc(3 * sizeof(float));
    if (newco == NULL) {
        printf("Error: Memory allocation failed.\n");
        return NULL; // Return NULL if memory allocation fails
    }

    // Set the new coordinates
    newco[0] = d->x;
    newco[1] = d->y;
    newco[2] = d->z;

    //Il faut Vérifier si les coordonnées sont dans les limites
    
    optimizeDronePositions(id-1);
    detect_neighbors();

    return newco;
}

// === Fonctions de gestion des voisins ===
void detect_neighbors()
{
    for (int i = 0; i < drone_count; i++)
    {
        drones[i].neighbor_count = 0;
        for (int j = 0; j < drone_count; j++)
        {
            if (i != j && drones[j].is_active)
            {
                float dx = drones[i].x - drones[j].x;
                float dy = drones[i].y - drones[j].y;
                float dz = drones[i].z - drones[j].z;
                float distance = sqrt(dx * dx + dy * dy + dz * dz);

                if (distance <= drones[i].communication_range)
                {
                    if (drones[i].neighbor_count < MAX_NEIGHBORS)
                    {
                        drones[i].neighbors[drones[i].neighbor_count] = drones[j].id;
                        drones[i].neighbor_count++;
                    }
                }
            }
        }
    }
}

int within_range(int drone_id, int target_id)
{
    Drone *d = &drones[drone_id];
    for (int i = 0; i < d->neighbor_count; i++)
    {
        if (d->neighbors[i] == target_id)
        {
            return 1;
        }
    }
    return 0;
}

// === Fonctions de gestion des destructions ===

void destruct_drone(int drone_id, float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res)
{
    if (drone_id < 1 || drone_id > drone_count)
    {
        printf("Erreur: ID de drone invalide.\n");
        return;
    }

    int index = drone_id - 1;
    if (!drones[index].is_active)
    {
        printf("Le drone %d est déjà détruit.\n", drone_id);
        return;
    }

    drones[index].is_active = 0;
    printf("Drone %d a été détruit.\n", drones[index].id);

    update_neighbors_for_destroyed_drones(drone_id);
    remove_drone(index);
    adjust_drones_after_destruction(xmin, ymin, xmax, ymax, comm_range, cam_res);
}

void adjust_drones_after_destruction(float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res)
{
    int active_drone_count = 0;
    for (int i = 0; i < drone_count; i++)
    {
        if (drones[i].is_active)
        {
            active_drone_count++;
        }
    }

    if (active_drone_count == 0)
    {
        printf("Tous les drones ont été détruits.\n");
        return;
    }

    spread_drones(active_drone_count, xmin, ymin, xmax, ymax, comm_range, cam_res);
}

void update_neighbors_for_destroyed_drones(int destroyed_drone_id)
{
    if (destroyed_drone_id < 1 || destroyed_drone_id > drone_count)
    {
        printf("Erreur: ID de drone invalide.\n");
        return;
    }

    Drone *destroyed_drone = &drones[destroyed_drone_id - 1];
    if (!destroyed_drone->is_active)
    {
        for (int i = 0; i < drone_count; i++)
        {
            Drone *d = &drones[i];
            if (d->is_active)
            {
                for (int j = 0; j < d->neighbor_count; j++)
                {
                    if (d->neighbors[j] == destroyed_drone_id)
                    {
                        printf("Drone %d: suppression du voisin %d\n", d->id, destroyed_drone_id);
                        d->neighbors[j] = d->neighbors[d->neighbor_count - 1];
                        d->neighbor_count--;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        printf("Le drone %d est toujours actif, pas besoin de mise à jour.\n", destroyed_drone_id);
    }
}

// === Fonctions de gestion des collisions ===

void avoid_collisions()
{
    for (int i = 0; i < drone_count; i++)
    {
        for (int j = i + 1; j < drone_count; j++)
        {
            if (drones[i].is_active && drones[j].is_active)
            {
                float distance = sqrt(pow(drones[i].x - drones[j].x, 2) +
                                      pow(drones[i].y - drones[j].y, 2) +
                                      pow(drones[i].z - drones[j].z, 2));

                if (distance < 0.5)
                {
                    float dx = drones[i].x - drones[j].x;
                    float dy = drones[i].y - drones[j].y;
                    float dz = drones[i].z - drones[j].z;

                    float magnitude = sqrt(dx * dx + dy * dy + dz * dz);
                    dx /= magnitude;
                    dy /= magnitude;
                    dz /= magnitude;

                    drones[i].x += dx * 0.5;
                    drones[i].y += dy * 0.5;
                    drones[i].z += dz * 0.5;

                    drones[j].x -= dx * 0.5;
                    drones[j].y -= dy * 0.5;
                    drones[j].z -= dz * 0.5;

                    printf("Collision évitée entre Drone %d et Drone %d.\n", drones[i].id, drones[j].id);
                }
            }
        }
    }
}

// === Fonctions de gestion des drones ===

void spread_drones(int num_drones, float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res)
{
    float total_area = (xmax - xmin) * (ymax - ymin);
    float c = sqrt(total_area / num_drones);
    float h = c / (2 * tan(alpha));
    float grid_size = c;
    int drones_per_row = (int)ceil((xmax - xmin) / grid_size);
    int drones_per_column = (int)ceil((ymax - ymin) / grid_size);

    int drone_id = 0;
    for (int i = 0; i < drones_per_row && drone_id < num_drones; i++)
    {
        for (int j = 0; j < drones_per_column && drone_id < num_drones; j++)
        {
            float x = xmin + i * grid_size + grid_size / 2;
            float y = ymin + j * grid_size + grid_size / 2;
            float z = h;

            drones[drone_id].id = drone_id + 1;
            drones[drone_id].x = x;
            drones[drone_id].y = y;
            drones[drone_id].z = z;
            drones[drone_id].coverage = c * c;
            drones[drone_id].vx = 0;
            drones[drone_id].vy = 0;
            drones[drone_id].vz = 0;
            drones[drone_id].communication_range = comm_range;
            drones[drone_id].camera_resolution = cam_res;
            drones[drone_id].is_active = 1;
            drone_id++;
        }
    }
    drone_count = drone_id;
}

void remove_drone(int index)
{
    if (index < 0 || index >= drone_count)
    {
        printf("Index de drone invalide pour suppression.\n");
        return;
    }

    for (int i = index; i < drone_count - 1; i++)
    {
        drones[i] = drones[i + 1];
    }
    drone_count--;
    printf("Drone à l'index %d supprimé. Nouveau nombre de drones: %d\n", index + 1, drone_count);
}
