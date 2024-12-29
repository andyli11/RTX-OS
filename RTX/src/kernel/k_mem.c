/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                     Copyright 2020-2021 Yiqing Huang
 *                          All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice and the following disclaimer.
 *
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 ****************************************************************************
 */

/**************************************************************************//**
 * @file        k_mem.c
 * @brief       Kernel Memory Management API C Code
 *
 * @version     V1.2021.01.lab2
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @note        skeleton code
 *
 *****************************************************************************/

/** 
 * @brief:  k_mem.c kernel API implementations, this is only a skeleton.
 * @author: Yiqing Huang
 */

//#define DEBUG_0

#include "Serial.h"

#ifdef DEBUG_0
#include "printf.h"
#endif  /* DEBUG_0 */

#include "k_mem.h"
#include "../lib/queue.h"

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */

// kernel stack size, referred by startup_a9.s
const U32 g_k_stack_size = K_STACK_SIZE;
// task proc space stack size in bytes, referred by system_a9.cs
const U32 g_p_stack_size = U_STACK_SIZE;

// Task kernel stacks
U32 g_k_stacks[MAX_TASKS][K_STACK_SIZE >> 2] __attribute__((aligned(8)));

node_t * HEAD_SENTINEL = NULL;
node_t * TAIL_SENTINEL = NULL;

extern PRIO_Q queue;
extern int is_allocating_stack;

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

U32* k_alloc_k_stack(task_t tid)
{
    return g_k_stacks[tid+1];
}

U32* k_alloc_p_stack(task_t tid)
{
#ifdef DEBUG_0
	printf("---- This is the memory before user stack alloc ----\n");
	print_mem();
#endif /* DEBUG_0 */

	// return the address (low) after dynamic memory allocation for user stack
	U16 stack_size = g_tcbs[tid].stack_size;

	// call k_mem_alloc()
    is_allocating_stack = 1;
    void* stack_block = (void*)((int)k_mem_alloc(stack_size) + stack_size);
	is_allocating_stack = 0;
    if (stack_block == NULL)
	{
		return NULL;
	}

#ifdef DEBUG_0
	printf("---- This is the memory after alloc ----\n");
    print_mem();
#endif /* DEBUG_0 */
    return stack_block;
}

int k_mem_init(void) {
    unsigned int end_addr = (unsigned int) &Image$$ZI_DATA$$ZI$$Limit;
    end_addr = ALIGN(end_addr);
#ifdef DEBUG_0
    printf("k_mem_init: image ends at 0x%x\r\n", end_addr);
    printf("k_mem_init: RAM ends at 0x%x\r\n", RAM_END);
#endif /* DEBUG_0 */

    // If kernel takes up the whole memory, is zero size, or RAM_END is 0x0 => return error
    if (end_addr == RAM_END || 0x0 == RAM_END || 0x0 == end_addr)
    {
        return RTX_ERR;
    }

    /* STATE OF INITIALIZED MEMORY:
*
*   LOW LEVEL VIEW:
*
*   RAM_START
*   |  (?)  | kernel code
*    end_addr
*   |  (4)  | <== HEAD_SENTINEL->size (int)
*   |  (4)  | <== HEAD_SENTINEL->next (node_t*)
*   |  (4)  | <== first_node->size (int)
*   |  (4)  | <== first_node->next (node_t*)
*   |  (?)  | rest of memory (1073741823 bytes - tail sentinel - kernel end addr)
*   |  (4)  | <== TAIL_SENTINEL->size (int)
*   |  (4)  | <== TAIL_SENTINEL->next (node_t*)
*    RAM_END
*
*   HIGH LEVEL VIEW:
*
*   RAM_START 
*   |  (?)  | kernel
*    end_addr
*	|  (8)  | <== HEAD SENTINEL
*   |  (8)  | <== first_node
*   |  (?)   | <== rest of memory (1073741823 bytes - tail sentinel - kernel end addr)
*	|  (8)  | <== TAIL SENTINEL
*    RAM_END
*	
*/
	// Initialize head and tail sentinel
	HEAD_SENTINEL = (node_t*)end_addr;
	TAIL_SENTINEL = (node_t*)(RAM_END+1-8);
    // First block in free list (12 bytes)
    node_t* first_node = (node_t*)(end_addr+8);
    first_node->size = (int)TAIL_SENTINEL-(int)first_node;
    first_node->next = TAIL_SENTINEL;
	//Configure head and tail sentinel
    HEAD_SENTINEL->size = 0;
	HEAD_SENTINEL->next = first_node;
	TAIL_SENTINEL->size = 0;
	TAIL_SENTINEL->next = NULL;

	// Initialize the ready queue struct
	pq_init(&queue);

#ifdef DEBUG_0
	printf("k_mem_init: Initialized state of memory: \n");
    printf("head:0x%x\r\n", HEAD_SENTINEL);
	printf("head size:0x%x\r\n", HEAD_SENTINEL->size);
	printf("head next:0x%x\r\n", HEAD_SENTINEL->next);
	printf("\n");
	printf("first_node:0x%x\r\n", first_node);
	printf("first_node size:0x%x\r\n", first_node->size);
	printf("first_node next:0x%x\r\n", first_node->next);
	printf("\n");
	printf("tail:0x%x\r\n", TAIL_SENTINEL);
	printf("tail size:0x%x\r\n", TAIL_SENTINEL->size);
	printf("tail next:0x%x\r\n", TAIL_SENTINEL->next);
	printf("\n");
#endif /* DEBUG_0 */

    return RTX_OK;
}

