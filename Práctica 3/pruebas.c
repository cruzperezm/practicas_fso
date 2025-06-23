
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {

    // Crea el ejecutable para poder usar tanto sala.c como misala.c por si no estuviera
    system("gcc -o misala misala.c sala.h sala.c");
    printf("\n");

    // Prueba de creación de sala y reserva de asientos
    printf("- Prueba de creación de sala y reserva de asientos:\n");
    system("./misala crea -f canarias.dat -c 10");
    system("./misala reserva -f canarias.dat -personas 23 47 95");
    system("./misala reserva -f canarias.dat -personas 33 41 435");
    system("./misala anula -f canarias.dat -personas 41");
    system("./misala reserva -f canarias.dat -personas 233 99 15");
    printf("\n");

    // Prueba de anulación de reservas de asientos
    printf("- Prueba de anulación de reservas de asientos:\n");
    system("./misala anula -f canarias.dat -asientos 2 3");
    system("./misala estado -f canarias.dat");
    system("./misala anula -f canarias.dat -personas 23");
    system("./misala estado -f canarias.dat");
    system("./misala anula -f canarias.dat -personas 41");
    system("./misala reserva -f canarias.dat -personas 233 99 15");
    system("./misala estado -f canarias.dat");
    
    printf("\n");

    return 0;
}
