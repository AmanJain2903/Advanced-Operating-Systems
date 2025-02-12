#include "common.h"
#include <limits.h>

void initializeMemory(Memory *memory) {
    memory->count = 0;
}

void generateWorkload(JobQueue *jobQueue) {
    srand(time(NULL));
    jobQueue->head = NULL;
    jobQueue->count = 0;

    for (int i = 0; i < MAX_JOBS; i++) {
        Job* newJob = (Job*)malloc(sizeof(Job));
        newJob->processName = 'A' + (i % 26);
        int sizes[] = {5, 11, 17, 31};
        newJob->processSize = sizes[rand() %4]; // Sizes: 5, 11, 17, 31 pages
        newJob->currentPage = -1;

        // Spread arrival times to reduce overlap

        newJob->arrivalTime = rand() % SIMULATION_TIME; // Arrival time in seconds 

        // Increase service duration to keep jobs active longer
        newJob->serviceDuration = (rand() % 10) + 20; // 20 - 29 seconds

        newJob->next = NULL;

        // Insert into job queue (sorted by arrival time)
        if (jobQueue->head == NULL) {
            jobQueue->head = newJob;
        } else {
            Job* current = jobQueue->head;
            while (current->next != NULL && current->next->arrivalTime < newJob->arrivalTime) {
                current = current->next;
            }
            newJob->next = current->next;
            current->next = newJob;
        }
        jobQueue->count++;
    }
}

int FIFO(Memory *memory, char processName, int pageNumber, int timestamp) {
    if (memory->count < MAX_PAGES) {
        memory->pages[memory->count].processName = processName;
        memory->pages[memory->count].pageNumber = pageNumber;
        memory->pages[memory->count].timestamp = timestamp;
        memory->count++;
        return -1; // No page evicted
    } else {
        int oldestIndex = 0;
        for (int i = 1; i < memory->count; i++) {
            if (memory->pages[i].timestamp < memory->pages[oldestIndex].timestamp) {
                oldestIndex = i;
            }
        }
        memory->pages[oldestIndex].processName = processName;
        memory->pages[oldestIndex].pageNumber = pageNumber;
        memory->pages[oldestIndex].timestamp = timestamp;
        memory->pages[oldestIndex].frequency = 1;
        memory->pages[oldestIndex].lastUsed = timestamp;
        return oldestIndex;
    }
}

// LFU Implementation
int LFU(Memory *memory, char processName, int pageNumber, int timestamp) {

    // Add new page if memory is not full
    if (memory->count < MAX_PAGES) {
        memory->pages[memory->count].processName = processName;
        memory->pages[memory->count].pageNumber = pageNumber;
        memory->pages[memory->count].frequency = 1;
        memory->pages[memory->count].lastUsed = timestamp;
        memory->count++;
        return -1; // No eviction
    }

    // Find the page with the least frequency
    int lfuIndex = 0;
    for (int i = 1; i < memory->count; i++) {
        if (memory->pages[i].frequency < memory->pages[lfuIndex].frequency) {
            lfuIndex = i;
        } else if (memory->pages[i].frequency == memory->pages[lfuIndex].frequency) {
            // Tiebreaker: LRU
            if (memory->pages[i].lastUsed < memory->pages[lfuIndex].lastUsed) {
                lfuIndex = i;
            }
        }
    }

    // Replace the page
    memory->pages[lfuIndex].processName = processName;
    memory->pages[lfuIndex].pageNumber = pageNumber;
    memory->pages[lfuIndex].timestamp = timestamp;
    memory->pages[lfuIndex].frequency = 1;
    memory->pages[lfuIndex].lastUsed = timestamp;
    return lfuIndex;
}

// LRU Implementation
int LRU(Memory *memory, char processName, int pageNumber, int timestamp) {

    // If memory is not full, add the new page
    if (memory->count < MAX_PAGES) {
        memory->pages[memory->count].processName = processName;
        memory->pages[memory->count].pageNumber = pageNumber;
        memory->pages[memory->count].lastUsed = timestamp;
        memory->count++;
        return -1; // No eviction
    }

    // Find the least recently used VALID page
    int lruIndex = 0;
    for (int i = 1; i < memory->count; i++) {
        if (memory->pages[i].lastUsed < memory->pages[lruIndex].lastUsed) {
            lruIndex = i;
        }
    }

    // Replace the LRU page
    memory->pages[lruIndex].processName = processName;
    memory->pages[lruIndex].pageNumber = pageNumber;
    memory->pages[lruIndex].timestamp = timestamp;
    memory->pages[lruIndex].frequency = 1;
    memory->pages[lruIndex].lastUsed = timestamp;
    return lruIndex;
}

int MFU(Memory *memory, char processName, int pageNumber, int timestamp) {

    // Add new page if memory is not full
    if (memory->count < MAX_PAGES) {
        memory->pages[memory->count].processName = processName;
        memory->pages[memory->count].pageNumber = pageNumber;
        memory->pages[memory->count].frequency = 1;
        memory->pages[memory->count].lastUsed = timestamp;
        memory->count++;
        return -1; // No eviction
    }

    // Find the page with the highest frequency
    int mfuIndex = 0;
    for (int i = 1; i < memory->count; i++) {
        if (memory->pages[i].frequency > memory->pages[mfuIndex].frequency) {
            mfuIndex = i;
        }
    }

    // Replace the page
    memory->pages[mfuIndex].processName = processName;
    memory->pages[mfuIndex].pageNumber = pageNumber;
    memory->pages[mfuIndex].timestamp = timestamp;
    memory->pages[mfuIndex].frequency = 1;
    memory->pages[mfuIndex].lastUsed = timestamp;
    return mfuIndex;
}

