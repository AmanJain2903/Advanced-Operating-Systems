#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

typedef struct{
    char name;
    int arrivalTime;
    int runTime;
    int priority;
    int remainingTime;
    int startTime;
    int endTime;
    int executedTime;
} Process;

typedef struct{
    int seed;
    int idleTime;
} Seeds;

void seedGenerator(Seeds bestSeeds[]);
void generateProcesses(Process processes[], int count, int seed);
void sortProcesses(Process processes[], int count);
void srtf(Process processes[], int count);

#endif
