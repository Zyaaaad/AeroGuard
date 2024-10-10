// mes_fonctions.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mes_structures.h"
#include "mes_signatures.h"

void initDrone(Drone *d, int id, float x, float y, float z, float vitesse)
{
    d->id = id;
    d->pos.x = x;
    d->pos.y = y;
    d->pos.z = z;
    d->vitesse = vitesse;
    d->etat.endommage = 0;
    d->etat.enMission = 1;
    d->etat.droneMalveillant = 0;
}

void initCarte(Carte *carte, int largeur, int hauteur)
{
    carte->largeur = largeur;
    carte->hauteur = hauteur;
    carte->carte = malloc(hauteur * sizeof(char *));
    if (carte->carte == NULL)
    {
        printf("Erreur d'allocation mémoire pour la carte\n");
        return;
    }
    for (int i = 0; i < hauteur; i++)
    {
        carte->carte[i] = malloc(largeur * sizeof(char));
        if (carte->carte[i] == NULL)
        {
            printf("Erreur d'allocation mémoire pour la ligne %d de la carte\n", i);
            return;
        }
        memset(carte->carte[i], '.', largeur); // Remplir avec '.'
    }
}

void deplacerDrone(Drone *d, float dx, float dy, float dz)
{
    d->pos.x += dx;
    d->pos.y += dy;
    d->pos.z += dz;
    printf("Drone %d déplacé à (%.2f, %.2f, %.2f)\n", d->id, d->pos.x, d->pos.y, d->pos.z);
}

void photographier(Drone *d, Carte *carte)
{
    printf("Drone %d prend une photo à la position (%.2f, %.2f, %.2f)\n", d->id, d->pos.x, d->pos.y, d->pos.z);

    int x = (int)d->pos.x;
    int y = (int)d->pos.y;

    if (x >= 0 && x < carte->largeur && y >= 0 && y < carte->hauteur)
    {
        carte->carte[y][x] = 'P'; // Marquer la zone comme photographiée
    }
    else
    {
        printf("Erreur : Drone %d essaie de photographier en dehors des limites de la carte\n", d->id);
    }
}

void detecterDronesMalveillants(Drone *drones, int nombreDrones)
{
    for (int i = 0; i < nombreDrones; i++)
    {
        if (drones[i].etat.droneMalveillant)
        {
            printf("Drone malveillant détecté: ID %d\n", drones[i].id);
        }
    }
}

void analyserZone(Drone *drones, int nombreDrones, Carte *carte)
{
    // Simuler une analyse de zone par les drones
    for (int i = 0; i < nombreDrones; i++)
    {
        if (!drones[i].etat.endommage)
        {
            photographier(&drones[i], carte);
        }
    }
}

void lireScenario(char *fichierScenario, Drone *drones, int *nombreDrones)
{
    FILE *fichier = fopen(fichierScenario, "r");
    if (fichier == NULL)
    {
        printf("Erreur d'ouverture du fichier %s\n", fichierScenario);
        return;
    }

    printf("Fichier %s ouvert avec succès\n", fichierScenario);

    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier))
    {
        printf("Ligne lue : %s", ligne);

        // Traitement des lignes
        if (strncmp(ligne, "init", 4) == 0)
        {
            int id;
            float x, y, z, vitesse;
            sscanf(ligne, "init %d %f %f %f %f", &id, &x, &y, &z, &vitesse);
            initDrone(&drones[*nombreDrones], id, x, y, z, vitesse);
            (*nombreDrones)++;
            printf("Drone %d initialisé à (%.2f, %.2f, %.2f) avec vitesse %.2f\n", id, x, y, z, vitesse);
        }
        else if (strncmp(ligne, "deplacer", 8) == 0)
        {
            int id;
            float dx, dy, dz;
            sscanf(ligne, "deplacer %d %f %f %f", &id, &dx, &dy, &dz);
            deplacerDrone(&drones[id - 1], dx, dy, dz);
        }
        else if (strncmp(ligne, "capture", 7) == 0)
        {
            int id;
            sscanf(ligne, "capture %d", &id);
            photographier(&drones[id - 1], NULL); // Passer la carte si nécessaire
        }
        else if (strncmp(ligne, "endommage", 9) == 0)
        {
            int id;
            sscanf(ligne, "endommage %d", &id);
            drones[id - 1].etat.endommage = 1;
            printf("Drone %d endommagé.\n", id);
        }
        else if (strncmp(ligne, "detecterMalveillant", 20) == 0)
        {
            detecterDronesMalveillants(drones, *nombreDrones);
        }
    }

    fclose(fichier);
}

void afficherCarte(Carte *carte)
{
    for (int i = 0; i < carte->hauteur; i++)
    {
        for (int j = 0; j < carte->largeur; j++)
        {
            printf("%c ", carte->carte[i][j]);
        }
        printf("\n");
    }
}