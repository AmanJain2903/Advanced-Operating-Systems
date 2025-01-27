#include <stdbool.h>
#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H
#define MAX_PROCESSES 100
#define MAX_PRIORITY 4

typedef struct{
    char name;
    int arrivalTime;
    int runTime;
    int priority;
    int remainingTime;
    int startTime;
    int endTime;
    int executedTime;
    bool started;
} Process;

typedef struct{
    int seed;
    int idleTime;
} Seeds;

typedef struct {
    Process* queue[MAX_PROCESSES];
    int front;
    int rear;
    int size;
} PriorityQueue;

void seedGenerator(Seeds bestSeeds[]);
void generateProcesses(Process processes[], int count, int seed);
void sortProcesses(Process processes[], int count);
void printProcessTable(Process processes[], int count);
void printJobSequence(char* result);
char* runFCFS(Process processes[], int count);
char* runSJF(Process processes[], int count);
char* runSRTF(Process processes[], int count);
char* runHPFP(Process processes[], int count);
char* runHPFNP(Process processes[], int count);
void calculateMetrics(Process processes[], int count, float *avgTurnaround, float *avgWaiting, float *avgResponse, int *throughput);
void resetProcesses(Process processes[], int NUM_PROCESSES);


#endif

