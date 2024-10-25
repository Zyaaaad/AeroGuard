#include "mes_structures.h"

#ifndef MES_SIGNATURES_H
#define MES_SIGNATURES_H

void create_directory(const char *dir);
void init_drone(Drone *d, int id, float x, float y, float z, float v, int zoom_level);
void add_drone(Fleet *f, Drone d);
void adjust_drones_after_destruction(Fleet *f);
void latlon_to_tile(double lat, double lon, int zoom, int *x_tile, int *y_tile);
int download_tile(int x_tile, int y_tile, int zoom, const char *filename);
void drone_capture_tile(Drone *d);
void combine_drone_images(Fleet *f, const char *output_filename);
void init_region(Fleet *f, Area *area, int num_drones_side, int zoom_level, float center_lat, float center_lon);
void capturer(Fleet *f, const char *filename);

#endif
