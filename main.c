// Inclusion des fichiers nécessaires

#include <stdio.h>
#include <stdlib.h>
#include <string.h>         // Pour les fonctions de manipulation de chaînes (strcmp, sscanf)
#include "mes_structures.h" // Structures de base (Drone, Image, etc.)
#include "mes_signatures.h" // Signatures des fonctions utilisées (move, spread_drones, adjustDronesPosition, etc.)

#define MAX_LINE_LENGTH 100 // Définir la longueur maximale des lignes du fichier scénario

int main()
{
    int num_drones, drone_id;
    float xMin, yMin, xMax, yMax, comm_range, cam_res;
    char line[MAX_LINE_LENGTH];

    // Ouvrir le fichier scenario.txt
    FILE *file = fopen("scenario.txt", "r");
    if (file == NULL)
    {
        printf("Erreur : Impossible d'ouvrir le fichier scenario.txt\n");
        return 1;
    }

    // Lire les instructions ligne par ligne
    while (fgets(line, sizeof(line), file))
    {
        char command[10];

        // Lire la première instruction (INIT, MOVE, etc.)
        sscanf(line, "%s", command);

        // Si la commande est INIT, initialiser les drones et la zone
        if (strcmp(command, "INIT") == 0)
        {
            sscanf(line, "%*s %d %f %f %f %f %f %f", &num_drones, &xMin, &yMin, &xMax, &yMax, &comm_range, &cam_res);
            spread_drones(num_drones, xMin, yMin, xMax, yMax, comm_range, cam_res);
            print_drone_infos();
            detect_neighbors();
        }
        // Si la commande est MOVE, déplacer le drone spécifié
        else if (strcmp(command, "MOVE") == 0)
        {
            float new_x, new_y;
            sscanf(line, "%*s %d %f %f", &drone_id, &new_x, &new_y);
            print_move_action(drone_id, new_x, new_y);
            adjustDronesPosition(drone_id, new_x, new_y, xMin, yMin, xMax, yMax);
            print_drone_infos();
            print_neighbors();
        }
        // Si la commande est DESTROY, détruire le drone spécifié
        else if (strcmp(command, "DESTROY") == 0)
        {
            sscanf(line, "%*s %d", &drone_id);
            destruct_drone(drone_id, xMin, yMin, xMax, yMax, comm_range, cam_res);
            print_drone_infos();
            print_neighbors();
        }
        // Autres commandes peuvent être ajoutées ici
        else
        {
            printf("Commande non reconnue : %s\n", command);
        }
    }

    // Fermer le fichier scenario.txt
    fclose(file);

    return 0;
}
