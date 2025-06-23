#include <stdio.h>
#include "sala.h"

void estado_sala() {
    printf("Capacidad: %d\n", capacidad_sala());
    printf("Asientos libres: %d\n", asientos_libres());
    printf("Asientos ocupados: %d\n", asientos_ocupados());
    for (int i = 1; i <= capacidad_sala(); i++) {
        int estado = estado_asiento(i);
        if(estado == 0){
          printf("Asiento %d: Libre\n", i);
        } else{
          printf("Asiento %d: Ocupado\n", i);
        }
    }
}

void sentarse(int id_persona) {
    int asiento = reserva_asiento(id_persona);
    if (asiento != -1) {
        printf("%d ha reservado el asiento %d.\n", id_persona, asiento);
    } else {
        printf("No hay asientos disponibles.\n");
    }
}

void levantarse(int id_asiento) {
    int id_persona = libera_asiento(id_asiento);
    if (id_persona != -1) {
        printf("El asiento %d se ha liberado %d.\n", id_persona, id_asiento);
    } else {
        printf("Error al liberar asiento %d.\n", id_asiento);
    }
}

void reserva_multiple(int n, int *personas) {
    int reserva[n];
    int e = 1;

    for (int i = 0; i < n; i++) {
        reserva[i] = reserva_asiento(personas[i]);
        
        if (reserva[i] == -1) {
            printf("Error al reservar asiento para %d. \n", personas[i]);
            e = 0;
        } else{
          printf("Se ha reservado el asiento %d a %d. \n", reserva[i], personas[i]);
        }
    }
    if (!e) {
        printf("Se revertirán los cambios.\n");
        for (int i = 0; i < n; i++) {
            if (reserva[i] != -1) {
                libera_asiento(reserva[i]);
            }
        }
    }
}
void libera_multiple(int n, int *sitios){
  int libera[n];
  int e = 1;
  for (int i = 0; i < n; i++){
    libera[i] = libera_asiento(sitios[i]);
    if (libera[i] == -1){
      printf("Error al liberar el asiento %d. \n", sitios[i]);
    } else {
      printf("Se ha liberado el asiento %d, previamente ocupado por %d. \n", sitios[i], libera[i]);
    }
  }
  if (!e) {
    printf("Se revertirán los cambios. \n");
    for (int i = 0; i < n; i++){
      if (libera[i] != -1){
        reserva_asiento(libera[i]);
      }
    }
  }
}

void main() {
    printf("Nueva sala de capacidad %d\n", 20);
    crea_sala(20);
    
    sentarse(-1);

    estado_sala();

    sentarse(31);
    sentarse(33);
    sentarse(92);
    sentarse(26);
    sentarse(16);
    sentarse(92);
    sentarse(21);
    sentarse(61);
    sentarse(23);
    sentarse(11);

    estado_sala();

    levantarse(3);

    sentarse(17);

    estado_sala();

    int personas[] = {10, 67, 42};
    reserva_multiple(3, personas);

    int asientos[] = {1, 4, 6};
    libera_multiple(3, asientos);

    estado_sala();
    
    elimina_sala();
}
