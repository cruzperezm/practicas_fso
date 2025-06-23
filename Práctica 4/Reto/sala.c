#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include "sala.h"
#include "retardo.h"

// Código que permite crear una sola sala
int capacidad;
int* sala;
int ocupados;
int libres;
static pthread_mutex_t mutex;
static pthread_cond_t c_reserva;
static pthread_cond_t c_libera;


int reserva_asiento(int id_persona) {
    if (sala == NULL) { // Si no existe la sala, se devuelve -1
        return -1;
    }
  pthread_mutex_lock(&mutex);
    if (libres == 0) {
  pthread_mutex_unlock(&mutex);
        return -1;  // No hay asientos libres
    } else {
        for (int i = 0; i < capacidad; i++) {
            if (*(sala + i) == 0) {  // Si el asiento está libre
      pausa_aleatoria(0.5);
                *(sala + i) = id_persona;  // Se reserva el asiento
                ocupados = ocupados + 1;
                libres = libres - 1;
  pthread_mutex_unlock(&mutex);
                return i + 1;  // Devuelve el id del asiento reservado
            }
        }
    }
  return -1;
}

int libera_asiento(int id_asiento) {
    if (sala == NULL || id_asiento > capacidad || id_asiento < 1) {
        return -1;
    }
  pthread_mutex_lock(&mutex);
    if (*(sala + (id_asiento - 1)) == 0) {  // El asiento ya está libre
  pthread_mutex_unlock(&mutex);
        return -1;
    } else {
        int temp_id = *(sala + (id_asiento - 1));
        pausa_aleatoria(0.5);
        *(sala + (id_asiento - 1)) = 0;  // Libera el asiento
        ocupados = ocupados - 1;
        libres = libres + 1;
  pthread_mutex_unlock(&mutex);
        return temp_id;
    }
}

// Añadir si el id del asiento no es válido 
int estado_asiento(int id_asiento) {
    if (sala == NULL || id_asiento > capacidad || id_asiento < 1) {
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
    if (sala != NULL) return -1;  // Si ya existe la sala
    pthread_mutex_init(&mutex, NULL);
  pthread_mutex_lock(&mutex);
    capacidad = cap;
    sala = (int*)calloc(cap, sizeof(int));  // Crea la sala con los asientos vacíos (inicializados a 0)
    libres = capacidad;
    ocupados = 0;
    pthread_mutex_unlock(&mutex);
    if (sala == NULL) {
        return -1;  // Error en la creación de la sala
    }
    return capacidad;
}

int elimina_sala() {
    if (sala != NULL) {
    pthread_mutex_lock(&mutex);
        free(sala);
        sala = NULL;
        capacidad = ocupados = libres = 0;
    pthread_mutex_unlock(&mutex);
    pthread_mutex_destroy(&mutex);
        return 0;
    }
    return -1;
}

