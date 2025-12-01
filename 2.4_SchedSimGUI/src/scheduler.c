#include "../include/scheduler.h"
#include "../include/algorithms.h"
#include "../include/metrics.h"
#include <string.h>

// Función de ayuda para la carga de procesos desde archivo (Parte 4, Bonus)
int load_processes_from_file(const char *filename, process_t *processes, int max_n) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error al cargar workload");
        return 0;
    }

    char line[100];
    int count = 0;
    // Leer línea por línea, ignorando comentarios
    while (fgets(line, sizeof(line), fp) && count < max_n) {
        if (line[0] == '#') continue;

        process_t *p = &processes[count];
        // Formato esperado: arrival burst priority
        if (sscanf(line, "%d %d %d", 
                   &p->arrival_time, 
                   &p->burst_time, 
                   &p->priority) == 3) {
            p->pid = count + 1; // Asignar PID incremental
            count++;
        }
    }
    fclose(fp);
    return count;
}


// Función que ejecuta TODOS los algoritmos para un proceso dado y genera el reporte
void run_all_algorithms_for_report(process_t *original_processes, int n) {
    const char *algo_names[] = {"FIFO", "SJF", "STCF", "RR (q=2)", "MLFQ"};
    const int NUM_ALGOS = 5;
    metrics_t all_results[NUM_ALGOS];
    
    // Necesitas una copia de los procesos para cada simulación
    process_t temp_processes[MAX_PROCESSES];
    timeline_event_t temp_timeline[MAX_TIMELINE_EVENTS];
    int total_time;
    
    // Configuración de RR y MLFQ (ejemplo)
    int RR_QUANTUM = 2;
    mlfq_config_t mlfq_conf = {
        .num_queues = 3,
        .quantums = {2, 4, 8}, // Q0=2, Q1=4, Q2=8
        .boost_interval = 10
    };

    for (int i = 0; i < NUM_ALGOS; i++) {
        // 1. Resetear y copiar procesos
        memcpy(temp_processes, original_processes, n * sizeof(process_t));
        memset(temp_timeline, 0, MAX_TIMELINE_EVENTS * sizeof(timeline_event_t));
        
        // 2. Ejecutar Algoritmo
        switch (i) {
            case 0: schedule_fifo(temp_processes, n, temp_timeline); break;
            case 1: schedule_sjf(temp_processes, n, temp_timeline); break;
            case 2: schedule_stcf(temp_processes, n, temp_timeline); break;
            case 3: schedule_rr(temp_processes, n, RR_QUANTUM, temp_timeline); break;
            case 4: schedule_mlfq(temp_processes, n, &mlfq_conf, temp_timeline); break;
        }

        // 3. Calcular tiempo total y métricas
        int timeline_len = 0; // Necesitas contar la longitud real en los algoritmos
        for(int j=0; j<MAX_TIMELINE_EVENTS; j++) {
            if(temp_timeline[j].pid == 0) { // PID 0 es el marcador de fin
                total_time = temp_timeline[j].time;
                timeline_len = j;
                break;
            }
        }
        
        calculate_metrics(temp_processes, n, total_time, &all_results[i]);
    }

    // 4. Generar Reporte
    generate_report("performance_report.md", original_processes, n, all_results, algo_names, NUM_ALGOS);
    printf("Reporte de rendimiento generado en 'performance_report.md'\n");
}

/*
// En este diseño, la función main() debe estar en src/gui_gtk.c
// Si usaras una versión CLI, main() estaría aquí.
int main(int argc, char *argv[]) {
    // Si se ejecuta sin GUI (modo reporte)
    if (argc == 2 && strcmp(argv[1], "--report") == 0) {
        process_t processes[MAX_PROCESSES];
        int n = load_processes_from_file("workloads/workload1.txt", processes, MAX_PROCESSES);
        if (n > 0) {
            run_all_algorithms_for_report(processes, n);
        }
        return 0;
    }
    // Si no, la ejecución va a la GUI a través de main en src/gui_gtk.c
    return 0;
}
*/
