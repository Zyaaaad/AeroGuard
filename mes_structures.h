typedef struct
{
    float x, y, z;
} Position;

typedef enum
{
    INACTIF = 0,
    ACTIF = 1
} Status;

typedef struct
{
    int id;
    Position pos;
    float v;
    Status status;

} Drone;