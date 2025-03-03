#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <fcntl.h>
#include "common.h"

#define NUM_CHILDREN 5
#define BUFFER_SIZE 128
#define OUTPUT_FILE "output.txt"

int main() {
    int fd[NUM_CHILDREN][2];
    pid_t pids[NUM_CHILDREN];
    fd_set read_fds;
    int max_fd = 0;
    FILE *output_file = fopen(OUTPUT_FILE, "w");

    if (!output_file) {
        perror("Failed to open output file");
        exit(EXIT_FAILURE);
    }

    // Create pipes and fork child processes
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
            // Child process
            close(fd[i][READ_END]);
            child_process(i, fd[i][WRITE_END]);
            close(fd[i][WRITE_END]);
            exit(0);
        } else {
            // Parent process
            close(fd[i][WRITE_END]);
            if (fd[i][READ_END] > max_fd) {
                max_fd = fd[i][READ_END];
            }
        }
    }

    // Parent process using select()
    while (1) {
        FD_ZERO(&read_fds);
        for (int i = 0; i < NUM_CHILDREN; i++) {
            FD_SET(fd[i][READ_END], &read_fds);
        }

        int activity = select(max_fd + 1, &read_fds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (FD_ISSET(fd[i][READ_END], &read_fds)) {
                char buffer[BUFFER_SIZE];
                ssize_t nread = read(fd[i][READ_END], buffer, BUFFER_SIZE);

                if (nread > 0) {
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    fprintf(output_file, "%ld:%06ld: %s", tv.tv_sec, tv.tv_usec, buffer);
                } else if (nread == 0) {
                    close(fd[i][READ_END]);
                }
            }
        }

        // Check if all children have exited
        int all_children_exited = 1;
        for (int i = 0; i < NUM_CHILDREN; i++) {
            if (FD_ISSET(fd[i][READ_END], &read_fds)) {
                all_children_exited = 0;
                break;
            }
        }

        if (all_children_exited) {
            break;
        }
    }

    fclose(output_file);
    return 0;
}