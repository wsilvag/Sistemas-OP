#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define NUM_CHILDREN 3

int main() {
    pid_t pids[NUM_CHILDREN];
    int status;
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            printf("Child %d (PID %d) starting exec...\n", i, getpid());
            // Run the 'ls' command; can be replaced with any executable
            execl("/bin/ls", "ls", NULL);
            // If exec fails:
            perror("exec failed");
            exit(100 + i);
        }
        else if (pid > 0) {
            // Parent process: store child's pid
            pids[i] = pid;
        }
        else {
            // Fork failed
            perror("fork failed");
            exit(EXIT_FAILURE);
        }
    }
    // Parent process waits for all children
    for (int i = 0; i < NUM_CHILDREN; ++i) {
        pid_t child_pid = wait(&status);
        if (child_pid > 0) {
            if (WIFEXITED(status)) {
                printf("Parent: Child %d (PID %d) exited with status %d\n",
                       i, child_pid, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Parent: Child %d (PID %d) terminated by signal %d\n",
                       i, child_pid, WTERMSIG(status));
            }
        }
    }
    printf("Parent: All children have finished.\n");
    return 0;
}



