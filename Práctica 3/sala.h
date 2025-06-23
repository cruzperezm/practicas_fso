
#ifndef SALA_H
#define SALA_H

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>


int reserva_asiento(int id_persona);
int libera_asiento(int id_asiento);
int estado_asiento(int id_asiento);
int asientos_libres();
int asientos_ocupados();
int capacidad_sala();
int crea_sala(int cap);
int elimina_sala();

// *******************************************************************************
// 				PARTE DE LA PRÁCTICA 3
// *******************************************************************************
int obtener_capacidad_del_fichero(const char* ruta_fichero);
int guarda_estado_sala(const char* ruta_fichero);
int recupera_estado_sala(const char* ruta_fichero);
int guarda_estado_parcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos);
int recupera_estado_parcial_sala(const char* ruta_fichero, size_t num_asientos, int* id_asientos);

#endif // SALA_H
