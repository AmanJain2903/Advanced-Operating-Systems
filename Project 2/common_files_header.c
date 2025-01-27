# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <limits.h>
#include <stdbool.h>
#include <string.h>
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


void generateProcesses(Process processes[], int count, int seed){
    srand(seed);
    for (int i=0; i<count; i++){
        processes[i].name = 'A' + i;
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

// print the process table with id , arrival time , run time and priority
void printProcessTable(Process processes[], int count) {
    printf("-------------------------------------------------\n");
    printf("Process ID | Arrival Time | Run Time | Priority |\n");
    printf("-------------------------------------------------\n");
    for (int i = 0; i < count; i++) {
        printf("%10c | %12.1d | %8.1d | %8d |\n", processes[i].name, processes[i].arrivalTime, processes[i].runTime, processes[i].priority);
    }
    printf("--------------------------------------------------\n");
    printf("Total No. of Processes : %d\n", count);
    printf("--------------------------------------------------\n");
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

void printJobSequence(char* result){
    for (int i=0; result[i]!='\0'; i++){
        printf("%c -> ", result[i]);
    }
    printf("\n");
}


char* runFCFS(Process processes[], int count) {
    char *result = malloc(100);
    result[0] = '\0';
    int currentTime = 0;
    for (int i = 0; i < count; i++) {
        if (processes[i].arrivalTime > currentTime) {
            currentTime = processes[i].arrivalTime;
        }
        processes[i].startTime = currentTime;
        processes[i].endTime = currentTime + processes[i].runTime;
        size_t len = strlen(result);
        result[len] = processes[i].name;
        result[len+1]='\0';
        currentTime = processes[i].endTime;
    }
    return result;
}

char* runSJF(Process processes[], int count) {
    char *result = malloc(100);
    result[0]='\0';
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
            size_t len = strlen(result);
            result[len] = processes[minIndex].name;
            result[len+1]='\0';

        } 
        else {
            currentTime++;
        }
    }
    return result;
}

char* runSRTF(Process processes[], int count) {
    char *result = malloc(100);
    result[0]='\0';
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
        processes[shortest].startTime = currentTime;
        size_t len = strlen(result);
        result[len] = processes[shortest].name;
        result[len+1]='\0';
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
    return result;
}

char* runRR(Process processes[], int count) {
    char *result = malloc(100);
    result[0] = '\0';

    int currentTime = 0;
    int completed = 0;
    int queue[MAX_PROCESSES], front = 0, rear = -1;

    // Enqueue processes that have arrived at time 0
    for (int i = 0; i < count; i++) {
        if (processes[i].arrivalTime == 0) {
            queue[++rear] = i;
        }
    }

    while (completed < count) {
        if (front <= rear) {
            int idx = queue[front++];

            // Process execution
            if (!processes[idx].started) {
                processes[idx].startTime = currentTime;
                processes[idx].started = true;
            }

            size_t len = strlen(result);
            result[len] = processes[idx].name;
            result[len + 1] = '\0';

            processes[idx].remainingTime--;
            currentTime++;

            // If process completes
            if (processes[idx].remainingTime == 0) {
                processes[idx].endTime = currentTime;
                completed++;
            } else {
                // Re-enqueue if not completed
                queue[++rear] = idx;
            }
        } else {
            // CPU is idle
            currentTime++;
        }

        // Enqueue newly arrived processes
        for (int i = 0; i < count; i++) {
            if (processes[i].arrivalTime == currentTime) {
                queue[++rear] = i;
            }
        }
    }

    return result;
}

// HPFP

// Initialize the queue circular queue is used
void initializeQueue(PriorityQueue* pq) {
    pq->front = 0;
    pq->rear = -1;
    pq->size = 0;
}

// Add a process to the queue
void enqueue(PriorityQueue* pq, Process* process) {
    if (pq->size < MAX_PROCESSES) {
        pq->rear = (pq->rear + 1) % MAX_PROCESSES;
        pq->queue[pq->rear] = process;
        pq->size++;
    }
}

// Remove and return the process at the front of the queue
Process* dequeue(PriorityQueue* pq) {
    if (pq->size > 0) {
        Process* process = pq->queue[pq->front];
        pq->front = (pq->front + 1) % MAX_PROCESSES;
        pq->size--;
        return process;
    }
    return NULL;
}

bool isEmpty(PriorityQueue* pq) {
    return pq->size == 0;
}

char* runHPFP(Process processes[], int count) {
    PriorityQueue priorityQueues[MAX_PRIORITY];
    for (int i = 0; i < MAX_PRIORITY; i++) {
        initializeQueue(&priorityQueues[i]);
    }
    char *result = malloc(100);
    result[0]='\0';
    int currentTime = 0;
    int completedProcesses = 0;

    while (completedProcesses < count) {
        // Enqueue newly arrived processes
        for (int i = 0; i < count; i++) {
            if (processes[i].arrivalTime == currentTime) {
                // Add the process to the queue corresponding to its priority and priority - 1 because priority index starts from 0
                enqueue(&priorityQueues[processes[i].priority - 1], &processes[i]);  

            }
        }

        // Find the highest priority queue with work
        Process* currentProcess = NULL;
        for (int i = 0; i < MAX_PRIORITY; i++) {
            if (!isEmpty(&priorityQueues[i])) {
                currentProcess = dequeue(&priorityQueues[i]);
                break;
            }
        }

        // If no process is available, move to the next time unit

        if (currentProcess == NULL) {
            currentTime++;
            continue;
        }
        // Execute the process for 1 quantum

        if (!currentProcess->started) {
            currentProcess->startTime = currentTime;
            currentProcess->started = true;
        }

        currentProcess->remainingTime--;

        // If the process completes, mark it

        if (currentProcess->remainingTime == 0) {
            currentProcess->endTime = currentTime + 1;
            completedProcesses++;
        } else {
            enqueue(&priorityQueues[currentProcess->priority - 1], currentProcess);
        }

        currentTime++;
        size_t len = strlen(result);
        result[len] = currentProcess->name;
        result[len+1]='\0';
    }
    return result;

}

char* runHPFNP(Process processes[], int count) {
    PriorityQueue priorityQueues[MAX_PRIORITY];
    for (int i = 0; i < MAX_PRIORITY; i++) {
        initializeQueue(&priorityQueues[i]);
    }
    char *result = malloc(100);
    result[0]='\0';
    int currentTime = 0;
    int completedProcesses = 0;

    while (completedProcesses < count) {
        // Enqueue newly arrived processes
        for (int i = 0; i < count; i++) {
            if (processes[i].arrivalTime == currentTime) {
                enqueue(&priorityQueues[processes[i].priority - 1], &processes[i]);
            }
        }

        // Find the highest priority queue with work
        Process* currentProcess = NULL;
        for (int i = 0; i < MAX_PRIORITY; i++) {
            if (!isEmpty(&priorityQueues[i])) {
                currentProcess = dequeue(&priorityQueues[i]);
                break;
            }
        }

        // If no process is available, CPU idles
        if (currentProcess == NULL) {
            currentTime++;
            continue;
        }

        // Execute the process until completion
        if (!currentProcess->started) {
            currentProcess->startTime = currentTime;
            currentProcess->started = true;
        }

        currentTime += currentProcess->remainingTime;
        currentProcess->remainingTime = 0;
        currentProcess->endTime = currentTime;
        completedProcesses++;
        size_t len = strlen(result);
        result[len] = currentProcess->name;
        result[len+1]='\0';
    }
    return result;
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
        processes[i].started = false; 
    }
}
