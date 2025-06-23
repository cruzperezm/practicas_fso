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

int reserva_asiento(int id_persona) {
    if (sala == NULL) { // Si no existe la sala, se devuelve -1
        return -1;
    }
    if (libres == 0 || id_persona < 0) {
        return -1;  // No hay asientos libres
    } else {
        for (int i = 0; i < capacidad; i++) {
            if (*(sala + i) == 0) {  // Si el asiento está libre
                pausa_aleatoria(0.5);
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
        pausa_aleatoria(0.5);
        *(sala + (id_asiento - 1)) = 0;  // Libera el asiento
        ocupados = ocupados - 1;
        libres = libres + 1;
        return temp_id;
    }
}

// Añadir si el id del asiento no es válido 
int estado_asiento(int id_asiento) {
    if (sala == NULL || id_asiento > capacidad || id_asiento < 0) {
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
    capacidad = cap;
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
        capacidad = ocupados = libres = 0;
        return 0;
    }
    return -1;
}


// *******************************************************************************
// 				PARTE DE LA PRÁCTICA 3
// *******************************************************************************


int guarda_estado_sala(const char *ruta_fichero) {
    if (!sala || !ruta_fichero) { errno = EINVAL; return -1; }
    int fd = open(ruta_fichero, O_WRONLY| O_TRUNC | O_CREAT, 0644);
    if (fd < 0) return -1;
    if (write(fd, &capacidad, sizeof(capacidad)) != sizeof(capacidad)) { close(fd); return -1; }
    for (int i = 0; i < capacidad; i++){
    lseek(fd, sizeof(int), SEEK_CUR);
    int estado_temp = estado_asiento(i+1);
    if(write(fd, &estado_temp, sizeof(int)) != sizeof(int)) {
      perror("Error al guardar el estado");
      close(fd);
      return -1;
    }
  }  
  //ssize_t bytes = capacidad * sizeof(int);
    //if (write(fd, sala, bytes) != bytes) { close(fd); return -1; }
    close(fd);
    return 0;
}

int recupera_estado_sala(const char *ruta_fichero) {
    if (!sala || !ruta_fichero) { errno = EINVAL; return -1; }
    int fd = open(ruta_fichero, O_RDONLY);
    if (fd < 0) return -1;
    int cap_file;
    if (read(fd, &cap_file, sizeof(cap_file)) != sizeof(cap_file)) { close(fd); return -1; }
    if (cap_file != capacidad) { close(fd); return -1; }
    //size_t bytes = cap_file * sizeof(int);
    //int *buffer = malloc(bytes);
    //if (!buffer) { close(fd); return -1; }
    for (int i = 0; i < cap_file; i++){
      int *buffer;
      buffer = malloc(sizeof(int));
      if(!buffer){close(fd); return -1;}
      lseek(fd, sizeof(int), SEEK_CUR);
      if(read(fd, buffer, sizeof(int)) != sizeof(int)){
        close(fd);
        return -1;
      }
      if (*buffer > 0){
      libres -= 1;
      ocupados += 1;
    }
      *(sala+i) = *buffer;
      free(buffer);
  }
//if (read(fd, buf;fer, bytes) != (ssize_t)bytes) { free(buffer); close(fd); return -1; }
    //for (int i = 0; i < cap_file; i++) sala[i] = buffer[i];
    //free(buffer);
    close(fd);
    return 0;
}
int guarda_estado_parcial_sala(const char *ruta_fichero, size_t num_asientos, int *id_asientos) {
    if (sala == NULL || ruta_fichero == NULL) {
		perror("La sala o el fichero tiene un puntero núlo.");
		return -1;
	}
	int fd = open(ruta_fichero, O_WRONLY);
	if (fd < 0) {
		perror("Error al abrir el fichero.");
		return -1;
	}
	
	// leemos el fichero
	int capacidad_sala_fichero; // Variable donde se guarda la capacidad de la sala dada por el fichero.
	int b_capacidad_sala = read(fd, &capacidad_sala_fichero, sizeof(int));
	if (b_capacidad_sala < 0 || b_capacidad_sala != sizeof(int)) {
		perror("Error al leer la capacidad de la sala desde el fichero.");
		close(fd);
		return -1;
	}
	
	// Comprobamos la dimensiones de las salas con las dadas por el fichero.
	if (capacidad_sala() != capacidad_sala_fichero) {
		perror("La capacidad de la sala actual no coincide con la del fichero.");
		return -1;
	}
	
	size_t i = 0;
	while (i < num_asientos) {
		int id_asiento = id_asientos[i];
		
		if (id_asiento >= 1 && id_asiento <= capacidad_sala()) {
			/*
			primer "sizeof(int)" -> es donde se almacena la capacidad de la sala en el array del fichero.
			(id_asiento - 1)*sizeof(int) -> la información de los asientos.
			(id_asiento - 1) -> para hacer que los índices empiecen desde 0 y no de 1.
			*/
			off_t offset = sizeof(int) + (id_asiento - 1)*sizeof(int);
			
			/*
			SEEK_SET -> comenzamos a mover el puntero desde el inicio del fichero.
			Con lseek movemos el puntero a la posición del asiento deseado.
			*/
			off_t redireccion = lseek(fd, offset, SEEK_SET);
			if (redireccion < 0) {
				perror("Error al redireccionar el puntero del fichero.");
				close(fd);
				return -1;
			}
			
			// Obtenemos el estado actual del asiento
			int estado = estado_asiento(id_asiento);
			
			// Sobreescribimos la información de los asientos en el fichero.
			ssize_t b_escritos = write(fd, &estado, sizeof(int));
			if (b_escritos < 0) {
				perror("Error a escribir en el fichero.");
				return -1;
			}
		} else {
			perror("El ID del asiento es inválido.");
			return -1;
		}
		i++;
	}
	
	/// Cerramos el fichero.
  	if (close(fd) == -1) {
  		perror("Ocurrió un ERROR al cerrar el fichero.");
  		return -1;
  	}
  	
  	return 0;
}

int recupera_estado_parcial_sala(const char *ruta_fichero, size_t num_asientos, int *id_asientos) {
    if (sala == NULL || ruta_fichero == NULL) {
		perror("La sala o el fichero tiene un puntero núlo.");
		return -1;
	}
	
	// Abrimos el fichero.
	int fd = open(ruta_fichero, O_RDONLY);
	if (fd < 0) {
		perror("Error al abrir el fichero.");
	}
	
	// leemos el fichero
	int capacidad_sala_fichero; // Variable donde se guarda la capacidad de la sala dada por el fichero.
	int b_capacidad_sala = read(fd, &capacidad_sala_fichero, sizeof(int));
	if (b_capacidad_sala < 0 || b_capacidad_sala != sizeof(int)) {
		perror("Error al leer la capacidad de la sala desde el fichero.");
		close(fd);
		return -1;
	}
	
	// Comprobamos la dimensiones de las salas con las dadas por el fichero.
	if (capacidad_sala() != capacidad_sala_fichero) {
		perror("La capacidad de la sala actual no coincide con la del fichero.");
		return -1;
	}
	
	
	size_t i = 0;
	while (i < num_asientos) {
		int id_asiento = id_asientos[i];
		
		if (id_asiento >= 1 && id_asiento <= capacidad_sala()) {
			/*
			primer "sizeof(int)" -> es donde se almacena la capacidad de la sala en el array del fichero.
			(id_asiento - 1)*sizeof(int) -> la información de los asientos.
			(id_asiento - 1) -> para hacer que los índices empiecen desde 0 y no de 1.
			*/
			off_t offset = sizeof(int) + (id_asiento - 1)*sizeof(int);
			
			/*
			SEEK_SET -> comenzamos a mover el puntero desde el inicio del fichero.
			Con lseek movemos el puntero a la posición del asiento deseado.
			*/
			off_t redireccion = lseek(fd, offset, SEEK_SET);
			if (redireccion < 0) {
				perror("Error al redireccionar el puntero del fichero.");
				close(fd);
				return -1;
			}
			
			// Obtenemos el estado actual del asiento
			int estado_asiento;
			ssize_t b_leidos_asiento = read(fd, &estado_asiento, sizeof(int));
			if (b_leidos_asiento < 0 || b_leidos_asiento != sizeof(int)) {
				perror("Error al leer el estado del asiento desde el fichero.");
				close(fd);
				return -1;
			}
			
			// Actualizamos el estado del asiento en la sala.
			sala[id_asiento - 1] = estado_asiento;
		} else {
			perror("El ID del asiento es inválido.");
			return -1;
		}
		i++;
	}
	
	/// Cerramos el fichero.
  	if (close(fd) == -1) {
  		perror("Ocurrió un ERROR al cerrar el fichero.");
  		return -1;
  	}
  	
  	return 0;
}

