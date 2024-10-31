#include <stdio.h>
#include <stdlib.h>

#define MAX_DRONES 10

typedef struct {
    int id;
    float x, y, z; // Position du drone
    float v;       // Vitesse du drone
} Drone;

typedef struct {
    Drone drones[MAX_DRONES];
    int num_drones;
} Fleet;

void init_drone(Drone* d,int id, float x, float y, float z, float v) {
    d->id = id;
    d->x = x;
    d->y = y;
    d->z = z;
    d->v = v;
    printf("Drone %d demarre a la position x=%.2f y=%.2f z=%.2f avec un vitesse=%.2f\n",d->id,d->x,d->y,d->z,d->v);

}

void add_drone(Fleet* f, Drone d) {
    if (f->num_drones < MAX_DRONES) {
        f->drones[f->num_drones] = d;
        f->num_drones++;
    }
}

void move_drone(Drone* d, float dx, float dy, float dz) {
    printf("Drone %d se deplace vers x=%.2f y=%.2f z=%.2f\n",d->id,dx,dy,dz);
    d->x = dx;
    d->y = dy;
    d->z = dz;
}

void update_fleet(Fleet* f) {
    for (int i = 0; i < f->num_drones; i++) {
        printf("Drone %d arrive a la position: (%.2f, %.2f, %.2f)\n", f->drones[i].id, f->drones[i].x, f->drones[i].y, f->drones[i].z);
    }
}

int main() {
    Fleet fleet = { .num_drones = 0 };

    Drone d1;
    init_drone(&d1, 1, 0.0, 0.0, 0.0, 1.0);
    move_drone(&d1,2.0,2.0,2.0);
    add_drone(&fleet, d1);
    

    Drone d2;
    init_drone(&d2, 2, 10.0, 0.0, 0.0, 1.0);
    move_drone(&d2,12.0,12.0,4.0);
    add_drone(&fleet, d2);


    update_fleet(&fleet);

    return 0;
}
