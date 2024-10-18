#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_DRONES 30
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

void spread_drones_simple(int num_drones, float xmin, float ymin, float xmax, float ymax) {
    float surface=(xmax - xmin)*(ymax - ymin);
    float cov=surface/num_drones;
    float h=sqrt(cov)/(2*tan(alpha));
    float grid_width =sqrt(cov);
    float grid_height =sqrt(cov);
    
    int drone_id = 0;
    printf("Surface = %f\n",xmax*ymax);
    printf("Coverage = %f\n",cov);
     for (float x = xmin; x < xmax && drone_id < num_drones; x += grid_width) {
        for (float y = ymin; y < ymax && drone_id < num_drones; y += grid_height) {
            drones[drone_id].id = drone_id + 1;
            drones[drone_id].x = x + grid_width / 2;
            drones[drone_id].y = y + grid_height / 2;
            drones[drone_id].z = h;
            drones[drone_id].coverage = grid_width * grid_height;
            drone_id++;
        }
    }
    drone_count = drone_id;
}

int main() {
    int num_drones,drone_id;
    printf("give me num of drones: ");
    scanf("%d",&num_drones);
    int width, height;
    printf("give me width: ");
    scanf("%d",&width);
    printf("give me height: ");
    scanf("%d",&height);
    spread_drones_simple(num_drones, 0, 0, width, height);    

    drone_id=0;
    
    for (int i = 0; i < num_drones; i++) {
           printf("%d\n",drones[drone_id].id);
           printf("%f\n",drones[drone_id].x);
           printf("%f\n",drones[drone_id].y);
           printf("%f\n",drones[drone_id].z);
           printf("%f\n",drones[drone_id].coverage);

            drone_id++;
        }

    return 0;

}
