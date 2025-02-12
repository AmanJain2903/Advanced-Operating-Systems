#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_PAGES 100
#define MAX_JOBS 150
#define PAGE_SIZE 1 // 1 MB
#define SIMULATION_TIME 60 // 1 minute
#define REFERENCE_INTERVAL 100 // 100 ms

typedef struct Job {
    char processName;
    int processSize; // in pages
    int arrivalTime; // in seconds
    int serviceDuration; // in seconds
    int currentPage; // Current page number
    struct Job* next; // Pointer to the next job in the linked list
} Job;

typedef struct {
    char processName;
    int pageNumber;
    int lastUsed; // For LRU
    int frequency; // For LFU and MFU
    int timestamp; // For FIFO
} Page;

typedef struct {
    Page pages[MAX_PAGES];
    int count;
} Memory;

typedef struct {
    Job* head; // Head of the linked list of jobs
    int count;
} JobQueue;

// Function declarations
void initializeMemory(Memory *memory);
void generateWorkload(JobQueue *jobQueue);
void sortJobsByArrivalTime(JobQueue *jobQueue);
void runSimulation(JobQueue *jobQueue, Memory *memory, int (*replacementAlgorithm)(Memory *, char, int, int), char *algorithmName, double *hitRatioSum, double *missRatioSum, int *swappedInSum);
int FIFO(Memory *memory, char processName, int pageNumber, int timestamp);
int LRU(Memory *memory, char processName, int pageNumber, int timestamp);
int LFU(Memory *memory, char processName, int pageNumber, int timestamp);
int MFU(Memory *memory, char processName, int pageNumber, int timestamp);
int RandomPick(Memory *memory, char processName, int pageNumber, int timestamp);
void runSimulation(JobQueue *jobQueue, Memory *memory, int (*replacementAlgorithm)(Memory *, char, int, int), char *algorithmName, double *hitRatioSum, double *missRatioSum, int *swappedInSum);

#endif // COMMON_H