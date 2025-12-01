#include "../include/metrics.h"
#include <math.h>

void calculate_metrics(process_t *processes, int n, int total_time, metrics_t *metrics) {
    double total_tat = 0.0;
    double total_wt = 0.0;
    double total_rt = 0.0;
    long total_burst = 0;
    
    // Variables para el Índice de Fairness de Jain
    double sum_xi = 0.0;
    double sum_xi_sq = 0.0;
    int completed_processes = 0;

    for (int i = 0; i < n; i++) {
        process_t *p = &processes[i];

        // Asegurarse de que el proceso haya terminado
        if (p->completion_time > 0) {
            // 1. Turnaround Time (TAT)
            p->turnaround_time = p->completion_time - p->arrival_time;
            total_tat += p->turnaround_time;

            // 2. Waiting Time (WT)
            // Usar p->original_burst si se mantiene, si no, usa p->burst_time (asumimos que p->burst_time es el original)
            p->waiting_time = p->turnaround_time - p->burst_time; 
            total_wt += p->waiting_time;

            // 3. Response Time (RT)
            p->response_time = p->start_time - p->arrival_time;
            total_rt += p->response_time;
            
            // 4. Variables para Fairness
            sum_xi += p->turnaround_time;
            sum_xi_sq += (double)p->turnaround_time * p->turnaround_time;
            
            total_burst += p->burst_time;
            completed_processes++;
        }
    }

    if (completed_processes > 0) {
        // 1-3. Promedios
        metrics->avg_turnaround_time = total_tat / completed_processes;
        metrics->avg_waiting_time = total_wt / completed_processes;
        metrics->avg_response_time = total_rt / completed_processes;

        // 4. CPU Utilization
        if (total_time > 0) {
            metrics->cpu_utilization = ((double)total_burst / total_time) * 100.0;
        } else {
            metrics->cpu_utilization = 0.0;
        }

        // 5. Throughput
        if (total_time > 0) {
            metrics->throughput = (double)completed_processes / total_time;
        } else {
            metrics->throughput = 0.0;
        }

        // 6. Fairness Index (Jain's Fairness Index)
        // Formula: (Σxi)² / (n × Σxi²) donde xi = turnaround time
        double numerator = sum_xi * sum_xi;
        double denominator = (double)completed_processes * sum_xi_sq;
        
        if (denominator > 0) {
            metrics->fairness_index = numerator / denominator;
        } else {
            metrics->fairness_index = 0.0;
        }

    } else {
        // Ningún proceso completado
        memset(metrics, 0, sizeof(metrics_t));
    }
}
