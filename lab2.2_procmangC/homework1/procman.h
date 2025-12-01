#ifndef PROCMAN_H
#define PROCMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

// --- Constantes ---
#define MAX_PROCESSES 10
#define MAX_COMMAND_LENGTH 256
#define STATUS_RUNNING 1
#define STATUS_TERMINATED 2

// --- Estructura de Información del Proceso ---
typedef struct {
    pid_t pid;
    time_t start_time;
    int status;
    int exit_status;
    char command[MAX_COMMAND_LENGTH];
} process_info_t;

// --- Variables Globales (Implementadas en procman.c) ---
extern process_info_t process_table[MAX_PROCESSES];
extern int process_count;
extern pid_t main_pid;

// --- Prototipos de Funciones (Implementados en procman.c) ---
void initialize_process_table();
int find_free_slot();
void add_process_to_table(pid_t pid, const char *command_line);
void update_process_status(pid_t pid, int status, int exit_code);
void remove_process_from_table(pid_t pid);

// Funciones de Procesos y Control
int create_process(const char *command, char *args[]);
void list_processes(void);
int terminate_process(pid_t pid, int force);
void wait_all_processes(void);

// Funciones de Señales
void setup_signal_handlers();
void sigchld_handler(int signum);
void sigint_handler(int signum);

// Funciones de Visualización
pid_t get_ppid_from_proc_stat(pid_t pid);
void print_process_tree(pid_t current_pid, int depth);

#endif
