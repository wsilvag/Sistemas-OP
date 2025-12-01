#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "scheduler.h"

// Parte 2: Algoritmos

void schedule_fifo(process_t *processes, int n, timeline_event_t *timeline);
void schedule_sjf(process_t *processes, int n, timeline_event_t *timeline);
void schedule_stcf(process_t *processes, int n, timeline_event_t *timeline);
void schedule_rr(process_t *processes, int n, int quantum, timeline_event_t *timeline);
void schedule_mlfq(process_t *processes, int n, mlfq_config_t *config, timeline_event_t *timeline);

#endif // ALGORITHMS_H
