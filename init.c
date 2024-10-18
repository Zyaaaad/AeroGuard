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
#define MAX_NEIGHBORS 4

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

void print_drone_infos() {
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
     
}
