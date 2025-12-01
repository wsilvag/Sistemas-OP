#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdio.h>
#include <stdlib.h>

// Parte 1: Estructuras de Datos

typedef struct {
    int pid;                    // Process ID
    int arrival_time;           // When process arrives
    int burst_time;             // Total CPU time needed
    int priority;               // Priority (lower = higher priority)
    int remaining_time;         // Time left to execute
    int start_time;             // First time scheduled
    int completion_time;        // When finished
    int turnaround_time;        // completion - arrival
    int waiting_time;           // turnaround - burst
    int response_time;          // start - arrival
    int current_queue;          // MLFQ: Current queue index
    int original_burst;         // Used for metrics calculation
} process_t;

typedef struct {
    int time;                   // Time slice start
    int pid;                    // Process running (or -1 for idle)
    int duration;               // How long it ran
} timeline_event_t;

typedef struct {
    double avg_turnaround_time;
    double avg_waiting_time;
    double avg_response_time;
    double cpu_utilization;
    double throughput;
    double fairness_index;      // Jain's fairness index
} metrics_t;

// MLFQ Configuration
typedef struct {
    int num_queues;
    int quantums[10];           // Quantum for each queue (max 10)
    int boost_interval;         // Priority boost every N time units
} mlfq_config_t;

// Máximo de procesos y eventos
#define MAX_PROCESSES 100
#define MAX_TIMELINE_EVENTS 1000

#endif // SCHEDULER_H
