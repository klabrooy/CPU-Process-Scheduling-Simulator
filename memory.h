/* memory.h
** Computer Systems / 30023 / Project 1 / Semester 1 2017
** Author: Kara La'Brooy 	Student No.: 757553
** Header file for the linked list functionality specific to the memory
*/

/* Removes a process from memory based on its memory block, and merges blocks
** based on the adjacent holes
*/
process_t *removeProcess(memlist_t *list, memory_t *memory);

/* Mounts a process into a particular memory block (found by fit algorithm of
** choice, slices the memory block to create hole for the unused portion)
*/
void mountProcess(memlist_t *list, process_t *process, memory_t *memory);

/* This fit algorithm scans for the first hole of suitable size
*/
memory_t *firstFit(memlist_t *list, process_t *process);

/* This fit algorithm scans for the largest hole of suitable size
*/
memory_t *worstFit(memlist_t *list, process_t *process);

/* This fit algorithm scans for the snuggest hole of suitable size
*/
memory_t *bestFit(memlist_t *list, process_t *process);

/* Prints the memory list with free/empty and the id of process if mounted
*/
void printMemoryList(memlist_t *memoryList);
