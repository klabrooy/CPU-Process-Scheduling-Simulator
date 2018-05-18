/* IO.c
** Computer Systems / 30023 / Project 1 / Semester 1 2017
** Author: Kara La'Brooy 	Student No.: 757553
** Handles IO, drives the program*/
/* ! Adapted from the provided using_getopt.c */

#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <assert.h>
#include		<math.h>
#include		"list.h"
#include		"memory.h"


/* Extern declarations: */

extern  int     optind;
extern  char    *optarg;

list_t* fileToList(char *filename);
memlist_t *initialiseMemory(int memorySize);
memory_t *findMemoryBlockofID(memlist_t *list, int ID);
void roundRobin(list_t *allProcesses, memlist_t *memoryList, int quantum, int memsize, memory_t *(*fitAlgorithm)(memlist_t *, process_t *));
void print_rr_queue(rrlist_t *queue);
void print_rr_node(rr_t *node);

int main(int argc, char** argv)
{
	char input;
	char *filename = "input.txt";	     // default filename
	char *algorithm_name = "first";		 // default algorithm
	int memsize = 1000;                 // default memsize in MB
	int quantum = 7;                    // default quantum in Seconds

	// function pointer for algorithm choice
	memory_t *(*fitAlgorithm)(memlist_t *, process_t *);

	/* a sample command line is provided below:

					./swap -f myinput.txt -a worst -m 2000 -q 5

	   where f option is used to specify the text file (char array)
	         a option is used to specify the name of the algorith (char array)
					 m option is used to specify the size of the memory (int)
					 q option is used to specify the size of the quantum (int)

	*/


	while ((input = getopt(argc, argv, "f:a:m:q:")) != EOF)
	{
		switch ( input )
		{
			case 'f':
				// no valid file checking is done here
				filename = optarg;
 				break;
					// should not get here

			case 'a':
				// checks the algorithm name is valid
				if (strcmp(optarg, "best") == 0) {
					fitAlgorithm = bestFit;
				}
				else if (strcmp(optarg, "worst") == 0) {
					fitAlgorithm = worstFit;
				}
				else if (strcmp(optarg, "first") == 0) {
					fitAlgorithm = firstFit;
				}
				else {
					fprintf(stderr, "Invalid -a argument: %s\n ; Select one of best, first or worst", optarg);
					exit(1);
				}
				break;
					// should not get here

			case 'm':
				// check valid input
				if (atoi(optarg) != 0) {
					memsize = atoi(optarg);
				}
				else {
					fprintf(stderr, "Invalid -m argument: %s\n", optarg);
					exit(1);
				}
				break;

			case 'q':
				// check valid input
				if (atoi(optarg) != 0) {
					quantum = atoi(optarg);
				}
				else {
					fprintf(stderr, "Invalid -q argument: %s\n", optarg);
					exit(1);
				}
				break;

			default:
				// do something
				break;
		}
	}

	list_t *processList;
	processList = fileToList(filename);

	memlist_t *memoryList;
	memoryList = initialiseMemory(memsize);

	roundRobin(processList, memoryList, quantum, memsize, fitAlgorithm);

	return 0;
}


list_t* fileToList(char *filename) {
	list_t *processList;
	processList = make_empty_list();

	FILE *fp;
	int timeCreated, processID, memorySize, jobTime;
	char line[100];
	fp = fopen(filename, "r");

	if (fp == NULL) {
		fprintf(stderr, "File not found: %s\n", filename);
		exit(1);
	}

	// each process is a line
	while (fgets(line, 100, fp) != NULL) {
		// get data for the process
		if (sscanf(line, "%d %d %d %d", &timeCreated, &processID, &memorySize, &jobTime) != 4) {
			printf("Error scanning line: %s", line);
			exit(1);
		}
		insert_at_foot(processList, timeCreated, processID, memorySize, jobTime);
	}

	fclose(fp);

	return processList;
}


