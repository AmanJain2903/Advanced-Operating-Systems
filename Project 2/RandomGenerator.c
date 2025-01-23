# include <stdio.h>
# include <stdlib.h>
# include <time.h>

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

void seedGenerator(Seeds seeds[]){
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

int main(){
    Seeds seeds[10000];
    seedGenerator(seeds);
    for(int i=0; i<5; i++){
        printf("For seed : %d, CPU Idle Time is : %d\n", seeds[i].seed, seeds[i].idleTime);
    }
}
