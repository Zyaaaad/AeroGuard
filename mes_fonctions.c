#include<stdio.h>
#include"structures.h"


// Fonction pour initialiser un drone
void initDrone(Drone* d,int id, float x, float y, float z, float V, Status status) {
    d->id = id;
    printf("Drone %d encours de creation\n",d->id);
    d->pos.x = x;
    d->pos.y = y;
    d->pos.z = z;
    d->v = V;
    d->status = status;
    if(d->status == 1 ){
        printf("Le drone %d est actif\n",d->id);
    }
}

