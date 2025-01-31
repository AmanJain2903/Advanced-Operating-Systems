#include "globals.h"

void initialize_sellers(int customers_per_seller) {
    // High-priced seller
    strncpy(sellers[0].name, "H", sizeof(sellers[0].name));

    sellers[0].type = HIGH_PRICE;
    sellers[0].queue_size = 0;
    for (int i = 0; i < customers_per_seller; i++) {
        Customer* c = malloc(sizeof(Customer));
        c->id = i + 1;
        c->arrival_time = rand() % SIMULATION_TIME;
        sellers[0].queue[sellers[0].queue_size++] = c;
    }

    // Medium-priced sellers
    for (int m = 1; m <= 3; m++) {
        sprintf(sellers[m].name, "M%d", m);
        sellers[m].type = MEDIUM_PRICE;
        sellers[m].queue_size = 0;
        for (int i = 0; i < customers_per_seller; i++) {
            Customer* c = malloc(sizeof(Customer));
            c->id = i + 1;
            c->arrival_time = rand() % SIMULATION_TIME;
            sellers[m].queue[sellers[m].queue_size++] = c;
        }
    }

    // Low-priced sellers
    for (int l = 4; l < NUM_SELLERS; l++) {
        sprintf(sellers[l].name, "L%d", l-3);
        sellers[l].type = LOW_PRICE;
        sellers[l].queue_size = 0;
        for (int i = 0; i < customers_per_seller; i++) {
            Customer* c = malloc(sizeof(Customer));
            c->id = i + 1;
            c->arrival_time = rand() % SIMULATION_TIME;
            sellers[l].queue[sellers[l].queue_size++] = c;
        }
    }
}