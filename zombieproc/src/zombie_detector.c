#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_ZOMBIES 1024

// Structure to hold zombie data
typedef struct {
    int pid;
    int ppid;
    char comm[256];
    char state;
    long utime;
    long stime;
} zombie_info_t;

/**
 * Scans /proc filesystem for zombie processes
 * Returns: number of zombies found
 */
int find_zombies(zombie_info_t *zombie_list, int max_zombies) {
    DIR *dir;
    struct dirent *entry;
    int zombie_count = 0;

    if ((dir = opendir("/proc")) == NULL) {
        perror("opendir /proc failed");
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        // Check if the entry name is a directory representing a PID (all digits)
        int is_pid_dir = 1;
        for (char *p = entry->d_name; *p; p++) {
            if (!isdigit(*p)) {
                is_pid_dir = 0;
                break;
            }
        }

        if (is_pid_dir) {
            char stat_path[512];
            snprintf(stat_path, sizeof(stat_path), "/proc/%s/stat", entry->d_name);

            FILE *fp = fopen(stat_path, "r");
            if (fp) {
                // Parse the first few fields from /proc/[pid]/stat
                // PID(1), COMM(2), STATE(3), PPID(4), ... UTIME(14), STIME(15)
                int pid, ppid;
                char comm[256], state;
                long utime, stime;
                // Need to read many fields to get to UTIME/STIME (fields 14 and 15)
                long dummy_l[10]; // To skip intermediate long fields (5-13)
                int dummy_i[4];   // To skip intermediate int fields (5-8)

                int scanned = fscanf(fp, "%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu", 
                    &pid, comm, &state, &ppid, 
                    &dummy_i[0], &dummy_i[1], &dummy_i[2], &dummy_i[3], 
                    (unsigned int*)&dummy_l[0], // Skip pgrp, session, tty_nr, tpgid, flags
                    &dummy_l[1], &dummy_l[2], &dummy_l[3], &dummy_l[4], // Skip minflt, cminflt, majflt, cmajflt
                    &utime, &stime // Fields 14 and 15
                );
                
                fclose(fp);

                // Note: The /proc/[pid]/stat format is long and changes. 
                // A simpler, safer approach is just to check the 'Z' state.
                if (state == 'Z' && zombie_count < max_zombies) {
                    zombie_list[zombie_count].pid = pid;
                    zombie_list[zombie_count].ppid = ppid;
                    zombie_list[zombie_count].state = state;
                    // Remove parentheses from command name
                    size_t len = strlen(comm);
                    if (comm[0] == '(' && comm[len - 1] == ')') {
                        comm[len - 1] = '\0';
                        strcpy(zombie_list[zombie_count].comm, comm + 1);
                    } else {
                        strcpy(zombie_list[zombie_count].comm, comm);
                    }
                    zombie_list[zombie_count].utime = utime;
                    zombie_list[zombie_count].stime = stime;
                    zombie_count++;
                }
            }
        }
    }
    closedir(dir);
    return zombie_count;
}

/**
 * Converts jiffies (utime + stime) to H:M:S format (approximate)
 */
void format_time(long jiffies, char *buffer, size_t buf_size) {
    long clk_tck = sysconf(_SC_CLK_TCK);
    long total_seconds = (jiffies / clk_tck);
    long seconds = total_seconds % 60;
    long minutes = (total_seconds / 60) % 60;
    long hours = (total_seconds / 3600);
    snprintf(buffer, buf_size, "%02ld:%02ld:%02ld", hours, minutes, seconds);
}

void print_zombie_info(const zombie_info_t *zombie_list, int count) {
    printf("=== Zombie Process Report ===\n");
    printf("Total Zombies: %d\n\n", count);

    if (count == 0) return;

    printf("PID\tPPID\tCommand\t\tState\tTime (UTime+STime)\n");
    printf("-----\t-----\t---------------\t-----\t------------------\n");

    // Simple analysis for parent
    int parent_pid = zombie_list[0].ppid;
    int parent_count = 0;

    for (int i = 0; i < count; i++) {
        char time_str[32];
        long total_time = zombie_list[i].utime + zombie_list[i].stime;
        format_time(total_time, time_str, sizeof(time_str));

        printf("%-5d\t%-5d\t%-15s\t%c\t%s\n", 
            zombie_list[i].pid, 
            zombie_list[i].ppid, 
            zombie_list[i].comm, 
            zombie_list[i].state, 
            time_str
        );
        
        if (zombie_list[i].ppid == parent_pid) {
            parent_count++;
        }
    }

    // In this simple test, we assume they all share the same creator parent PID
    char parent_comm[256] = "unknown";
    char ppid_path[512];
    snprintf(ppid_path, sizeof(ppid_path), "/proc/%d/cmdline", parent_pid);
    FILE *fp_ppid = fopen(ppid_path, "r");
    if (fp_ppid) {
        // Read up to 255 chars, cmdline is null-separated but we only want the first part
        if (fgets(parent_comm, sizeof(parent_comm), fp_ppid)) {
            // Replace null terminators with space or just stop at first one
            for(int i = 0; i < sizeof(parent_comm) - 1; i++) {
                if (parent_comm[i] == '\0') {
                    parent_comm[i] = '\0';
                    break;
                }
            }
        }
        fclose(fp_ppid);
    }

    printf("\nParent Process Analysis:\n");
    printf("  PID %d (%s) has %d zombie children\n", 
        parent_pid, parent_comm, parent_count);
}

int main() {
    zombie_info_t zombie_list[MAX_ZOMBIES];
    int count = find_zombies(zombie_list, MAX_ZOMBIES);
    
    print_zombie_info(zombie_list, count);

    return 0;
}
