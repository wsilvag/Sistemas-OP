#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>

#define LOG_FILE "/tmp/daemon.log"
#define WORKER_INTERVAL 5

static volatile int keep_running = 1;

// Global log file pointer (for simple logging)
FILE *log_fp = NULL;

// Safe logging function (can be used inside signal handler if needed, but here we use fprintf)
void log_message(const char *format, ...) {
    if (log_fp) {
        va_list args;
        va_start(args, format);
        vfprintf(log_fp, format, args);
        va_end(args);
        fflush(log_fp);
    }
}


/**
 * Signal Handler for SIGCHLD: auto-reap terminated children
 */
void sigchld_handler(int sig) {
    int status;
    pid_t pid;
    // WNOHANG for non-blocking wait
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // Use fprintf(stderr) or a safe log mechanism inside a real handler.
        // For this demo, we can just note it.
        // log_message("[SIGCHLD] Reaped child PID %d\n", pid); // Cannot use log_message/fprintf safely
        (void)pid; // suppress unused warning
    }
}

/**
 * Signal Handler for SIGTERM: graceful shutdown
 */
void sigterm_handler(int sig) {
    log_message("[SIGTERM] Received signal. Shutting down gracefully...\n");
    keep_running = 0;
}

/**
 * Sets up SIGCHLD handler to auto-reap and SIGTERM handler
 */
void setup_signals(void) {
    struct sigaction sa_chld, sa_term;

    // SIGCHLD setup
    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("sigaction SIGCHLD failed");
        exit(EXIT_FAILURE);
    }

    // SIGTERM setup
    sa_term.sa_handler = sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction SIGTERM failed");
        exit(EXIT_FAILURE);
    }
}


/**
 * Daemonize process - become background daemon
 */
void daemonize(void) {
    pid_t pid;

    // 1. Fork, parent exits
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // Parent exits

    // 2. Create new session (setsid)
    if (setsid() < 0) exit(EXIT_FAILURE);

    // 3. Fork again, first child exits (Prevents daemon from acquiring a controlling terminal)
    signal(SIGHUP, SIG_IGN); // Ignore SIGHUP
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); // First child (session leader) exits

    // 4. Set file mode mask (umask)
    umask(0);

    // 5. Change the current working directory
    if ((chdir("/")) < 0) {
        perror("chdir failed");
        // Not critical, but good practice
    }

    // 6. Close all file descriptors (or re-open to /dev/null)
    close(STDIN_FILENO);
    // Re-open stdout/stderr to log file for simplicity, though closing is standard daemon practice.
    // For this assignment, we use a separate FILE* log_fp.
    close(STDOUT_FILENO);
    close(STDERR_FILENO); 
}

/**
 * Spawn worker processes periodically
 */
void spawn_workers(void) {
    pid_t pid = fork();

    if (pid < 0) {
        log_message("[ERROR] Worker fork failed.\n");
        return;
    } else if (pid == 0) {
        // Child (Worker process)
        int work_time = (rand() % 2) + 1;
        log_message("[WORKER %d] Starting work for %d seconds...\n", getpid(), work_time);
        sleep(work_time);
        log_message("[WORKER %d] Finished work. Exiting.\n", getpid());
        exit(EXIT_SUCCESS);
    } else {
        // Parent (Daemon)
        log_message("[DAEMON] Spawned worker PID %d.\n", pid);
    }
}

int main(void) {
    // Open log file early for initial messages
    log_fp = fopen(LOG_FILE, "a");
    if (!log_fp) {
        perror("Could not open log file");
        return EXIT_FAILURE;
    }
    log_message("--- Daemon Initializing ---\n");
    
    // Set up signal handlers *before* daemonizing or immediately after
    setup_signals();

    daemonize();

    log_message("Daemon PID: %d. Running...\n", getpid());
    srand(time(NULL) ^ getpid()); 

    // Main daemon loop
    while (keep_running) {
        spawn_workers();
        // Sleep using a safe function (like nanosleep) or check keep_running in the loop
        for(int i = 0; i < WORKER_INTERVAL && keep_running; i++) {
            sleep(1);
        }
    }

    log_message("--- Daemon Shutting Down ---\n");
    if (log_fp) {
        fclose(log_fp);
    }

    return EXIT_SUCCESS;
}
