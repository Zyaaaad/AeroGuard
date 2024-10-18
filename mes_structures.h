#ifndef MES_STRUCTURES_H
#define MES_STRUCTURES_H

#define MAX_NEIGHBORS 10

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

#endif // MES_STRUCTURES_H
