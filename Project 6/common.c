#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include "common.h"

void child_process(int child_id, int write_fd) {
    struct timeval tv;
    char buffer[128];
    int msg_count = 0;

    while (1) {
        if (child_id == 4) {
            // Child 5 interacts with the terminal
            printf("Child %d: Enter a message: ", child_id + 1);
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                break;
            }
        } else {
            // Other children sleep and generate messages
            usleep((rand() % 3) * 1000000); // Sleep for 0, 1, or 2 seconds
            gettimeofday(&tv, NULL);
            snprintf(buffer, sizeof(buffer), "%d:%06d: Child %d message %d\n",
                     (int)tv.tv_sec, (int)tv.tv_usec, child_id + 1, ++msg_count);
        }

        write(write_fd, buffer, strlen(buffer) + 1);
    }
}