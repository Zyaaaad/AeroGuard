#include <stdio.h>
#include <math.h>
#include <string.h>
#include "mes_structures.h"
#include "mes_signatures.h"
#include <stdbool.h>
#include <sys/stat.h>

#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "STB/stb_image_write.h"

#define M_PI 3.14159265358979323846

// Fonction pour créer un répertoire
void create_directory(const char *dir)
{
#ifdef _WIN32
    // Utiliser mkdir avec un seul argument sous Windows
    if (_mkdir(dir) != 0)
    {
        perror("Erreur lors de la création du répertoire");
    }
#else
    // Utiliser mkdir avec deux arguments sous POSIX
    if (mkdir(dir, 0755) != 0)
    {
        perror("Erreur lors de la création du répertoire");
    }
#endif
}

// Initialisation des drones avec le statut actif
void init_drone(Drone *d, int id, float x, float y, float z, float v, int zoom_level)
{
    d->id = id;
    d->x = x;
    d->y = y;
    d->z = z;
    d->v = v;
    d->leader = 0;
    d->zoom_level = zoom_level;
    d->image_data = NULL;
    d->destroyed = 0; // Indicateur de destruction
    d->active = 1;    // Drone actif au départ
    printf("Drone %d initialisé à la position (%.6f, %.6f, %.2f) avec le niveau de zoom %d\n", id, x, y, z, zoom_level);
}

void add_drone(Fleet *f, Drone d)
{
    if (f->num_drones < MAX_DRONES)
    {
        f->drones[f->num_drones] = d;
        f->num_drones++;
    }
}

// Ajuster la position et couverture après la destruction
void adjust_drones_after_destruction(Fleet *f)
{
    for (int i = 0; i < f->num_drones; i++)
    {
        if (f->drones[i].destroyed == 1)
        {
            printf("Le drone %d a été détruit.\n", f->drones[i].id);
            f->drones[i].active = 0; // Marquer le drone comme inactif
            // Vous pouvez ajouter des ajustements spécifiques ici, par exemple,
            // redistribuer les tâches des drones voisins.
        }
    }
}

// Calcul des coordonnées de tuile à partir de la latitude, de la longitude et du niveau de zoom
void latlon_to_tile(double lat, double lon, int zoom, int *x_tile, int *y_tile)
{
    int n = 1 << zoom;
    *x_tile = (int)((lon + 180.0) / 360.0 * n);
    *y_tile = (int)((1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0 * n);
}

// Télécharger une tuile d'image
int download_tile(int x_tile, int y_tile, int zoom, const char *filename)
{
    char url[256];
    sprintf(url, "https://tile.openstreetmap.org/%d/%d/%d.png", zoom, x_tile, y_tile);
    char command[512];
    sprintf(command, "curl -s -o %s %s", filename, url);
    return system(command);
}

// Capture d'une tuile par le drone
void drone_capture_tile(Drone *d)
{
    int x_tile, y_tile;
    double lat = d->y; // Latitude
    double lon = d->x; // Longitude
    latlon_to_tile(lat, lon, d->zoom_level, &x_tile, &y_tile);

    printf("Drone %d (%.6f, %.6f) va capturer la tuile (%d, %d) au zoom %d\n", d->id, lon, lat, x_tile, y_tile, d->zoom_level);

    char filename[128];
    sprintf(filename, "../Captures/drone_%d_tile.png", d->id);
    if (download_tile(x_tile, y_tile, d->zoom_level, filename) != 0)
    {
        printf("Erreur lors du téléchargement de la tuile pour le drone %d\n", d->id);
        return; // Quitte si le téléchargement échoue
    }

    // Charger les données d'image
    d->image_data = stbi_load(filename, &d->img_width, &d->img_height, &d->img_channels, 0);
    if (d->image_data)
    {
        printf("Drone %d a capturé la tuile (%d, %d) au zoom %d\n", d->id, x_tile, y_tile, d->zoom_level);
    }
    else
    {
        printf("Drone %d n'a pas pu capturer la tuile\n", d->id);
    }
}

// Combiner les images des drones en une image composite
void combine_drone_images(Fleet *f, const char *output_filename)
{
    if (f->num_drones == 0)
        return;

    int grid_size = (int)sqrt(f->num_drones); // Créer une grille pour les drones
    int total_width = f->drones[0].img_width * grid_size;
    int total_height = f->drones[0].img_height * grid_size;
    int channels = f->drones[0].img_channels;

    unsigned char *composite_image = (unsigned char *)malloc(total_width * total_height * channels);
    memset(composite_image, 0, total_width * total_height * channels);

    for (int i = 0; i < f->num_drones; i++)
    {
        if (f->drones[i].active == 1)
        { // Vérifier si le drone est actif
            int row = i / grid_size;
            int col = i % grid_size;
            for (int y = 0; y < f->drones[i].img_height; y++)
            {
                for (int x = 0; x < f->drones[i].img_width; x++)
                {
                    int src_idx = (y * f->drones[i].img_width + x) * channels;
                    int dest_x = col * f->drones[i].img_width + x;
                    int dest_y = row * f->drones[i].img_height + y;
                    int dest_idx = (dest_y * total_width + dest_x) * channels;
                    memcpy(&composite_image[dest_idx], &f->drones[i].image_data[src_idx], channels);
                }
            }
        }
    }

    // Sauvegarde de l'image composite dans le répertoire Capture
    stbi_write_png(output_filename, total_width, total_height, channels, composite_image, total_width * channels);
    free(composite_image);
    printf("Image composite sauvegardée sous %s\n", output_filename);
}

// Initialiser la région autour des coordonnées fournies
void init_region(Fleet *f, Area *area, int num_drones_side, int zoom_level, float center_lat, float center_lon)
{
    double tile_size = 360.0 / (1 << zoom_level); // Taille d'une tuile en degrés

    area->min_lat = center_lat - (num_drones_side / 2.0) * tile_size;
    area->max_lat = center_lat + (num_drones_side / 2.0) * tile_size;
    area->min_lon = center_lon - (num_drones_side / 2.0) * tile_size;
    area->max_lon = center_lon + (num_drones_side / 2.0) * tile_size;

    // Initialiser les drones avec la bonne position
    for (int i = 0; i < num_drones_side; i++)
    {
        for (int j = 0; j < num_drones_side; j++)
        {
            float x = area->min_lon + j * tile_size; // Longitude
            float y = area->max_lat - i * tile_size; // Latitude
            Drone d;
            init_drone(&d, i * num_drones_side + j + 1, x, y, 1000.0, 0.0, zoom_level);
            add_drone(f, d);
        }
    }

    printf("Zone initiale : min_lat = %.6f, max_lat = %.6f, min_lon = %.6f, max_lon = %.6f\n", area->min_lat, area->max_lat, area->min_lon, area->max_lon);
}

// Surveillance continue par les drones et capture d'images
void capturer(Fleet *f, const char *filename)
{
    for (int i = 0; i < f->num_drones; i++)
    {
        if (f->drones[i].destroyed == 0)
        { // Ne capture pas si le drone est détruit
            drone_capture_tile(&f->drones[i]);
        }
    }
    combine_drone_images(f, filename);
}
