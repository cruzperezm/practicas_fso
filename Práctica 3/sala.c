#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

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
    if (sala == NULL || id_asiento > capacidad || id_asiento < 0) {
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
        capacidad = ocupados = libres = 0;
        return 0;
    }
    return -1;
}


// *******************************************************************************
// 				PARTE DE LA PRÁCTICA 3
// *******************************************************************************
int obtener_capacidad_del_fichero(const char* ruta_fichero) {
	if (!ruta_fichero) return -1;
	
	int fd = open(ruta_fichero, O_RDONLY);
	if(fd < 0) return -1;
	
	int capacidad_fichero;
	if (read(fd, &capacidad_fichero, sizeof(capacidad_fichero)) != sizeof(capacidad_fichero)) {
		close(fd);
		return -1;
	}
	if (close(fd)) {
		perror("Error al cerrar el fichero.\n");
		return -1;
	}
	return capacidad_fichero;
}

int guarda_estado_sala (const char* ruta_fichero) {
	if (!ruta_fichero  || !sala) {
        	errno = EINVAL;
        	printf("ERROR: puntero del fichero o sala nulo.\n", strerror(errno));
        	return -1;
    	}
    	
  	// O_TRUNC -> Vacía el fichero si éste ya existía.
	int fd = open(ruta_fichero, O_WRONLY | O_CREAT | O_TRUNC, 0666);
	if (fd < 0) {
		perror("Se ha producido un error al leer el archivo.\n");
		return -1;
	}
	
	// Atributo "capacidad" necesario para reconstruir la sala.
	ssize_t b_escritos = write(fd, &capacidad, sizeof(int));
	// b_escritos != sizeof(int) -> verificamos que se escriba la cantidad correcta de bytes que deseamos
  	if (b_escritos < 0 || b_escritos != sizeof(int)) {
  		perror("Error al escribir la capacidad de la sala en el fichero.");
  		return -1;
  	}
	
  	// Escribimos el estado de los asientos por bloques de escritura.
  	
  	size_t total_bytes = (size_t)capacidad * sizeof(int);
    	struct stat file_info;
    	size_t bloque_size = 0;
    	if (fstat(fd, &file_info) < 0) {
      		  perror("Advertencia en guarda_estado_sala: No se pudo obtener fstat (st_blksize), usando tamaño de bloque por defecto para escribir");
      		  bloque_size = 4096; 
    	}
        bloque_size = (size_t)file_info.st_blksize;
    
    	const char *buffer = (const char *)sala;
    	size_t bytes_escritos = 0;

    	while (bytes_escritos < total_bytes) {
    		size_t bytes_a_escribir = bloque_size;
      		if (bytes_escritos + bytes_a_escribir > total_bytes) {
           		bytes_a_escribir = total_bytes - bytes_escritos;
        	}

        	ssize_t b_asientos = write(fd, buffer + bytes_escritos, bytes_a_escribir);
       		
        	if (b_asientos < 0) {
          		perror("Error en guarda_estado_sala al escribir un trozo de los estados de los asientos");
            		close(fd);
            		return -1;
        	}
        	
        	bytes_escritos += (size_t)b_asientos;
    	}

    	if (close(fd) < 0) {
       		perror("Error en guarda_estado_sala al cerrar el fichero después de escribir");
      		return -1; 
    	}
	return 0;
}

