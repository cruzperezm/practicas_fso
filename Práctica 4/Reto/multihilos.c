#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "sala.h"
#include "retardo.h"


#define MAX_HILOS 5000
int cap = 5;

static pthread_mutex_t mutex;
static pthread_cond_t cond1;
static pthread_cond_t cond2;

int n, m;
int hombres = 0;
int mujeres = 0;
int total_gratis = 0;
int max_simultaneos = 0;
int reservas_activas = 0;
int liberaciones_activas = 0;

typedef struct {
  int id;
  char tipo;
} datos_hilo;

void* reserva(void* arg){
  int id_hilo_reserva = (int)(size_t)arg;
  for (int i = 0; i < 3; i++){
    pthread_mutex_lock(&mutex);
    while(asientos_libres() == 0) pthread_cond_wait(&cond1, &mutex);
    int id_reserva = reserva_asiento((rand() % 100) + 1); 
    if (id_reserva != -1){
      printf("Asiento %d reservado, hilo de reserva %d\n", id_reserva, id_hilo_reserva);
      pthread_cond_signal(&cond2);
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
    while(asientos_ocupados() == 0) pthread_cond_wait(&cond2, &mutex);
    for (int j = 1; j <= cap; j++){
    if (estado_asiento(j) > 0){
    if (libera_asiento(j) != -2){
        fprintf(stdout, "Asiento %d liberado, hilo de libera %d\n", j, id_hilo_libera);
        pthread_cond_signal(&cond1);
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
void* hilo_sexo(void* arg) {
    datos_hilo *d = (datos_hilo*)arg;
    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&mutex);
        while (asientos_libres() == 0 || 
              (asientos_ocupados() >= 10 &&
              ((d->tipo == 'H' && hombres > 0.6 * asientos_ocupados()) ||
               (d->tipo == 'M' && mujeres > 0.6 * asientos_ocupados())))) {
            if (d->tipo == 'H') pthread_cond_wait(&cond1, &mutex);
            else pthread_cond_wait(&cond2, &mutex);
        }

        int res = reserva_asiento(d->id);
        if (res != -1) {
            if (d->tipo == 'H') hombres++;
            else mujeres++;
            printf("R1: Hilo %d (%c) reservó %d\n", d->id, d->tipo, res);
            pthread_cond_signal(&cond1);
            pthread_cond_signal(&cond2);
        }
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    return NULL;
}

void* libera_sexo(void* arg) {
    for (int i = 0; i < 6; i++) {
        pthread_mutex_lock(&mutex);
        for (int j = 1; j <= cap; j++) {
            int persona = estado_asiento(j);
            if (persona > 0) {
                libera_asiento(j);
                if (persona % 2 == 0) hombres--;
                else mujeres--;
                printf("R1: Liberado asiento %d (persona %d)\n", j, persona);
                pthread_cond_broadcast(&cond1);
                pthread_cond_broadcast(&cond2);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    return NULL;
}

void* hilo_gratis(void* arg) {
    datos_hilo *d = (datos_hilo*)arg;
    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&mutex);
        while (total_gratis >= 0.1 * cap) pthread_cond_wait(&cond1, &mutex);
        int res = reserva_asiento(d->id);
        if (res != -1) {
            total_gratis++;
            printf("R2: Hilo %d (G) reservó %d\n", d->id, res);
        }
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    return NULL;
}

void* hilo_pago(void* arg) {
    datos_hilo *d = (datos_hilo*)arg;
    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&mutex);
        int res = reserva_asiento(d->id);
        if (res != -1)
            printf("R2: Hilo %d (P) reservó %d\n", d->id, res);
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    return NULL;
}

void* libera_gratis(void* arg) {
    for (int i = 0; i < 6; i++) {
        pthread_mutex_lock(&mutex);
        for (int j = 1; j <= cap; j++) {
            int persona = estado_asiento(j);
            if (persona > 0) {
                libera_asiento(j);
                if (persona % 3 == 0) total_gratis--;
                printf("R2: Liberado asiento %d (persona %d)\n", j, persona);
                pthread_cond_signal(&cond1);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    return NULL;
}

void* hilo_reserva_limite(void* arg) {
    datos_hilo *d = (datos_hilo*)arg;
    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&mutex);
        while (reservas_activas >= max_simultaneos)
            pthread_cond_wait(&cond1, &mutex);
        reservas_activas++;

        int res = reserva_asiento(d->id);
        if (res != -1)
            printf("R3: Hilo %d (R) reservó %d\n", d->id, res);

        reservas_activas--;
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    return NULL;
}

void* hilo_libera_limite(void* arg) {
    for (int i = 0; i < 3; i++) {
        pthread_mutex_lock(&mutex);
        while (liberaciones_activas >= max_simultaneos)
            pthread_cond_wait(&cond2, &mutex);
        liberaciones_activas++;

        for (int j = 1; j <= cap; j++) {
            if (estado_asiento(j) > 0) {
                libera_asiento(j);
                printf("R3: Liberado asiento %d\n", j);
                break;
            }
        }

        liberaciones_activas--;
        pthread_cond_signal(&cond2);
        pthread_mutex_unlock(&mutex);
        pausa_aleatoria(2.0);
    }
    return NULL;
}
int main(int argc, char *argv[]){
  
  crea_sala(cap);
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond1, NULL);
  pthread_cond_init(&cond2, NULL);
    if(argc == 3 && isdigit(argv[1][0])){
  n = atoi(argv[1]);
  m = atoi(argv[2]);
        if(n < 1 || n + m > MAX_HILOS || m < 1){
            printf("Debe haber entre 1 y %d hilos\n", MAX_HILOS);
            return -1;
        }
        pthread_t hilos_reserva[n];
        pthread_t hilos_libera[m];
        for(int i = 0; i < n; i++) pthread_create(&hilos_reserva[i], NULL, reserva, (void*)(size_t)(i+1));
        for(int i = 0; i < m; i++) pthread_create(&hilos_libera[i], NULL, libera, (void*)(size_t)(i+1));

        pthread_t hilo_estado;
        pthread_create(&hilo_estado, NULL, estado, NULL);

        for (int i = 0; i < n; i++) pthread_join(hilos_reserva[i], NULL);
        for (int i = 0; i < m; i++) pthread_join(hilos_libera[i], NULL);

        pthread_cancel(hilo_estado);
        pthread_join(hilo_estado, NULL);

    } else if (argc >= 2) {
        if (strcmp(argv[1], "sexo") == 0 && argc == 4) {
            int nh = atoi(argv[2]);
            int nm = atoi(argv[3]);
            pthread_t th[nh + nm], tl;
            datos_hilo datos[nh + nm];

            for (int i = 0; i < nh; i++) {
                datos[i].id = 2 * (i + 1); datos[i].tipo = 'H';
                pthread_create(&th[i], NULL, hilo_sexo, &datos[i]);
            }
            for (int i = 0; i < nm; i++) {
                datos[nh + i].id = 2 * (i + 1) - 1; datos[nh + i].tipo = 'M';
                pthread_create(&th[nh + i], NULL, hilo_sexo, &datos[nh + i]);
            }
            pthread_create(&tl, NULL, libera_sexo, NULL);
            for (int i = 0; i < nh + nm; i++) pthread_join(th[i], NULL);
            pthread_join(tl, NULL);

        }  else if (strcmp(argv[1], "gratis") == 0 && argc == 4) {
            int ng = atoi(argv[2]);
            int np = atoi(argv[3]);
            pthread_t tg[ng + np], tl;
            datos_hilo datos[ng + np];

            for (int i = 0; i < ng; i++) {
                datos[i].id = 3 * (i + 1); datos[i].tipo = 'G';
                pthread_create(&tg[i], NULL, hilo_gratis, &datos[i]);
            }
            for (int i = 0; i < np; i++) {
                datos[ng + i].id = 10 + i; datos[ng + i].tipo = 'P';
                pthread_create(&tg[ng + i], NULL, hilo_pago, &datos[ng + i]);
            }
            pthread_create(&tl, NULL, libera_gratis, NULL);
            for (int i = 0; i < ng + np; i++) pthread_join(tg[i], NULL);
            pthread_join(tl, NULL);

        } else if (strcmp(argv[1], "limite") == 0 && argc == 5) {
            max_simultaneos = atoi(argv[2]);
            int nr = atoi(argv[3]);
            int nl = atoi(argv[4]);
            pthread_t tr[nr], tl[nl];
            datos_hilo datos[nr];

            for (int i = 0; i < nr; i++) {
                datos[i].id = i + 1; datos[i].tipo = 'R';
                pthread_create(&tr[i], NULL, hilo_reserva_limite, &datos[i]);
            }
            for (int i = 0; i < nl; i++) pthread_create(&tl[i], NULL, hilo_libera_limite, NULL);

            for (int i = 0; i < nr; i++) pthread_join(tr[i], NULL);
            for (int i = 0; i < nl; i++) pthread_join(tl[i], NULL);

        } else {
            printf("Modo desconocido\n");
        }
    } else {
        printf("Uso:\n ./multihilos <reserva> <libera>\n ./multihilos sexo ...\n ./multihilos gratis ...\n ./multihilos limite ...\n");
    }
    elimina_sala();
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond1);
    pthread_cond_destroy(&cond2);
    return 0;
}

