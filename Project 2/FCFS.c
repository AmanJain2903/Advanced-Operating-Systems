# include <stdio.h>
# include "RandomGenerator.c"

void runFCFS(Process processes[], int count) {
    int currentTime = 0;
    for (int i = 0; i < count; i++) {
        if (processes[i].arrivalTime > currentTime) {
            currentTime = processes[i].arrivalTime;
        }
        processes[i].startTime = currentTime;
        processes[i].endTime = currentTime + processes[i].runTime;
        currentTime = processes[i].endTime;
    }
}