int recupera_estado_sala (const char* ruta_fichero) {
	if (!ruta_fichero  || !sala) {
        	errno = EINVAL;
        	printf("ERROR: puntero del fichero o sala nulo.\n", strerror(errno));
       		return -1;
       	}

    	int fd = open(ruta_fichero, O_RDONLY);
    	if (fd < 0) {
       	 	perror("Error al abrir el fichero.");
       	 	close(fd);
        	return -1;
    	}

    	int capacidad_sala_fichero;
    	if (read(fd, &capacidad_sala_fichero, sizeof(capacidad_sala_fichero)) != sizeof(capacidad_sala_fichero)) {
        	perror("Error al leer la capacidad de la sala.");
		close(fd);
		return -1;
    	}
    
    	if (capacidad != capacidad_sala_fichero) {
        	fprintf(stderr, "La capacidad de la sala existente no corresponde con la del fichero.\n");
       	printf("cap sala: %d, cap fichero: %d", capacidad, capacidad_sala_fichero);
        	close(fd);
        	return -1;
    	}

    	// Obtenemos la información del archivo.
    	struct stat file_info;
    	if (fstat(fd, &file_info) < 0) {
       	perror("No se pudo obtener la información del archivo.");
        	close(fd);
        	return -1;
    	}
    
    // Cargamos el tamaño del bloque optimo
    	size_t bloque_size = 0; 
    	if (fstat(fd, &file_info) < 0) {
    		perror("Advertencia en guarda_estado_sala: No se pudo obtener fstat (st_blksize), usando tamaño de bloque por defecto para escribir");
      		bloque_size = 4096; 
    	}
    	bloque_size = (size_t)file_info.st_blksize; // Tamaño del bloque
    
    	// Recuperamos la información del archivo por bloque de Bytes.
    	char *buffer = (char*)sala;
    	size_t total_bytes = (size_t)capacidad * sizeof(int); // Capacidad del archivo
    	size_t bytes_leidos = 0;
    
    	while (bytes_leidos < total_bytes) {
    		size_t bytes_a_leer = bloque_size;
    	
    		if (bytes_leidos + bytes_a_leer > total_bytes) {
    			bytes_a_leer = total_bytes - bytes_leidos;
    		}
    	
    		ssize_t b_asientos = read(fd, buffer + bytes_leidos, bytes_a_leer);
    		if (b_asientos < 0) {
    			close(fd);
    			return -1;
    		}
    	
    		bytes_leidos += (size_t)b_asientos;
    	}

    	// Recalculamos los asientos.	
    	ocupados = 0;
    	libres = capacidad;
    	for (int i = 0; i < capacidad; i++) {
    	   	if (sala[i] != 0) {
            	ocupados++;
            	libres--;
        	}
    	}

    	if (close(fd) == -1) {
        	perror("Ocurrió un ERROR al cerrar el fichero.");
        	return -1;
    	}

    	return 0;
}

int guarda_estado_parcial_sala (const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
	
	if (!sala || !ruta_fichero) {errno = EINVAL; return - 1;}
	
	int fd = open(ruta_fichero, O_RDWR);
	if (fd < 0) {
		perror("Error al abrir el fichero.");
		return -1;
	}
	
	// leemos el fichero
	int b_capacidad_sala = read(fd, &capacidad, sizeof(int));
	if (b_capacidad_sala < 0) {
		perror("Error al leer la capacidad de la sala desde el fichero. guarda_parcial");
		close(fd);
		return -1;
	}
	
	// Comprobamos la dimensiones de las salas con las dadas por el fichero.
	if (capacidad != b_capacidad_sala) {
		perror("La capacidad de la sala actual no coincide con la del fichero.");
		return -1;
	}
	
	size_t i = 0;
	int ids_invalidos = 0;
	while (i <= num_asientos) {
		int id_asiento = id_asientos[i];
		
		if (id_asiento < 1 || id_asiento > capacidad) {
			fprintf(stderr, "Id del asiento %d es inválido, id pesona: %d\n", id_asiento, estado_asiento(i));
			ids_invalidos++;
			i++;
			continue;
		}
		
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

		i++;
	}
	
	printf("Total de ID's inválidos: %d.\n", ids_invalidos);
	/// Cerramos el fichero.
  	if (close(fd) == -1) {
  		perror("Ocurrió un ERROR al cerrar el fichero.");
  		return -1;
  	}
  	
  	return 0;
	
}

int recupera_estado_parcial_sala (const char* ruta_fichero, size_t num_asientos, int* id_asientos) {
	if (!ruta_fichero || !sala ) {
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
	int ids_invalidos = 0;
	while (i < num_asientos) {
		int id_asiento = id_asientos[i];
		
		if (id_asiento < 1 || id_asiento > capacidad_sala_fichero) {
			fprintf(stderr, "Id del asiento %d es inválido\n", id_asiento);
			ids_invalidos++;
			i++;
			continue;
		}
		
		
		/*
		primer "sizeof(int)" -> es donde se almacena la capacidad de la sala en el array del fichero.
		(id_asiento - 1)*sizeof(int) -> la información de los asientos.
		(id_asiento - 1) -> para hacer que los índices empiecen desde 0 y no de 1.
		*/
		off_t offset = sizeof(int) + (off_t)(id_asiento - 1)*sizeof(int);
			
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
		
		i++;
	}
	
	ocupados = 0;
    	libres = capacidad;
   	for (int i = 0; i < capacidad; i++) {
        	if (sala[i] != 0) {
            	ocupados++;
            	libres--;
        	}
    	}
	
	/// Cerramos el fichero.
  	if (close(fd) == -1) {
  		perror("Ocurrió un ERROR al cerrar el fichero.");
  		return -1;
  	}
  	
  	return 0;
}