void roundRobin(list_t *allProcesses, memlist_t *memoryList, int quantum, int memsize, memory_t *(*fitAlgorithm)(memlist_t *, process_t *)) {
	int t = 0;
	int currQuantum = quantum;
	process_t *processToMount;
	process_t *processToQueue;
	memory_t *memBlockToUse;
	memory_t *memBlockToDelete;
	rr_t *cpuUser;

	//FLAGS
	int swapped = 0;
	int jobExpired = 0;
	int quantumExpired = 0;
	int cpuUserWasSwapped = 0;

	//print_list(allProcesses, 'p');

	memory_t *tempM;
	process_t *tempP;

	int numProcesses = 0;
	int numHoles = 1;
	int length = 0;

	int maxID, maxTime;

	rrlist_t *rrQueue;
	rrQueue = make_empty_rr_list();

	list_t *disk;
	disk = make_empty_list();

	// ONE CLOCK TICK PER CYCLE
	// Continue running until nothing is in memory and nothing is waiting in processQ
	while ( !(rrQueue->head == NULL && get_head(disk) == NULL && get_head(allProcesses) == NULL) ) {
		/*printf("---------TIME START %d\n", t);*/
		jobExpired = 0;
		swapped = 0;
		quantumExpired = 0;
		cpuUserWasSwapped = 0;
		//SIMULATION HAS NOT FINISHED

		// Check arrivals of processes
		while (get_head(allProcesses) != NULL && get_head(allProcesses)->timeCreated == t) {

			process_t *toAdd = dequeueDisk(allProcesses);
			enqueueDisk(disk, toAdd);

		}

		//front of rrQueue is using the CPU
		if (rrQueue->head != NULL) {
			cpuUser = rrQueue->head;
			//
			// Checking for an event to swap out
			if (cpuUser->process->jobTime == 0) {
					//printf("AT TIME %d JOB %d IS FINISHED\n", t, cpuUser->process->processID);
					// THIS PROCESS IS FINISHED! - DISPOSE OF IT
					// Find memory block of cpuUser terminating
					memBlockToUse = findMemoryBlockofID(memoryList, cpuUser->process->processID);
					// Remove process from memory
					processToQueue = removeProcess(memoryList, memBlockToUse);
					//Remove from the rr Queue
					rrPop(rrQueue);
					free(processToQueue);
					//Restore quantum for next process
					currQuantum = quantum;
					jobExpired = 1;
					//DEBUG
					//printf("RR QUEUE AFTER JOBTIME END");
					//print_list(rrQueue, 'r');
			}
			else if (currQuantum == 0) {
				// QUANTUM EXPIRED! UPDATE AND QUEUE SINCE JOBTIME REMAINING

				//Restore quantum for next process
				currQuantum = quantum;
				quantumExpired = 1;

				//DEBUG
				//printf("RR QUEUE AFTER QUANTUM EXPIRE");
				//print_list(rrQueue, 'r');
			}
		}


		tempM=memoryList->head;
		//SWAP
		if ((tempM->type == 0 && tempM->length == memsize) || jobExpired || quantumExpired) {
			// !!! IF THERE ARE TWO PROCESSES ON DISK FOR THE SAME TIME // PICK WHICH HAS BEEN ON FOR THE LONGEST
			processToMount = get_head(disk);
			// Check if the process has arrived
			if (processToMount!=NULL && processToMount->timeCreated <= t) {
				disk = get_tail(disk);		//update disk with next head

				//--------------- NO SPACE ON DISK ---------------//
				while (fitAlgorithm(memoryList, processToMount) == NULL) {
					maxID = 0; //THERE IS AT LEAST ONE PROCESS IN MEMORY IF == NULL
					maxTime = 0; //THESE VALS WILL BE UPDATED
					// SWAP OUT THE PROCESS LONGEST SPENT IN MEMORY
					tempM=memoryList->head;
					while(tempM != NULL) {
						if (tempM->type == 1) {
							if (tempM->process->timeInMemory > maxTime) {
								maxID = tempM->process->processID;
								maxTime = tempM->process->timeInMemory;
							}
						}
						tempM = tempM->next;
					}

					//Need to check if the chosen 'to be swapped' process is the one that just used the cpu!
					if (rrQueue->head != NULL && rrQueue->head->process->processID == maxID) {
						cpuUserWasSwapped = 1;
					}

					//block where it will fit
					memBlockToDelete = findMemoryBlockofID(memoryList, maxID);
					//dequeueRR
					rrQueue = dequeueRR(rrQueue, maxID);

					processToQueue = removeProcess(memoryList, memBlockToDelete);
					disk = enqueueDisk(disk, processToQueue);
					//printf("SWAPPED OUT: %d at time %d\n",processToQueue->processID,t);
				}

				memBlockToUse = fitAlgorithm(memoryList, processToMount);

				mountProcess(memoryList, processToMount, memBlockToUse);
				////////DEBUG
				/*memory_t *temp;
			    temp = memoryList->head;
				printf("After process %d was loaded: \n", processToMount->processID);
			    while (temp != NULL) {
			        if (temp->type == 0) {
			            printf("[%d  FREE  %d]", temp->startAddress, temp->endAddress);
			        }
			        if (temp->type == 1) {
			            printf("[%d  ID:%d  %d]", temp->startAddress, temp->process->processID, temp->endAddress);
			        }
			        temp = temp->next;
			    }
			    printf("\n");*/

				enqueueRR(rrQueue, processToMount);

				//RESET TIME ON DISK / MEMORY
				processToMount->timeInMemory = 0;
				processToMount->timeOnDisk = 0;

				printf("time %d, %d loaded, ", t, processToMount->processID);

				swapped = 1;
			}
		}

		// Schedule new process to use CPU
		if (quantumExpired == 1 && cpuUserWasSwapped == 0) {
			enqueueRR(rrQueue, rrPop(rrQueue));
			cpuUser = rrQueue->head;
		}

		//--------------- COUNT PROCESSES AND HOLES ---------------//


		if (swapped == 1) {
			numProcesses = 0;
			numHoles = 0;
			length = 0;
			tempM=memoryList->head;
			while(tempM != NULL) {
				if (tempM->type == 1) {
					numProcesses++;
					//count total size of used memory
					length += tempM->length;
				}
				else {
					numHoles++;
				}
				tempM = tempM->next;
			}
			printf("numprocesses=%d, numholes=%d, memusage=%d%%\n", numProcesses, numHoles, (int)ceil((double)length/memsize*100));
			//print_rr_queue(rrQueue);

			swapped = 0;
		}

		// Update job times
		cpuUser=rrQueue->head;
		if (cpuUser != NULL) {
			cpuUser->process->jobTime--;
			// update quantum
			currQuantum--;
		}

		// THIS IS TO UPDATE MEMORY WAITING TIMERS
		tempM = memoryList->head;
		while(tempM != NULL) {
			if (tempM->type == 1) {
				tempM->process->timeInMemory++;
			}
			tempM = tempM->next;
		}

		// THIS IS TO UPDATE DISK WAITING TIMERS
		tempP = disk->head;
		while(tempP != NULL) {
			tempP->timeOnDisk++;
			tempP = tempP->next;
		}
		/*printf("---------TIME END %d\n", t);*/
		t++;
	}

	printf("time %d, simulation finished.\n", --t);
}


