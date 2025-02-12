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
        newJob->processSize = (rand() % 4) * 6 + 5; // Sizes: 5, 11, 17, 31 pages

        // Spread arrival times to reduce overlap
        newJob->arrivalTime = rand() % (SIMULATION_TIME / 2);

        // Increase service duration to keep jobs active longer
        newJob->serviceDuration = (rand() % 20) + 20; // 20-39 seconds

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

void sortJobsByArrivalTime(JobQueue *jobQueue) {
    if (jobQueue->head == NULL || jobQueue->head->next == NULL) {
        return; // No need to sort if the list is empty or has only one job
    }

    Job* sorted = NULL;
    Job* current = jobQueue->head;

    while (current != NULL) {
        Job* next = current->next;

        if (sorted == NULL || sorted->arrivalTime >= current->arrivalTime) {
            current->next = sorted;
            sorted = current;
        } else {
            Job* temp = sorted;
            while (temp->next != NULL && temp->next->arrivalTime < current->arrivalTime) {
                temp = temp->next;
            }
            current->next = temp->next;
            temp->next = current;
        }

        current = next;
    }

    jobQueue->head = sorted;
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
        return oldestIndex;
    }
}

// LFU Implementation
int LFU(Memory *memory, char processName, int pageNumber, int timestamp) {
    // Check if the page is already in memory
    for (int i = 0; i < memory->count; i++) {
        if (memory->pages[i].processName == processName && memory->pages[i].pageNumber == pageNumber) {
            memory->pages[i].frequency++;
            memory->pages[i].lastUsed = timestamp;
            return -1; // Hit
        }
    }

    // Remove invalid pages (processName == '\0')
    int validCount = 0;
    for (int i = 0; i < memory->count; i++) {
        if (memory->pages[i].processName != '\0') {
            memory->pages[validCount] = memory->pages[i];
            validCount++;
        }
    }
    memory->count = validCount;

    // Age frequencies periodically (e.g., every 10 seconds)
    if (timestamp % 10000 == 0) {
        for (int i = 0; i < memory->count; i++) {
            memory->pages[i].frequency = memory->pages[i].frequency / 2; // Decay
        }
    }

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
    memory->pages[lfuIndex].frequency = 1;
    memory->pages[lfuIndex].lastUsed = timestamp;
    return lfuIndex;
}

// LRU Implementation
int LRU(Memory *memory, char processName, int pageNumber, int timestamp) {
    // Check if the page is already in memory
    for (int i = 0; i < memory->count; i++) {
        if (memory->pages[i].processName == processName && memory->pages[i].pageNumber == pageNumber) {
            memory->pages[i].lastUsed = timestamp; // Update lastUsed on hit
            return -1; // Hit
        }
    }

    // If memory is not full, add the new page
    if (memory->count < MAX_PAGES) {
        memory->pages[memory->count].processName = processName;
        memory->pages[memory->count].pageNumber = pageNumber;
        memory->pages[memory->count].lastUsed = timestamp;
        memory->count++;
        return -1; // No eviction
    }

    // Find the least recently used VALID page
    int lruIndex = -1;
    int minLastUsed = INT_MAX;
    for (int i = 0; i < memory->count; i++) {
        if (memory->pages[i].processName != '\0' && memory->pages[i].lastUsed < minLastUsed) {
            minLastUsed = memory->pages[i].lastUsed;
            lruIndex = i;
        }
    }

    // Replace the LRU page
    memory->pages[lruIndex].processName = processName;
    memory->pages[lruIndex].pageNumber = pageNumber;
    memory->pages[lruIndex].lastUsed = timestamp;
    return lruIndex;
}

int MFU(Memory *memory, char processName, int pageNumber, int timestamp) {
    // Check if the page is already in memory
    for (int i = 0; i < memory->count; i++) {
        if (memory->pages[i].processName == processName && memory->pages[i].pageNumber == pageNumber) {
            memory->pages[i].frequency++;
            memory->pages[i].lastUsed = timestamp;
            return -1; // Hit
        }
    }

    // Remove invalid pages (processName == '\0')
    int validCount = 0;
    for (int i = 0; i < memory->count; i++) {
        if (memory->pages[i].processName != '\0') {
            memory->pages[validCount] = memory->pages[i];
            validCount++;
        }
    }
    memory->count = validCount;

    // Age frequencies periodically (e.g., every 10 seconds)
    if (timestamp % 10000 == 0) {
        for (int i = 0; i < memory->count; i++) {
            memory->pages[i].frequency = memory->pages[i].frequency / 2; // Decay
        }
    }

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
        } else if (memory->pages[i].frequency == memory->pages[mfuIndex].frequency) {
            // Tiebreaker: LRU
            if (memory->pages[i].lastUsed < memory->pages[mfuIndex].lastUsed) {
                mfuIndex = i;
            }
        }
    }

    // Replace the page
    memory->pages[mfuIndex].processName = processName;
    memory->pages[mfuIndex].pageNumber = pageNumber;
    memory->pages[mfuIndex].frequency = 1;
    memory->pages[mfuIndex].lastUsed = timestamp;
    return mfuIndex;
}

int RandomPick(Memory *memory, char processName, int pageNumber, int timestamp) {
    // Check if the page is already in memory
    for (int i = 0; i < memory->count; i++) {
        if (memory->pages[i].processName == processName && memory->pages[i].pageNumber == pageNumber) {
            memory->pages[i].frequency++; // Increment frequency
            memory->pages[i].lastUsed = timestamp; // Update last used time
            return -1; // Page is already in memory (hit)
        }
    }

    // If the page is not in memory, randomly select a page for eviction
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
    memory->pages[randomIndex].frequency = 1;
    memory->pages[randomIndex].lastUsed = timestamp;
    return randomIndex; // Return the index of the evicted page
}
