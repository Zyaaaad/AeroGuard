#include <stdio.h>
#include <stdlib.h>
#include <string.h> // pour memset
#include "mes_structures.h"
#include "mes_signatures.h"

// Inclusion des modules STB
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

// Fonction pour initialiser la carte
void initCarte(Carte *carte, const char *imagePath)
{
    int n;
    // Charger l'image au format RGB
    carte->image = stbi_load(imagePath, &carte->largeur, &carte->hauteur, &n, 3);
    if (carte->image == NULL)
    {
        printf("Erreur lors du chargement de l'image\n");
        exit(1);
    }

    // Initialiser une carte noire de la même taille
    carte->revealed = (unsigned char *)malloc(3 * carte->largeur * carte->hauteur);
    memset(carte->revealed, 0, 3 * carte->largeur * carte->hauteur); // Remplir de noir
}

// Initialisation du drone
void initDrone(Drone *d, int id, float x, float y, float z, float vitesse)
{
    d->id = id;
    d->pos.x = x;
    d->pos.y = y;
    d->pos.z = z;
    d->vitesse = vitesse;
    d->endommage = 0; // Par défaut, le drone n'est pas endommagé
    d->enMission = 1; // Le drone est en mission par défaut
    printf("Drone %d initialisé à (%.2f, %.2f, %.2f) avec vitesse %.2f\n", d->id, d->pos.x, d->pos.y, d->pos.z, d->vitesse);
}

void tracerLigne(int x0, int y0, int x1, int y1, Carte *carte)
{
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;

    while (1)
    {
        // Révéler la position actuelle (x0, y0) dans l'image
        if (x0 >= 0 && x0 < carte->largeur && y0 >= 0 && y0 < carte->hauteur)
        {
            int index = 3 * (y0 * carte->largeur + x0);
            carte->revealed[index] = carte->image[index];
            carte->revealed[index + 1] = carte->image[index + 1];
            carte->revealed[index + 2] = carte->image[index + 2];
        }

        if (x0 == x1 && y0 == y1)
            break;
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

// Déplacement du drone avec prise en compte de la vitesse et de l'état
void deplacerDrone(Drone *d, float dx, float dy, float dz, Carte *carte)
{
    if (d->endommage == 1)
    {
        printf("Drone %d est endommagé et ne peut pas se déplacer.\n", d->id);
        return;
    }
    // Enregistrer la position actuelle avant le déplacement
    int x0 = (int)d->pos.x;
    int y0 = (int)d->pos.y;

    // Appliquer la vitesse au déplacement
    d->pos.x += dx * d->vitesse;
    d->pos.y += dy * d->vitesse;
    d->pos.z += dz * d->vitesse;

    // Afficher la progression du drone
    printf("Drone %d déplacé à (%.2f, %.2f, %.2f)\n", d->id, d->pos.x, d->pos.y, d->pos.z);

    // Tracer une ligne entre l'ancienne position (x0, y0) et la nouvelle position
    int x1 = (int)d->pos.x;
    int y1 = (int)d->pos.y;
    tracerLigne(x0, y0, x1, y1, carte);

    // Simuler la photographie de la carte
    photographier(d, carte);
}

// Fonction pour photographier une zone de la carte
void photographier(Drone *d, Carte *carte)
{
    printf("Drone %d prend une photo à la position (%.2f, %.2f, %.2f)\n", d->id, d->pos.x, d->pos.y, d->pos.z);

    int x = (int)d->pos.x;
    int y = (int)d->pos.y;

    // Vérifier que les positions sont dans les limites de l'image
    if (x >= 0 && x < carte->largeur && y >= 0 && y < carte->hauteur)
    {
        int index = 3 * (y * carte->largeur + x);
        // Copier la partie de l'image originale dans l'image révélée
        carte->revealed[index] = carte->image[index];
        carte->revealed[index + 1] = carte->image[index + 1];
        carte->revealed[index + 2] = carte->image[index + 2];
    }
    else
    {
        printf("Erreur : Le drone est en dehors des limites de la carte\n");
    }
}

// Détection de drones malveillants
void detecterDronesMalveillants(Drone *drones, int nombreDrones)
{
    for (int i = 0; i < nombreDrones; i++)
    {
        if (drones[i].enMission == 0)
        {
            printf("Drone %d détecté comme malveillant\n", drones[i].id);
        }
    }
}

// Fonction pour sauvegarder l'image révélée
void sauvegarderCarte(Carte *carte, const char *outputPath)
{
    // Sauvegarder l'image révélée
    if (!stbi_write_png(outputPath, carte->largeur, carte->hauteur, 3, carte->revealed, carte->largeur * 3))
    {
        printf("Erreur lors de la sauvegarde de l'image\n");
    }
    else
    {
        printf("Carte sauvegardée avec succès à %s\n", outputPath);
    }
}

// Libérer la mémoire allouée pour la carte
void libererCarte(Carte *carte)
{
    stbi_image_free(carte->image); // Libérer l'image d'origine
    free(carte->revealed);         // Libérer la carte révélée
}

void endommagerDrone(Drone *d)
{
    d->endommage = 1;
    printf("Drone %d est endommagé.\n", d->id);
}

void lireScenario(char *fichierScenario, Drone *drones, int *nombreDrones, Carte *carte)
{
    FILE *fichier = fopen(fichierScenario, "r");
    if (fichier == NULL)
    {
        printf("Erreur d'ouverture du fichier %s\n", fichierScenario);
        exit(1);
    }

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier))
    {
        printf("Ligne lue : %s", ligne);
        // Analyser la commande
        if (strncmp(ligne, "init", 4) == 0)
        {
            int id;
            float x, y, z, vitesse;
            sscanf(ligne, "init %d %f %f %f %f", &id, &x, &y, &z, &vitesse);
            initDrone(&drones[*nombreDrones], id, x, y, z, vitesse);
            (*nombreDrones)++;
        }
        else if (strncmp(ligne, "deplacer", 8) == 0)
        {
            int id;
            float dx, dy, dz;
            sscanf(ligne, "deplacer %d %f %f %f", &id, &dx, &dy, &dz);
            deplacerDrone(&drones[id - 1], dx, dy, dz, carte);
        }
        else if (strncmp(ligne, "endommager", 9) == 0)
        {
            int id;
            sscanf(ligne, "endommager %d", &id);
            endommagerDrone(&drones[id - 1]);
        }
    }

    fclose(fichier);
}