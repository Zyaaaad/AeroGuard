#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_DRONES 30
#define VMAX 50
#define alpha 0.05
#define MAX_NEIGHBORS 10
#define MAX_LINE_LENGTH 100

typedef struct
{
    int id;
    float x, y, z; // position
    float coverage;
    float vx, vy, vz; // velocity
    float communication_range;
    float camera_resolution;
    int neighbors[MAX_NEIGHBORS]; // list of neighbors
    int neighbor_count;
    int is_active; // 1 if drone is active, 0 if destroyed
} Drone;

typedef struct
{
    float xMin, yMin;
    float width, height;
} Image;

Drone drones[MAX_DRONES];
int drone_count = 0;

// Initialize drones and spread them over a defined area
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

// Move a drone to new coordinates and return the time taken
float move(int id, float x, float y, float z)
{
    Drone *d = &drones[id];
    float dx = x - d->x;
    float dy = y - d->y;
    float dz = z - d->z;
    float distance = sqrt(dx * dx + dy * dy + dz * dz);

    d->x = x;
    d->y = y;
    d->z = z;
    float t = distance / VMAX;

    printf("Drone %d arrive à (x=%.3f, y=%.3f, z=%.3f)\n", d->id, d->x, d->y, d->z);
    return t;
}

// Detect neighbors within communication range
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

// Check if target drone is within the range of another drone
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

// Adjust drones' positions and coverage after a destruction
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

// Update neighbors list for destroyed drones
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

// Remove a drone from the array and adjust
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

// Destroy a drone and adjust the system
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

void print_drone_infos()
{
    printf("\n=== Informations des drones ===\n");

    // Afficher une fois les informations communes à tous les drones
    if (drone_count > 0)
    {
        printf("Zone de couverture par drone = %.3f m²\n", drones[0].coverage);
        printf("Portée de communication = %.3f mètres\n", drones[0].communication_range);
    }

    // Afficher les informations spécifiques pour chaque drone
    for (int i = 0; i < drone_count; i++)
    {
        printf("\nDrone %d :\n", drones[i].id);
        printf("  Position (x=%.3f, y=%.3f, z=%.3f)\n", drones[i].x, drones[i].y, drones[i].z);
        printf("  Statut : %s\n", drones[i].is_active ? "Actif" : "Détruit");
    }
    printf("\n===============================\n");
}

void print_move_action(int drone_id, float new_x, float new_y)
{
    printf("\nDrone %d déplacé à la nouvelle position (x=%.3f, y=%.3f)\n", drone_id, new_x, new_y);
    printf("Réajustement des autres drones...\n");
}

void adjustDronesPosition(int fixed_drone_id, float new_x, float new_y, float xmin, float ymin, float xmax, float ymax)
{
    // Déplacer le drone spécifié à sa nouvelle position
    move(fixed_drone_id - 1, new_x, new_y, drones[fixed_drone_id - 1].z);

    // Calculer le centroïde de tous les drones
    float centroid_x = 0.0, centroid_y = 0.0, centroid_z = 0.0;
    for (int i = 0; i < drone_count; i++)
    {
        centroid_x += drones[i].x;
        centroid_y += drones[i].y;
        centroid_z += drones[i].z;
    }
    centroid_x /= drone_count;
    centroid_y /= drone_count;
    centroid_z /= drone_count;

    // Ajuster les positions des drones non déplacés
    for (int i = 0; i < drone_count; i++)
    {
        if (i != fixed_drone_id - 1 && drones[i].is_active)
        {
            Drone *d = &drones[i];

            // Calculer le vecteur entre le drone actuel et le centroïde
            float vx = d->x - centroid_x;
            float vy = d->y - centroid_y;
            float vz = d->z - centroid_z;

            // Normaliser le vecteur
            float magnitude = sqrt(vx * vx + vy * vy + vz * vz);
            vx /= magnitude;
            vy /= magnitude;
            vz /= magnitude;

            // Éloigner le drone en fonction du vecteur pour maximiser la couverture
            float offset_distance = 2.0; // Tu peux ajuster cette valeur pour déterminer l'espacement
            d->x += vx * offset_distance;
            d->y += vy * offset_distance;
            d->z += vz * offset_distance;

            // S'assurer que les drones restent dans les limites de la zone
            if (d->x < xmin)
                d->x = xmin;
            if (d->x > xmax)
                d->x = xmax;
            if (d->y < ymin)
                d->y = ymin;
            if (d->y > ymax)
                d->y = ymax;

            printf("Drone %d ajusté à (x=%.3f, y=%.3f, z=%.3f)\n", d->id, d->x, d->y, d->z);
        }
    }

    // Appel pour éviter les collisions après le réajustement des drones
    avoid_collisions();
}

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

                // Si la distance entre les drones est inférieure à un seuil, on les éloigne
                if (distance < 1.0)
                { // Ajuste ce seuil si nécessaire
                    float dx = drones[i].x - drones[j].x;
                    float dy = drones[i].y - drones[j].y;
                    float dz = drones[i].z - drones[j].z;

                    // Normaliser le vecteur
                    float magnitude = sqrt(dx * dx + dy * dy + dz * dz);
                    dx /= magnitude;
                    dy /= magnitude;
                    dz /= magnitude;

                    // Éloigner les deux drones l'un de l'autre
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

// Main function
int main()
{
    int num_drones, drone_id;
    float xMin, yMin, xMax, yMax, comm_range, cam_res;
    char line[MAX_LINE_LENGTH];

    // Ouvrir le fichier scenario.txt
    FILE *file = fopen("scenario.txt", "r");
    if (file == NULL)
    {
        printf("Erreur : Impossible d'ouvrir le fichier scenario.txt\n");
        return 1;
    }

    // Lire les instructions ligne par ligne
    while (fgets(line, sizeof(line), file))
    {
        char command[10];

        // Lire la première instruction (INIT, MOVE, etc.)
        sscanf(line, "%s", command);

        // Si la commande est INIT, initialiser les drones et la zone
        if (strcmp(command, "INIT") == 0)
        {
            sscanf(line, "%*s %d %f %f %f %f %f %f", &num_drones, &xMin, &yMin, &xMax, &yMax, &comm_range, &cam_res);
            spread_drones(num_drones, xMin, yMin, xMax, yMax, comm_range, cam_res);
            print_drone_infos();
            detect_neighbors();
        }
        // Si la commande est MOVE, déplacer le drone spécifié
        else if (strcmp(command, "MOVE") == 0)
        {
            float new_x, new_y;
            sscanf(line, "%*s %d %f %f", &drone_id, &new_x, &new_y);
            adjustDronesPosition(drone_id, new_x, new_y, xMin, yMin, xMax, yMax);
            print_drone_infos();
        }
        // Autres commandes peuvent être ajoutées ici
        else
        {
            printf("Commande non reconnue : %s\n", command);
        }
    }

    // Fermer le fichier scenario.txt
    fclose(file);

    return 0;
}