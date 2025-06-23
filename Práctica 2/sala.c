#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sala.h"

// Código que permite crear una sola sala

int capacidad;
int* sala;
int ocupados;
int libres;
#define BUFFER_SIZE 100

int reserva_asiento(int id_persona) {
    if (sala == NULL) { // Si no existe la sala, se devuelve -1
        return -1;
    }
    if (asientos_libres() == 0 || id_persona < 0) {
        return -1;  // No hay asientos libres
    } else {
        for (int i = 0; i < capacidad; i++) {
            if (*(sala + i) == 0) {  // Si el asiento está libre
                *(sala + i) = id_persona;  // Se reserva el asiento
                ocupados = ocupados + 1;
                libres = libres - 1;
                return i + 1;  // Devuelve el id del asiento reservado
            }
        }
    }
    return -1;
}

int libera_asiento(int id_asiento) {
    if (sala == NULL || id_asiento >= capacidad || id_asiento < 0) {
        return -1;
    }
    if (*(sala + (id_asiento - 1)) == 0) {  // El asiento ya está libre
        return -1;
    } else {
        int temp_id = *(sala + (id_asiento - 1));
        *(sala + (id_asiento - 1)) = 0;  // Libera el asiento
        ocupados = ocupados - 1;
        libres = libres + 1;
        return temp_id;
    }
}

// Añadir si el id del asiento no es válido 
int estado_asiento(int id_asiento) {
    if (sala == NULL || id_asiento >= capacidad || id_asiento < 0) {
        return -1;
    }
    return *(sala + (id_asiento - 1));  // Devuelve el id de la persona en el asiento
}

int asientos_libres() { // Variables ocupados y libres 
    if (sala == NULL) {
        return -1;
    }
    return libres;
}

int asientos_ocupados() {
    if (sala == NULL) {
        return -1;
    }
    return ocupados;
}

int capacidad_sala() {
    if (sala == NULL) {
        return -1;
    }
    return capacidad;
}

int crea_sala(int cap) {
    if (cap < 0) {
        return -1;
    }
    capacidad = cap;
    if (sala != NULL) return -1;  // Si ya existe la sala
    sala = (int*)calloc(cap, sizeof(int));  // Crea la sala con los asientos vacíos (inicializados a 0)
    libres = capacidad;
    ocupados = 0;
    if (sala == NULL) {
        return -1;  // Error en la creación de la sala
    }
    return capacidad;
}

int elimina_sala() {
    if (sala != NULL) {
        free(sala);
        sala = NULL;
        return 0;
    }
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Uso: %s <sucursal> <capacidad_sala>\n", argv[0]);
        return -1;
    }

    printf("************************************************\n");
    printf("	PETICIONES QUE PUEDES HACER:\n");
    printf("1. reserva <id-persona>\n");
    printf("2. libera <id-persona>\n");
    printf("3. estado_asiento <id-asiento>\n");
    printf("4. estado_sala.\n");
    printf("5. cerrar_sala\n");
    printf("6. salir\n");
    printf("*************************************************\n");
    

    crea_sala(atoi(argv[2]));  // Crear la sala con la capacidad especificada
    while (1) {
        char buffer[BUFFER_SIZE];
        char *args[BUFFER_SIZE];
        printf("Sucursal %s -> ", argv[1]);
        fflush(stdout);

        // Leemos el input
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {  // Si no hay input, sale del programa
            printf("\n");
            break;
        }

        int check = 0;
        for (int i = 0; i < BUFFER_SIZE; i++) {
            if (buffer[i] == '\n') {
                break;
            }
            if (buffer[i] == ' ') {
                check = 1;
                break;
            }
        }

        if (buffer[strlen(buffer) - 1] == '\n') {
            buffer[strlen(buffer) - 1] = '\0';  // Eliminar el salto de línea
        }

        if (strcmp(buffer, "salir") == 0) {
            return (asientos_libres() > 0) ? 1: 0;
        }

        if (check == 1) {
            int i = 0;
            args[i] = strtok(buffer, " ");
            while (args[i] != NULL) {
                i++;
                args[i] = strtok(NULL, " ");
            }
        } else {
            args[0] = buffer;
        }

        char *command = args[0];
        int id = -1;
        if (check == 1) {
            id = atoi(args[1]);
        }

        if (strcmp(command, "reserva") == 0) {
            if (id > 0) {
                int id_asiento = reserva_asiento(id);
                if (id_asiento == -1) {
                    printf("No se pudo reservar el asiento para %d\n", id);
                } else {
                    printf("Asiento %d reservado para el id %d\n", id_asiento, id);
                }
            } else {
		printf("Introducta un id válido\n");
		}
        } else if (strcmp(command, "libera") == 0) {
		if (id > 0) {
                int result = libera_asiento(id);
                if (result == -1) {
                    printf("No se pudo liberar el asiento %d\n", id);
                } else {
                    printf("El asiento %d ha sido liberado\n", id);
                }
            } else {
		printf("Introducta un id de asiento válido\n");
		}
        } else if (strcmp(command, "estado_sala") == 0) {
            if (capacidad_sala() == 0) {
                printf("No existe la sala\n");
            } else {
                printf("Asientos libres: %d, Asientos ocupados: %d\n", asientos_libres(), asientos_ocupados());
            }
        } else if (strcmp(command, "estado_asiento") == 0) {
            int estado = estado_asiento(id);
            if (estado == -1) {
                printf("Ha ocurrido un error\n");
            } else if (estado == 0) {
                printf("El asiento %d está libre\n", id);
            } else {
                printf("El asiento %d está ocupado por la persona con ID = %d\n", id, estado);
            }
        } else if (strcmp(command, "cerrar_sala") == 0) {
            int p = elimina_sala();
            if (p == 0){     
                return(asientos_libres() > 0)? 1: 0;
            }
        } else {
            printf("Instrucción no reconocida\n");
        }
    }
}

