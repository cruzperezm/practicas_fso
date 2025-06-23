#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <pthread.h>
#include "sala.h"
#include "retardo.h"
#define MAX_HILOS 5000
int cap = 5;

static pthread_mutex_t mutex;
static pthread_cond_t c_reserva;
static pthread_cond_t c_libera;

int n, m;

void* reserva(void* arg){
  int id_hilo_reserva = (int)(size_t)arg;
  for (int i = 0; i < 3; i++){
    pthread_mutex_lock(&mutex);
    while(asientos_libres() == 0) pthread_cond_wait(&c_reserva, &mutex);
    int id_reserva = reserva_asiento((rand() % 100) + 1); 
    if (id_reserva != -1){
      printf("Asiento %d reservado, hilo de reserva %d\n", id_reserva, id_hilo_reserva);
      pthread_cond_signal(&c_libera);
      pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    if (id_reserva == -1){
      fprintf(stdout, "Error al reservar un asiento, es posible que no haya sitio\n");
  pthread_mutex_unlock(&mutex);
  }
  }
}
void* libera(void* arg){
  int id_hilo_libera = (int)(size_t)arg;
  for (int i = 0; i < 3; i++){
  pthread_mutex_lock(&mutex);
    while(asientos_ocupados() == 0) pthread_cond_wait(&c_libera, &mutex);
    for (int j = 1; j <= cap; j++){
    if (estado_asiento(j) > 0){
    if (libera_asiento(j) != -2){
        fprintf(stdout, "Asiento %d liberado, hilo de libera %d\n", j, id_hilo_libera);
        pthread_cond_signal(&c_reserva);
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
      }
      else{
        fprintf(stdout, "Error al liberar el asiento %d, es posible que ya esté libre\n", j);
        pthread_mutex_unlock(&mutex);
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
      if (estado_asiento(i) > 0) printf("Asiento %d: Ocupado\n", i);
    }
  }
}

int main(int argc, char *argv[]){
  if(argc != 3) {
      printf("Formato: ./multihilos <n de hilos de reserva> <n de hilos de libera>\n");
      return -1;
  }
  crea_sala(cap);
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&c_libera, NULL);
  pthread_cond_init(&c_reserva, NULL);
  n = atoi(argv[1]);
  m = atoi(argv[2]);
  if(n < 1 || n + m > MAX_HILOS || m < 1){
      printf("Debe haber entre 1 y %d hilos\n", MAX_HILOS);
    return -1;
      }
  pthread_t hilos_reserva[n];
  pthread_t hilos_libera[m];
  for(int i = 0; i < n; i++){
    if(pthread_create(&hilos_reserva[i], NULL, reserva, (void*)(size_t)(i+1)) != 0){
      fprintf(stderr, "Error al crear el hilo de reserva");
      return -1;
    }
  }
  for(int i = 0; i < m; i ++){
    if(pthread_create(&hilos_libera[i], NULL, libera, (void*)(size_t)(i+1)) != 0){
      fprintf(stderr, "Error al crear el hilo de libera");
      return -1;
    }
  }
  pthread_t hilo_estado;
  if(pthread_create(&hilo_estado, NULL, estado, NULL) != 0){
    fprintf(stderr, "Error al crear el hilo de estado\n");
    return -1;
  }
  for (int i = 0; i < n; i++){
    pthread_join(hilos_reserva[i], NULL);
  }
  for (int i = 0; i < m; i++){
    pthread_join(hilos_libera[i], NULL);
  }
  pthread_cancel(hilo_estado);
  pthread_join(hilo_estado, NULL);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&c_libera);
  pthread_cond_destroy(&c_reserva);
  elimina_sala();
  return 0;
}
