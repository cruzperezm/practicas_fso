#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include "sala.h"   
static void mostrar_uso(const char *progname) {
    fprintf(stderr, "Uso: %s <comando> [opciones]\n", progname);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "  crea    -f <fichero> -c <capacidad> [-o (sobrescribir)]\n");
    fprintf(stderr, "  reserva -f <fichero> <id_persona1> ...\n");
    fprintf(stderr, "  anula   -f <fichero> [-asientos|-personas] <id1> ...\n");
    fprintf(stderr, "  estado  -f <fichero>\n");
    fprintf(stderr, "  compara <fichero1> <fichero2>\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        mostrar_uso(argv[0]);
        return 1;
    }

    char *comando = argv[1];
    char ruta_fichero[256] = "";
    int capacidad_cli = 0; 
    int opt;
    int overwrite_flag = 0;

    opterr = 0; 

    if (strcmp(comando, "crea") == 0) {
        optind = 2; 
        while ((opt = getopt(argc, argv, "f:c:o")) != -1) {
            switch (opt) {
                case 'f': strncpy(ruta_fichero, optarg, sizeof(ruta_fichero) - 1); ruta_fichero[sizeof(ruta_fichero) - 1] = '\0'; break;
                case 'c': capacidad_cli = atoi(optarg); break;
                case 'o': overwrite_flag = 1; break;
            }
        }

        if (strlen(ruta_fichero) == 0 || capacidad_cli <= 0) {
            fprintf(stderr, "Error: 'crea' requiere -f <fichero> y -c <capacidad_positiva>.\n"); return 1;
        }
        if (optind < argc) { 
            fprintf(stderr, "Error: Argumentos extra para 'crea'.\n"); return 1; 
        }

        if (access(ruta_fichero, F_OK) == 0 && !overwrite_flag) {
            fprintf(stderr, "Error: El fichero '%s' existe y no se uso -o para sobrescribir.\n", ruta_fichero);
            return 1;
        }

        if (crea_sala(capacidad_cli) < 0) { 
            fprintf(stderr, "Error al crear la sala con capacidad %d.\n", capacidad_cli); return 1;
        }
        if (guarda_estado_sala(ruta_fichero) < 0) { 
            fprintf(stderr, "Error al guardar estado en %s.\n", ruta_fichero); return 1;
        }
        printf("Sala creada/actualizada con exito.\n");

    } else if (strcmp(comando, "reserva") == 0 || strcmp(comando, "anula") == 0 || strcmp(comando, "estado") == 0) {
        optind = 2;
        while ((opt = getopt(argc, argv, "f:")) != -1) {
            if (opt == 'f') { strncpy(ruta_fichero, optarg, sizeof(ruta_fichero) - 1); ruta_fichero[sizeof(ruta_fichero) - 1] = '\0'; }
        }

        if (strlen(ruta_fichero) == 0) { 
            fprintf(stderr, "Error: Comando '%s' requiere -f <fichero>.\n", comando); return 1; 
        }

        int capacidad_fich = obtener_capacidad_del_fichero(ruta_fichero);
        if (capacidad_fich < 0) { 
            fprintf(stderr, "Error: No se pudo leer la capacidad del fichero '%s'.\n", ruta_fichero); return 1; 
        }
        
        if (crea_sala(capacidad_fich) < 0) {
            fprintf(stderr, "Error al inicializar la sala en memoria con capacidad %d.\n", capacidad_fich); return 1;
        }

        if (strcmp(comando, "reserva") == 0) {
            if (optind >= argc) { fprintf(stderr, "Error: 'reserva' requiere IDs de persona.\n"); return 1; }

            if (recupera_estado_sala(ruta_fichero) < 0) { 
                fprintf(stderr, "Error al recuperar estado de %s.\n", ruta_fichero); return 1; 
            }

            int num_p = argc - optind; 
            int *p_ids = malloc(num_p * sizeof(int)); 
            int *as_r = malloc(num_p * sizeof(int));
            if (!p_ids || !as_r) { 
                fprintf(stderr, "Error: Memoria.\n"); 
                if(p_ids) free(p_ids); 
                if(as_r) free(as_r); 
                return 1; 
            }

            for(int i = 0; i < num_p; i++) p_ids[i] = atoi(argv[optind + i]);
            
            int c_ok = 0; 
            int err = 0;
            for(int i = 0; i < num_p; i++){
                int res = reserva_asiento(p_ids[i]); 
                if(res == -1){ err = 1; fprintf(stderr,"Error reservando para ID %d\n", p_ids[i]); break; } 
                as_r[c_ok++] = res;
            }

            if(err){ free(p_ids); free(as_r); return 1; }

            if(c_ok > 0 && guarda_estado_sala(ruta_fichero) < 0){
                fprintf(stderr,"Error guardando estado parcial de reserva.\n"); 
                free(p_ids); free(as_r); return 1;
            }
            printf("%d Reservas procesadas.\n", c_ok); 
            free(p_ids); free(as_r);
        
        } else if (strcmp(comando, "anula") == 0) {
            char tipo_anul[32] = ""; 
      printf("%d", optind);
            if(optind >= argc){ fprintf(stderr,"Error: 'anula' requiere tipo (-asientos/-personas) e IDs.\n"); return 1; }
            
            strncpy(tipo_anul, argv[optind], sizeof(tipo_anul)-1); tipo_anul[sizeof(tipo_anul)-1] = '\0'; 
            optind++;
            if(optind >= argc){ fprintf(stderr,"Error: 'anula' requiere IDs.\n"); return 1; }
            
            int num_ids = argc - optind; 
            int *ids_cli = malloc(num_ids * sizeof(int)); 
            if(!ids_cli){ fprintf(stderr,"Error: Memoria.\n"); return 1; }
            for(int i = 0; i < num_ids; i++) ids_cli[i] = atoi(argv[optind + i]);

            int *as_afect = NULL; 
            int c_afect = 0; 
            int cap_actual = capacidad_sala(); 

            if(strcmp(tipo_anul, "-asientos") == 0){
                if(recupera_estado_parcial_sala(ruta_fichero, num_ids, ids_cli) < 0){ // ids_cli son asientos
                    fprintf(stderr,"Error recuperando parcialmente asientos.\n"); free(ids_cli); return 1;
                }
                as_afect = malloc(num_ids * sizeof(int)); 
                if(!as_afect){ fprintf(stderr,"Error: Memoria.\n"); free(ids_cli); return 1; }
                for(int i = 0; i < num_ids; i++) {
                    if(libera_asiento(ids_cli[i]) != -1) as_afect[c_afect++] = ids_cli[i];
                }
            } else if (strcmp(tipo_anul, "-personas") == 0) {
                // La sala ya fue inicializada con crea_sala(capacidad_fich) y su estado puede haber sido
                // cargado si recupera_estado_sala se llamara aquí (pero no es necesario antes de refrescar asientos específicos).
                // Primero, recuperamos el estado completo para saber qué asientos tienen esas personas
                if (recupera_estado_sala(ruta_fichero) < 0) { // Carga completa para saber mapeo persona->asiento
                     fprintf(stderr,"Error recuperando estado completo para anula -personas.\n"); free(ids_cli); return 1;
                }

                int *as_refresh = malloc(cap_actual * sizeof(int)); 
                if(!as_refresh && cap_actual > 0){ fprintf(stderr,"Error: Memoria.\n"); free(ids_cli); return 1; }
                int c_refresh = 0;

                if(cap_actual > 0){
                    for(int p = 0; p < num_ids; p++) { // num_ids son personas aquí
                        for(int s = 1; s <= cap_actual; s++) {
                            if(estado_asiento(s) == ids_cli[p]){
                                int f = 0; for(int k = 0; k < c_refresh; ++k) if(as_refresh[k] == s) f = 1; 
                                if(!f) as_refresh[c_refresh++] = s;
                            }
                        }
                    }
                }

                if(c_refresh > 0 && recupera_estado_parcial_sala(ruta_fichero, c_refresh, as_refresh) < 0){
                    fprintf(stderr,"Error refrescando parcialmente asientos de personas.\n"); 
                    free(ids_cli); if(as_refresh) free(as_refresh); return 1;
                }
                if(as_refresh) free(as_refresh);

                as_afect = malloc(cap_actual * sizeof(int)); 
                if(!as_afect && cap_actual > 0){ fprintf(stderr,"Error: Memoria.\n"); free(ids_cli); return 1; }
                
                if(cap_actual > 0){
                    for(int p = 0; p < num_ids; p++) {
                        for(int s = 1; s <= cap_actual; s++) {
                            if(estado_asiento(s) == ids_cli[p] && libera_asiento(s) != -1){
                                int f = 0; for(int k = 0; k < c_afect; ++k) if(as_afect[k] == s) f = 1; 
                                if(!f) as_afect[c_afect++] = s;
                            }
                        }
                    }
                }
            } else { 
                fprintf(stderr,"Error: Tipo de anulacion '%s' desconocido.\n", tipo_anul); 
                free(ids_cli); return 1; 
            }
            free(ids_cli);

            if(c_afect > 0 && guarda_estado_parcial_sala(ruta_fichero, c_afect, as_afect) < 0){
                fprintf(stderr,"Error guardando anulaciones parciales.\n"); 
                if(as_afect) free(as_afect); return 1;
            }
            printf("%d Anulaciones procesadas.\n", c_afect); 
            if(as_afect) free(as_afect);

        } else if (strcmp(comando, "estado") == 0) {
            if(optind < argc){ fprintf(stderr,"Error: 'estado' no toma argumentos extra despues de -f <fichero>.\n"); return 1; }
            
            // La sala ya fue inicializada con crea_sala(capacidad_fich)
            if(recupera_estado_sala(ruta_fichero) < 0){ 
                fprintf(stderr,"Error recuperando estado desde %s\n", ruta_fichero); return 1; 
            }

            printf("Estado Sala (Cap: %d):\n", capacidad_sala());
            for(int i = 1; i <= capacidad_sala(); i++) {
                printf("Asiento %d: %d\n", i, estado_asiento(i));
            }
            if(capacidad_sala() == 0) printf(" (Sala vacia)\n");
        }

    } else if (strcmp(comando, "compara") == 0) {
        const char *ruta1 = argv[2];
        const char *ruta2 = argv[3];

        crea_sala(obtener_capacidad_del_fichero(ruta1));
        if (recupera_estado_sala(ruta1) < 0) {
            fprintf(stderr, "Error al recuperar archivo 1: %s\n", ruta1);
            perror("recupera_estado_sala ruta1");
            return -1;
        }

        int cap1 = capacidad_sala();
        int *estado1 = malloc(cap1 * sizeof(int));
        if (!estado1) {
            fprintf(stderr, "Error al reservar memoria para estado1\n");
            return -1;
        }

        for (int i = 0; i < cap1; i++) estado1[i] = estado_asiento(i + 1);
        
        crea_sala(obtener_capacidad_del_fichero(ruta2));
        if (recupera_estado_sala(ruta2) < 0) {
            fprintf(stderr, "Error al recuperar archivo 2: %s\n", ruta2);
            perror("recupera_estado_sala ruta2");
            free(estado1);
            return -1;
        }

        int cap2 = capacidad_sala();
        if (cap1 != cap2) {
            fprintf(stderr, "Las capacidades no coinciden: %d vs %d\n", cap1, cap2);
            free(estado1);
            return -1;
        }

        for (int i = 0; i < cap1; i++) {
            if (estado1[i] != estado_asiento(i + 1)) {
                fprintf(stderr, "Diferencia en asiento %d: %d != %d\n", i + 1, estado1[i], estado_asiento(i + 1));
                free(estado1);
                printf("Archivos diferentes\n");
                return 1;
            }
        }

        free(estado1);
        printf("Archivos iguales\n");
        return 0;

    } else {
        fprintf(stderr, "Error: Comando '%s' no reconocido.\n", comando);
        mostrar_uso(argv[0]);
        return 1;
    }
    return 0;
}
