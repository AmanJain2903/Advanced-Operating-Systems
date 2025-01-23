# include <stdio.h>
# include "RandomGenerator.c"

void calculateMetrics(Process processes[], int count, float *avgTurnaround, float *avgWaiting, float *avgResponse, int *throughput) {
    int totalTurnaround = 0, totalWaiting = 0, totalResponse = 0;
    int completedProcesses = 0;

    for (int i = 0; i < count; i++) {
        if (processes[i].endTime > 0) {
            int turnaround = processes[i].endTime - processes[i].arrivalTime;
            int waiting = turnaround - processes[i].runTime;
            int response = processes[i].startTime - processes[i].arrivalTime;

            totalTurnaround += turnaround;
            totalWaiting += waiting;
            totalResponse += response;
            completedProcesses++;
        }
    }
    *avgTurnaround = totalTurnaround / (float)completedProcesses;
    *avgWaiting = totalWaiting / (float)completedProcesses;
    *avgResponse = totalResponse / (float)completedProcesses;
    *throughput = completedProcesses;
}