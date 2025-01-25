# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <limits.h>

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

void generateProcesses(Process processes[], int count, int seed){
    srand(seed);
    for (int i=0; i<count; i++){
        processes[i].name = 'a' + i;
        processes[i].arrivalTime = rand()%100;
        processes[i].runTime = rand()%10 + 1;
        processes[i].priority = rand()%4 + 1;
        processes[i].remainingTime = processes[i].runTime;
        processes[i].startTime = -1;
        processes[i].endTime = 0;
        processes[i].executedTime = 0;
    }
}

void sortProcesses(Process processes[], int count){
    for (int i=0; i<count-1; i++){
        for (int j=i+1; j<count; j++){
            if (processes[j].arrivalTime<processes[i].arrivalTime){
                Process temp = processes[j];
                processes[j] = processes[i];
                processes[i] = temp;
            }
        }
    }
}

void seedGenerator(Seeds bestSeeds[]){
    Seeds seeds[10000];
    Process processes[10];
    Process process;
    int idleTime;
    int currTime;
    int seedNumber=0;
    for (int seed=1; seed<=10000; seed++){
        generateProcesses(processes, 10, seed);
        sortProcesses(processes, 10);
        currTime = 0;
        idleTime = 0;
        for (int i=0; i<10; i++){
            process = processes[i];
            if (process.arrivalTime > currTime){
                idleTime += process.arrivalTime - currTime;
                currTime = process.arrivalTime + process.runTime;
            }
            else{
                currTime = process.arrivalTime + process.runTime;
            }
        }
        seeds[seedNumber].idleTime = idleTime;
        seeds[seedNumber++].seed = seed;
    }
    for (int i=0; i<9999; i++){
        for (int j=i+1; j<10000; j++){
            if (seeds[j].idleTime<seeds[i].idleTime){
                Seeds temp = seeds[j];
                seeds[j] = seeds[i];
                seeds[i] = temp;
            }
        }
    }
    for (int i=0;i<5;i++){
        bestSeeds[i] = seeds[i];
    }

}


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

void runSRTF(Process processes[], int count) {
	int currentTime = 0, completed = 0, minRemaining = INT_MAX;
	int shortest = -1;
	int isRunning = 0;
	int wait_time = 0, completion_time = 0, turnaround_time = 0;
	while(completed!=count) {
		for(int i=0;i<count;i++) {
			if(processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && processes[i].remainingTime < minRemaining) {
				shortest = i;
				minRemaining = processes[i].remainingTime;
				processes[i].startTime = currentTime;
				isRunning = 1;	
			}
		}
		if(isRunning==0) {
			currentTime++;
			continue;
		}
		processes[shortest].remainingTime--;
		minRemaining = processes[shortest].remainingTime;
		if(processes[shortest].remainingTime==0) {
			completed++;
			isRunning=0;
			processes[shortest].endTime = currentTime+1;
			minRemaining = INT_MAX;
		}
		currentTime++;
	}
}

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

void resetProcesses(Process processes[], int NUM_PROCESSES){
    for(int i =0; i<NUM_PROCESSES; i++){
        processes[i].startTime = processes[i].endTime = 0;
        processes[i].remainingTime = processes[i].runTime;
    }
}
