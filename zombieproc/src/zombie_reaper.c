#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <signal.h>
#include <time.h>

// --- Strategy 1: Explicit Wait ---

/**
 * Reaps all child processes explicitly using wait()
 */
void reap_explicit(void) {
    int status;
    pid_t pid;
    printf("Strategy 1: Explicitly reaping all children with wait()...\n");
    
    // wait() blocks until a child terminates. We use a loop until wait returns -1 (no more children)
    while ((pid = wait(&status)) > 0) {
        printf("Reaped child PID %d (exit status: %d)\n", pid, WEXITSTATUS(status));
    }
    if (pid == -1) {
        //perror("wait error (expected for no more children)");
    }
}

// --- Strategy 2: SIGCHLD Handler ---

/**
 * SIGCHLD handler: non-blocking waitpid to reap all terminated children
 */
void sigchld_handler(int sig) {
    int status;
    pid_t pid;
    // Use WNOHANG to ensure the handler doesn't block
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        // In a real daemon, you would use write() or a log file, not printf!
        // We use printf here for simple demonstration output.
        fprintf(stderr, "[SIGCHLD Handler] Reaped child PID %d (status: %d)\n", pid, WEXITSTATUS(status));
    }
}

/**
 * Sets up SIGCHLD handler to auto-reap
 */
void setup_auto_reaper(void) {
    printf("Strategy 2: Setting up SIGCHLD handler for auto-reaping...\n");
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // SA_NOCLDSTOP prevents signal on job control stops
    
    if (sigaction(SIGCHLD, &sa, 0) == -1) {
        perror("sigaction failed");
        exit(1);
    }
}

// --- Strategy 3: Ignore SIGCHLD ---

/**
 * Uses signal(SIGCHLD, SIG_IGN) for automatic reaping by the kernel
 */
void setup_ignore_reaper(void) {
    printf("Strategy 3: Ignoring SIGCHLD for kernel auto-reaping...\n");
    if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
        perror("signal(SIGCHLD, SIG_IGN) failed");
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <strategy> (1=explicit, 2=handler, 3=ignore)\n", argv[0]);
        return 1;
    }

    srand(time(NULL) ^ getpid()); // Seed for random sleep

    int strategy = atoi(argv[1]);
    int num_children = 10;

    printf("Parent PID: %d. Creating %d children...\n", getpid(), num_children);
    
    // Create 10 children
    for (int i = 0; i < num_children; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            exit(1);
        }
        if (pid == 0) {
            // Child: Sleep randomly then exit
            int sleep_time = (rand() % 3) + 1;
            //printf("Child %d (PID %d) sleeping for %d seconds and exiting...\n", i, getpid(), sleep_time);
            sleep(sleep_time);
            exit(i);
        }
    }
    
    // Wait a moment for children to start/exit
    sleep(1); 

    // Use chosen strategy
    switch(strategy) {
        case 1: 
            // Strategy 1 needs children to be zombies or terminated BEFORE wait. 
            // We wait for children to exit (become zombies) before calling reap_explicit.
            printf("Waiting for all children to terminate (become zombies/exited)...\n");
            // Wait max 5 seconds for all children to exit
            for(int i = 0; i < 5; i++) {
                // Check for zombies/terminated
                if (system("ps aux | awk '{if ($8 == \"Z\") print $0}' | grep -q 'zombie_reaper'") != 0) {
                    break; // No zombies found, maybe they all exited
                }
                sleep(1);
            }
            reap_explicit(); 
            break;
        case 2: 
            setup_auto_reaper(); 
            // In strategy 2, the parent must run for a while for the handler to catch all SIGCHLD
            sleep(5);  
            break;
        case 3: 
            setup_ignore_reaper(); 
            // In strategy 3, the kernel reaps immediately upon child exit
            sleep(5);
            break;
        default:
            fprintf(stderr, "Invalid strategy: %d\n", strategy);
            return 1;
    }
    
    printf("\nFinished strategy. Final check for zombies:\n");
    // Verify no zombies
    // system("ps aux | grep 'Z' | grep -v grep"); // Original verification
    // A more specific check for zombies from this program (might not work well if children change command name immediately)
    system("ps aux | awk '{if ($8 == \"Z\") print $0}'"); 

    return 0;
}
