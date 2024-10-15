#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MAX_DRONES 10
#define M_PI 3.14159265358979323846

typedef struct {
    float x, y, z; // coord
    float v;       // Vitesse
    int id;        
    int leader;    // Indicateur de leader
    int zoom_level; // Niveau de zoom correspondant à l'altitude
    // Données d'image
    unsigned char* image_data;
    int img_width, img_height, img_channels;
} Drone;


typedef struct {
    Drone drones[MAX_DRONES];
    int num_drones;
} Fleet;

//initialisation des drones
void init_drone(Drone* d, int id, float x, float y, float z, float v, int zoom_level) {
    d->id = id;
    d->x = x;
    d->y = y;
    d->z = z;
    d->v = v;
    d->leader = 0;
    d->zoom_level = zoom_level;
    d->image_data = NULL;
    printf("Drone %d initialisé à la position (%.6f, %.6f, %.2f) avec le niveau de zoom %d\n", id, x, y, z, zoom_level);
}

void add_drone(Fleet* f, Drone d) {
    if (f->num_drones < MAX_DRONES) {
        f->drones[f->num_drones] = d;
        f->num_drones++;
    }
}

//Calcul des coordonnées de tuile à partir de la latitude, de la longitude et du niveau de zoom
void latlon_to_tile(double lat, double lon, int zoom, int* x_tile, int* y_tile) {
    int n = 1 << zoom;
    *x_tile = (int)((lon + 180.0) / 360.0 * n);
    *y_tile = (int)((1.0 - log(tan(lat * M_PI / 180.0) + 1.0 / cos(lat * M_PI / 180.0)) / M_PI) / 2.0 * n);
}

//Télécharger une tuile d'image :
int download_tile(int x_tile, int y_tile, int zoom, const char* filename) {
    char url[256];
    sprintf(url, "https://tile.openstreetmap.org/%d/%d/%d.png", zoom, x_tile, y_tile);
    char command[512];
    sprintf(command, "wget -q -O %s %s", filename, url);
    return system(command);
}

void drone_capture_tile(Drone* d) {
    int x_tile, y_tile;
    double lat = d->y; // Supposons que y correspond à la latitude
    double lon = d->x; // Supposons que x correspond à la longitude
    latlon_to_tile(lat, lon, d->zoom_level, &x_tile, &y_tile);

    char filename[64];
    sprintf(filename, "drone_%d_tile.png", d->id);
    download_tile(x_tile, y_tile, d->zoom_level, filename);

    // Charger les données d'image
    d->image_data = stbi_load(filename, &d->img_width, &d->img_height, &d->img_channels, 0);
    if (d->image_data) {
        printf("Drone %d a capturé la tuile (%d, %d) au zoom %d\n", d->id, x_tile, y_tile, d->zoom_level);
    } else {
        printf("Drone %d n'a pas pu capturer la tuile\n", d->id);
    }
}


//Combiner les images des drones en une image
void combine_drone_images(Fleet* f, const char* output_filename) {
    if (f->num_drones == 0) return;

    int grid_size = (int)sqrt(f->num_drones);
    int total_width = f->drones[0].img_width * grid_size;
    int total_height = f->drones[0].img_height * grid_size;
    int channels = f->drones[0].img_channels;

    unsigned char* composite_image = (unsigned char*)malloc(total_width * total_height * channels);
    memset(composite_image, 0, total_width * total_height * channels);

    for (int i = 0; i < f->num_drones; i++) {
        int row = i / grid_size;
        int col = i % grid_size;
        for (int y = 0; y < f->drones[i].img_height; y++) {
            for (int x = 0; x < f->drones[i].img_width; x++) {
                int src_idx = (y * f->drones[i].img_width + x) * channels;
                int dest_x = col * f->drones[i].img_width + x;
                int dest_y = row * f->drones[i].img_height + y;
                int dest_idx = (dest_y * total_width + dest_x) * channels;
                memcpy(&composite_image[dest_idx], &f->drones[i].image_data[src_idx], channels);
            }
        }
    }

    stbi_write_png(output_filename, total_width, total_height, channels, composite_image, total_width * channels);
    free(composite_image);
    printf("Image composite sauvegardée sous %s\n", output_filename);
}


int main() {
    Fleet fleet = { .num_drones = 0 };
    srand(time(NULL));

    // Définir la zone (par exemple, les coordonnées de Paris)
    float area_lat_min = 48.815573;
    float area_lat_max = 48.9021449;
    float area_lon_min = 2.224199;
    float area_lon_max = 2.4699208;

    int num_drones_side = 2; // Par simplicité, une grille 2x2
    int total_drones = num_drones_side * num_drones_side;

    // Initialiser les drones pour couvrir la zone au niveau de zoom correspondant à 1000 m
    int initial_zoom_level = 14; // Niveau de zoom pour 1000 m
    for (int i = 0; i < total_drones; i++) {
        float x = area_lon_min + (area_lon_max - area_lon_min) * (i % num_drones_side) / num_drones_side;
        float y = area_lat_min + (area_lat_max - area_lat_min) * (i / num_drones_side) / num_drones_side;
        Drone d;
        init_drone(&d, i + 1, x, y, 1000.0, 0.0, initial_zoom_level);
        add_drone(&fleet, d);
    }

    // Chaque drone capture sa tuile
    for (int i = 0; i < fleet.num_drones; i++) {
        drone_capture_tile(&fleet.drones[i]);
    }

    // Combiner les images des drones
    combine_drone_images(&fleet, "image_composite_1000m.png");

    // Simuler la descente des drones à 500 m
    for (int i = 0; i < fleet.num_drones; i++) {
        fleet.drones[i].z = 500.0;
        fleet.drones[i].zoom_level = 16; // Niveau de zoom pour 500 m
        drone_capture_tile(&fleet.drones[i]);
    }
    combine_drone_images(&fleet, "image_composite_500m.png");

    // Simuler la descente des drones à 300 m
    for (int i = 0; i < fleet.num_drones; i++) {
        fleet.drones[i].z = 300.0;
        fleet.drones[i].zoom_level = 17; // Niveau de zoom pour 300 m
        drone_capture_tile(&fleet.drones[i]);
    }
    combine_drone_images(&fleet, "image_composite_300m.png");

    // Libérer les ressources
    for (int i = 0; i < fleet.num_drones; i++) {
        if (fleet.drones[i].image_data) {
            stbi_image_free(fleet.drones[i].image_data);
        }
    }

    return 0;
}
