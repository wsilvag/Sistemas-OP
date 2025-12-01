#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Creates N zombie processes for testing
 * Parent does NOT call wait() - children become zombies
 * Returns: 0 on success, -1 on failure
 */
int create_zombies(int count) {
    if (count <= 0) {
        fprintf(stderr, "Error: Count must be positive.\n");
        return -1;
    }

    printf("Creating %d zombie processes...\n", count);

    for (int i = 0; i < count; i++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            return -1;
        } else if (pid == 0) {
            // Child process exits immediately
            // Exit with a different code for testing
            exit(i % 10); 
        } else {
            // Parent process does NOT call wait()
            printf("Created zombie: PID %d (exit code %d)\n", pid, i % 10);
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <N_zombies>\n", argv[0]);
        return 1;
    }

    int n_zombies = atoi(argv[1]);
    if (create_zombies(n_zombies) == 0) {
        printf("\nParent PID: %d. Press Enter to exit and clean up zombies...\n", getpid());
        // Wait for user input to keep parent alive
        getchar();
    }

    // When the parent exits, init (PID 1) will reap the zombies.
    return 0;
}
