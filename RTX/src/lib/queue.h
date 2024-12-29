#include "../kernel/k_inc.h"

//#define DEBUG_0

/*///////////////////////////////////////////////////////
/                        MACROS                         /
 *////////////////////////////////////////////////////////

#define ARR (pq->arr)
#define SIZE (pq->size)
#define NEXT_DQ (pq->next_dq)

/*///////////////////////////////////////////////////////
/                STRUCT DEFINITIONS                     /
 *////////////////////////////////////////////////////////

typedef struct priority_q
{
	unsigned short size;
	unsigned int next_dq;
	TCB *arr[PRIO_NULL + 1];
} PRIO_Q;

/*///////////////////////////////////////////////////////
/                FUNCTION DECLARATIONS                  /
 *////////////////////////////////////////////////////////

/*  @brief initializes the priority queue
 *  @param pointer to location in memory to create priority queue
 *  @return 0 if success, -1 if error
 */
int pq_init(PRIO_Q *pq);

/*  @brief empties the priority queue
 *  @param pointer to priority queue struct
 *  @return 0 if success, -1 if error
 */
int pq_clear(PRIO_Q *pq);

/*  @brief peeks the element that is next to be dequeued
 *  @param pointer to priority queue struct
 *  @return TCB pointer if success, NULL if error
 */
TCB* pq_peek(PRIO_Q *pq);

/*  @brief adds an element to the priority queue
 *  @param pointer to priority queue struct
           pointer to TCB to add to priority queue
 *  @return 0 if success, -1 if error
 */
int pq_add(PRIO_Q *pq, TCB *tcb);

/*  @brief helper function that inserts in tail of linked list
 *  @param pointer to priority queue struct
           pointer to TCB to add to priority queue
 *  @return 0 if success, -1 if error
 */
int pq_add_node(PRIO_Q *pq, TCB *tcb);

/*  @brief removes the highest priority element from the queue
 *  @param pointer to the priority queue struct
 *  @return TCB pointer of the dequeued task, NULL if priority queue is empty
 */
TCB *pq_dequeue(PRIO_Q *pq);

/*  @brief removes a specified task from the queue
 *  @param pointer to the priority queue struct
           tcb to remove from active queue
 *  @return 0 if success, -1 if error
 */
int pq_remove(PRIO_Q *pq, TCB *tcb);

/*  @brief changes the priority of an element
 *  @param pointer to priority queue
           pointer to the TCB to change the prio of
           the new priority value
 *  @return 0 if success, -1 if error
 */
int pq_move(PRIO_Q *pq, TCB *tcb, unsigned int new_val);

/*  @brief helper function to print every FIFO of tasks
 *  @param pointer to the priority queue struct
 *  @return none
 */
#ifdef DEBUG_0
void pq_print(PRIO_Q *pq);
#endif
