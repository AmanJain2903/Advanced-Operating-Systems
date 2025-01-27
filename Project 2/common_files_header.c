#include "common_files_header.h"
# include <stdio.h>
# include <stdlib.h>
# include <time.h>
# include <limits.h>
#include <stdbool.h>

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

    // resetProcesses(processes, count);
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

// Check if the queue is empty
bool isEmpty(PriorityQueue* pq) {
    return pq->size == 0;
}

// Function to run HPF (Preemptive)
void runHPFP(Process processes[], int count) {
    PriorityQueue priorityQueues[MAX_PRIORITY];
    for (int i = 0; i < MAX_PRIORITY; i++) {
        initializeQueue(&priorityQueues[i]);
    }

    printf("\nHighest Priority First Preemptive:\n");
    printf("Order of Processes in Execution: _");

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

        //  printf("%c ", currentProcess->name);
    }
    printStats(processes, count , completedProcesses);
}

// Function to run HPF (Non-preemptive)
void runHPFNP(Process processes[], int count) {
    PriorityQueue priorityQueues[MAX_PRIORITY];
    for (int i = 0; i < MAX_PRIORITY; i++) {
        initializeQueue(&priorityQueues[i]);
    }

    printf("\nHighest Priority First Nonpreemptive:\n");
    printf("Order of Processes in Execution: _");

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

        // printf("%c ", currentProcess->name);
    }
    printStats(processes, count , completedProcesses);
}


void printStats(Process processes[] , int count, int completedProcesses) {

    // Initialize the arrays to store the average statistics for each priority level
    double avgTurnaroundTime[MAX_PRIORITY] = {0};
    double avgWaitingTime[MAX_PRIORITY] = {0};
    double avgResponseTime[MAX_PRIORITY] = {0};
    int priorityProcessCount[MAX_PRIORITY] = {0};  // Count of num of processes belong to each priority level

    // Calculate and print statistics for each priority level
    
    for (int p = 0; p < MAX_PRIORITY; p++) {
        printf("\nPriority %d Processes:\n", p + 1);
        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	    printf("Process Name\t| Arrival Time | Start Time | End Time | Run Time | Response Time | Wait Time | Turn Around Time | Priority |\n");
	    printf("-----------------------------------------------------------------------------------------------------------------------------\n");
	
        double totalArrivalTime = 0, totalStartTime = 0, totalEndTime = 0;
        double totalRunTime = 0, totalWaitTime = 0, totalResponse= 0, totalTurnaroundTime = 0;

        for (int i = 0; i < count; i++) {
            if (processes[i].priority == p + 1) {
                int turnaroundTime = processes[i].endTime - processes[i].arrivalTime;
                int waitTime = turnaroundTime - processes[i].runTime;
                int response = processes[i].startTime - processes[i].arrivalTime;

                avgTurnaroundTime[p] += turnaroundTime;
                avgWaitingTime[p] += waitTime;
                avgResponseTime[p] += (processes[i].startTime - processes[i].arrivalTime);
                priorityProcessCount[p]++;

                totalArrivalTime += processes[i].arrivalTime;
                totalStartTime += processes[i].startTime;
                totalEndTime += processes[i].endTime;
                totalRunTime += processes[i].runTime;
                totalWaitTime += waitTime;
                totalTurnaroundTime += turnaroundTime;
                totalResponse += response;

                printf("%16c|%14.1d|%12.1d|%10.1d|%10.1d|%15.1d|%11.1d| %17.1d|%10u|\n",
                       processes[i].name, processes[i].arrivalTime, processes[i].startTime, processes[i].endTime,
                       processes[i].runTime, response ,waitTime, turnaroundTime, processes[i].priority);
            }
        }

        if (priorityProcessCount[p] > 0) {
            avgTurnaroundTime[p] /= priorityProcessCount[p];
            avgWaitingTime[p] /= priorityProcessCount[p];
            avgResponseTime[p] /= priorityProcessCount[p];

	        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
            printf("%16s|%14.1f|%12.1f|%10.1f|%10.1f|%15.1f|%11.1f| %17.1f|\n"," Average", 
                   0.0, 
                   0.0, 
                   0.0,
                   0.0, 
                   totalResponse / priorityProcessCount[p],
                   totalWaitTime / priorityProcessCount[p],
                   totalTurnaroundTime / priorityProcessCount[p]
                );
	        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
        }
    }

    double totalAvgTurnaroundTime = 0;
    double totalAvgWaitingTime = 0;
    double totalAvgResponseTime = 0;
    int totalProcesses = 0;

    for (int i = 0; i < MAX_PRIORITY; i++) {
        totalAvgTurnaroundTime += avgTurnaroundTime[i];
        totalAvgWaitingTime += avgWaitingTime[i];
        totalAvgResponseTime += avgResponseTime[i];
        totalProcesses += priorityProcessCount[i];
    }

    printf("\nOverall Statistics:\n");
    printf("Avg Turnaround Time: %.2f\n", totalAvgTurnaroundTime / completedProcesses);
    printf("Avg Waiting Time: %.2f\n", totalAvgWaitingTime / completedProcesses);
    printf("Avg Response Time: %.2f\n", totalAvgResponseTime / completedProcesses);
    printf("Throughput: %d\n", completedProcesses);
}


void resetProcesses(Process processes[], int count) {
    for (int i = 0; i < count; i++) {
        processes[i].startTime = -1;  
        processes[i].endTime = -1;    
        processes[i].remainingTime = processes[i].runTime; 
        processes[i].started = false; 
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
