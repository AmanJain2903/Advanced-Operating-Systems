# include <stdio.h>
# include "RandomGenerator.c"
# include "SJF.c"
# include "FCFS.c"
# include "Metrics.c"

int main() {
    int NUM_PROCESSES=10;
    Process processes[NUM_PROCESSES];
    float avgTurnaround, avgWaiting, avgResponse;
    int throughput;

    for (int run = 0; run < 5; run++) {
        printf("Run #%d:\n", run + 1);

        // Generate random processes
        generateProcesses(processes, NUM_PROCESSES, run);

        // FCFS
        runFCFS(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("FCFS: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput);

        // SJF
        runSJF(processes, NUM_PROCESSES);
        calculateMetrics(processes, NUM_PROCESSES, &avgTurnaround, &avgWaiting, &avgResponse, &throughput);
        printf("SJF: Turnaround = %.2f, Waiting = %.2f, Response = %.2f, Throughput = %d\n",
               avgTurnaround, avgWaiting, avgResponse, throughput);

        printf("\n");
    }

    return 0;
}