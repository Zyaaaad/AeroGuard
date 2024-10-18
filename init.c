#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DRONES 30
//Definir un vitesse maximal
#define VMAX 50
#define alpha 0.05
#define MAX_NEIGHBORS 10

typedef struct {
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
 float xMin , yMin ;
 float width,height;

}Image;

Drone drones[MAX_DRONES];

int drone_count = 0;

// Function to initialize drones and spread them over a defined area
void spread_drones(int num_drones, float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res) {
    float total_area = (xmax - xmin) * (ymax - ymin);
    
    float c=sqrt(total_area/num_drones);
    float h=c/(2*tan(alpha));
    
    //float drone_coverage = calculate_coverage(altitude, alpha);
    
    // Determine the grid size based on drone coverage
    float grid_size = c;
    int drones_per_row = (int)ceil((xmax - xmin) / grid_size);
    int drones_per_column = (int)ceil((ymax - ymin) / grid_size);
    
    // Initialize the drones
    int drone_id = 0;
    for (int i = 0; i < drones_per_row && drone_id < num_drones; i++) {
        for (int j = 0; j < drones_per_column && drone_id < num_drones; j++) {
            // Calculate the position of each drone
            float x = xmin + i * grid_size + grid_size/2 ;
            float y = ymin + j * grid_size + grid_size/2;
            float z = h; // Adjust altitude dynamically if needed
            
            // Initialize the drone
            drones[drone_id].id = drone_id + 1;
            drones[drone_id].x = x;
            drones[drone_id].y = y;
            drones[drone_id].z = z;
            drones[drone_id].coverage = c*c;
            drones[drone_id].vx = 0;
            drones[drone_id].vy = 0;
            drones[drone_id].vz = 0;
            drones[drone_id].communication_range = comm_range;
            drones[drone_id].camera_resolution = cam_res;
            drones[drone_id].is_active=1;
        

            drone_id++;
        }
    }

    drone_count = drone_id;
}


float move(int id, float x, float y, float z){
    //Récuperer le drone par son id
    Drone *d = &drones[id];
    
    //Calculer le distance parcourus 
    float dx,dy,dz,distance;
    dx = x - d->x;
    dy = y - d->y;
    dz = z - d->z;
    distance = sqrt(dx*dx + dy*dy + dz*dz);

    //Mise à jour des coordonées du drone
    d->x = x;
    d->y = y;
    d->z = z;

    //Calculer le temps de deplacement
    float t;
    t = distance/VMAX;

    //tester la fonction
    printf("Drone %d arrive à (x=%.3f, y=%.3f, z=%.3f)\n",d->id, d->x, d->y ,d->z);
    // retourner le temps pour pouvoir l'utiliser après
    return t;
}

void detect_neighbors() {
    for (int i = 0; i < drone_count; i++) {
        drones[i].neighbor_count = 0; // Réinitialiser le nombre de voisins
        for (int j = 0; j < drone_count; j++) {
            if (i != j && drones[j].is_active) {
                // Calculer la distance entre le drone i et le drone j
                float dx = drones[i].x - drones[j].x;
                float dy = drones[i].y - drones[j].y;
                float dz = drones[i].z - drones[j].z;
                float distance = sqrt(dx * dx + dy * dy + dz * dz);

                // Si la distance est dans la portée de communication
                if (distance <= drones[i].communication_range) {
                    if (drones[i].neighbor_count < MAX_NEIGHBORS) {
                        drones[i].neighbors[drones[i].neighbor_count] = drones[j].id;
                        drones[i].neighbor_count++;
                    }
                }
            }
        }
    }
}

int within_range(int drone_id, int target_id) {
    Drone *d = &drones[drone_id];  // Récupérer le drone source par son ID

    // Parcourir la liste des voisins du drone source
    for (int i = 0; i < d->neighbor_count; i++) {
        if (d->neighbors[i] == target_id) {
            // Si l'ID du voisin correspond au drone cible, retourner 1 (true)
            return 1;
        }
    }
    // Si aucun voisin ne correspond au drone cible, retourner 0 (false)
    return 0;
}

