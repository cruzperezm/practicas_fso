// test_sala.c
// ===============
// Bater�a de pruebas de la biblioteca "sala.h/sala.c"
//

#include <assert.h>
#include <stdio.h>
#include "sala.h"

#define DebeSerCierto(x)	assert(x)
#define DebeSerFalso(x)		assert(!(x))
#define CAPACIDAD 500
#define ID_1 1500

void INICIO_TEST (const char* titulo_test)
{
  printf("********** bater�a de pruebas para %s: ", titulo_test); 
 	// fflush fuerza que se imprima el mensaje anterior
	// sin necesidad de utilizar un salto de l�nea
	fflush(stdout);
}

void FIN_TEST (const char* titulo_test)
{
  printf ("********** hecho\n");
}


void test_ReservaBasica()
{
	int mi_asiento;
  

	INICIO_TEST("Reserva b�sica");
	crea_sala(CAPACIDAD);
	DebeSerCierto(capacidad_sala()==CAPACIDAD);
	DebeSerCierto((mi_asiento=reserva_asiento(ID_1))>=0);
	DebeSerCierto((asientos_libres()+asientos_ocupados())==CAPACIDAD);
	DebeSerCierto(estado_asiento(mi_asiento)>0);
	DebeSerCierto(libera_asiento(mi_asiento)==ID_1);
	DebeSerCierto((asientos_libres()+asientos_ocupados())==CAPACIDAD);
	elimina_sala();
	FIN_TEST("Reserva b�sica");
}


void ejecuta_tests ()
{
	test_ReservaBasica();
	// A�adir nuevos tests 
}

void main()
{
	puts("Iniciando tests...");
	
	ejecuta_tests();
	
	puts("Bater�a de test completa.");
}

