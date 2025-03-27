#include <signal.h>  // this is necessary for SIGTERM used by kill
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#define NUM_PROCESSES 4
#define NUM_FRAMES 8
#define PAGE_SIZE 4
#define PAGE_TABLE_SIZE 8
#define QUANTUM 8
#define debug 0

struct memoryData {
    char data[PAGE_SIZE];
};

struct memoryMetaData {
    struct memoryMetaData* next;
    int frameNumber;
    bool replacement;
    struct timeval tv;
    int count;
};

/* Preamble:

Functions to implement:
short lruPageReplacement(struct processControlBlock * process);
short altPageReplacement(struct processControlBlock * process);
void sjfCPUScheduler(struct processControlBlock proc[]);
void roundRobinCPUScheduler(struct processControlBlock proc[], int quantum);

Function to change:
int calculatePhysical(struct processControlBlock * process);

If needed to debug, switch which function runs inside of:
short selectVictim(struct processControlBlock * process);
void cpuScheduler(struct processControlBlock processes[]);

*/

struct processControlBlock {
    int procID;
    short state;
    short programCounter;
    short cpuBurst;
    short arrivalTime;
    short pageTable[PAGE_TABLE_SIZE];
    bool pageTableValid[PAGE_TABLE_SIZE];
    struct memoryMetaData* myFrames;
};

struct memoryData memory[NUM_FRAMES];
struct processControlBlock processes[NUM_PROCESSES];
struct memoryMetaData* freeFrames;

/*CODE HERE*/
short lruPageReplacement(struct processControlBlock* process) {
    /* Populate this function to show page replacement.
    Choose two different algorithms to implement.
    The one implemented here should approximate LRU. */
    short frame;
    struct memoryMetaData* temp = process->myFrames;
    struct memoryMetaData* victim = temp;

    while (temp != NULL) {
        // traverse the entire linked list to find the earliest timestamp
        if (timercmp(&temp->tv, &victim->tv, <)) {
            // replace the victim with the least recently used page
            victim = temp;
        }
        temp = temp->next;
    }
    frame = victim->frameNumber;

    return frame; /*whichever frame was accessed is longest ago */
}

/*CODE HERE*/
short altPageReplacement(struct processControlBlock* process) {
    /* Populate this function to show page replacement.
    Choose two different algorithms to implement.
    The one implemented here should use fifo, a counting algorithm, or
    a different LRU algorithm, but not optimal.  */
    short frame;
    // FIFO
    frame = process->myFrames->frameNumber;

    return frame; /*whichever frame was accessed is longest ago */
}

short selectVictim(struct processControlBlock* process) {
    // return lruPageReplacement(process);
    return altPageReplacement(process);
}

void loadPage(int pid, short page, short frame) {
    if (debug) printf("Got to loadPage\n");
    /* get the file for process id, #bytes starting from page * PAGE_SIZE, push
     * into memory frame */
    char filename[8];
    sprintf(filename, "%ld.txt", pid);
    // printf("%s\n", filename);
    FILE* procFile;
    procFile = fopen(filename, "rb");
    fseek(procFile, page * sizeof(struct memoryData), SEEK_SET);
    fread(&memory[frame], sizeof(struct memoryData), 1, procFile);
    fclose(procFile);
    printf("Process %d loaded frame %d with data %c %c %c %c \n", pid, frame,
           memory[frame].data[0], memory[frame].data[1], memory[frame].data[2],
           memory[frame].data[3]);
}

int numFrames(struct processControlBlock* process) {
    int frames = 1;
    struct memoryMetaData* temp;
    temp = process->myFrames;
    while (temp->next != NULL) {
        frames = frames + 1;
        temp = temp->next;
    }
    return frames;
}

void resetPageUsageStats(struct processControlBlock* process, short frame) {
    if (debug) printf("Got to resetPageUsageStats\n");
    /* update page usage stats here */
    struct memoryMetaData* temp = process->myFrames;
    while (temp->next != NULL) {
        if (temp->frameNumber == frame) {
            break;
        }
        temp = temp->next;
    }

    if (temp->frameNumber == frame) {
        gettimeofday(&(temp->tv), NULL);
        temp->replacement = 0;
        temp->count = 0;
    }
}

