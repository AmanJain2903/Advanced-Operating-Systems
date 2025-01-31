#include "globals.h"

void print_statistics(const char* type, SellerStats stats) {
    printf("%s-priced:\n", type);
    printf("  Served: %d\n", stats.served);
    printf("  Turned Away: %d\n", stats.turned_away);
    
    if (stats.served > 0) {
        printf("  Avg Response Time: %.2f mins\n", 
              (float)stats.total_response_time/stats.served);
        printf("  Avg Turnaround Time: %.2f mins\n",
              (float)stats.total_turnaround_time/stats.served);
        printf("  Throughput: %.2f customers/min\n",
              (float)stats.served/SIMULATION_TIME);
    } else {
        printf("  No customers served\n");
    }
}

int main(int argc, char* argv[]) {
    pthread_mutex_init(&stats_mutex, NULL);
    if (argc != 2) {
        printf("Usage: %s <customers_per_seller>\n", argv[0]);
        return 1;
    }

    int customers_per_seller = atoi(argv[1]);
    if (customers_per_seller <= 0) {
        printf("Customers per seller must be > 0.\n");
        return 1;
    }

    srand(time(NULL));
    initialize_seating_chart();
    initialize_sellers(customers_per_seller);

    for (int i = 0; i < NUM_SELLERS; i++) {
        pthread_create(&sellers[i].thread, NULL, seller_thread, &sellers[i]);
    }

    for (global_time = 0; global_time < SIMULATION_TIME; global_time++) {
        printf("\nTime: %02d", global_time);
        fflush(stdout);
        sleep(1);
    }

    for (int i = 0; i < NUM_SELLERS; i++) {
        pthread_join(sellers[i].thread, NULL);
    }

    for (int i = 0; i < NUM_SELLERS; i++) {
    Seller* s = &sellers[i];
    pthread_mutex_lock(&stats_mutex);
    if (s->type == HIGH_PRICE) {
        high_stats.turned_away += s->queue_size;
    } else if (s->type == MEDIUM_PRICE) {
        medium_stats.turned_away += s->queue_size;
    } else {
        low_stats.turned_away += s->queue_size;
    }
    pthread_mutex_unlock(&stats_mutex);
    
    // Free remaining customers
    for (int j = 0; j < s->queue_size; j++) {
        free(s->queue[j]);
    }
}

    printf("\nFinal Results:");
    print_seating_chart();

    printf("\nDetailed Statistics:\n");
    print_statistics("High", high_stats);
    print_statistics("Medium", medium_stats);
    print_statistics("Low", low_stats);

    

    destroy_seat_mutexes();
    pthread_mutex_destroy(&stats_mutex);
    return 0;
}