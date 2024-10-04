#include "mes_structures.h"

unsigned char* load_image(const char* chemin, int* largeur, int* hauteur, int* comp);
Drone creer_drone(int id, float x, float y, float z, float V, Status status);
//void initDrone(Drone *, int, float, float, float, float, Status);
void ajouter_drone(int id, float x, float y,float z,float V, Status status);
void deplacerDrone(Drone *, float, float, float);
void afficherPosition(Drone *);
void afficherStatut(Drone *);
void changerStatus(Drone *, Status);
