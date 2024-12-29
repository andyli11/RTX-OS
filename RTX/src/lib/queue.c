#include "queue.h"

//#define DEBUG_0

#ifdef DEBUG_0
#include "printf.h"
#include <stdlib.h>
#endif

/*///////////////////////////////////////////////////////
/                FUNCTION DEFINITIONS                   /
 *////////////////////////////////////////////////////////

int pq_init(PRIO_Q *pq)
{
	// Check for nullptr
	if (pq == NULL) return RTX_ERR;
	// Init member variables
	SIZE = 0;
	NEXT_DQ = 0;
	if(pq_clear(pq) == RTX_ERR) return RTX_ERR;
	return RTX_OK;
}

int pq_clear(PRIO_Q *pq)
{
	// Check for nullptr
	if (pq == NULL)
	{
		return RTX_ERR;
	}
	// Zero out the elements of the array
	for (int i = 0; i < PRIO_NULL + 1; i++)
	{
		ARR[i] = NULL;
	}
	SIZE = 0;
	NEXT_DQ = 0;
	return RTX_OK;
}

TCB* pq_peek(PRIO_Q *pq)
{
	// Check for nullptr
	if (pq == NULL || SIZE == 0)
	{
		return NULL;
	}
	return ARR[NEXT_DQ];
}

int pq_add(PRIO_Q *pq, TCB *tcb) {
	// Check for nullptr or max tasks
	if (pq == NULL || tcb == NULL || SIZE == MAX_TASKS)
	{
		return RTX_ERR;
	}

	if (SIZE == 0)
	{
		// Case A: Queue initially empty
		// Unconditionally set NEXT_DQ
		NEXT_DQ = tcb->prio;
	}
	else if (SIZE != 0 && ARR[tcb->prio] == NULL)
	{
		// Case B: First task of a given priority
		// Update NEXT_DQ if new task is higher prio than previous highest prio
		if (tcb->prio < NEXT_DQ)
		{
			NEXT_DQ = tcb->prio;
		}
	}
	// Case C: Not first task of a given priority
	// Do not need to update NEXT_DQ

	// In all cases, add the node to the tail of its appropriate FIFO
	pq_add_node(pq, tcb);

	// In all cases, increment SIZE FIFO
	SIZE++;

	return RTX_OK;
}

int pq_add_node(PRIO_Q *pq, TCB *tcb)
{
	if (pq == NULL || tcb == NULL || SIZE == MAX_TASKS)
	{
		return RTX_ERR;
	}
	if (ARR[tcb->prio] == NULL)
	{
		// Insert as first node
		ARR[tcb->prio] = tcb;
		ARR[tcb->prio]->next = NULL;
		ARR[tcb->prio]->prev = NULL;
	}
	else
	{
		// Traverse linked list and insert at back
		TCB *curr_ptr = ARR[tcb->prio];
		while (1)
		{
			if (curr_ptr->next == NULL)
			{
				curr_ptr->next = tcb;
				tcb->prev = curr_ptr;
				tcb->next = NULL;
				break;
			}
			curr_ptr = curr_ptr->next;
		}
	}
	return RTX_OK;
}

TCB *pq_dequeue(PRIO_Q *pq)
{
	// Check for nullptr or empty queue
	if (pq == NULL || SIZE == 0)
	{
		return NULL;
	}

	// Cached TCB
	TCB *dq_tcb = ARR[NEXT_DQ];

	// In both cases, dequeue the cached TCB from FIFO the same way
	ARR[NEXT_DQ] = ARR[NEXT_DQ]->next;

	if (ARR[NEXT_DQ] == NULL)
	{
		// CASE A: Only 1 task of highest priority
		// NEXT_DQ must be updated (Linear search for now)
		for (int i = NEXT_DQ + 1; i < PRIO_NULL + 1; i++)
		{
			if (ARR[i] != NULL)
			{
				NEXT_DQ = i;
				break;
			}
		}
	}
	else
	{
		ARR[NEXT_DQ]->prev = NULL;
	}

	// Case B: More than 1 task of highest priority
	// NEXT_DQ does not need to be updated
	SIZE--;
	// Return the cached tcb
	return dq_tcb;
}

int pq_remove(PRIO_Q *pq, TCB *tcb) {
	// Check for null ptr
	if (pq == NULL || tcb == NULL)
	{
		return RTX_ERR;
	}
	if (tcb->prev == NULL && tcb->next == NULL)
	{
		// Case A: Only task in FIFO
		// NEXT_DQ must be updated (Linear search for now)
		// Update front of FIFO to NULL
		ARR[tcb->prio] = NULL;
		for (int i = NEXT_DQ; i < PRIO_NULL + 1; i++)
		{
			if (ARR[i] != NULL)
			{
				NEXT_DQ = i;
				break;
			}
		}
	} // All other cases do not need to update NEXT_DQ
	else if (tcb->prev == NULL)
	{
		// Case B: Is the head
		// Update front of FIFO
		ARR[tcb->prio] = ARR[tcb->prio]->next;
		// Only update prev pointer for new front of FIFO
		(tcb->next)->prev = NULL;
	}
	else if (tcb->next == NULL)
	{
		// Case C: Is the the tail
		// Only update next pointer
		(tcb->prev)->next = NULL;
	}
	else
	{
		// Case D: Somewhere in the middle
		// Update both prev and next pointer
		(tcb->prev)->next = tcb->next;
		(tcb->next)->prev = tcb->prev;
	}
	SIZE--;
	return RTX_OK;
}

int pq_move(PRIO_Q *pq, TCB *tcb, unsigned int new_val)
{
	// Check for nullptr or max tasks
	if (pq == NULL || tcb == NULL || SIZE == 0 || tcb->state == DORMANT || new_val >= PRIO_NULL)
	{
		return RTX_ERR;
	}
	// Search the FIFO for the tcb
	TCB *curr_ptr = ARR[tcb->prio];
	while (curr_ptr != NULL)
	{
		if (curr_ptr->tid == tcb->tid)
		{
			// Found the correct tcb
			// Remove it from the list
			pq_remove(pq, tcb);
			break;
		}
		curr_ptr = curr_ptr->next;
	}
	// Change the priority and re-add it to the queue
	tcb->prio = new_val;
	tcb->prev = NULL;
	tcb->next = NULL;
	// Need to decrease size by 1 since pq_add will increase it by one again
	int status = pq_add(pq, tcb);
	return RTX_OK;
}

#ifdef DEBUG_0
void pq_print(PRIO_Q *pq)
{
	//
	if (pq == NULL)
	{
		return;
	}
	if (SIZE == 0)
	{
		printf("(empty)\n");
	}
	// Print out the prio queue for all non NULL FIFOs
	printf("Contents of priority queue:\n");
	for (int i = 0; i < PRIO_NULL + 1; i++)
	{
		if (ARR[i] != NULL)
		{
			TCB *curr_ptr = ARR[i];
			printf("Prio %d: ", i);
			while (curr_ptr != NULL)
			{
				printf("%d ", curr_ptr->tid);
				curr_ptr = curr_ptr->next;
			}
			printf("\n");
		}
	}
}
#endif
