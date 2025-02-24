#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define CYLINDERS 5000
#define REQUESTS 10

int requests[REQUESTS] = {2055, 1175, 2304, 2700, 513, 1680, 256, 1401, 4922, 3692};
int current_position = 2255;
int previous_position = 1723;

// Function to calculate absolute difference
int abs_diff(int a, int b) {
    return abs(a - b);
}

// FCFS (First Come First Serve)
void fcfs() {
    int total_distance = 0;
    int current = current_position;

    printf("FCFS Order: %d", current);
    for (int i = 0; i < REQUESTS; i++) {
        total_distance += abs_diff(current, requests[i]);
        current = requests[i];
        printf(" -> %d", current);
    }
    printf("\nTotal Distance: %d\n", total_distance);
}

// SSTF (Shortest Seek Time First)
void sstf() {
    int total_distance = 0;
    int current = current_position;
    int visited[REQUESTS] = {0};

    printf("SSTF Order: %d", current);
    for (int i = 0; i < REQUESTS; i++) {
        int min_distance = INT_MAX;
        int next_index = -1;

        for (int j = 0; j < REQUESTS; j++) {
            if (!visited[j]) {
                int distance = abs_diff(current, requests[j]);
                if (distance < min_distance) {
                    min_distance = distance;
                    next_index = j;
                }
            }
        }

        visited[next_index] = 1;
        total_distance += min_distance;
        current = requests[next_index];
        printf(" -> %d", current);
    }
    printf("\nTotal Distance: %d\n", total_distance);
}

