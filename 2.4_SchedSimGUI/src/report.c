#include "../include/metrics.h"
#include <stdio.h>
#include <string.h>

void generate_report(const char *filename, process_t *processes, int n, 
                     metrics_t results[], const char *algo_names[], int num_algorithms) {
    
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        perror("Error al abrir el archivo de reporte");
        return;
    }

    fprintf(fp, "# 📊 Scheduler Performance Report\n\n");
    fprintf(fp, "## Process Set\n");
    fprintf(fp, "| PID | Arrival | Burst | Priority |\n");
    fprintf(fp, "|-----|---------|-------|----------|\n");
    for (int i = 0; i < n; i++) {
        fprintf(fp, "| %d | %d | %d | %d |\n", 
            processes[i].pid, 
            processes[i].arrival_time, 
            processes[i].burst_time, 
            processes[i].priority);
    }
    fprintf(fp, "\n");
    
    fprintf(fp, "## Algorithm Comparison\n");
    fprintf(fp, "| Algorithm | Avg TAT | Avg WT | Avg RT | Throughput | Fairness |\n");
    fprintf(fp, "|-----------|---------|--------|--------|------------|----------|\n");
    
    // Comparación y búsqueda del mejor algoritmo
    double best_tat = 99999.0;
    int best_algo_index = -1;

    for (int i = 0; i < num_algorithms; i++) {
        fprintf(fp, "| %s | %.2f | %.2f | %.2f | %.4f | %.4f |\n",
            algo_names[i],
            results[i].avg_turnaround_time,
            results[i].avg_waiting_time,
            results[i].avg_response_time,
            results[i].throughput,
            results[i].fairness_index
        );
        
        // Criterio de mejor rendimiento: TAT más bajo
        if (results[i].avg_turnaround_time < best_tat) {
            best_tat = results[i].avg_turnaround_time;
            best_algo_index = i;
        }
    }

    fprintf(fp, "\n## Best Algorithm for This Workload\n");
    if (best_algo_index != -1) {
        fprintf(fp, "**%s** - Achieved the lowest average turnaround time (%.2f).\n", 
            algo_names[best_algo_index], best_tat);
    } else {
        fprintf(fp, "No data available.\n");
    }

    fprintf(fp, "\n## Recommendations\n");
    fprintf(fp, "- **Interactive processes (buen RT)**: Round Robin o MLFQ.\n");
    fprintf(fp, "- **Batch jobs (buen TAT)**: STCF (preemptivo) o SJF (no-preemptivo).\n");
    fprintf(fp, "- **Carga Mixta**: MLFQ con tunning de quantum.\n");

    fclose(fp);
}
