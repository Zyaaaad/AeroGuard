#include <stdio.h>
#include "mes_structures.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"

#define MAX_DRONES 10

// Tableau des drones (simulant les drones en vol)
Drone drones[MAX_DRONES];
int nb_drones = 0;

// Charger l'image en utilisant stb_image
unsigned char *load_image(const char *chemin, int *largeur, int *hauteur, int *comp)
{
    // Le 0 pour charger l'image dans son format d'origine, sans conversion.
    unsigned char *image = stbi_load(chemin, largeur, hauteur, comp, 0);

    // Vérifie si le chargement a échoué
    if (!image)
    {
        printf("Erreur : Impossible de charger l'image à l'emplacement '%s'.\n", chemin);
        return NULL;
    }

    // Affiche les dimensions de l'image chargée
    printf("Image chargée avec succès: Largeur = %d, Hauteur = %d\n", *largeur, *hauteur);

    return image; // Retourne le pointeur vers l'image chargée
}



// Fonction pour initialiser un drone
Drone creer_drone(int id, float x, float y, float z, float V, Status status)
{
    Drone *d;
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
    return *d;
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

// Fonction pour afficher la position actuelle du drone
void afficherPosition(Drone *d)
{
    printf("Position actuelle du drone: (%.2f, %.2f, %.2f)\n", d->pos.x, d->pos.y, d->pos.z);
}

// Fonction pour afficher le statut actuelle du drone
void afficherStatut(Drone *d)
{
    printf("Statut du drone: %s\n", (d->status == ACTIF) ? "Actif" : "Inactif");
}

// Fonction pour changer le statut du drone
void changerStatus(Drone *d, Status nouveauStatus)
{
    d->status = nouveauStatus;
    afficherStatut(d);
}