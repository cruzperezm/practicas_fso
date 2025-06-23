#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define SIZE 50  // Sucursales máximas
#define SIZE_NOMBRES 30 // Cantidad máximas de carácteres para los nombres de las salas

pid_t procesos[SIZE];
char ciudades[SIZE][SIZE_NOMBRES];  // Array de ciudades
int i = 0;

void manejador_signals_sala(int sig, siginfo_t *info, void *context) {
	int status;
	pid_t pid = info -> si_pid;

	// Estado de la terminación del proceso hijo usando 'waitpid'
	while (waitpid(pid, &status, WNOHANG) > 0) {
		for (int j = 0; j < i; j++) { // Recorrero los PID
			if (procesos[j] == pid) {
				// Verificamos si el proceso terminó correctamente.
				if (WIFEXITED(status)) { // pag 152
					int estado_salida = WEXITSTATUS(status); // Obtemos el código de salida del proceso hijo
					if (estado_salida == 0) { // Sin asietos libres
						printf("La sucursal '%s' con PID='%d' cerró con todos los asientos ocupados.\n", ciudades[j], pid);
					} else if (estado_salida == 1) { // Con asientos libres
						printf("La sucursal '%s' con PID='%d' cerró con asientos libres.\n", ciudades[j], pid);
					} else { // Salida abrupta, per se captura el estado final.
						printf("La sucursal '%s' con PID='%d' terminó con estado %d.\n", ciudades[j], pid, estado_salida);
					}
				} else {
					printf("La sucursal '%s' con PID='%d' terminó de forma inesperada.\n", ciudades[j], pid);
				}
				procesos[j] = 0; // Marcamos la sucursal como terminada en el array.
				break;
			}
		}
	} //if (waitpid(pid, &status, WNOHANG) == -1) {
		//perror("Error al verificar el estado con waitpid.\n");
	//}
}

void crea_sucursal(const char* ciudad, int capacidad);
void crea_sucursal(const char* ciudad, int capacidad) {
    pid_t proceso = fork();

    if (proceso == -1) {
        perror("Error al crear proceso");  // Control de error al hacer fork
        return;
    }

    if (proceso == 0) {
        char char_capacidad[SIZE];
        sprintf(char_capacidad, "%d", capacidad);

        // Ejecuta sala con argumentos
        execlp("gnome-terminal", "gnome-terminal", "--wait", "--", "./sala", ciudad, char_capacidad, NULL);
        
        // Si execlp falla
        perror("execlp falló");
        exit(EXIT_FAILURE);
    } else {
        procesos[i] = proceso;
        strcpy(ciudades[i], ciudad); // Guardamos el nombre de la ciudad
        i++;
    }
}

int main() {
    // Manejador SIGNAL para las sucursales.
    struct sigaction sa;	// pag 353
    sa.sa_sigaction = manejador_signals_sala;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_SIGINFO;
    
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    	perror("sigaction");
    }

    char nombresala[SIZE_NOMBRES];
    int capacidad;
    
    printf("----------------------------\n");
    printf("- Para terminar la ejecución del programa escriba 'salir'.\n");
    printf("- Los nombres no pueden tener espacios vacios, como\n");
    printf("  separador utilice '-' o '_'\n");
    printf("----------------------------\n");

    while (1) {
        printf("Introduce el nombre de la ciudad: ");
        if (scanf("%s", nombresala) != 1) {
            perror("Error al leer nombre");
            continue;
        } 
        if (strcmp(nombresala, "salir") == 0) break;

	// Verificamos que el nombre de la sucursal no se encuentre repetido
        int sucursal_repetida = 0;
        for (int j = 0; j < i; j++) {
        	if (strcmp(ciudades[j], nombresala) == 0 && procesos[j] != 0) {
        		sucursal_repetida = 1;
        		break;
        	}
        }
        if (sucursal_repetida) {
        	printf("Ya existe una sucursal con ese nombre.\n");
        	continue;
        }

	// Solicitamos la capacidad de la sucursal.
        printf("Introduce la capacidad de la sucursal: ");
        if (scanf("%d", &capacidad) != 1) {
            perror("Error al leer capacidad");
            continue;
        }
        
        if (capacidad <= 0) {
        	printf("La capacidad debe ser mayor a 0.\n");
        	continue;
        }
        
        // Se crea la sucursal.
        crea_sucursal(nombresala, capacidad);
        printf("Nueva sucursal '%s' creada con capacidad %d.\n", nombresala, capacidad);
        printf("---\n");
	
	// Avisamos cuando se haya alcanzado el número máximo de sucursales.
        if (i >= SIZE) {
            printf("Se alcanzó el número máximo de sucursales (%d)\n", SIZE);
            break;
        }
    }
    
    // Se espera a que terminen los procesos activos
    while(1) {
    	int activos = 0;
    	for (int j = 0; j < i; j++) {
    		if (procesos[j] != 0) activos++;
    	}
    	
        if (activos == 0) break;
        sleep(1); // Esperamos 1seg antes de volver a comprobar
    }

    return 0;
}









