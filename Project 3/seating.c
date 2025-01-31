#include "globals.h"

void initialize_seating_chart() {
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            strcpy(seating_chart[row][col], "---");
            pthread_mutex_init(&seat_mutexes[row][col], NULL);
        }
    }
}

void destroy_seat_mutexes() {
    for (int row = 0; row < NUM_ROWS; row++) {
        for (int col = 0; col < NUM_COLS; col++) {
            pthread_mutex_destroy(&seat_mutexes[row][col]);
        }
    }
}

void print_seating_chart() {
    printf("\n\nCurrent Seating Chart:\n");
    

    // Print chart
    for (int row = 0; row < NUM_ROWS; row++) {
        printf("Row %02d: ", row+1);
        for (int col = 0; col < NUM_COLS; col++) {
            printf("%.3s ", seating_chart[row][col]);
        }
        printf("\n");
    }
    printf("\n");
}