int RandomPick(Memory *memory, char processName, int pageNumber, int timestamp) {

    if (memory->count < MAX_PAGES) {
        memory->pages[memory->count].processName = processName;
        memory->pages[memory->count].pageNumber = pageNumber;
        memory->pages[memory->count].frequency = 1;
        memory->pages[memory->count].lastUsed = timestamp;
        memory->count++;
        return -1; // No page evicted
    }

    // Randomly select a page for eviction
    int randomIndex = rand() % memory->count;
    memory->pages[randomIndex].processName = processName;
    memory->pages[randomIndex].pageNumber = pageNumber;
    memory->pages[randomIndex].timestamp = timestamp;
    memory->pages[randomIndex].frequency = 1;
    memory->pages[randomIndex].lastUsed = timestamp;
    return randomIndex; // Return the index of the evicted page
}

// Function to generate the next page reference based on locality of reference
int generateNextPageReference(int currentPage, int processSize) {
    int delta;
    int randomNumber = rand() % 10 + 1;

    if (currentPage==-1){
        return 0;
    }

    // 70% chance of small delta (locality of reference)
    if (randomNumber <= 7) {
        delta = (rand() % 3) - 1; // -1, 0, or 1
    } else {
        // 30% chance of larger delta
        delta = (rand() % 8) + 2; 
        if (rand() % 2 == 0) {
            delta = -1*delta; // Randomly make it negative
        }
    }

    // Calculate the next page reference with wrapping
    int nextPage = currentPage + delta;
    if (nextPage < 0) {
        nextPage += processSize;
    } else if (nextPage >= processSize) {
        nextPage -= processSize;
    }

    return nextPage;
}

void runSimulation(JobQueue *jobQueue, Memory *memory, int (*replacementAlgorithm)(Memory *, char, int, int), char *algorithmName, double *hitRatioSum, double *missRatioSum, int *swappedInSum) {
    int hit = 0, miss = 0, referenceCount = 0, swappedInProcesses = 0;
    Job* activeJobs[MAX_JOBS] = {NULL};
    int activeJobCount = 0;
    int pageNumber;

    printf("\n=== Running Simulation with %s Algorithm ===\n", algorithmName);
    printf("Time (s) | Process | Page Referenced | Page in Memory | Evicted Process/Page\n");
    printf("---------|---------|-----------------|----------------|----------------------\n");

    Job* currentJob = jobQueue->head;

    for (int globalTime = 0; globalTime < SIMULATION_TIME * 1000; globalTime += REFERENCE_INTERVAL) {
        // Add jobs that have arrived by this time
        while (currentJob!=NULL && currentJob->arrivalTime * 1000 <= globalTime && memory->count <= 96) {
            activeJobs[activeJobCount++] = currentJob;
            currentJob = currentJob->next;
        }

        // Process active jobs
        for (int j = 0; j < activeJobCount; j++) {
            Job* job = activeJobs[j];
            if (job == NULL) continue;

            // Check if job has completed
            if (globalTime >= (job->arrivalTime + job->serviceDuration) * 1000) {
                // Remove all pages belonging to this job
                int new_count = 0;
                for (int k = 0; k < memory->count; k++) {
                    if (memory->pages[k].processName != job->processName) {
                        memory->pages[new_count++] = memory->pages[k];
                    }
                }
                memory->count = new_count;
                activeJobs[j] = NULL; // Mark job as inactive
                continue;
            }

            // Generate page reference for active jobs
            if ((globalTime - job->arrivalTime * 1000) % REFERENCE_INTERVAL == 0) {
                job->currentPage = generateNextPageReference(job->currentPage, job->processSize);

                int found = 0;
                for (int k = 0; k < memory->count; k++) {
                    if (memory->pages[k].processName == job->processName && memory->pages[k].pageNumber == job->currentPage) {
                        hit++;
                        found = 1;
                        memory->pages[k].lastUsed = globalTime;
                        memory->pages[k].frequency++;
                        break;
                    }
                }

                if (!found) {
                    miss++;
                    int evictedIndex = replacementAlgorithm(memory, job->processName, job->currentPage, globalTime);
                    if (evictedIndex != -1) {
                        printf("%8d | %7c | %15d | %14s | %c/%d\n",
                               globalTime / 1000, job->processName, job->currentPage, "No", 
                               memory->pages[evictedIndex].processName, memory->pages[evictedIndex].pageNumber);
                    } else {
                        printf("%8d | %7c | %15d | %14s | %s\n",
                               globalTime / 1000, job->processName, job->currentPage, "Yes", "N/A");
                    }
                    swappedInProcesses++;
                } 
                else {
                    printf("%8d | %7c | %15d | %14s | %s\n",
                           globalTime / 1000, job->processName, job->currentPage, "Yes", "N/A");
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