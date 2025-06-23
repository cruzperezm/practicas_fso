#include <stdio.h>
#include <stdlib.h>
#include "sala.h"

// Código que permite crear una sola sala

int capacidad;
int* sala;
int ocupados;
int libres;

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
// añadir si el id del asiento no es valido 
int estado_asiento(int id_asiento) {
    if (sala == NULL || id_asiento > capacidad || id_asiento < 0) {
        return -1;
    }
    return *(sala + (id_asiento - 1));  // Devuelve el id de la persona en el asiento
}

int asientos_libres() { //variables ocupados y libres 
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
    if (cap < 0){
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

