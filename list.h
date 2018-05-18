/* list.h
** Computer Systems / 30023 / Project 1 / Semester 1 2017
** Author: Kara La'Brooy 	Student No.: 757553
** Contains linked list functionality
*/

typedef struct process process_t;
typedef struct list list_t;
typedef struct memory memory_t;
typedef struct rr rr_t;
typedef struct memlist memlist_t;
typedef struct rrlist rrlist_t;

/* Holds a process and its information*/
struct process {
	int timeCreated;
	int processID;
	int memorySize;
	int jobTime;
	int timeInMemory;
	int timeOnDisk;

	//processInfo_t *info;
	process_t *previous;
	process_t *next;
};

/* Holds a process list - disk & all read in processes*/
struct list {
	process_t *head;
	process_t *foot;
};

/* Contains information about a memory segment*/
struct memory {
	int type;			//0 for hole, 1 for process
	int startAddress;
	int endAddress;
	int length;
    process_t *process;	//should be NULL if hole

	memory_t *prev;
	memory_t *next;
};

/* Memory node list - for memory management */
struct memlist {
	memory_t *head;
	memory_t *foot;
};

/* Round Robin list - for scheduling */
struct rr {
	process_t *process;
	rr_t *next;
};

/* Round Robin node list - for rrQueue */
struct rrlist {
	rr_t *head;
	rr_t *foot;
};

/* Creates an empty processlist - to contain process_t nodes
*/
list_t *make_empty_list();

/* Creates an empty roundRobinList - to contain rr_t nodes - for Round Robin Queue
*/
rrlist_t *make_empty_rr_list();

/* Creates an empty memlist - to contain memory_t nodes
*/
memlist_t *make_empty_mem_list();

/* Checks if a list is empty
*/
int is_empty_list(list_t *list);

/* Frees the memory of a list
*/
void free_list(list_t *list);

/* Inserts a process at the head of the list - unused
*/
list_t *insert_at_head(list_t *list, int timeCreated, int processID, int memorySize, int jobTime);

/* Inserts a process at the tail of the list
*/
list_t *insert_at_foot(list_t *list, int timeCreated, int processID, int memorySize, int jobTime);

/* Puts a process from memory onto disk, maintaining priority ordering for
** the disk linked list
*/
list_t *enqueueDisk(list_t *list, process_t *process);

/* Removes a  process_t node from the front of the a list
*/
process_t *dequeueDisk(list_t *list);

/* Used for queueing a process off disk into the Round Robin Queue
**(same time as memory insert)
*/
rrlist_t *enqueueRR(rrlist_t *list, process_t *process);

/* Takes a rr_t node out of the Round Robin Queue based on ID
*/
rrlist_t *dequeueRR(rrlist_t *list, int ID);

/* Pops the head of rrlist_t (queue)
*/
process_t *rrPop(rrlist_t *list);

/* Returns a pointer to the head of a process_t list without modifying the original list
*/
process_t *get_head(list_t *list);

/*  Unlinks the node at the head of the list and steps past it
*/
list_t *get_tail(list_t *list);

/* Prints a process
*/
void print_process(process_t *process);

/* Prints a memory block
*/
void print_memory(memory_t *memory);
