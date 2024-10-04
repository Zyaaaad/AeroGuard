#include <stdio.h>
#include "mes_structures.h"

// Fonction pour initialiser un drone
void initDrone(Drone *d, int id, float x, float y, float z, float V, Status status)
{
    d->id = id;
    printf("Drone %d encours de creation\n", d->id);
    d->pos.x = x;
    d->pos.y = y;
    d->pos.z = z;
    d->v = V;
    d->status = status;
    if (d->status == 1)
    {
        printf("Le drone %d est actif\n", d->id);
    }
}

// Fonction pour déplacer le drone si le statut est ACTIF
void deplacerDrone(Drone *d, float dx, float dy, float dz)
{
    if (d->status == ACTIF)
    {
        d->pos.x += dx * d->v;
        d->pos.y += dy * d->v;
        d->pos.z += dz * d->v;
        printf("Drone déplacé à la nouvelle position.\n");
    }
    else
    {
        printf("Drone inactif, déplacement impossible.\n");
    }
}

// Fonction pour afficher la position actuelle et le statut du drone
void afficherPosition(Drone *d)
{
    printf("Position actuelle du drone: (%.2f, %.2f, %.2f)\n", d->pos.x, d->pos.y, d->pos.z);
    printf("Statut du drone: %s\n", (d->status == ACTIF) ? "Actif" : "Inactif");
}

// Fonction pour changer le statut du drone
void changerStatus(Drone *d, Status nouveauStatus)
{
    d->status = nouveauStatus;
}