void* k_mem_alloc(size_t size) {
#ifdef DEBUG_0
    printf("k_mem_alloc: requested memory size = %d\r\n", size);
#endif /* DEBUG_0 */

    if (size == 0 || HEAD_SENTINEL->next == TAIL_SENTINEL) return NULL;

	//ensure size is aligned and is a min value of 8
	size = (size > 8) ? ALIGN(size) : 8;
	//add header to requested size
	size+=8;

#ifdef DEBUG_0
	printf("full requested size: %d\n", size);
	printf("\n");
#endif /* DEBUG_0 */

    // iterate through all nodes
    node_t *current_node = HEAD_SENTINEL->next;
	node_t *previous_node = HEAD_SENTINEL;
    while (current_node != TAIL_SENTINEL)
    {
		//1) extra space
		if(current_node->size > size) {
			// ----- create free block -----
			node_t* new_node = (node_t*)((int)current_node + size);
			new_node->size = current_node->size - size;
			//set the next pointer to of the new node
            new_node->next = current_node->next;
            //update the next pointers of the previous node
            previous_node->next = new_node;
#ifdef DEBUG_0
	printf("new:0x%x\r\n", new_node);
	printf("new size:0x%x\r\n", new_node->size);
	printf("new next:0x%x\r\n", new_node->next);
	printf("\n");
#endif /* DEBUG_0 */
			break;
		}
		//2) exact fit
		else if(current_node->size == size) {
			//remove from linked list
			previous_node->next = current_node->next;
			break;
		}
		//3 not fit
		else {
			previous_node = current_node;
			current_node = current_node->next;
		}
    }

    // exit if suitable free block not found
    if(current_node==TAIL_SENTINEL) return NULL;

	// ----- create allocated block -----
	//change is_allocated bit to 1
	current_node->size = size | 0x80000000;
	int *tid = (int*)((int)current_node+4);
	*tid = is_allocating_stack ? MAGICNUMBER : gp_current_task->tid;

#ifdef DEBUG_0
	printf("TID from alloc address: 0x%x\n", tid);
	printf("TID from alloc value: %d\n", *tid);
    printf("head:0x%x\r\n", HEAD_SENTINEL);
	printf("head size:0x%x\r\n", HEAD_SENTINEL->size);
	printf("head next:0x%x\r\n", HEAD_SENTINEL->next);
	printf("\n");
	printf("current:0x%x\r\n", current_node);
	printf("current size:0x%x\r\n", current_node->size);
	printf("current next:0x%x\r\n", current_node->next);
	printf("\n");
	printf("tail:0x%x\r\n", TAIL_SENTINEL);
	printf("tail size:0x%x\r\n", TAIL_SENTINEL->size);
	printf("tail next:0x%x\r\n", TAIL_SENTINEL->next);
	printf("\n");
#endif /* DEBUG_0 */

    return (void *)((int)current_node+8);
}

