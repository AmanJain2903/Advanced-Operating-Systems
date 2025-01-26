# include <stdio.h>
# include "common_files_header.h"

int main() {
    int NUM_PROCESSES=10;
    Process processes[NUM_PROCESSES];
    float avgTurnaround, avgWaiting, avgResponse;
    int throughput;
    Seeds bestSeeds[5];
    seedGenerator(bestSeeds);
    

    for (int run = 0; run < 5; run++) {
        printf("Run #%d:\n", run + 1);
        resetProcesses(processes, NUM_PROCESSES);
        

        // Generate random processes
        generateProcesses(processes, NUM_PROCESSES, bestSeeds[run].seed);
        printProcessTable(processes, NUM_PROCESSES);


        // FCFS
       /* runFCFS(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("FCFS: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput); 

        // SJF
        runSJF(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("SJF: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput); 
              
       //  SRTF
        srtf(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("SRTF: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput);*/

        printf("\n");

       // HPFP
       runHPFP(processes, NUM_PROCESSES); 
       printf("\n");

       // HPFNP
       runHPFNP(processes, NUM_PROCESSES);      
       printf("\n");
    }

    return 0;
}