memlist_t *initialiseMemory(int memorySize) {
	memlist_t *memoryList = make_empty_mem_list();
	memory_t *emptyBlock;

  emptyBlock = (memory_t*)malloc(sizeof(*emptyBlock));
  assert(memoryList!=NULL && emptyBlock!=NULL);

  emptyBlock->type = 0;
  emptyBlock->startAddress = 0; //??????? or 0
  emptyBlock->endAddress = memorySize;
  emptyBlock->length = memorySize;

  emptyBlock->next = NULL;
	emptyBlock->prev = NULL;
    /* this is the first new insertion into the list */
  memoryList->head = emptyBlock;
	memoryList->foot = emptyBlock;

  return memoryList;
}



// CAN BE USED TO CHECK IF A PROCESS IS IN MEMORY
memory_t *findMemoryBlockofID(memlist_t *list, int ID) {
	memory_t *currBlock;
	currBlock = list->head;

	while(currBlock != NULL) {
		// first checks if it is holding a process, if so, will check its ID
    if (currBlock->type == 1 && currBlock->process->processID == ID) {
      return currBlock;
    }
    currBlock = currBlock->next;
  }

	// the process did not exist in memory
	printf("COULD NOT FIND ID TO REMOVE IN MEMORY - findMemoryBlockofID %d\n", ID);
	exit(1);

}

void print_rr_queue(rrlist_t *queue) {
	rr_t *temp = queue->head;
	while (temp != NULL) {
		print_rr_node(temp);
		temp = temp->next;
	}
	printf("\n");
}
void print_rr_node(rr_t *node) {
	printf("%d->", node->process->processID);
}
