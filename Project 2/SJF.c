# include <stdio.h>
# include <limits.h>
# include "RandomGenerator.c"

void runSJF(Process processes[], int count) {
    int completed = 0, currentTime = 0;

    while (completed < count) {
        int minIndex = -1, minRunTime = INT_MAX;
        for (int i = 0; i < count; i++) {
            if (processes[i].remainingTime > 0 && processes[i].arrivalTime <= currentTime) {
                if (processes[i].runTime < minRunTime) {
                    minRunTime = processes[i].runTime;
                    minIndex = i;
                }
            }
        }

        if (minIndex != -1) {
            processes[minIndex].startTime = currentTime;
            currentTime += processes[minIndex].runTime;
            processes[minIndex].remainingTime = 0;
            processes[minIndex].endTime = currentTime;
            completed++;
        } 
        else {
            currentTime++;
        }
    }
}