int k_mem_dealloc(void *ptr) {
#ifdef DEBUG_0
    printf("k_mem_dealloc: freeing 0x%x\r\n", (U32) ptr);
#endif /* DEBUG_0 */

	// & 0x7FFFFFFF = sets LMB to 0 unconditionally, for getting size without fat amount
	// & 0x80000000 = checking if allocated or not

	// 1) return error if block is already free
	// block location
	int *alloc_block = (int*)((int)ptr-8);
	// return error if block size has a 0 LMB indicating already deallocated
	if(!(*alloc_block & 0x80000000)) return RTX_ERR;
	// 2) check if valid ptr
	if(ptr!=NULL) {
		// ----- ENTER COALESCE -----
		// check dealloc ownership
		int* tid = (int*)((int)alloc_block+4);
		if(is_allocating_stack && *tid!=MAGICNUMBER) return RTX_ERR;
		if(!is_allocating_stack && *tid!=gp_current_task->tid) return RTX_ERR;

		// remove alloc bit from size
		*alloc_block &= 0x7FFFFFFF;
		// iterate through all nodes to get adjacent free blocks
		node_t *right_node = HEAD_SENTINEL->next;
		node_t *left_node = HEAD_SENTINEL;
		while ((int)right_node < (int)alloc_block+*alloc_block) {
			left_node = right_node;
			right_node = right_node->next;
		}

#ifdef DEBUG_0
    printf("left:0x%x\r\n", left_node);
	printf("left size:0x%x\r\n", left_node->size);
	printf("left next:0x%x\r\n", left_node->next);
	printf("\n");
	printf("right:0x%x\r\n", right_node);
	printf("right size:0x%x\r\n", right_node->size);
	printf("right next:0x%x\r\n", right_node->next);
	printf("\n");
#endif /* DEBUG_0 */

		const int PREV_TOUCHING = ((int)left_node+left_node->size==(int)alloc_block) && (left_node!=HEAD_SENTINEL);
		const int NEXT_TOUCHING = ((int)alloc_block+*alloc_block==(int)right_node) && (right_node!=TAIL_SENTINEL);
		//----- 4 cases -----
		//1) between node and node
		//2) between node and allocated
		//3) between allocated and node
		//4) between allocated and allocated
		if(PREV_TOUCHING && NEXT_TOUCHING) {
			left_node->size+=(*alloc_block+right_node->size);
			left_node->next=right_node->next;
		} 
		else if (PREV_TOUCHING && !NEXT_TOUCHING) {
			left_node->size+=*alloc_block;
		}
		else if (!PREV_TOUCHING && NEXT_TOUCHING) {
			node_t* new_node = (node_t*)alloc_block;
			new_node->size=*alloc_block+right_node->size;
			new_node->next = right_node->next;
			left_node->next = new_node;
		} 
		else {
			node_t* new_node = (node_t*)alloc_block;
            new_node->next = right_node;
            left_node->next = new_node;
		}
	}

	return RTX_OK;
}

int k_mem_count_extfrag(size_t size) {
 #ifdef DEBUG_0
     printf("k_mem_extfrag: size = %d\r\n", size);
 #endif /* DEBUG_0 */

	if(size<=0) return 0;
	int count = 0;
	// iterate through all nodes
    node_t *current_node = HEAD_SENTINEL->next;
 	while (current_node != TAIL_SENTINEL) {
 		if (current_node->size < size) count++;
 		current_node = current_node->next;
 	}

#ifdef DEBUG_0
	printf("count: %d\n", count);
#endif /* DEBUG_0 */

 	return count;
 }

void print_mem()
{
	int allocated_count = 0;
	int unallocated_count = 0;
	void *current_pointer = (int*)HEAD_SENTINEL;
	int realtotalmemsize = RAM_END - (int)current_pointer + 1;
	int totalmemsize = 0;
	printf("\nNow printing memory:\n\n");
	printf(	".-----------. head: 0x%08X = %d\n"
			"|           | \n"
			"| HEAD_SEN  | size: 8\n"
			"|           | \n"
			"'-----------' \n",
			(int)current_pointer, (int)current_pointer);

	current_pointer = (void*)((unsigned int)current_pointer + 8);
	totalmemsize += 8;

	while((int)current_pointer < RAM_END - 15)
	{
		int blocksize = (*(int*)current_pointer) & 0x7FFFFFFF;
		// Determine if Allocated or Unallocated
		if ((*(int*)current_pointer & 0x80000000) == 0)
		{
			// Unallocated
			node_t *node = (node_t *)current_pointer;

			printf(	".-----------. head: 0x%08X = 0x%08X\n"
					"|           | \n"
					"|Unallocated| next: 0x%08X\n"
					"|           | \n"
					"|           | size: %d\n"
					"'-----------' \n",
					(int)node, *(int *)(node),
					(int)node->next,
					node->size);
			unallocated_count++;
		}
		else
		{
			// Allocated
			printf(	"############# head: 0x%08X = 0x%08X\n"
					"#           #\n"
					"# Allocated # size: %d\n"
					"#           #\n"
					"#           # tid:  %d\n"
					"#############\n",
					(int)current_pointer, (*(int*)current_pointer),
					((*(int*)current_pointer) & 0x7FFFFFFF),
					((node_t*)current_pointer)->next);
			allocated_count++;
		}
		current_pointer = (void*)((int)current_pointer + blocksize);
		totalmemsize += blocksize;
		// printf("updated current_pointer value: %d\n", (int)current_pointer);
	}

	printf(	".-----------. head: 0x%08X = %d\n"
			"|           | \n"
			"| TAIL_SEN  | size: 8\n"
			"|           | \n"
			"'-----------' \n",
			(int)current_pointer, (int)current_pointer);

	current_pointer = (void*)((unsigned int)current_pointer + 8);
	totalmemsize += 8;

	// printf("RAM_END: %d\n", RAM_END);
	printf("\n----------------------------------------\n");
	printf("Real total mem size: %d\n", realtotalmemsize);
	printf("Total mem size:      %d\n", totalmemsize);
	printf("Allocated blocks:    %d\n", allocated_count);
	printf("Unallocated blocks:  %d\n", unallocated_count);
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
