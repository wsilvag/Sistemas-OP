#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "zombie.h"
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    printf("--- Test Program for libzombie ---\n");
    
    // 1. Initialize the zombie prevention mechanism
    zombie_init();

    // 2. Test zombie_safe_fork
    printf("\nTesting zombie_safe_fork (5 children)...\n");
    for (int i = 0; i < 5; i++) {
        pid_t pid = zombie_safe_fork();
        if (pid == 0) {
            // Child: do work and exit
            sleep((i % 2) + 1);
            exit(i);
        } else if (pid > 0) {
            printf("Spawned child PID %d via zombie_safe_fork.\n", pid);
        }
    }

    // 3. Test zombie_safe_spawn
    printf("\nTesting zombie_safe_spawn (2 children running 'sleep')...\n");
    char *sleep_args[] = {"/bin/sleep", "2", NULL};
    pid_t spawned_pid1 = zombie_safe_spawn("/bin/sleep", sleep_args);
    printf("Spawned process 1 PID %d via zombie_safe_spawn.\n", spawned_pid1);
    
    char *echo_args[] = {"/bin/echo", "Spawned echo process done.", NULL};
    pid_t spawned_pid2 = zombie_safe_spawn("/bin/echo", echo_args);
    printf("Spawned process 2 PID %d via zombie_safe_spawn.\n", spawned_pid2);

    // Give time for all children to exit and the SIGCHLD handler to reap
    printf("\nWaiting 4 seconds for reaping...\n");
    sleep(4);
    
    // 4. Check statistics
    zombie_stats_t stats;
    zombie_get_stats(&stats);
    
    printf("\n--- Final Statistics ---\n");
    printf("Zombies Created: %d (Expected: 7)\n", stats.zombies_created);
    printf("Zombies Reaped: %d (Expected: ~7)\n", stats.zombies_reaped);
    printf("Zombies Active: %d (Expected: 0)\n", stats.zombies_active);
    
    printf("\nVerifying no zombies remain:\n");
    system("ps aux | grep 'Z' | grep -v grep");
    
    printf("Test finished.\n");
    return 0;
}
