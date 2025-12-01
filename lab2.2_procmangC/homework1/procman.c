#include "procman.h"

// Variables Globales definidas en procman.h
process_info_t process_table[MAX_PROCESSES];
int process_count = 0;
pid_t main_pid;

void initialize_process_table() {
    memset(process_table, 0, sizeof(process_table));
    main_pid = getpid();
}

int find_free_slot() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == 0) return i;
    }
    return -1;
}

int find_process_slot(pid_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid) return i;
    }
    return -1;
}

void add_process_to_table(pid_t pid, const char *command_line) {
    int slot = find_free_slot();
    if (slot == -1) {
        fprintf(stderr, "Error: Tabla de procesos llena. Máx %d.\n", MAX_PROCESSES);
        return;
    }
    process_table[slot].pid = pid;
    process_table[slot].start_time = time(NULL);
    process_table[slot].status = STATUS_RUNNING;
    strncpy(process_table[slot].command, command_line, MAX_COMMAND_LENGTH - 1);
    process_table[slot].command[MAX_COMMAND_LENGTH - 1] = '\0';
    process_count++;
}

void update_process_status(pid_t pid, int status, int exit_code) {
    int slot = find_process_slot(pid);
    if (slot != -1) {
        process_table[slot].status = status;
        process_table[slot].exit_status = exit_code;
    }
}

void remove_process_from_table(pid_t pid) {
    int slot = find_process_slot(pid);
    if (slot != -1) {
        process_table[slot].pid = 0;
        process_count--;
    }
}



// Parte 1.1: Comando 'create'
int create_process(const char *command, char *args[]) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        // --- Proceso Hijo ---
        signal(SIGINT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
        execvp(command, args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // --- Proceso Padre ---
        char command_line[MAX_COMMAND_LENGTH] = "";
        strcat(command_line, command);
        for (int i = 1; args[i] != NULL; i++) {
            strcat(command_line, " ");
            strcat(command_line, args[i]);
        }
        add_process_to_table(pid, command_line);
        printf("Proceso creado con PID: %d\n", pid);
        return pid;
    }
}

// Parte 1.3: Comando 'kill'
int terminate_process(pid_t pid, int force) {
    int signal_to_send = (force == 0) ? SIGTERM : SIGKILL;

    if (kill(pid, signal_to_send) == -1) {
        if (errno == ESRCH) {
            fprintf(stderr, "Advertencia: PID %d no existe (ya terminó).\n", pid);
            remove_process_from_table(pid); // Aseguramos que se limpie si aún está en la tabla
            return 0;
        }
        perror("kill");
        return -1;
    }

    int status;
    // Bloquear y esperar el cierre (0 significa bloquear)
    if (waitpid(pid, &status, 0) == -1 && errno != ECHILD) {
        perror("waitpid en terminate_process");
    }

    remove_process_from_table(pid);
    printf("Proceso %d terminado con éxito.\n", pid);
    return 0;
}


// Parte 3.2: Manejador SIGCHLD (Para recolectar zombies)
void sigchld_handler(int signum) {
    pid_t pid;
    int status;
    // WNOHANG: No bloquear. -1: Chequear cualquier hijo.
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            update_process_status(pid, STATUS_TERMINATED, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            update_process_status(pid, STATUS_TERMINATED, -WTERMSIG(status));
        }
        // La remoción final se hace en list_processes o wait_all_processes.
    }
}

// Parte 2.3: Comando 'wait'
void wait_all_processes(void) {
    printf("Esperando a que todos los procesos hijos terminen...\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        pid_t pid = process_table[i].pid;
        if (pid != 0 && process_table[i].status == STATUS_RUNNING) {
            int status;
            printf("Esperando PID %d (%s)...\n", pid, process_table[i].command);

            if (waitpid(pid, &status, 0) == pid) {
                // El proceso terminó. Actualizamos y limpiamos la tabla.
                if (WIFEXITED(status)) {
                    update_process_status(pid, STATUS_TERMINATED, WEXITSTATUS(status));
                } else if (WIFSIGNALED(status)) {
                    update_process_status(pid, STATUS_TERMINATED, -WTERMSIG(status));
                }
                remove_process_from_table(pid);
            } else if (errno == ECHILD) {
                 // Ya fue recolectado por SIGCHLD
                 remove_process_from_table(pid);
            }
        }
    }
    printf("Todos los procesos gestionados han terminado.\n");
    process_count = 0;
}

