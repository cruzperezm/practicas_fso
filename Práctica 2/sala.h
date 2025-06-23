#ifndef SALA_H
#define SALA_H

// Function prototypes
int reserva_asiento(int id_persona);
int libera_asiento(int id_asiento);
int estado_asiento(int id_asiento);
int asientos_libres();
int asientos_ocupados();
int capacidad_sala();
int crea_sala(int cap);
int elimina_sala();

#endif // SALA_H

