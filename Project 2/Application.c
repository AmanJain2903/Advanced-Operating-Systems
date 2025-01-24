# include <stdio.h>
# include "common_files_header.h"

int main() {
    int NUM_PROCESSES=10;
    Process processes[NUM_PROCESSES];
    float avgTurnaround, avgWaiting, avgResponse;
    int throughput;
    Seeds bestSeeds[5];
    seedGenerator(bestSeeds);
    float fcfs[3], sjf[3], srtf[3];

    for (int run = 0; run < 5; run++) {
        printf("Run #%d:\n", run + 1);

        // Generate random processes
        generateProcesses(processes, NUM_PROCESSES, bestSeeds[run].seed);

        // FCFS
        runFCFS(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("FCFS: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput); 
        fcfs[0]+=avgTurnaround/5;
        fcfs[1]+=avgWaiting/5;
        fcfs[2]+=avgResponse/5;
        resetProcesses(processes, NUM_PROCESSES);

        // SJF
        runSJF(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("SJF: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput); 
        sjf[0]+=avgTurnaround/5;
        sjf[1]+=avgWaiting/5;
        sjf[2]+=avgResponse/5; 
        resetProcesses(processes, NUM_PROCESSES);

        //SRTF
        runSRTF(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("SRTF: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput);
        srtf[0]+=avgTurnaround/5;
        srtf[1]+=avgWaiting/5;
        srtf[2]+=avgResponse/5;
        resetProcesses(processes, NUM_PROCESSES);

        printf("\n");
    }

    printf("\nAVERAGE AFTER RUNNING ON 5 WORKLOADS\n");
    printf("FCFS: Turnaround = %.2f, Waiting = %.2f, Response = %.2f\n",
               fcfs[0], fcfs[1], fcfs[2]); 
    printf("SJF: Turnaround = %.2f, Waiting = %.2f, Response = %.2f\n",
               sjf[0], sjf[1], sjf[2]); 
    printf("SRTF: Turnaround = %.2f, Waiting = %.2f, Response = %.2f\n",
               srtf[0], srtf[1], srtf[2]); 

    return 0;
}
