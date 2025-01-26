#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "common_files_header.h"

// Initialize a priority queue
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
            // If not completed, re-enqueue the process
            enqueue(&priorityQueues[currentProcess->priority - 1], currentProcess);
        }

        currentTime++;
    }
    printStats(processes , count , completedProcesses);
}

void printStats(Process processes[] , int count , int completedProcesses) {

    // Now we will calculate and print statistics for each priority level
    double avgTurnaroundTime[MAX_PRIORITY] = {0};
    double avgWaitingTime[MAX_PRIORITY] = {0};
    double avgResponseTime[MAX_PRIORITY] = {0};
    int priorityProcessCount[MAX_PRIORITY] = {0};
    
    // Group processes by priority and display the details for each group
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

        // Calculate average statistics for this priority level
        if (priorityProcessCount[p] > 0) {
            avgTurnaroundTime[p] /= priorityProcessCount[p];
            avgWaitingTime[p] /= priorityProcessCount[p];
            avgResponseTime[p] /= priorityProcessCount[p];

            // Display the averages at the end of the columns
	        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
            printf("%16s|%14.1f|%12.1f|%10.1f|%10.1f|%15.1f|%11.1f| %17.1f|\n"," Average", 
                   0.0, 
                   0.0, 
                   0.0,
                   0.0, 
                   totalWaitTime / priorityProcessCount[p],
                   totalResponse / priorityProcessCount[p],
                   totalTurnaroundTime / priorityProcessCount[p]
                );
	        printf("-----------------------------------------------------------------------------------------------------------------------------\n");
        }
    }

}