// SCAN (Elevator Algorithm)
void scan() {
    int total_distance = 0;
    int current = current_position;
    int direction = (current_position > previous_position) ? 1 : -1; // 1 for increasing, -1 for decreasing

    int sorted_requests[REQUESTS + 1];
    for (int i = 0; i < REQUESTS; i++) {
        sorted_requests[i] = requests[i];
    }
    sorted_requests[REQUESTS] = current;
    for (int i = 0; i < REQUESTS; i++) {
        for (int j = i + 1; j <= REQUESTS; j++) {
            if (sorted_requests[i] > sorted_requests[j]) {
                int temp = sorted_requests[i];
                sorted_requests[i] = sorted_requests[j];
                sorted_requests[j] = temp;
            }
        }
    }

    int index = 0;
    for (int i = 0; i <= REQUESTS; i++) {
        if (sorted_requests[i] == current) {
            index = i;
            break;
        }
    }

    printf("SCAN Order: %d", current);
    if (direction == 1) {
        for (int i = index + 1; i <= REQUESTS; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
        total_distance += abs_diff(current, CYLINDERS - 1);
        current = CYLINDERS - 1;
        printf(" -> %d", current);
        for (int i = index - 1; i >= 0; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    } else {
        for (int i = index - 1; i >= 0; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
        total_distance += abs_diff(current, 0);
        current = 0;
        printf(" -> %d", current);
        for (int i = index + 1; i <= REQUESTS; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    }
    printf("\nTotal Distance: %d\n", total_distance);
}

// LOOK Algorithm
void look() {
    int total_distance = 0;
    int current = current_position;
    int direction = (current_position > previous_position) ? 1 : -1; // 1 for increasing, -1 for decreasing

    int sorted_requests[REQUESTS + 1];
    for (int i = 0; i < REQUESTS; i++) {
        sorted_requests[i] = requests[i];
    }
    sorted_requests[REQUESTS] = current;
    for (int i = 0; i < REQUESTS; i++) {
        for (int j = i + 1; j <= REQUESTS; j++) {
            if (sorted_requests[i] > sorted_requests[j]) {
                int temp = sorted_requests[i];
                sorted_requests[i] = sorted_requests[j];
                sorted_requests[j] = temp;
            }
        }
    }

    int index = 0;
    for (int i = 0; i <= REQUESTS; i++) {
        if (sorted_requests[i] == current) {
            index = i;
            break;
        }
    }

    printf("LOOK Order: %d", current);
    if (direction == 1) {
        for (int i = index + 1; i <= REQUESTS; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
        for (int i = index - 1; i >= 0; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    } else {
        for (int i = index - 1; i >= 0; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
        for (int i = index + 1; i <= REQUESTS; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    }
    printf("\nTotal Distance: %d\n", total_distance);
}

// C-SCAN Algorithm
// C-SCAN Algorithm (Fully Corrected)
void c_scan() {
    int total_distance = 0;
    int current = current_position;
    int direction = (current_position > previous_position) ? 1 : -1; // Determine initial direction

    int sorted_requests[REQUESTS + 1];
    for (int i = 0; i < REQUESTS; i++) {
        sorted_requests[i] = requests[i];
    }
    sorted_requests[REQUESTS] = current;

    // Sorting requests
    for (int i = 0; i < REQUESTS; i++) {
        for (int j = i + 1; j <= REQUESTS; j++) {
            if (sorted_requests[i] > sorted_requests[j]) {
                int temp = sorted_requests[i];
                sorted_requests[i] = sorted_requests[j];
                sorted_requests[j] = temp;
            }
        }
    }

    // Find index of current position
    int index = 0;
    for (int i = 0; i <= REQUESTS; i++) {
        if (sorted_requests[i] == current) {
            index = i;
            break;
        }
    }

    printf("C-SCAN Order: %d", current);

    if (direction == 1) {
        // Move right
        for (int i = index + 1; i <= REQUESTS; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }

        // Move to end of disk (4999)
        total_distance += abs_diff(current, CYLINDERS - 1);
        current = CYLINDERS - 1;
        printf(" -> %d", current);

        // Jump to beginning (0)
        total_distance += CYLINDERS - 1;  // Distance from 4999 to 0
        current = 0;
        printf(" -> %d", current);

        // Move to lowest pending request
        for (int i = 0; i < index; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    } else {
        // Move left
        for (int i = index - 1; i >= 0; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }

        // Move to start of disk (0)
        total_distance += abs_diff(current, 0);
        current = 0;
        printf(" -> %d", current);

        // Jump to end (4999)
        total_distance += CYLINDERS - 1;
        current = CYLINDERS - 1;
        printf(" -> %d", current);

        // Move to highest pending request
        for (int i = REQUESTS; i > index; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    }

    printf("\nTotal Distance: %d\n", total_distance);
}


// C-LOOK Algorithm
// C-LOOK Algorithm (Fully Corrected)
void c_look() {
    int total_distance = 0;
    int current = current_position;
    int direction = (current_position > previous_position) ? 1 : -1; // Determine initial direction

    int sorted_requests[REQUESTS + 1];
    for (int i = 0; i < REQUESTS; i++) {
        sorted_requests[i] = requests[i];
    }
    sorted_requests[REQUESTS] = current;

    // Sorting requests
    for (int i = 0; i < REQUESTS; i++) {
        for (int j = i + 1; j <= REQUESTS; j++) {
            if (sorted_requests[i] > sorted_requests[j]) {
                int temp = sorted_requests[i];
                sorted_requests[i] = sorted_requests[j];
                sorted_requests[j] = temp;
            }
        }
    }

    // Find index of current position
    int index = 0;
    for (int i = 0; i <= REQUESTS; i++) {
        if (sorted_requests[i] == current) {
            index = i;
            break;
        }
    }

    printf("C-LOOK Order: %d", current);

    if (direction == 1) {
        // Move right
        for (int i = index + 1; i <= REQUESTS; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }

        // Jump to lowest request instead of 0
        total_distance += abs_diff(current, sorted_requests[0]);
        current = sorted_requests[0];
        printf(" -> %d", current);

        // Move up again
        for (int i = 1; i < index; i++) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    } else {
        // Move left
        for (int i = index - 1; i >= 0; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }

        // Jump to highest request instead of 4999
        total_distance += abs_diff(current, sorted_requests[REQUESTS]);
        current = sorted_requests[REQUESTS];
        printf(" -> %d", current);

        // Move down again
        for (int i = REQUESTS - 1; i > index; i--) {
            total_distance += abs_diff(current, sorted_requests[i]);
            current = sorted_requests[i];
            printf(" -> %d", current);
        }
    }

    printf("\nTotal Distance: %d\n", total_distance);
}


int main() {
    printf("FCFS:\n");
    fcfs();

    printf("\nSSTF:\n");
    sstf();

    printf("\nSCAN:\n");
    scan();

    printf("\nLOOK:\n");
    look();

    printf("\nC-SCAN:\n");
    c_scan();

    printf("\nC-LOOK:\n");
    c_look();

    return 0;
}