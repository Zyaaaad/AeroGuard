#include "mes_structures.h"

unsigned char* load_image(const char* chemin, int* largeur, int* hauteur, int* comp);
void initDrone(Drone *, int, float, float, float, float, Status);
void deplacerDrone(Drone *, float, float, float);
void afficherPosition(Drone *);
void afficherStatut(Drone *);
void changerStatus(Drone *, Status);
