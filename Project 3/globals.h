#ifndef GLOBALS_H
#define GLOBALS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define NUM_ROWS 10
#define NUM_COLS 10
#define NUM_SELLERS 10
#define SIMULATION_TIME 60
#define SEAT_ID_LEN 4

typedef enum {
    HIGH_PRICE,
    MEDIUM_PRICE,
    LOW_PRICE
} SellerType;

// Forward declarations
typedef struct Customer Customer;
typedef struct Seller Seller;

// Customer structure
struct Customer {
    int id;
    int arrival_time;
    int start_time;
    int completion_time;
};

// Seller structure
struct Seller {
    char name[4];
    SellerType type;
    Customer* queue[100];
    int queue_size;
    pthread_t thread;
};

// Statistics structure
typedef struct {
    int served;
    int turned_away;
    long total_response_time;
    long total_turnaround_time;
} SellerStats;

// Global variables
extern pthread_mutex_t seat_mutexes[NUM_ROWS][NUM_COLS];
extern pthread_mutex_t stats_mutex;
extern char seating_chart[NUM_ROWS][NUM_COLS][SEAT_ID_LEN];
extern Seller sellers[NUM_SELLERS];
extern int global_time;
extern SellerStats high_stats;
extern SellerStats medium_stats;
extern SellerStats low_stats;

// Function declarations
void initialize_sellers(int customers_per_seller);
void initialize_seating_chart();
void print_seating_chart();
void destroy_seat_mutexes();
void* seller_thread(void* arg);
int assign_seat(Seller* seller, int customer_id);
int get_random_time(SellerType type);

#endif