void pageReplacement(struct processControlBlock* process, short page) {
    if (debug) printf("Got to pageReplacement\n");
    struct memoryMetaData* temp;
    short frameN;
    if (process->myFrames == NULL) {
        process->myFrames = freeFrames;
        freeFrames = freeFrames->next;
        process->myFrames->next = NULL;
        process->pageTable[page] = process->myFrames->frameNumber;
        process->pageTableValid[page] = 1;
        if (debug)
            printf("PageTable %d %d\n", process->pageTable[page],
                   process->pageTableValid[page]);
        temp = process->myFrames;
        frameN = temp->frameNumber;
    } else {
        /* How many frames do I have */
        int frames = numFrames(process);
        temp = process->myFrames;
        while (temp->next != NULL) {
            temp = temp->next;
        }

        if (frames < NUM_FRAMES / NUM_PROCESSES) {
            temp->next = freeFrames;
            freeFrames = freeFrames->next;
            temp = temp->next;
            process->pageTable[page] = temp->frameNumber;
            process->pageTableValid[page] = 1;
            temp->next = NULL;
            frameN = temp->frameNumber;
        } else {
            short frameVic = selectVictim(process);
            short pageVic = -1;
            int i = 0;
            for (int i = 0; i < PAGE_TABLE_SIZE; i++) {
                if (process->pageTable[i] == frameVic) {
                    pageVic = i;
                    break;
                }
            }

            frameN = frameVic;
            temp = process->myFrames;
            while (temp->next != NULL) {
                if (temp->frameNumber == frameN) {
                    break;
                }
                temp = temp->next;
            }
            process->pageTable[page] = frameN;
            process->pageTableValid[page] = 1;
            process->pageTableValid[pageVic] = 0;
            printf("In Process %d, page %d replaced page %d\n", process->procID,
                   page, pageVic);
        }
    }
    /* load page into frame */
    loadPage(process->procID, page, frameN);
    resetPageUsageStats(process, frameN);
    if (debug)
        printf("%c %c %c %c \n", memory[frameN].data[0], memory[frameN].data[1],
               memory[frameN].data[2], memory[frameN].data[3]);
}

void updatePageUsageStats(struct processControlBlock* process, short frame) {
    if (debug) printf("Got to updatePageUsageStats\n");
    /* update page usage stats here */
    struct memoryMetaData* temp = process->myFrames;
    while (temp->next != NULL) {
        if (temp->frameNumber == frame) {
            break;
        }
        temp = temp->next;
    }

    if (temp->frameNumber == frame) {
        gettimeofday(&(temp->tv), NULL);
        temp->replacement = 1;
        temp->count++;
    }
}

void deallocateFramesFromProcess(struct processControlBlock* process) {
    if (freeFrames == NULL) {
        freeFrames = process->myFrames;
        process->myFrames = NULL;
    } else {
        struct memoryMetaData* temp = freeFrames;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = process->myFrames;
        process->myFrames = NULL;
    }
}

void freeAllFrames() {
    struct memoryMetaData* temp;
    struct memoryMetaData* next;
    while (freeFrames != NULL) {
        temp = freeFrames;
        next = NULL;
        if (temp->next != NULL) {
            next = temp->next;
        }
        free(temp);
        freeFrames = next;
    }
}

/*CODE HERE*/
int calculatePhysical(struct processControlBlock* process) {
    if (debug) printf("Got to calculatePhysical\n");

    /* How do you use code to calculate page and offset? */
    short page = process->programCounter / PAGE_SIZE;
    short offset = process->programCounter % PAGE_SIZE;

    /* Change nothing after this line in this function!! */
    printf("Process %d programCounter %d page %d offset %d \n", process->procID,
           process->programCounter, page, offset);
    printf("Process %d pageTable %d pageTableValid %d \n", process->procID,
           process->pageTable[page], process->pageTableValid[page]);
    if (!process->pageTableValid[page]) {
        pageReplacement(process, page);
    }

    short frame = process->pageTable[page];
    updatePageUsageStats(process, frame);
    printf("Data returned frame %d %d\n", frame, frame * PAGE_SIZE + offset);
    return frame * PAGE_SIZE + offset;
}

short dataFromMemory(int physAddress) {
    short frameN = physAddress / PAGE_SIZE;
    if (debug)
        printf("%c %c %c %c \n", memory[frameN].data[0], memory[frameN].data[1],
               memory[frameN].data[2], memory[frameN].data[3]);
    char d = memory[physAddress / PAGE_SIZE].data[physAddress % PAGE_SIZE];
    printf("Physical Address %d, frame %d, offset %d, data %d\n", physAddress,
           physAddress / PAGE_SIZE, physAddress % PAGE_SIZE,
           (d & 0x000000FF) - 48);
    return (0x000000FF & d) - 48;
}

