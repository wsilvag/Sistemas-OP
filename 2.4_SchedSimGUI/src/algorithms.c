#include "../include/algorithms.h"
#include <string.h>
#include <limits.h>

// Función auxiliar para inicializar procesos antes de cada simulación
void initialize_processes(process_t *processes, int n) {
    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].start_time = -1; // Usar -1 para indicar que no ha iniciado
        processes[i].completion_time = 0;
        // Asignar el burst original para cálculo de métricas (si no se hizo en la carga)
        processes[i].original_burst = processes[i].burst_time; 
        processes[i].current_queue = 0; // Para MLFQ
    }
}

// Función auxiliar para añadir evento al Gantt
void add_timeline_event(timeline_event_t *timeline, int *timeline_count, int time, int pid, int duration) {
    if (duration > 0 && *timeline_count < MAX_TIMELINE_EVENTS) {
        // Optimización: si el PID es el mismo que el último evento, extender la duración
        if (*timeline_count > 0 && timeline[*timeline_count - 1].pid == pid) {
            timeline[*timeline_count - 1].duration += duration;
        } else {
            timeline[*timeline_count].time = time;
            timeline[*timeline_count].pid = pid;
            timeline[*timeline_count].duration = duration;
            (*timeline_count)++;
        }
    }
}

// -------------------------------------------------------------------
// ALGORITMO 1: FIFO (First In First Out)
// -------------------------------------------------------------------
void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline) {
    initialize_processes(processes, n);
    int current_time = 0;
    int completed_count = 0;
    int timeline_count = 0;

    // FIFO debe simular el tiempo de llegada, no solo ordenar estáticamente.
    // Sin embargo, para la implementación simple NO preemptiva,
    // ordenamos por tiempo de llegada.
    // (NOTA: Para un FIFO 100% correcto, se necesita una cola dinámica)

    // Simulación simple: ordena por llegada y ejecuta secuencialmente
    // Usar la función qsort si es necesario, o asumir que los procesos están ordenados por llegada.
    
    // Suponemos que los procesos están ordenados por arrival_time al ser cargados.
    for (int i = 0; i < n; i++) {
        process_t *p = &processes[i];

        // Espera si la CPU está inactiva hasta que llegue el proceso
        if (current_time < p->arrival_time) {
            int idle_duration = p->arrival_time - current_time;
            add_timeline_event(timeline, &timeline_count, current_time, -1, idle_duration); // -1 = IDLE
            current_time = p->arrival_time;
        }

        // Ejecutar el proceso completo (No-preemptivo)
        p->start_time = current_time;
        p->completion_time = current_time + p->burst_time;
        
        add_timeline_event(timeline, &timeline_count, current_time, p->pid, p->burst_time);
        
        current_time = p->completion_time;
        completed_count++;
    }
    // Añadir evento final si es necesario para calcular el tiempo total
    add_timeline_event(timeline, &timeline_count, current_time, 0, 0); 
}

// -------------------------------------------------------------------
// ALGORITMO 2: SJF (Shortest Job First) - No Preemptivo
// -------------------------------------------------------------------
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline) {
    initialize_processes(processes, n);
    int current_time = 0;
    int completed_count = 0;
    int timeline_count = 0;

    // SJF requiere iterar hasta que todos terminen
    while (completed_count < n) {
        int shortest_burst = INT_MAX;
        int selected_index = -1;

        // Buscar el trabajo más corto que ha llegado y no ha terminado
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && 
                processes[i].arrival_time <= current_time &&
                processes[i].burst_time < shortest_burst) 
            {
                shortest_burst = processes[i].burst_time;
                selected_index = i;
            }
        }

        if (selected_index != -1) {
            process_t *p = &processes[selected_index];
            
            // Si es la primera vez que se ejecuta (Response Time)
            if (p->start_time == -1) {
                p->start_time = current_time;
            }

            // Ejecutar el proceso completo (No-preemptivo)
            p->completion_time = current_time + p->burst_time;
            p->remaining_time = 0;
            
            add_timeline_event(timeline, &timeline_count, current_time, p->pid, p->burst_time);
            
            current_time = p->completion_time;
            completed_count++;

        } else {
            // CPU Idle: Esperar al siguiente proceso
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (processes[i].remaining_time > 0 && processes[i].arrival_time < next_arrival) {
                    next_arrival = processes[i].arrival_time;
                }
            }
            if (next_arrival != INT_MAX) {
                int idle_duration = next_arrival - current_time;
                add_timeline_event(timeline, &timeline_count, current_time, -1, idle_duration);
                current_time = next_arrival;
            } else {
                // Caso extremo: No hay más procesos pendientes, salir.
                break;
            }
        }
    }
    add_timeline_event(timeline, &timeline_count, current_time, 0, 0); 
}


