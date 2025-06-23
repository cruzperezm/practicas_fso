#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sala.h"

#define BUFFER_SIZE 100

int main(int argc, char *argv[]) {
    if (strcmp(argv[1], "crea") == 0) {
        if (argc < 5 || strcmp(argv[2], "-f") != 0 && (strcmp(argv[2], "-fo") != 0 || strcmp(argv[4], "-c") != 0)) {
            fprintf(stderr, "Formato: ./misala crea -f[o] <fichero> -c <capacidad>\n");
            return 1;
        }

        const char *ruta = argv[3];
        int capacidad = atoi(argv[5]);

        if (strcmp(argv[2], "-f") == 0) {
            if (crea_sala(capacidad) == -1) {
            	fprintf(stderr, "Error al crear la sala\n");
                return 1;
            }
            if (guarda_estado_sala(ruta) == -1) {
                fprintf(stderr, "Error al guardar el estado de la sala en el archivo %s\n", ruta);
                return 1;
            }
            printf("Sala creada con éxito\n");
            
        } else if (strcmp(argv[2], "-fo") == 0) {
            crea_sala(capacidad);
            if (guarda_estado_sala(ruta) == -1) {
                fprintf(stderr, "Error al guardar el estado de la sala\n");
                return 1;
            }
            printf("Sala sobreescrita con éxito\n");
        }

    } else if (strcmp(argv[1], "reserva") == 0) {
    if (argc < 4 || strcmp(argv[2], "-f") != 0) {
        fprintf(stderr, "Formato: ./misala reserva -f <fichero> <ids personas>\n");
        return 1;
    }
    
    const char *ruta = argv[3];
    int num_personas = argc - 4;
    int *personas = malloc(num_personas * sizeof(int));
    if (personas == NULL) {
        fprintf(stderr, "Error al crear la sala\n");
        return 1;
    }

    for (int i = 0; i < num_personas; i++) {
        personas[i] = atoi(argv[i + 4]);
    }

    if (recupera_estado_sala(ruta) == -1) {
        fprintf(stderr, "Error al recuperar el estado de la sala\n");
        free(personas);
        return 1;
    }

    int *asientos_reservados = malloc(num_personas * sizeof(int));
    if (asientos_reservados == NULL) {
        fprintf(stderr, "Error al reservar los asientos\n");
        free(personas);
        return 1;
    }

    int id = 0;
    for (int i = 0; i < num_personas; i++) {
        int res = reserva_asiento(personas[i]);
        if (res == -1) {
            fprintf(stderr, "Error al reservar asiento para la persona %d\n", personas[i]);
            return -1;
        } else {
            asientos_reservados[id++] = res;
        }
    }

    if (guarda_estado_parcial_sala(ruta, id, asientos_reservados) == -1) {
        fprintf(stderr, "Error al guardar el estado parcial de la sala en el archivo %s\n", ruta);
        free(personas);
        free(asientos_reservados);
        return 1;
    }

    free(personas);
    free(asientos_reservados);
    printf("Asientos reservados y guardados con éxito.\n");


    } else if (strcmp(argv[1], "anula") == 0) {
    
        if (argc < 6 && strcmp(argv[2], "-f") != 0 && strcmp(argv[4], "-asientos") != 0 && strcmp(argv[4], "-personas")) {
            fprintf(stderr, "Formato: ./misala anula -f <ruta> [-asientos <ids asientos>] [-personas <ids personas>]\n");
            return 1;
        }
        if (strcmp(argv[4], "-asientos") == 0){
        const char *ruta = argv[3];
        int num_asientos = argc - 5;
        int *asientos_a_anular = malloc(num_asientos * sizeof(int));
        if (asientos_a_anular == NULL) {
            fprintf(stderr, "Error de asignacion de memoria\n");
            return 1;
        }

        for (int i = 0; i < num_asientos; i++) {
            asientos_a_anular[i] = atoi(argv[i + 5]);
        }

        if (recupera_estado_sala(ruta) == -1) {
            fprintf(stderr, "Error al recuperar el estado\n");
            free(asientos_a_anular);
            return 1;
        }

        for (int i = 0; i < num_asientos; i++) {
            if (libera_asiento(asientos_a_anular[i]) == -1) {
                fprintf(stderr, "Error al anular la reserva del asiento %d\n", asientos_a_anular[i]);
                free(asientos_a_anular);
                return 1;
            }
        }

        if (guarda_estado_sala(ruta) == -1) {
            fprintf(stderr, "Error al guardar el estado de la sala\n");
            free(asientos_a_anular);
            return 1;
        }

        free(asientos_a_anular);
        printf("Asientos anulados con éxito.\n");
    } else if(strcmp(argv[4], "-personas") == 0){


 const char *ruta = argv[3];
        int n = argc - 5;
        int *ids = malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) ids[i] = atoi(argv[5 + i]);

        crea_sala(0);
        if (recupera_estado_sala(ruta) < 0) {
            perror(ruta);
            free(ids);
            return -1;
        }

        for (int i = 0; i < n; i++) {
            int encontrado = 0;
            for (int j = 1; j <= capacidad_sala(); j++) {
                if (estado_asiento(j) == ids[i]) {
                    libera_asiento(j);
                    encontrado = 1;
                }
            }
            if (!encontrado)
                fprintf(stderr, "ID no encontrado: %d\n", ids[i]);
        }

        if (guarda_estado_sala(ruta) < 0) {
            perror(ruta);
            free(ids);
            return -1;
        }

        free(ids);
        return 0;
    }

    } else if (strcmp(argv[1], "estado") == 0) {
        if (argc < 4 && strcmp(argv[2], "-f") != 0) {
            fprintf(stderr, "Formato: ./misala estado -f <ruta>\n");
            return 1;
        }

        const char *ruta = argv[3];

        if (recupera_estado_sala(ruta) == -1) {
            fprintf(stderr, "Error al recuperar el estado\n");
            return 1;
        }

        printf("Estado de la sala:\n");
        for (int i = 1; i <= capacidad_sala(); i++) {
            printf("Asiento %d: %d\n", i, estado_asiento(i));
        }

  } else if (strcmp(argv[1], "compara") == 0) {
        const char *ruta1 = argv[2];
        const char *ruta2 = argv[3];

        crea_sala(0);
        if (recupera_estado_sala(ruta1) < 0) {
            fprintf(stderr, "Error al recuperar archivo 1: %s\n", ruta1);
            perror("recupera_estado_sala ruta1");
            return -1;
        }

        int cap1 = capacidad_sala();
        int *estado1 = malloc(cap1 * sizeof(int));
        if (!estado1) {
            fprintf(stderr, "Error al reservar memoria para estado1\n");
            return -1;
        }

        for (int i = 0; i < cap1; i++) estado1[i] = estado_asiento(i + 1);

        if (recupera_estado_sala(ruta2) < 0) {
            fprintf(stderr, "Error al recuperar archivo 2: %s\n", ruta2);
            perror("recupera_estado_sala ruta2");
            free(estado1);
            return -1;
        }

        int cap2 = capacidad_sala();
        if (cap1 != cap2) {
            fprintf(stderr, "Las capacidades no coinciden: %d vs %d\n", cap1, cap2);
            free(estado1);
            return -1;
        }

        for (int i = 0; i < cap1; i++) {
            if (estado1[i] != estado_asiento(i + 1)) {
                fprintf(stderr, "Diferencia en asiento %d: %d != %d\n", i + 1, estado1[i], estado_asiento(i + 1));
                free(estado1);
                printf("Archivos diferentes\n");
                return 1;
            }
        }

        free(estado1);
        printf("Archivos iguales\n");
        return 0;
    }

  else {
    printf("Comandos disponibles:\n");
    printf("1. ./misala crea -f[o] <fichero> -c <capacidad>\n");
    printf("2. ./misala reserva -f <fichero> <ids personas>\n");
    printf("3. ./misala anula -f <ruta> -asientos <ids asientos>\n");
    printf("3. ./misala anula -f <ruta> -personas <ids personas>\n");
    printf("5. ./misala estado -f <ruta>\n");
    printf("6. ./misala compara <ruta1> <ruta2>");
    return -1;
  }
  return 0;
}
