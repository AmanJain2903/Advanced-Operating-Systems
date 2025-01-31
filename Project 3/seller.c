#include "globals.h"

int assign_seat(Seller* seller, int customer_id) {
    int seat_assigned = 0;
    char seat_id[SEAT_ID_LEN];
    snprintf(seat_id, SEAT_ID_LEN, "%s%d", seller->name, customer_id);

    if (seller->type == HIGH_PRICE) {
        snprintf(seat_id, SEAT_ID_LEN, "%s%02d", seller->name, customer_id);
        for (int row = 0; row < NUM_ROWS && !seat_assigned; row++) {
            for (int col = 0; col < NUM_COLS && !seat_assigned; col++) {
                pthread_mutex_lock(&seat_mutexes[row][col]);
                if (strcmp(seating_chart[row][col], "---") == 0) {
                    strcpy(seating_chart[row][col], seat_id);
                    seat_assigned = 1;
                }
                pthread_mutex_unlock(&seat_mutexes[row][col]);
            }
        }
    }
    else if (seller->type == LOW_PRICE) {
        for (int row = NUM_ROWS-1; row >= 0 && !seat_assigned; row--) {
            for (int col = 0; col < NUM_COLS && !seat_assigned; col++) {
                pthread_mutex_lock(&seat_mutexes[row][col]);
                if (strcmp(seating_chart[row][col], "---") == 0) {
                    strcpy(seating_chart[row][col], seat_id);
                    seat_assigned = 1;
                }
                pthread_mutex_unlock(&seat_mutexes[row][col]);
            }
        }
    }
    else if (seller->type == MEDIUM_PRICE) {
    int start_row = 4; // Row 5 (0-based index)
    int max_offset = 0;

    while (!seat_assigned && max_offset < NUM_ROWS) {
        // Check positive offset first, then negative
        for (int sign = 1; sign >= -1 && !seat_assigned; sign -= 2) {
            if (max_offset == 0 && sign == -1) continue; // Skip negative for first iteration

            int offset = sign * max_offset;
            int row = start_row + offset;

            if (row >= 0 && row < NUM_ROWS) {
                for (int col = 0; col < NUM_COLS && !seat_assigned; col++) {
                    pthread_mutex_lock(&seat_mutexes[row][col]);
                    if (strcmp(seating_chart[row][col], "---") == 0) {
                        strcpy(seating_chart[row][col], seat_id);
                        seat_assigned = 1;
                    }
                    pthread_mutex_unlock(&seat_mutexes[row][col]);
                }
            }
        }
        
        max_offset++;
    }
}

    return seat_assigned;
}

int get_random_time(SellerType type) {
    switch (type) {
        case HIGH_PRICE: return (rand() % 2) + 1;
        case MEDIUM_PRICE: return (rand() % 3) + 2;
        case LOW_PRICE: return (rand() % 4) + 4;
        default: return 0;
    }
}

void* seller_thread(void* arg) {
    Seller* seller = (Seller*)arg;
    
    while (global_time < SIMULATION_TIME) {
        if (seller->queue_size > 0) {
            Customer* customer = seller->queue[0];
            
            if (customer->arrival_time <= global_time) {
                // Track service start time
                customer->start_time = global_time;
                int sale_time = 0;
                int assigned = 0;

                // Attempt seat assignment
                assigned = assign_seat(seller, customer->id);
                
                if (assigned) {
                    // Get service time and simulate processing
                    sale_time = get_random_time(seller->type);
                    sleep(sale_time);  // Real seconds = simulated minutes
                    customer->completion_time = global_time + sale_time;

                    // Print transaction and updated seat chart
                    printf("\nTime: %02d - %s served C-%02d (Service Time: %d mins)",
                           global_time, seller->name, customer->id, sale_time);
                    print_seating_chart();
                }

                // Update statistics
                pthread_mutex_lock(&stats_mutex);
                if (seller->type == HIGH_PRICE) {
                    if (assigned) {
                        high_stats.served++;
                        high_stats.total_response_time += (customer->start_time - customer->arrival_time);
                        high_stats.total_turnaround_time += (customer->completion_time - customer->arrival_time);
                    } else {
                        high_stats.turned_away++;
                    }
                } else if (seller->type == MEDIUM_PRICE) {
                    if (assigned) {
                        medium_stats.served++;
                        medium_stats.total_response_time += (customer->start_time - customer->arrival_time);
                        medium_stats.total_turnaround_time += (customer->completion_time - customer->arrival_time);
                    } else {
                        medium_stats.turned_away++;
                    }
                } else {
                    if (assigned) {
                        low_stats.served++;
                        low_stats.total_response_time += (customer->start_time - customer->arrival_time);
                        low_stats.total_turnaround_time += (customer->completion_time - customer->arrival_time);
                    } else {
                        low_stats.turned_away++;
                    }
                }
                pthread_mutex_unlock(&stats_mutex);

                // Remove customer from queue
                for (int i = 0; i < seller->queue_size - 1; i++) {
                    seller->queue[i] = seller->queue[i + 1];
                }
                seller->queue_size--;
            }
        }
    }
    return NULL;
}