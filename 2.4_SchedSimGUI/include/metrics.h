#ifndef METRICS_H
#define METRICS_H

#include "scheduler.h"

// Parte 3: Cálculo de Métricas

void calculate_metrics(process_t *processes, int n, int total_time, metrics_t *metrics);

// Parte 5: Reporte

void generate_report(const char *filename, process_t *processes, int n, metrics_t results[], const char *algo_names[], int num_algorithms);

#endif // METRICS_H
