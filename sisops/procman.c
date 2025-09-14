#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>



// THIS IS TEST 2!!!!!!!!1

#define NUM_CHILDREN 5

// Commands to execute
const char *commands[NUM_CHILDREN][4] = {
    {"ls", NULL},
    {"date", NULL},
    {"whoami", NULL},
    {"pwd", NULL},
    {"ps", NULL}
};

// Function to get formatted time string from timeval
void format_time(struct timeval *tv, char *buffer, size_t buflen) {
    time_t sec = tv->tv_sec;
    struct tm *tm_info = localtime(&sec);
    strftime(buffer, buflen, "%H:%M:%S", tm_info);
}

int main() {
    pid_t pids[NUM_CHILDREN];
    struct timeval start_times[NUM_CHILDREN];
    struct timeval end_times[NUM_CHILDREN];

    struct timeval total_start, total_end;
    gettimeofday(&total_start, NULL);

    printf("=== Process Management Demonstration ===\n");
    printf("Parent PID: %d\n\n", getpid());

    for (int i = 0; i < NUM_CHILDREN; i++) {
        gettimeofday(&start_times[i], NULL);
        pid_t pid = fork();
        if (pid < 0) {
            // fork failure
            fprintf(stderr, "Failed to fork child %d: %s\n", i, strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            // In child process
            printf("[Child %d] PID %d - Executing command: %s\n", i, getpid(), commands[i][0]);
            if (execvp(commands[i][0], (char *const *)commands[i]) == -1) {
                fprintf(stderr, "[Child %d] execvp failed: %s\n", i, strerror(errno));
                exit(127); // exec failure exit code
            }
        }
        else {
            // In parent process, store child's pid
            pids[i] = pid;
        }
    }

    // Parent waits for all children
    printf("\nWaiting for child processes to complete...\n\n");
    for (int i = 0; i < NUM_CHILDREN; i++) {
        int status;
        pid_t wpid = waitpid(pids[i], &status, 0);
        gettimeofday(&end_times[i], NULL);

        if (wpid == -1) {
            fprintf(stderr, "Failed to wait for child PID %d: %s\n", pids[i], strerror(errno));
            continue;
        }

        long elapsed_ms = (end_times[i].tv_sec - start_times[i].tv_sec) * 1000 +
                          (end_times[i].tv_usec - start_times[i].tv_usec) / 1000;

        printf("Process Info:\n");
        printf("  PID          : %d\n", pids[i]);
        printf("  Command      : %s\n", commands[i][0]);

        if (WIFEXITED(status)) {
            printf("  Exit Status  : %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("  Terminated by signal: %d\n", WTERMSIG(status));
        }

        char start_str[9], end_str[9];
        format_time(&start_times[i], start_str, sizeof(start_str));
        format_time(&end_times[i], end_str, sizeof(end_str));

        printf("  Start Time   : %s\n", start_str);
        printf("  End Time     : %s\n", end_str);
        printf("  Duration     : %ld ms\n\n", elapsed_ms);
    }

    gettimeofday(&total_end, NULL);
    long total_elapsed_ms = (total_end.tv_sec - total_start.tv_sec) * 1000 +
                            (total_end.tv_usec - total_start.tv_usec) / 1000;

    printf("All processes completed.\n");
    printf("Total Execution Time: %ld ms\n", total_elapsed_ms);

    return 0;
}

