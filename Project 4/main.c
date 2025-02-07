#include "common.h"

// Function to generate the next page reference based on locality of reference
int generateNextPageReference(int currentPage, int processSize) {
    int delta;
    if (rand() % 100 < 90) { // 90% probability: reuse recent pages
        // Reuse one of the last 5 pages (temporal locality)
        delta = (rand() % 5) - 2; // -2, -1, 0, +1, +2
    } else {
        // 10% probability: random page
        delta = (rand() % (processSize - 1)) + 1; // Ensure |delta| ≥ 1
        if (rand() % 2 == 0) {
            delta = -delta; // Randomly make delta negative
        }
    }

    // Calculate the next page reference with wrapping
    int nextPage = currentPage + delta;
    if (nextPage < 0) nextPage += processSize;
    else if (nextPage >= processSize) nextPage -= processSize;
    return nextPage;
}

void runSimulation(JobQueue *jobQueue, Memory *memory, int (*replacementAlgorithm)(Memory *, char, int, int), char *algorithmName, double *hitRatioSum, double *missRatioSum, int *swappedInSum) {
    int hit = 0, miss = 0, referenceCount = 0, swappedInProcesses = 0;
    Job* activeJobs[MAX_JOBS] = {NULL};
    int activeJobCount = 0;

    // Sort jobs by arrival time (already done in generateWorkload)
    Job* currentJob = jobQueue->head;

    // Simulate each millisecond (or REFERENCE_INTERVAL)
    for (int globalTime = 0; globalTime < SIMULATION_TIME * 1000; globalTime += REFERENCE_INTERVAL) {
        // Add jobs that have arrived by this time
        while (currentJob != NULL && currentJob->arrivalTime * 1000 <= globalTime && activeJobCount < MAX_JOBS) {
            activeJobs[activeJobCount++] = currentJob;
            currentJob = currentJob->next;
        }

        // Process active jobs
        for (int j = 0; j < activeJobCount; j++) {
            Job* job = activeJobs[j];
            if (job == NULL) continue; // Skip if job is NULL

            // Check if job has completed
            if (globalTime >= (job->arrivalTime + job->serviceDuration) * 1000) {
                // Remove all pages belonging to this job
                int new_count = 0;
                for (int k = 0; k < memory->count; k++) {
                    if (memory->pages[k].processName != job->processName) {
                        // Keep pages not belonging to the completed job
                        memory->pages[new_count] = memory->pages[k];
                        new_count++;
                    }
                }
                memory->count = new_count;
                activeJobs[j] = NULL; // Mark job as inactive
                continue;
            }

            // Generate page reference for active jobs
            if ((globalTime - job->arrivalTime * 1000) % REFERENCE_INTERVAL == 0) {
                int currentPage = rand() % job->processSize;
                int pageNumber = generateNextPageReference(currentPage, job->processSize);

                int found = 0;

                // Check if the page is already in memory
                for (int k = 0; k < memory->count; k++) {
                    if (memory->pages[k].processName == job->processName && memory->pages[k].pageNumber == pageNumber) {
                        hit++;
                        found = 1;
                        memory->pages[k].frequency++;
                        memory->pages[k].lastUsed = globalTime;
                        break;
                    }
                }

                if (!found) {
                    miss++;
                    int evictedIndex = replacementAlgorithm(memory, job->processName, pageNumber, globalTime);
                    if (evictedIndex != -1) {
                        printf("%8d | %7c | %15d | %14s | %c/%d\n",
                               globalTime / 1000, job->processName, pageNumber, "No", 
                               memory->pages[evictedIndex].processName, memory->pages[evictedIndex].pageNumber);
                    } else {
                        printf("%8d | %7c | %15d | %14s | %s\n",
                               globalTime / 1000, job->processName, pageNumber, "Yes", "N/A");
                    }

                    // Increment swappedInProcesses when a new page is loaded into memory
                    swappedInProcesses++;
                } else {
                    printf("%8d | %7c | %15d | %14s | %s\n",
                           globalTime / 1000, job->processName, pageNumber, "Yes", "N/A");
                }

                referenceCount++;
                if (referenceCount == 100) {
                    printf("\n=== First 100 References Completed ===\n");
                    referenceCount++; // Prevent this message from repeating
                }
            }
        }
    }

    // Calculate hit and miss ratios
    int totalAccesses = hit + miss;
    double hitRatio = (double)hit / totalAccesses;
    double missRatio = (double)miss / totalAccesses;

    printf("\n=== Simulation Results for %s ===\n", algorithmName);
    printf("Hit Ratio: %.2f\n", hitRatio);
    printf("Miss Ratio: %.2f\n", missRatio);
    printf("Processes Swapped-In: %d\n", swappedInProcesses);
    printf("---------------------------------\n");

    // Update sums for averages
    *hitRatioSum += hitRatio;
    *missRatioSum += missRatio;
    *swappedInSum += swappedInProcesses;
}

int main() {
    JobQueue jobQueue;
    generateWorkload(&jobQueue); // Generate workload once

    int (*algorithms[])(Memory *, char, int, int) = {FIFO, LRU, LFU, MFU, RandomPick};
    char *algorithmNames[] = {"FIFO", "LRU", "LFU", "MFU", "RandomPick"};

    for (int i = 0; i < 5; i++) {
        double hitRatioSum = 0, missRatioSum = 0;
        int swappedInSum = 0;

        for (int run = 0; run < 5; run++) {
            Memory memory; // Fresh memory for each run
            initializeMemory(&memory); 
            runSimulation(&jobQueue, &memory, algorithms[i], algorithmNames[i], &hitRatioSum, &missRatioSum, &swappedInSum);
        }

        // Calculate averages
        double avgHitRatio = hitRatioSum / 5;
        double avgMissRatio = missRatioSum / 5;
        double avgSwappedIn = (double)swappedInSum / 5;

        printf("\n=== Average Results for %s Algorithm ===\n", algorithmNames[i]);
        printf("Average Hit Ratio: %.2f\n", avgHitRatio);
        printf("Average Miss Ratio: %.2f\n", avgMissRatio);
        printf("Average Processes Swapped-In: %.2f\n", avgSwappedIn);
        printf("---------------------------------\n");
    }

    return 0;
}