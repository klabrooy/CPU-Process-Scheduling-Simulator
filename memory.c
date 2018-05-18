/* memory.c
** Computer Systems / 30023 / Project 1 / Semester 1 2017
** Author: Kara La'Brooy 	Student No.: 757553
** Contains linked list functionality specific to the memory
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#include "memory.h"

process_t *removeProcess(memlist_t *list, memory_t *memory) {
  process_t *outgoingProcess;
  outgoingProcess = NULL;

  // REMOVING AT HEAD&FOOT OF MEMORY LIST
  if (memory == list->head && memory == list->foot) {
    memory->type = 0;
    // keep reference to process being removed
    outgoingProcess = memory->process;
    memory->process = NULL;
  }

  // REMOVING AT HEAD OF MEMORY LIST
  else if (memory == list->head) {
    // ONLY CHECK NEXT
    if (memory->next->type == 0) {
      // update attributes of current memory block
      int newEnd = memory->next->endAddress;
      memory->endAddress = newEnd;
      memory->type = 0;
      // keep reference to process being removed
      outgoingProcess = memory->process;
      memory->process = NULL;
      memory->length = newEnd - memory->startAddress;

      // update next pointer to delete next block and achieve contiguous hole
      memory_t *oldnext;
      oldnext = memory->next;

      //CHECK IF NEXT IS FOOT ELSE UPDATES WILL SEGFAULT
      if (memory->next != list->foot) {
        //nextnext's prev is us
        memory->next->next->prev = memory;
        //our next is nextnext
        memory->next = memory->next->next;
      }
      //IF NEXT IS foot, WE ARE NOW THE FOOT
      else {
        memory->next = NULL;
        list->foot = memory;
      }

      free(oldnext);
    }
    else {
        outgoingProcess = memory->process;
        memory->process = NULL;
        memory->type = 0;
    }
  }

  // REMOVING AT FOOT OF MEMORY LIST
  else if (memory == list->foot) {
    // ONLY CHECK PREV
    if (memory->prev->type == 0){
      // update attributes of current memory block
      int newStart = memory->prev->startAddress;
      memory->startAddress = newStart;
      memory->type = 0;
      // keep reference to process being removed
      outgoingProcess = memory->process;
      memory->process = NULL;
      memory->length = memory->endAddress - newStart;

      // update prev pointer to delete previous block and achieve contiguous hole
      memory_t *oldprev;
      oldprev = memory->prev;

      //CHECK IS PREV IS HEAD ELSE SEGFAULT / WE WILL LOSE THE LIST!
      if (memory->prev != list->head) {
        //prevprev's next is us
        memory->prev->prev->next = memory;
        //our prev is prevprev
        memory->prev = memory->prev->prev;
      }
      // PREV IS HEAD WE ARE NOW THE HEAD
      else {
        memory->prev = NULL;
        list->head = memory;
      }

      free(oldprev);
    }
  }


  // CONDITION A: PROCESS PREV / PROCESS NEXT
  else if (memory->prev->type == 1 && memory->next->type == 1) {
    //update attributes of current memory block
    // keep reference to process being removed
    outgoingProcess = memory->process;
    memory->process = NULL;
    memory->type = 0;
  }


  // CONDITION B: PROCESS PREV / FREE NEXT
  else if (memory->prev->type == 1 && memory->next->type == 0) {
    // update attributes of current memory block
    int newEnd = memory->next->endAddress;
    memory->endAddress = newEnd;
    memory->type = 0;
    // keep reference to process being removed
    outgoingProcess = memory->process;
    memory->process = NULL;
    memory->length = newEnd - memory->startAddress;

    // update next pointer to delete next block and achieve contiguous hole
    memory_t *oldnext;
    oldnext = memory->next;

    //CHECK IF NEXT IS FOOT ELSE UPDATES WILL SEGFAULT
    if (memory->next != list->foot) {
    //nextnext's prev is us
    memory->next->next->prev = memory;
    //our next is nextnext
    memory->next = memory->next->next;
    }
    //IF NEXT IS foot, WE ARE NOW THE FOOT
    else {
      memory->next = NULL;
      list->foot = memory;
    }

    free(oldnext);
  }


  // CONDITION C: FREE PREV / PROCESS NEXT
  else if (memory->prev->type == 0 && memory->next->type == 1) {
    // update attributes of current memory block
    int newStart = memory->prev->startAddress;
    memory->startAddress = newStart;
    memory->type = 0;
    // keep reference to process being removed
    outgoingProcess = memory->process;
    memory->process = NULL;
    memory->length = memory->endAddress - newStart;

    // update prev pointer to delete previous block and achieve contiguous hole
    memory_t *oldprev;
    oldprev = memory->prev;

    //CHECK IS PREV IS HEAD ELSE SEGFAULT / WE WILL LOSE THE LIST!
    if (memory->prev != list->head) {
      //prevprev's next is us
      memory->prev->prev->next = memory;
      //our prev is prevprev
      memory->prev = memory->prev->prev;
    }
    // PREV IS HEAD WE ARE NOW THE HEAD
    else {
      memory->prev = NULL;
      list->head = memory;
    }

    free(oldprev);
  }


  // CONDITION D: FREE PREV / FREE NEXT
  else if (memory->prev->type == 0 && memory->next->type == 0) {
    int newStart = memory->prev->startAddress;
    int newEnd = memory->next->endAddress;
    memory->startAddress = newStart;
    memory->endAddress = newEnd;
    memory->type = 0;
    // keep reference to process being removed
    outgoingProcess = memory->process;
    memory->process = NULL;
    memory->length = newEnd - newStart;

    //update prev and next pointer to delete redundant nodes and achieve continguous holes
    memory_t *oldprev;
    oldprev = memory->prev;

    //CHECK IS PREV IS HEAD ELSE SEGFAULT / WE WILL LOSE THE LIST!
    if (memory->prev != list->head) {
      //prevprev's next is us
      memory->prev->prev->next = memory;
      //our prev is prevprev
      memory->prev = memory->prev->prev;
    }
    // PREV IS HEAD WE ARE NOW THE HEAD
    else {
      memory->prev = NULL;
      list->head = memory;
    }

    free(oldprev);

    memory_t *oldnext;
    oldnext = memory->next;

    //CHECK IF NEXT IS FOOT ELSE UPDATES WILL SEGFAULT
    if (memory->next != list->foot) {
    //nextnext's prev is us
    memory->next->next->prev = memory;
    //our next is nextnext
    memory->next = memory->next->next;
    }
    //IF NEXT IS foot, WE ARE NOW THE FOOT
    else {
      memory->next = NULL;
      list->foot = memory;
    }

    free(oldnext);
  }


  //DEBUG
  //printf("OUTGOING PROCESS FROM REMOVEMEMORYFUNC = %d USING CASE = %d\n", outgoingProcess->processID, positioning);

  return outgoingProcess;
}


void mountProcess(memlist_t *list, process_t *process, memory_t *memory) {
  // make a new memory_t node for the excess space of the mount
  if (process->memorySize == memory->length) {
      // just update this segments details, no unusedSpace
      memory->process = process;
      memory->type = 1;
      return;
  }

  memory_t *unusedSpace;
  unusedSpace = (memory_t*)malloc(sizeof(*unusedSpace));

  //CHECK ADD ONE
  unusedSpace->startAddress = memory->startAddress + process->memorySize;
  unusedSpace->endAddress = memory->endAddress;
  unusedSpace->type = 0;
  unusedSpace->length = unusedSpace->endAddress - unusedSpace->startAddress;
  unusedSpace->process = NULL;
  unusedSpace->next = memory->next;
  unusedSpace->prev = memory;

  // UPDATE FOOT?
  if (memory == list->foot) {
    list->foot = unusedSpace;
  }
  else {
      //adding link
      memory->next->prev = unusedSpace;
  }

  // update memory block to hold new process
  memory->next = unusedSpace;
  memory->process = process;
  memory->type = 1;
  memory->endAddress = memory->startAddress + process->memorySize;
  memory->length = process->memorySize;
}


memory_t *firstFit(memlist_t *list, process_t *process) {
  // create a node to traverse the memory list
  memory_t *currBlock;
  currBlock = list->head;

  while(currBlock != NULL) {
    if (currBlock->type == 0 && process->memorySize <= currBlock->length) {
      return currBlock;
    }
    currBlock = currBlock->next;
  }

  //ELSE SWAP
  //The memory is full we will need to swap out the process that has been in memory longest
  return NULL;
}


memory_t *worstFit(memlist_t *list, process_t *process) {
  // create a node to traverse the memory list
  memory_t *currBlock;
  currBlock = list->head;
  memory_t *worstFit = NULL;

  while(currBlock != NULL) {
    // check the block will fit the process
    if (currBlock->type == 0 && process->memorySize <= currBlock->length) {
      if (worstFit == NULL || currBlock->length > worstFit->length) {
        // update worstfit if bigger
        worstFit = currBlock;
      }
    }
    currBlock = currBlock->next;
  }

  return worstFit;
}


memory_t *bestFit(memlist_t *list, process_t *process) {
  // create a node to traverse the memory list
  memory_t *currBlock;
  currBlock = list->head;
  memory_t *bestFit = NULL;

  while(currBlock != NULL) {
    // check the block will fit the process
    if (currBlock->type == 0 && process->memorySize <= currBlock->length) {
      if (bestFit == NULL || currBlock->length < bestFit->length) {
        // update bestfit if better fit
        bestFit = currBlock;
      }
    }
    currBlock = currBlock->next;
  }

  return bestFit;
}


void printMemoryList(memlist_t *list) {
    memory_t *temp;
    temp = list->head;
    //printf("After process %d was removed: \n", outgoingProcess->processID);
    while (temp != NULL) {
        if (temp->type == 0) {
            printf("[%d  FREE  %d]", temp->startAddress, temp->endAddress);
    }
    if (temp->type == 1) {
        printf("[%d  ID:%d  %d]", temp->startAddress, temp->process->processID, temp->endAddress);
    }
    temp = temp->next;
}
printf("\n");
}
