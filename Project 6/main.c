#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include "common.h"

#define NUM_CHILDREN 5
#define BUFFER_SIZE 256
#define OUTPUT_FILE "output.txt"

FILE *output_file = NULL;
struct timeval parent_start_tv;

void handle_signal(int signal) {
    if (output_file) {
        fflush(output_file);
        fclose(output_file);
        printf("Output file closed due to signal %d.\n", signal);
    }
    exit(0);
}

void get_timestamp(char *buffer, size_t buffer_size) {
    struct timeval current_tv;
    gettimeofday(&current_tv, NULL);
    double elapsed_time = (current_tv.tv_sec - parent_start_tv.tv_sec) +
                         (current_tv.tv_usec - parent_start_tv.tv_usec) / 1000000.0;
    int minutes = (int)(elapsed_time / 60);
    int seconds = (int)elapsed_time % 60;
    int milliseconds = (int)((elapsed_time - (int)elapsed_time) * 1000);
    snprintf(buffer, buffer_size, "%d:%02d.%03d", minutes, seconds, milliseconds);
}

int main() {
    int fd[NUM_CHILDREN][2];
    pid_t pids[NUM_CHILDREN];
    fd_set read_fds;
    int max_fd = 0;

    output_file = fopen(OUTPUT_FILE, "w");
    if (!output_file) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    gettimeofday(&parent_start_tv, NULL);

    for (int i = 0; i < NUM_CHILDREN; i++) {
        if (pipe(fd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
        pids[i] = fork();
        if (pids[i] < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
        if (pids[i] == 0) {
            close(fd[i][0]);
            child_process(i, fd[i][1]);
            close(fd[i][1]);
            exit(0);
        } else {
            close(fd[i][1]);
            if (fd[i][0] > max_fd) {
                max_fd = fd[i][0];
            }
        }
    }

    while (1) {
        FD_ZERO(&read_fds);
        for (int i = 0; i < NUM_CHILDREN; i++) {
            FD_SET(fd[i][0], &read_fds);
        }

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (FD_ISSET(fd[i][0], &read_fds)) {
                char buffer[BUFFER_SIZE];
                ssize_t nread = read(fd[i][0], buffer, BUFFER_SIZE - 1);
                if (nread > 0) {
                    buffer[nread] = '\0';
                    char parent_timestamp[20];
                    get_timestamp(parent_timestamp, sizeof(parent_timestamp));
                    char *line = strtok(buffer, "\n");
                    while (line != NULL) {
                        fprintf(output_file, "%s: Parent - %s\n", parent_timestamp, line);
                        line = strtok(NULL, "\n");
                    }
                    fflush(output_file);
                } else if (nread == 0) {
                    close(fd[i][0]);
                }
            }
        }

        int all_children_exited = 1;
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (FD_ISSET(fd[i][0], &read_fds)) {
                all_children_exited = 0;
                break;
            }
        }
        if (all_children_exited) {
            break;
        }
    }

    for (int i = 0; i < NUM_CHILDREN; i++) {
        waitpid(pids[i], NULL, 0);
    }
    fclose(output_file);
    printf("All child processes have terminated. Exiting parent process.\n");
    return 0;
}
