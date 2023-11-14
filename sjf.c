// Author: Nigel Lobo
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// we will create a priority queue to simplify the logic in Shortest Job First   
//implemented with a singly linked list

typedef struct ProcessNode {
    char* name;
    int arrivalTime, burstTime, timeRemaining, complete;        
    struct ProcessNode *next;
} ProcessNode;

typedef struct ProcessQueue {
    ProcessNode* head;
} ProcessQueue;

ProcessNode* newNode(char name[], int arr, int bur) {
    ProcessNode *newProc = (ProcessNode*)malloc(sizeof(ProcessNode));
    
    for (int i = 0 ; i < strlen(name); i++) name[i] = toupper(name[i]); //capitalize name

    newProc->name = name;
    newProc->arrivalTime = arr;
    newProc->burstTime = bur;
    newProc->complete = 0; //false, by default
    newProc->timeRemaining = bur;
    newProc->next = NULL;
    return newProc;
}

void push(ProcessQueue* pq, char name[], int arr, int bur) {
    ProcessNode* temp = newNode(name, arr, bur);
    // if queue is empty or new job is a shorter job than the shortest job 
    if (pq->head == NULL || bur < pq->head->burstTime) {
        temp->next = pq->head;
        pq->head = temp;
    } 
    else { // we must find where to place the job
        ProcessNode* current = pq->head;
        // traverse jobs until we hit end of list or job length is shorter than the next job
        while (current->next != NULL && bur >= current->next->burstTime) {
            current = current->next;
        }
        temp->next = current->next;
        current->next = temp;
    }
}

ProcessNode* pop(ProcessQueue* pq) {
    // check if pop can be done 
    if (pq->head == NULL) return NULL;
    // replace head with next in list
    ProcessNode* temp = pq->head;
    pq->head = temp->next;
    return temp;
}

// define custom sort function to allow sorting of ProcessNode structs
int compareByArrivalTime(const void *a, const void *b) {
    const struct ProcessNode *pA = (const struct ProcessNode *)a;
    const struct ProcessNode *pB = (const struct ProcessNode *)b;
    return pA->arrivalTime - pB->arrivalTime;
}

void runSJF(char testCase[]) {
    const char SPACE_CHAR[2] = " ";
    int cpuTimer = 0, numProcesses = 0, numCompleted = 0, turnaroundTimeTotal = 0, waitingTimeTotal = 0;
    char *token;
    ProcessNode procArr[100];

    // initialize priority queue
    ProcessQueue* jobs = (ProcessQueue*)malloc(sizeof(ProcessQueue));
    jobs->head = NULL;

    // parse test case string to an array of ProcessNode structs
    token = strtok(testCase, SPACE_CHAR); //get name for first process in test case   
    while(token != NULL) {
        ProcessNode* p;
        char* nameToken = token;

        // get arrival time
        token = strtok(NULL, SPACE_CHAR);
        int arrToken = atoi(token);

        // get burst time
        token = strtok(NULL, SPACE_CHAR);
        int burToken = atoi(token);

        // add to list
        p = newNode(nameToken, arrToken, burToken);
        procArr[numProcesses] = *p;
        numProcesses++;

        token = strtok(NULL, SPACE_CHAR); //get name for next process if it exists    
    }
    
    printf("Number of Processes: %d\nProcess Scheduling Started:", numProcesses);

    // sort list by arrival time using our custom sort function
    qsort(procArr, numProcesses, sizeof(struct ProcessNode), compareByArrivalTime); 

    ProcessNode *activeProcess;
    int firstProcessArrived = 0;

    // start executing SJF algo
    while (1) {
        printf("\nCPU Time %d: ", cpuTimer);

        // check for arrivals
        for (int i = 0; i < numProcesses; i++) {
            if (cpuTimer == procArr[i].arrivalTime) {
                push(jobs, procArr[i].name, procArr[i].arrivalTime, procArr[i].burstTime);
                printf("[%s Arrived] ", procArr[i].name);
                if (!firstProcessArrived) {
                    activeProcess = pop(jobs);
                    firstProcessArrived = 1;
                }
            }
        }

        // compute time deductions
        if (firstProcessArrived) {
            // active job is complete, get next
            if (activeProcess->complete == 1) {
                activeProcess = pop(jobs);
            }
            // pretty print execution status
            printf("%s [%d/%d]", activeProcess->name, activeProcess->burstTime - activeProcess->timeRemaining, activeProcess->burstTime);
            activeProcess->timeRemaining -= 1;
        }

        // check for completions. If we complete a job, move onto next task in same CPU time cycle
        if (firstProcessArrived && activeProcess->timeRemaining == -1 && activeProcess->complete == 0) {
            // calculate turnaround time and waiting time
            int turnaroundTime = cpuTimer - activeProcess->arrivalTime;
            int waitingTime = turnaroundTime - activeProcess->burstTime;
            printf("\nProcess %s completed with Turnaround Time: %d, Waiting Time: %d", activeProcess->name, turnaroundTime, waitingTime);
            turnaroundTimeTotal += turnaroundTime;
            waitingTimeTotal += waitingTime;

            //mark job as complete
            activeProcess->complete = 1; 
            numCompleted += 1;

            // get next job to run
            activeProcess = pop(jobs);

            //time deduction for next process, in same CPU Time cycle
            if (numCompleted != numProcesses) {
                printf("\nCPU Time %d: ", cpuTimer);
                printf("%s [%d/%d]", activeProcess->name, activeProcess->burstTime - activeProcess->timeRemaining, activeProcess->burstTime);
                activeProcess->timeRemaining -= 1;
            }
        }

        if (!firstProcessArrived) printf("None");
        if (numCompleted == numProcesses) break;
        cpuTimer++;
    }
    double avgTurnaround = (double)turnaroundTimeTotal / (double)numProcesses;   
    double avgWaiting =  (double)waitingTimeTotal / (double)numProcesses;        
    printf("\nProcess scheduling completed with Avg Turnaround Time: %0.2f, Avg Waiting Time:%0.2f\n", avgTurnaround, avgWaiting);
    free(activeProcess);
    free(jobs);
}

int main() {
    FILE *inputFile;
    char lineBuf[300];
    int testCaseCount = 0;

    // try opening file
    inputFile = fopen("sjf_input.txt", "r");
    if (inputFile == NULL) {
        perror("sjf_input.txt could not be opened.");
        return 1;
    }

    // iterate line by line, and run Shortest Job First on each test case
    while (fgets(lineBuf, sizeof(lineBuf), inputFile)) {
        testCaseCount++;
        printf("Test case #%d: %s", testCaseCount, lineBuf);
        runSJF(lineBuf);
        printf("\n");
    }

    fclose(inputFile);
}