void adjust_drones_after_destruction(float xmin, float ymin, float xmax, float ymax,float comm_range,float cam_res) {
    // Calculer la zone totale
    //float total_area = (xmax - xmin) * (ymax - ymin);
    int active_drone_count = 0;

    // Compter les drones actifs
    for (int i = 0; i < drone_count; i++) {
        if (drones[i].is_active) {
            active_drone_count++;
        }
    }

    if (active_drone_count == 0) {
        printf("Tous les drones ont été détruits.\n");
        return;
    }

    // Calculer la nouvelle couverture par drone
    spread_drones(active_drone_count,xmin,ymin,xmax,ymax,comm_range,cam_res);

    
}

// Calculer la distance entre deux drones
float calculate_distance(float x1, float y1, float x2, float y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

// Trouver le drone le plus proche
int find_closest_drone(int destroyed_index) {
    float min_distance = -1;
    int closest_drone_index = -1;

    for (int i = 0; i < drone_count; i++) {
        if (i != destroyed_index && drones[i].is_active) {
            float distance = calculate_distance(drones[destroyed_index].x, drones[destroyed_index].y, drones[i].x, drones[i].y);

            if (min_distance == -1 || distance < min_distance) {
                min_distance = distance;
                closest_drone_index = i;
            }
        }
    }

    return closest_drone_index;
}

/*
Lorsque le drone est détruit, il est marqué comme inactif avec is_active = 0.
La fonction update_neighbors_for_destroyed_drones va parcourir tous les autres drones et vérifier si le drone détruit est dans leur liste de voisins.
Si le drone détruit est trouvé dans la liste des voisins, il sera supprimé de cette liste.
*/
void update_neighbors_for_destroyed_drones(int destroyed_drone_id) {
    if (destroyed_drone_id < 1 || destroyed_drone_id > drone_count) {
        printf("Erreur: ID de drone invalide.\n");
        return;
    }

    Drone *destroyed_drone = &drones[destroyed_drone_id-1];

    if (!destroyed_drone->is_active) {
        printf("Mise à jour des voisins pour le drone %d détruit...\n", destroyed_drone_id);
        
        // Parcourir tous les drones
        for (int i = 0; i < drone_count; i++) {
            Drone *d = &drones[i];

            // Si le drone est actif, vérifier s'il a le drone détruit comme voisin
            if (d->is_active) {
                for (int j = 0; j < d->neighbor_count; j++) {
                    if (d->neighbors[j] == destroyed_drone_id) {
                        // Supprimer le drone détruit de la liste des voisins
                        printf("Drone %d: suppression du voisin %d\n", d->id, destroyed_drone_id);
                        
                        // Remplacer ce voisin par le dernier dans la liste pour maintenir la continuité
                        d->neighbors[j] = d->neighbors[d->neighbor_count - 1];
                        d->neighbor_count--; // Diminuer le nombre de voisins
                        
                        break; // Sortir de la boucle car un drone ne peut être dans la liste qu'une fois
                    }
                }
            }
        }
    } else {
        printf("Le drone %d est toujours actif, pas besoin de mise à jour.\n", destroyed_drone_id);
    }
}

// Fonction pour supprimer un drone du tableau et réorganiser le tableau
void remove_drone(int index) {
    if (index < 0 || index >= drone_count) {
        printf("Index de drone invalide pour suppression.\n");
        return;
    }
    
    // Décaler les drones suivants dans le tableau pour combler le vide
    for (int i = index; i < drone_count - 1; i++) {
        drones[i] = drones[i + 1];
    }
    // Réduire le nombre de drones
    drone_count--;
    printf("Drone à l'index %d supprimé. Nouveau nombre de drones: %d\n", index + 1, drone_count);
}


void destruct_drone(int drone_id, float xmin, float ymin, float xmax, float ymax,float comm_range,float cam_res) {
    if (drone_id < 1 || drone_id > drone_count) {
        printf("\nErreur: ID de drone invalide.\n");
        return;
    }

    // Decrease drone_id to match the index in the array
    int index = drone_id - 1;
    if (drones[index].is_active == 0) {
        printf("Le drone %d est déjà détruit.\n", drone_id);
        return;
    }

    // Marquer le drone comme détruit
    drones[index].is_active = 0;
    printf("\nDrone %d a été détruit.\n", drones[index].id);
    
    // Trouver le drone le plus proche
    //int closest_drone_index = find_closest_drone(index);
    
    // Remplacer le drone détruit par le drone le plus proche
    /*if (closest_drone_index != -1) {
        printf("Le drone le plus proche est le drone %d.\n", drones[closest_drone_index].id);

        // Remplacer le drone détruit par le plus proche
        drones[index] = drones[closest_drone_index];
        drones[closest_drone_index].is_active = 0;  // Marquer l'ancien drone comme détruit
    }*/

    update_neighbors_for_destroyed_drones(drone_id);
    // Supprimer le drone du tableau
    remove_drone(index);
    
    // Call the function to adjust remaining drones after destruction
    adjust_drones_after_destruction(xmin, ymin, xmax, ymax,comm_range,cam_res);
}



/*
void adjust_drones_after_destruction(float xmin, float ymin, float xmax, float ymax, float comm_range, float cam_res) {
    // Calculer la surface totale de la zone
    float total_area = (xmax - xmin) * (ymax - ymin);
    
    // Compter le nombre de drones actifs
    int active_drone_count = 0;
    for (int i = 0; i < drone_count; i++) {
        if (drones[i].is_active) {
            active_drone_count++;
        }
    }

    if (active_drone_count == 0) {
        printf("Tous les drones ont été détruits.\n");
        return;
    }

    // Réinitialiser la répartition des drones actifs
    float c = sqrt(total_area / active_drone_count);
    float h = c / (2 * tan(alpha));

    float grid_size = c;
    int drones_per_row = (int)ceil((xmax - xmin) / grid_size);
    int drones_per_column = (int)ceil((ymax - ymin) / grid_size);
    
    // Redistribuer les drones actifs
    int drone_id = 0;
    for (int i = 0; i < drones_per_row && drone_id < active_drone_count; i++) {
        for (int j = 0; j < drones_per_column && drone_id < active_drone_count; j++) {
            if (drones[drone_id].is_active) {
                // Calculer la position de chaque drone
                float x = xmin + i * grid_size + grid_size / 2;
                float y = ymin + j * grid_size + grid_size / 2;
                float z = h; // Ajuster l'altitude dynamique si nécessaire
                
                // Mettre à jour les positions et la couverture
                drones[drone_id].x = x;
                drones[drone_id].y = y;
                drones[drone_id].z = z;
                drones[drone_id].coverage = c * c;
                drones[drone_id].vx = 0;
                drones[drone_id].vy = 0;
                drones[drone_id].vz = 0;
                drones[drone_id].communication_range = comm_range;
                drones[drone_id].camera_resolution = cam_res;
                
                drone_id++;
            }
        }
    }
}
*/
void adjust_neighbors_for_destroyed_drone(int destroyed_drone_id) {
    Drone destroyed_drone = drones[destroyed_drone_id - 1]; // Récupérer le drone détruit

    // Boucle à travers tous les drones pour vérifier les voisins
    for (int i = 0; i < drone_count; i++) {
        if (drones[i].is_active && drones[i].neighbor_count > 0) {
            for (int j = 0; j < drones[i].neighbor_count; j++) {
                if (drones[i].neighbors[j] == destroyed_drone.id) {
                    // Déplacer le drone voisin vers la position moyenne entre son emplacement actuel et celui du drone détruit
                    drones[i].x = (drones[i].x + destroyed_drone.x) / 2;
                    drones[i].y = (drones[i].y + destroyed_drone.y) / 2;

                    // Calculer la nouvelle zone de couverture
                    float new_coverage = drones[i].coverage + destroyed_drone.coverage;

                    // Éviter les calculs si la nouvelle couverture est nulle ou négative
                    if (new_coverage > 0) {
                        // Recalculer l'altitude nécessaire pour la nouvelle zone de couverture
                        drones[i].z = sqrt(new_coverage) / (2 * tan(alpha));
                    } else {
                        // Si la couverture est nulle ou négative, gardez l'altitude actuelle
                        drones[i].z = drones[i].z; // ou définissez à une valeur par défaut
                    }

                    // Mettre à jour la zone de couverture pour le voisin
                    drones[i].coverage = new_coverage;

                    // Affichage du statut du drone voisin après ajustement
                    printf("Drone %d a adapté sa position à (%.2f, %.2f, %.2f) et sa zone de couverture à %.2f m^2 pour couvrir le drone détruit %d.\n", 
                        drones[i].id, drones[i].x, drones[i].y, drones[i].z, drones[i].coverage, destroyed_drone.id);
                }
            }
        }
    }
}


void adjustDronesPosition() {
    for (int i = 0; i < drone_count; i++) {
        if (drones[i].is_active) {
            // Position cible du drone i après mouvement
            float target_x = drones[i].x;
            float target_y = drones[i].y;
            float target_z = drones[i].z;

            // Vérifier les collisions avec les autres drones
            for (int j = 0; j < drone_count; j++) {
                if (i != j && drones[j].is_active) {
                    float dx = target_x - drones[j].x;
                    float dy = target_y - drones[j].y;
                    float dz = target_z - drones[j].z;
                    float distance = sqrt(dx * dx + dy * dy + dz * dz);

                    // Si la distance est nulle (collision), ajuster la position
                    if (distance < 1.0) { // Utiliser une petite valeur seuil pour éviter les collisions
                        // Ajuster la position du drone i
                        target_x += (1.0 - distance) * (dx / distance); // Éloigner le drone
                        target_y += (1.0 - distance) * (dy / distance); // Éloigner le drone
                        target_z += (1.0 - distance) * (dz / distance); // Éloigner le drone
                    }
                }
            }

            // Mettre à jour la position du drone
            drones[i].x = target_x;
            drones[i].y = target_y;
            drones[i].z = target_z;

            printf("Drone %d ajusté à (x=%.3f, y=%.3f, z=%.3f)\n", drones[i].id, drones[i].x, drones[i].y, drones[i].z);
        }
    }
}


void print_neighbors() {
    printf("\n");
    for (int i = 0; i < drone_count; i++) {
        printf("Drone %d a %d voisins: ", drones[i].id, drones[i].neighbor_count);
        for (int j = 0; j < drones[i].neighbor_count; j++) {
            printf("%d ", drones[i].neighbors[j]);
        }
        printf("\n");
    }
}


void print_drone_infos() {
    printf("\n");
    for (int i = 0; i < drone_count; i++) {
        printf("Drone %d:\n", drones[i].id);
        printf("  Position (x=%.3f, y=%.3f, z=%.3f)\n", drones[i].x, drones[i].y, drones[i].z);
        printf("  Coverage Area = %.3f m^2\n", drones[i].coverage);
        printf("  Velocity (vx=%.3f, vy=%.3f, vz=%.3f)\n", drones[i].vx, drones[i].vy, drones[i].vz);
        printf("  Communication Range = %.3f meters\n", drones[i].communication_range);
        printf("  Camera Resolution = %.3f pixels\n", drones[i].camera_resolution);
        printf("  Active Status = %s\n", drones[i].is_active ? "Active" : "Destroyed");
        printf("\n");
    }
}

void speed(int id, float vx, float vy, float vz , float t){
    //Recuperer le drone par son id
    Drone *d = &drones[id];

    // Tester si la vitesse deppaser la vitesse maximal
    if (sqrt(vx*vx + vy*vy + vz*vz) > VMAX){
        //Au lieu de laisser la vitesse deppasser la VMAX on l'associe
        float facteur = VMAX / sqrt(vx*vx + vy*vy + vz*vz);
        vx *= facteur;
        vy *= facteur;
        vz *= facteur;
    }

    int x0, y0, z0;
    x0 = d->x ;
    y0 = d->y;
    z0 = d->z;

    //Mettre à jour la position du drone selon la vitesse et le temps
    d->x = vx * t + x0;
    d->y = vy * t + y0;
    d->z = vz * t + z0;
}

int main() {
    int num_drones, drone_id;
    float xMin, yMin, xMax, yMax, comm_range, cam_res;

    // Répartir les drones dans l'aire spécifiée
    printf("Entrez le nombre de drones: ");
    scanf("%d", &num_drones);

    printf("Entrez les dimensions de la zone (xMin yMin xMax yMax): ");
    scanf("%f %f %f %f", &xMin, &yMin, &xMax, &yMax);

    printf("Entrez la portée de communication: ");
    scanf("%f", &comm_range);
    printf("Entrez la résolution de la caméra: ");
    scanf("%f", &cam_res);

    // Répartir initialement les drones
    spread_drones(num_drones, xMin, yMin, xMax, yMax, comm_range, cam_res);
    print_drone_infos();
    detect_neighbors();
    print_neighbors();

    // Demander à l'utilisateur quel drone détruire
    printf("\n\nEntrez l'ID du drone à détruire: ");
    scanf("%d", &drone_id);

    // Détruire le drone
    destruct_drone(drone_id,xMin,yMin,xMax,yMax,comm_range,cam_res);

    // Ajustement des drones voisins après la destruction
    adjustDronesPosition();

    // Afficher les informations des drones restants et leurs nouveaux voisins
    detect_neighbors();
    print_drone_infos();

    return 0;
}

/*
int main() {
    int num_drones,drone_id;
    float xMin, yMin, xMax, yMax,  comm_range, cam_res;
    
    // Répartir les drones dans l'aire spécifiée
    printf("Entrez le nombre de drones: ");
    scanf("%d", &num_drones);
    
    printf("Entrez les dimensions de la zone (xMin yMin xMax yMax): ");
    scanf("%f %f %f %f", &xMin, &yMin, &xMax, &yMax);

    printf("Entrez la portée de communication: ");
    scanf("%f", &comm_range);
    printf("Entrez la résolution de la caméra: ");
    scanf("%f", &cam_res);

    // Répartir initialement les drones
    spread_drones(num_drones, xMin, yMin, xMax, yMax,comm_range,cam_res);
    print_drone_infos();

    detect_neighbors();
    print_neighbors();

    
    // Demander à l'utilisateur quel drone détruire
    printf("Entrez l'ID du drone à détruire: ");
    scanf("%d", &drone_id);


    // Appeler la fonction pour détruire le drone
    destruct_drone(drone_id);

    // Mise à jour des voisins après destruction
    update_neighbors_for_destroyed_drones(drone_id);

    // Ajustement des drones voisins après la destruction
    adjust_neighbors_for_destroyed_drone(drone_id);

    // Redétection des voisins après ajustement
    detect_neighbors();

    // Afficher les informations des drones restants et leurs nouveaux voisins
    print_drone_infos();
    print_neighbors();
    return 0;
    
}
*/



/*int main() {
   int num_drones;
    float xMin, yMin, xMax, yMax,  comm_range, cam_res;
    

    // Example input: Number of drones, area dimensions, and angle alpha (in radians)
    printf("Enter the number of drones: ");
    scanf("%d", &num_drones);
    
    printf("Enter the area dimensions (xMin yMin xMax yMax): ");
    scanf("%f %f %f %f", &xMin, &yMin, &xMax, &yMax);

    printf("Enter the communication range: ");
    scanf("%f", &comm_range);
    printf("Enter the camera resolution: ");
    scanf("%f", &cam_res);

    // Spread drones in the given area
    spread_drones(num_drones, xMin, yMin, xMax, yMax,comm_range,cam_res);

    print_drone_infos();
    move(0,2,2,2);
     
}*/