// Parte 3.1: Manejador SIGINT (Comando 'quit' o Ctrl+C)
void sigint_handler(int signum) {
    printf("\n[SIGINT] Apagando ProcMan de forma segura...\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid != 0 && process_table[i].status == STATUS_RUNNING) {
            kill(process_table[i].pid, SIGTERM);
        }
    }
    wait_all_processes();
    printf("Apagado seguro completado. Saliendo.\n");
    exit(EXIT_SUCCESS);
}

// Parte 3: Configuración de Señales
void setup_signal_handlers() {
    struct sigaction sa_chld, sa_int;

    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa_chld, NULL);

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);
}

// Parte 2.2: Comando 'list'
void list_processes(void) {
    printf("\nPID\tCOMMAND\t\t\tRUNTIME\t\tSTATUS\t\tEXIT_CODE\n");
    printf("-----\t---------------------------\t-----------\t-----------\t---------\n");

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid != 0) {
            time_t current_time = time(NULL);
            double runtime_seconds = difftime(current_time, process_table[i].start_time);
            int hours = (int)runtime_seconds / 3600;
            int minutes = ((int)runtime_seconds % 3600) / 60;
            int seconds = (int)runtime_seconds % 60;

            const char *status_str;
            char exit_code_str[10] = "";

            if (process_table[i].status == STATUS_RUNNING) {
                status_str = "Running";
            } else {
                status_str = "Terminated";
                sprintf(exit_code_str, "%d", process_table[i].exit_status);
                remove_process_from_table(process_table[i].pid);
            }

            printf("%d\t%-25s\t%02d:%02d:%02d\t%s\t%s\n",
                   process_table[i].pid, process_table[i].command,
                   hours, minutes, seconds, status_str, exit_code_str);
        }
    }
    printf("\n");
}


// Función auxiliar para obtener el PPID (PID del padre)
pid_t get_ppid_from_proc_stat(pid_t pid) {
    char path[64];
    sprintf(path, "/proc/%d/stat", pid);

    FILE *f = fopen(path, "r");
    if (!f) return 0;

    pid_t ppid = 0;
    // Lectura de los primeros 4 campos: PID, (COMM), State, PPID
    fscanf(f, "%*d %*s %*c %d", &ppid);
    fclose(f);
    return ppid;
}

// Parte 4: Comando 'tree'
void print_process_tree(pid_t current_pid, int depth) {
    // 1. Imprimir el proceso actual (con sangría)
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }
    printf("-> %d (ProcMan)\n", current_pid);

    // 2. Buscar hijos gestionados
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid != 0) {
            // Si el padre del proceso gestionado es el proceso actual (procman o un hijo)
            if (get_ppid_from_proc_stat(process_table[i].pid) == current_pid) {
                // Imprimir el hijo
                for (int j = 0; j <= depth; j++) {
                    printf("  ");
                }
                printf("  -> %d (%s)\n", process_table[i].pid, process_table[i].command);
            }
        }
    }
}


void interactive_shell() {
    char line[512];
    printf("ProcMan iniciado. PID: %d\n", main_pid);

    while (1) {
        printf("ProcMan> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            if (feof(stdin)) break;
            continue;
        }

        line[strcspn(line, "\n")] = 0; // Eliminar el salto de línea

        char *args[256];
        char *token = strtok(line, " ");
        int argc = 0;

        while (token != NULL) {
            args[argc++] = token;
            token = strtok(NULL, " ");
        }
        args[argc] = NULL;

        if (argc == 0) continue;
        const char *command = args[0];

        if (strcmp(command, "quit") == 0) { sigint_handler(SIGINT); break; }
        else if (strcmp(command, "create") == 0 && argc >= 2) { create_process(args[1], &args[1]); }
        else if (strcmp(command, "list") == 0) { list_processes(); }
        else if (strcmp(command, "wait") == 0) { wait_all_processes(); }
        else if (strcmp(command, "kill") == 0 && argc >= 2) {
            pid_t pid_to_kill = (pid_t)atoi(args[1]);
            int force = (argc >= 3 && strcmp(args[2], "force") == 0) || (argc >= 3 && atoi(args[2]) == 1);
            terminate_process(pid_to_kill, force);
        } else if (strcmp(command, "tree") == 0) {
            print_process_tree(main_pid, 0);
        } else {
            fprintf(stderr, "Comando desconocido. Use 'quit', 'create', 'list', 'kill', 'wait', o 'tree'.\n");
        }
    }
}

int main(int argc, char *argv[]) {
    initialize_process_table();
    setup_signal_handlers();
    interactive_shell();
    return 0;
}



