#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "mes_structures.h"
#include "mes_signatures.h"

int main()
{
    Fleet fleet = {.num_drones = 0};
    Area area;

    int num_drones;
    float comm_range, cam_res;

    srand(time(NULL));
    create_directory("../Captures");

    // Demander les paramètres à l'utilisateur

    printf("Entrez le nombre de drones (1-%d) : ", MAX_DRONES);
    scanf("%d", &num_drones);

    printf("Entrez la plage de communication : ");
    scanf("%f", &comm_range);

    printf("Entrez la résolution de la caméra : ");
    scanf("%f", &cam_res);

    // Initialiser la région autour de la ville
    init_region(&fleet, &area, (int)sqrt(num_drones), 12, 48.988438, 2.220364); // Coordonnées de Paris

    // Capturer les images des drones
    capturer(&fleet, "../Captures/image_composite.png");

    return 0;
}