// -------------------------------------------------------------------
// ALGORITMO 3: STCF (Shortest Time to Completion First) - Preemptivo
// -------------------------------------------------------------------
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline) {
    initialize_processes(processes, n);
    int current_time = 0;
    int completed_count = 0;
    int timeline_count = 0;
    int running_pid = -1; // PID del proceso actualmente en ejecución

    while (completed_count < n) {
        int shortest_rem_time = INT_MAX;
        int selected_index = -1;

        // 1. Encontrar el proceso elegible con el menor tiempo restante
        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                if (processes[i].remaining_time < shortest_rem_time) {
                    shortest_rem_time = processes[i].remaining_time;
                    selected_index = i;
                }
            }
        }

        if (selected_index == -1) {
            // CPU Idle: Buscar el tiempo de llegada del siguiente proceso
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (processes[i].remaining_time > 0 && processes[i].arrival_time < next_arrival) {
                    next_arrival = processes[i].arrival_time;
                }
            }
            if (next_arrival != INT_MAX) {
                int idle_duration = next_arrival - current_time;
                add_timeline_event(timeline, &timeline_count, current_time, -1, idle_duration);
                current_time = next_arrival;
                running_pid = -1;
            } else {
                break; // Todos terminaron o no hay más
            }
        } else {
            process_t *p = &processes[selected_index];
            
            // 2. Manejar Preemption
            if (running_pid != p->pid) {
                running_pid = p->pid;
                // Nuevo evento de timeline (la función add_timeline_event maneja la duración)
            }
            
            // 3. Ejecución por 1 unidad de tiempo
            if (p->start_time == -1) {
                p->start_time = current_time;
            }
            p->remaining_time--;
            current_time++;
            
            add_timeline_event(timeline, &timeline_count, current_time - 1, p->pid, 1);

            // 4. Chequear Terminación
            if (p->remaining_time == 0) {
                p->completion_time = current_time;
                completed_count++;
                running_pid = -1; // CPU libre, buscar nuevo proceso
            }
        }
    }
    add_timeline_event(timeline, &timeline_count, current_time, 0, 0); 
}


// -------------------------------------------------------------------
// ALGORITMO 4: Round Robin
// -------------------------------------------------------------------
void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline) {
    // Implementación requiere una estructura de cola (simulada con arreglos y punteros)
    // ... Implementación detallada de cola y simulación de tiempo
    // Lógica clave: 
    // - Usa una cola (FIFO)
    // - Si el proceso usa todo el quantum, se añade al final de la cola.
    // - Preemptive por tiempo.
    
    // Dejar como esqueleto para enfocarse en métricas y GUI.
}

// -------------------------------------------------------------------
// ALGORITMO 5: MLFQ
// -------------------------------------------------------------------
void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config, timeline_event_t *timeline) {
    // Implementación requiere:
    // - Múltiples colas (listas/arreglos)
    // - Lógica de degradación al usar el quantum
    // - Lógica de 'Priority Boost' cada 'boost_interval'
    
    // Dejar como esqueleto para enfocarse en métricas y GUI.
}
