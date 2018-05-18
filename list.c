/* list.c
** Computer Systems / 30023 / Project 1 / Semester 1 2017
** Author: Kara La'Brooy 	Student No.: 757553
** Contains linked list functionality*/
/* !: Program based on listops.c written by Alistair Moffat*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"
#include "memory.h"

list_t *make_empty_list() {
  list_t *list;
  list = (list_t*)malloc(sizeof(*list));
  assert(list!=NULL);
  list->head = list->foot = NULL;
  return list;
}


rrlist_t *make_empty_rr_list() {
  rrlist_t *list;
  list = (rrlist_t*)malloc(sizeof(*list));
  assert(list!=NULL);
  list->head = list->foot = NULL;
  return list;
}


memlist_t *make_empty_mem_list() {
  memlist_t *list;
  list = (memlist_t*)malloc(sizeof(*list));
  assert(list!=NULL);
  list->head = list->foot = NULL;
  return list;
}


int is_empty_list(list_t *list) {
  assert(list!=NULL);
  return list->head==NULL;
}


void free_list(list_t *list) {
  process_t *curr, *prev;
  assert(list!=NULL);
  curr = list->head;
  while (curr) {
    prev = curr;
    curr = curr->next;
    free(prev);
  }
  free(list);
}

list_t *insert_at_head(list_t *list, int timeCreated, int processID, int memorySize, int jobTime) {
  process_t *new;
  new = (process_t*)malloc(sizeof(*new));
  assert(list!=NULL && new!=NULL);

  new->timeCreated = timeCreated;
  new->processID = processID;
  new->memorySize = memorySize;
  new->jobTime = jobTime;
  new->timeInMemory = 0;
  new->timeOnDisk = 0;

  new->next = list->head;
  list->head = new;
  if(list->foot==NULL) {
    /* this is the first new insertion into the list */
    list->foot = new;
  }
  return list;
}


list_t *insert_at_foot(list_t *list, int timeCreated, int processID, int memorySize, int jobTime) {
  process_t *new;
  new = (process_t*)malloc(sizeof(*new));
  assert(list!=NULL && new!=NULL);

  new->timeCreated = timeCreated;
  new->processID = processID;
  new->memorySize = memorySize;
  new->jobTime = jobTime;
  new->timeInMemory = 0;
  new->timeOnDisk = 0;

  new->next =NULL;
  if (list->foot==NULL) {
    /* this is the first new insertion into the list */
    list->head = list->foot = new;
  } else {
    list->foot->next = new;
    list->foot = new;
  }
  return list;
}


list_t *enqueueDisk(list_t *list, process_t *process) {
    process->next = NULL;
    process->timeOnDisk = 0;
    if (list->foot==NULL) {
      /* this is the first new insertion into the list */
      list->head = list->foot = process;
    }
    else if (list->foot->timeOnDisk > 0 || list->foot->processID <= process->processID) {
      list->foot->next = process;
      list->foot = process;
    }
    //CASE 3: we need to place this item somewhere BEFORE the foot
    else {
        process_t *node = list->head, *prev = NULL;
        //CASE 3.1: one item in list, enqueue'd at same time
        if (node->next == NULL) {
            list->head = process;
            list->foot = node;
            process->next = node;
            node->next = NULL;
            return list;
        }
        //CASE 3.2: multiple items in list, foot enqeue'd at same time as new one
        //Traverse until we find foot, and pointer to foot.
        while (node->timeOnDisk != 0) {
            //Have not found what we're after, keep traversing
            prev = node;
            node = node->next;
        }
        //Node is pointing to the FIRST node in list which arrived on this tick.
        //Prev is pointing to THIS node (the one right before it)
        while (process->processID > node->processID) {
            //our new process has LOWER priority than the 'current' node.
            //Insert it AFTER it!
            prev = node;
            node = node->next;
        }
        //CASE 3.3: New process needs to be placed at HEAD of queue
        if (prev == NULL) {
            process->next = list->head;
            list->head = process;
            return list;
        }
        //CASE: 3.4: New process needs to put somewhere in the middle
        //Found the 'swap to' position. (between prev and node)
        prev->next = process;
        process->next = node;
        //No other info changes (list foot stays the same since we know (from earlier check)
        //that we had to put the new process somewhere 'above' the foot
    }
  return list;
}


process_t *dequeueDisk(list_t *list) {
  process_t *ret;
  if (list->head == NULL) return NULL;
  ret = list->head;
  list->head = list->head->next;
  if (list->head == NULL) {
    list->foot = NULL;
  }
  return ret;
}


rrlist_t *enqueueRR(rrlist_t *list, process_t *process) {
  rr_t *new;
  new = (rr_t*)malloc(sizeof(*new));
  assert(list!=NULL && new!=NULL);

  new->process = process;

  new->next =NULL;
  if (list->foot==NULL) {
    /* this is the first new insertion into the list */
    list->head = list->foot = new;
  } else {
    list->foot->next = new;
    list->foot = new;
  }
  return list;
}


rrlist_t *dequeueRR(rrlist_t *list, int ID) {
  rr_t *node = list->head, *prev = NULL;

  while (node != NULL) {
      if (node->process->processID == ID) {
          // this is the node we want to delete
          if (prev == NULL) {
              // deleting the first node
              list->head = node->next;
              free(node);
              if (list->head == NULL) {
                  // list has become empty
                  list->foot = NULL;
              }
          }
          else if (node->next == NULL) {
              // node was last in queue
              list->foot = prev;
              prev->next = NULL;
              free(node);
          }
          else {
              // node was in the midddle
              prev->next = node->next;
              free(node);
          }
          return list;
      }
      prev = node;
      node = node->next;
  }
  //Error
  printf("ERROR: failed to find item to delete from rrQueue\n");
  exit(-1);
}


process_t *rrPop(rrlist_t *list) {
  rr_t *node;
  if (list->head == NULL) {
    return NULL;
  }
  if (list->head->next == NULL) {
    // one item in the queue - handle
    node = list->head;
    list->head = NULL;
    list->foot = NULL;
  }
  else {
    node = list->head;
    list->head = node->next;
  }
  process_t *p = node->process;
  free(node);
  return p;
}


process_t *get_head(list_t *list) {
  return list->head;
}


list_t *get_tail(list_t *list) {
  process_t *oldhead;
  assert(list!=NULL && list->head!=NULL);
  oldhead = list->head;
  list->head = list->head->next;
  if (list->head==NULL) {
    /* the only list node just got deleted */
    list->foot = NULL;
  }

  return list;
}


void print_process(process_t *process) {
  printf("TCr: %d\tID: %d\tMSize: %d\tJTime: %d\tDiskT: %d\tMemTime: %d\n",
    process->timeCreated, process->processID, process->memorySize, process->jobTime, process->timeOnDisk, process->timeInMemory);
}


void print_memory(memory_t *memory) {
  printf("Type: %d\tStart: %d\tEnd: %d\tLn: %d",
    memory->type, memory->startAddress, memory->endAddress, memory->length);
  if (memory->type == 1) { // A process is mounted, print its ID
    printf("\tProcess ID: %d", memory->process->processID);
  }
  printf("\n");
}
