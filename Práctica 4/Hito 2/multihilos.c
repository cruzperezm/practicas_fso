#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include "sala.h"
#include "retardo.h"
#define MAX_HILOS 5000
int cap = 5;

void* reserva_libera(void* arg){
  int ids[3];
  int id_hilo = (int)(size_t)arg;
  for (int i = 0; i < 3; i++){
    ids[i] = (rand() % 100)+1;
  }
  for (int i = 0; i < 3; i++){
    int id_reserva = reserva_asiento(ids[i]); 
    if (id_reserva != -1){
      printf("Asiento %d reservado para %d, hilo: %d\n", id_reserva, ids[i], id_hilo);
    pausa_aleatoria(2.0);
    }
    if (id_reserva == -1){
      fprintf(stdout, "Error al reservar el asiento, es posible que ya esté reservado\n");
      pthread_exit(NULL);
  }
    // pausa_aleatoria(2.0); // espera entre 0 y 5 segundos
  }
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < cap; j++){
      if (estado_asiento(i + 1) == ids[i]){
        int liberado = libera_asiento(i+1);
        if (liberado == -1){
          fprintf(stdout, "Error al liberar el asiento, es posible que ya esté libre%d\n", i+1);
          pthread_exit(NULL);
        }
        if (liberado != -1){
          fprintf(stdout, "Asiento %d liberado, hilo %d\n", i +1, id_hilo);
          pausa_aleatoria(5.0);
        }
      }
    }
    }
}

void* estado(void* arg){
  while(1){
    pausa_aleatoria(2.0);
    printf("Estado de la sala:\n");
    for(int i = 1; i <= cap; i++){
      if (estado_asiento(i) == 0) printf("Asiento %d: Libre\n", i);
      if (estado_asiento(i) != 0) printf("Asiento %d: Ocupado\n", i);
    }
  }
}

int main(int argc, char *argv[]){
  if(argc != 2) {
      printf("Formato: ./multihilos <n de hilos>\n");
      return -1;
  }
  crea_sala(cap);
  int n = atoi(argv[1]);
  if(n < 1 || n > MAX_HILOS){
      printf("Debe haber entre 1 y %d hilos\n", MAX_HILOS);
      }
  pthread_t hilos[n+1];
  for(int i = 0; i < n; i++){
    if(pthread_create(&hilos[i], NULL, reserva_libera, (void*)(size_t)(i+1)) == -1){
      fprintf(stderr, "Error al crear el hilo");
      return -1;
    }
  }
  if(pthread_create(&hilos[n], NULL, estado, NULL) == -1){
    fprintf(stderr, "Error al crear el hilo de estado\n");
    return -1;
  }
  for (int i = 1; i < n; i++){
    pthread_join(hilos[i], NULL);
  }
  pthread_cancel(hilos[n]);
  pthread_join(hilos[n], NULL);
  elimina_sala();
  return 0;
}
