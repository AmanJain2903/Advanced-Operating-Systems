#include "globals.h"

pthread_mutex_t seat_mutexes[NUM_ROWS][NUM_COLS];
char seating_chart[NUM_ROWS][NUM_COLS][SEAT_ID_LEN];
Seller sellers[NUM_SELLERS];
int global_time = 0;

pthread_mutex_t stats_mutex;
SellerStats high_stats = {0, 0, 0L, 0L};
SellerStats medium_stats = {0, 0, 0L, 0L};
SellerStats low_stats = {0, 0, 0L, 0L};