int simulateBurst(struct processControlBlock* process, int instr) {
    if (debug) printf("Entered simulateBurst\n");
    int i = 0;
    for (i = 0; i < instr; i++) {
        int physAddress = calculatePhysical(process);
        process->programCounter = dataFromMemory(physAddress);
        if (debug) printf("Outcome: %d\n", process->programCounter);
    }
}

int fifoCPUScheduler(struct processControlBlock proc[]) {
    int i;
    for (i = 0; i < NUM_PROCESSES; i++) {
        printf("\nProcess %d runs %d instructions\n", i, proc[i].cpuBurst);
        simulateBurst(&proc[i], proc[i].cpuBurst);
    }
}

/*CODE HERE*/
void sjfCPUScheduler(struct processControlBlock proc[]) {
    /* Populate this function to show shortest job first CPU scheduling.
    You may use the above fifoCPUScheduler code to get a sense of the expected
    format. */

    // sort processes by their CPU burst time length, shortest first
    for (int i = 0; i < NUM_PROCESSES - 1; i++) {
        for (int j = i + 1; j < NUM_PROCESSES; j++) {
            // swap the ith process with a shorter process found after it
            if (proc[i].cpuBurst > proc[j].cpuBurst) {
                struct processControlBlock temp = proc[i];
                proc[i] = proc[j];
                proc[j] = temp;
            }
        }
    }

    // execute processes in SJF order
    for (int i = 0; i < NUM_PROCESSES; i++) {
        printf("\nProcess %d runs %d instructions\n", proc[i].procID,
               proc[i].cpuBurst);
        simulateBurst(&proc[i], proc[i].cpuBurst);
    }
}

/*CODE HERE*/
void roundRobinCPUScheduler(struct processControlBlock proc[], int quantum) {
    /* Populate this function to show round robin CPU scheduling.
    You may use the above fifoCPUScheduler code to get a sense of the expected
    format. */

    // create an array to store the remaining burst time for each process
    int remaining[NUM_PROCESSES];
    for (int i = 0; i < NUM_PROCESSES; i++) {
        remaining[i] = proc[i].cpuBurst;
    }

    // execute processes using round robin scheduling
    bool done = false;
    while (!done) {
        done = true;
        for (int i = 0; i < NUM_PROCESSES; i++) {
            if (remaining[i] > 0) {
                done = false;
                int exec_time;
                if (remaining[i] > quantum) {
                    // remaining burst time > quantum, assign quantum time
                    exec_time = quantum;
                } else {
                    // remaining burst time < quantum, finish remaining time
                    exec_time = remaining[i];
                }

                // execute current process based on assigned time
                printf("\nProcess %d runs %d instructions\n", proc[i].procID,
                       exec_time);
                simulateBurst(&proc[i], exec_time);

                // deduct executed time from remaining time
                remaining[i] -= exec_time;
            }
        }
    }
}

void cpuScheduler(struct processControlBlock processes[]) {
    // fifoCPUScheduler(processes);
    sjfCPUScheduler(processes);
    // roundRobinCPUScheduler(processes, QUANTUM);
}

int main(void) {
    int i;
    // srand(43);

    freeFrames = NULL;
    struct memoryMetaData* newFrame = malloc(sizeof(struct memoryMetaData));
    newFrame->next = NULL;
    newFrame->frameNumber = 0;
    newFrame->replacement = 0;
    freeFrames = newFrame;
    struct memoryMetaData* endPointer = freeFrames;
    for (i = 1; i < NUM_FRAMES; i++) {
        newFrame = malloc(sizeof(struct memoryMetaData));
        newFrame->next = NULL;
        newFrame->frameNumber = i;
        newFrame->replacement = 0;
        endPointer->next = newFrame;
        endPointer = newFrame;
    }
    endPointer = freeFrames;
    for (i = 0; i < NUM_FRAMES; i++) {
        printf("frame number %d exists \n", endPointer->frameNumber);
        endPointer = endPointer->next;
    }

    for (i = 0; i < NUM_PROCESSES; i++) {
        processes[i].procID = i;
        processes[i].state = 0;
        processes[i].programCounter = 0;
        processes[i].cpuBurst = rand() % 16;
        int j = 0;
        for (j = 0; j < PAGE_TABLE_SIZE; j++) {
            processes[i].pageTableValid[j] = 0;
        }
        processes[i].myFrames = NULL;
    }

    cpuScheduler(processes);
    for (i = 0; i < NUM_PROCESSES; i++) {
        deallocateFramesFromProcess(&processes[i]);
    }
    freeAllFrames();
}