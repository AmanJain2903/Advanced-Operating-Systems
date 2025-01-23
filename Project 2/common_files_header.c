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

void srtf(Process processes[], int count) {
	int currentTime = 0, completed = 0, minRemaining = INT_MAX;
	int shortest = -1;
	int isRunning = 0;
	int wait_time = 0, completion_time = 0, turnaround_time = 0;
	while(completed!=count) {
		for(int i=0;i<count;i++) {
			if(processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && processes[i].remainingTime < minRemaining) {
				shortest = i;
				minRemaining = processes[i].remainingTime;
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
			completion_time = currentTime+1;
			turnaround_time = completion_time - processes[shortest].arrivalTime;
			wait_time = turnaround_time - processes[shortest].runTime;
			minRemaining = INT_MAX;
			printf("Process_Shortest: %d, CompletionTime: %d, TurnAroundTime: %d, WaitTime: %d",shortest,completion_time,turnaround_time,wait_time);
			//calculateMetrics(completion_time,turnaround_time,wait_time);
		}
		currentTime++;
	}
}
/*int main(){
    Seeds bestSeeds[5];
    seedGenerator(bestSeeds);
    for(int i=0; i<5; i++){
        printf("For seed : %d, CPU Idle Time is : %d\n", bestSeeds[i].seed, bestSeeds[i].idleTime);
    }
}*/
