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
 * @file        k_task.c
 * @brief       task management C file
 *              l2
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 * @attention   assumes NO HARDWARE INTERRUPTS
 * @details     The starter code shows one way of implementing context switching.
 *              The code only has minimal sanity check.
 *              There is no stack overflow check.
 *              The implementation assumes only two simple privileged task and
 *              NO HARDWARE INTERRUPTS.
 *              The purpose is to show how context switch could be done
 *              under stated assumptions.
 *              These assumptions are not true in the required RTX Project!!!
 *              Understand the assumptions and the limitations of the code before
 *              using the code piece in your own project!!!
 *
 *****************************************************************************/

//#include "VE_A9_MP.h"
#include "Serial.h"
#include "k_rtx.h"

#include "k_mem.h"
#include "k_task.h"
#include "../lib/queue.h"

//#define DEBUG_0

extern void kcd_task(void);

#ifdef DEBUG_0
#include "printf.h"
#endif /* DEBUG_0 */

extern TCB* gp_current_task;
extern PRIO_Q queue;
extern TCB g_tcbs[];

/*
 *==========================================================================
 *                            GLOBAL VARIABLES
 *==========================================================================
 */

TCB            *gp_current_task = NULL;	    // the current RUNNING task
TCB             g_tcbs[MAX_TASKS];          // all the tcbs
RTX_TASK_INFO   g_null_task_info;			// The null task info
U32             g_num_active_tasks = 0;		// number of non-dormant tasks
PRIO_Q          queue;                      // priority queue
TCB            *array[MAX_TASKS];           // sorted array from priority queue

int is_allocating_stack = 0;

/*
 *===========================================================================
 *                            FUNCTIONS
 *===========================================================================
 */

/**************************************************************************//**
 * @brief   scheduler, pick the TCB of the next to run task
 *
 * @return  TCB pointer of the next to run task
 * @post    gp_curret_task is updated
 *
 *****************************************************************************/

TCB *scheduler(void)
{
	// Dequeue from the priority queue
	TCB* dq_ele = pq_dequeue(&queue);
	if (dq_ele == NULL)
	{
		return NULL;
	}
	return dq_ele;
}

/**************************************************************************//**
 * @brief       initialize all boot-time tasks in the system,
 *
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       task_info   boot-time task information structure pointer
 * @param       num_tasks   boot-time number of tasks
 * @pre         memory has been properly initialized
 * @post        none
 *
 * @see         k_tsk_create_new
 *****************************************************************************/

int k_tsk_init(RTX_TASK_INFO *task_info, int num_tasks)
{
#ifdef DEBUG_0
    printf("entering k_tsk_init\n");
#endif /* DEBUG_0 */

    // Clear out the tcbs memory to be a clean slate
    for (int i = 0 ; i < MAX_TASKS ; i++)
    {
    	g_tcbs[i].next = 0;
    	g_tcbs[i].ksp = 0;
    	g_tcbs[i].psp = 0;
    	g_tcbs[i].stack_size = 0;
    	g_tcbs[i].tid = 0;
    	g_tcbs[i].prio = 0;
    	g_tcbs[i].state = 0;
    	g_tcbs[i].priv = 0;
    	g_tcbs[i].prev = 0;
    	g_tcbs[i].ptask = 0;
    	g_tcbs[i].mailbox = 0;
    }

    extern U32 SVC_RESTORE;

    RTX_TASK_INFO *p_taskinfo = &g_null_task_info;
    g_num_active_tasks = 0;

    if (num_tasks > MAX_TASKS-1) {
    	return RTX_ERR;
    }

    // Create the first task (null task)
    TCB *p_tcb = (TCB*)(&g_tcbs[0]);
    p_tcb->next = NULL;
    p_tcb->ksp = g_k_stacks[1];
    p_tcb->psp = NULL;
    p_tcb->stack_size = 0;
    p_tcb->tid = TID_NULL;
    p_tcb->prio = PRIO_NULL;
    p_tcb->state = RUNNING;
    p_tcb->priv = 1;
    p_tcb->prev = NULL;
    p_tcb->ptask = task_info->ptask;
    p_tcb->mailbox = NULL;
    // Increment number of tasks
    g_num_active_tasks++;

#ifdef DEBUG_0
    printf("-------------------- INCREMENTING G_NUM_ACTIVE TASKS!!!!!! after null to: %d ----------------\n", g_num_active_tasks);
#endif /* DEBUG_0 */

    // Set the current task to the null task
    gp_current_task = p_tcb;

    // Insert null task into priority queue
#ifdef DEBUG_0
    printf ("&queue = %x\n", &queue);
#endif /* DEBUG_0 */
    pq_add(&queue, p_tcb);

    // create the rest of the tasks
    p_taskinfo = task_info;

    int j = 1;
    // i goes from 1 to 159 (max value for num_tasks specified in boot-tasks)
    for ( int i = 1; i <= num_tasks; i++ ) {
#ifdef DEBUG_0
    printf("\n-------------START i = %d----------\n", i);
#endif /* DEBUG_0 */
    	// Check error conditions on task info
//            void                (*ptask)();         /**> task entry address                 */
		if(p_taskinfo->ptask==NULL) return RTX_ERR;
//            U32                 k_stack_hi;         /**> kernel stack base (high addr.)     */
//            U32                 u_stack_hi;         /**> user stack base addr. (high addr.) */
//            U16                 k_stack_size;       /**> kernel stack size in bytes         */
		if(p_taskinfo->k_stack_size > K_STACK_SIZE) return RTX_ERR;
//            U16                 u_stack_size;       /**> user stack size in bytes           */
		if(p_taskinfo->priv==0 && p_taskinfo->u_stack_size < U_STACK_SIZE) return RTX_ERR;
//            task_t              tid;                /**> task ID                            */
//            U8                  prio;               /**> execution priority                 */
		if(p_taskinfo->prio>=PRIO_NULL || p_taskinfo->prio==PRIO_RT) return RTX_ERR;
//            U8                  state;              /**> task state                         */
//            U8                  priv;               /**> = 0 unprivileged, =1 privileged    */
		if(p_taskinfo->priv>1) return RTX_ERR;

		if (p_taskinfo->ptask == &kcd_task) {
			p_tcb = (TCB*)(&g_tcbs[TID_KCD]);
			j--;
		} else {
			p_tcb = (TCB*)(&g_tcbs[j]);
		}

        p_tcb->next = NULL;
        p_tcb->ksp = NULL;
        p_tcb->psp = NULL;
        p_tcb->stack_size = (p_tcb->priv == 1) ? 0 : ALIGN(p_taskinfo->u_stack_size);
        //tid set in k_tsk_create_new
        p_tcb->prio = p_taskinfo->prio;
        //state set in k_tsk_create_new
        p_tcb->priv = p_taskinfo->priv;
        p_tcb->prev = NULL;
        p_tcb->mailbox = NULL;
        //ptask set in k_tsk_create_new

        // create new task
        if (k_tsk_create_new(p_taskinfo, p_tcb, p_taskinfo->ptask == &kcd_task ? TID_KCD : j) == RTX_OK)
        {

#ifdef DEBUG_0
    printf("\n-------------START i = %d----------\n", i);
    printf("adding to queue!\n");
    printf("current: 0x%x\n", p_tcb);
    printf("ksp: 0x%x\n", p_tcb->ksp);
    printf("psp: 0x%x\n", p_tcb->psp);
    printf("next: 0x%x\n", p_tcb->next);
    printf("prev: 0x%x\n", p_tcb->prev);
    printf("tid: %d\n", p_tcb->tid);
    printf("user stack_size: %d\n", p_tcb->stack_size);
    printf("prio: %d\n", p_tcb->prio);
    printf("state: %d\n", p_tcb->state);
    printf("priv: %d\n", p_tcb->priv);
#endif /* DEBUG_0 */

        	pq_add(&queue, p_tcb);
        	g_num_active_tasks++;

#ifdef DEBUG_0
    printf("-------------------- INCREMENTING G_NUM_ACTIVE TASKS!!!!!! to: %d ----------------\n", g_num_active_tasks);
#endif /* DEBUG_0 */
        }
        else
        {
        	return RTX_ERR;
        }
        p_taskinfo++;
        j++;

#ifdef DEBUG_0
    printf("after pq_add: task prio %d nextpr: %d\n",p_tcb->prio, queue.arr[p_tcb->prio]->next);
    printf("-------------END i = %d----------\n\n", i);
#endif /* DEBUG_0 */
    }

#ifdef DEBUG_0
    pq_print(&queue);
#endif /* DEBUG_0 */
    return RTX_OK;
}
/**************************************************************************//**
 * @brief       initialize a new task in the system,
 *              one dummy kernel stack frame, one dummy user stack frame
 *
 * @return      RTX_OK on success; RTX_ERR on failure
 * @param       p_taskinfo  task information structure pointer
 * @param       p_tcb       the tcb the task is assigned to
 * @param       tid         the tid the task is assigned to
 *
 * @details     From bottom of the stack,
 *              we have user initial context (xPSR, PC, SP_USR, uR0-uR12)
 *              then we stack up the kernel initial context (kLR, kR0-kR12)
 *              The PC is the entry point of the user task
 *              The kLR is set to SVC_RESTORE
 *              30 registers in total
 *
 *****************************************************************************/
int k_tsk_create_new(RTX_TASK_INFO *p_taskinfo, TCB *p_tcb, task_t tid)
{
#ifdef DEBUG_0
    printf("entering k_tsk_create_new\n");
#endif /* DEBUG_0 */

   extern U32 SVC_RESTORE;

   U32 *sp;

   if (/*p_taskinfo == NULL ||*/ p_tcb == NULL)
   {
       return RTX_ERR;
   }

   p_tcb ->tid = tid;
   p_tcb->state = READY;
   p_tcb->ptask = p_taskinfo->ptask;

   /*---------------------------------------------------------------
    *  Step1: allocate kernel stack for the task
    *         stacks grows down, stack base is at the high address
    * -------------------------------------------------------------*/

   ///////sp = g_k_stacks[tid] + (K_STACK_SIZE >> 2) ;
   sp = k_alloc_k_stack(tid);

//   printf("tid val = %d, sp value is 0x%x\n", tid, sp);

   // 8B stack alignment adjustment
   if ((U32)sp & 0x04) {   // if sp not 8B aligned, then it must be 4B aligned
       sp--;               // adjust it to 8B aligned
   }

   /*-------------------------------------------------------------------
    *  Step2: create task's user/sys mode initial context on the kernel stack.
    *         fabricate the stack so that the stack looks like that
    *         task executed and entered kernel from the SVC handler
    *         hence had the user/sys mode context saved on the kernel stack.
    *         This fabrication allows the task to return
    *         to SVC_Handler before its execution.
    *
    *         16 registers listed in push order
    *         <xPSR, PC, uSP, uR12, uR11, ...., uR0>
    * -------------------------------------------------------------*/

   // if kernel task runs under SVC mode, then no need to create user context stack frame for SVC handler entering
   // since we never enter from SVC handler in this case
   // uSP: initial user stack
   if ( p_taskinfo->priv == 0 ) { // unprivileged task
       // xPSR: Initial Processor State
       *(--sp) = INIT_CPSR_USER;
       // PC contains the entry point of the user/privileged task
       *(--sp) = (U32) (p_taskinfo->ptask);

       //********************************************************************//
       //*** allocate user stack from the user space ***//
       //********************************************************************//

       *(--sp) = (U32)(k_alloc_p_stack(tid));
//       *(--sp) = (U32)((int)p_stack + p_taskinfo->u_stack_size);

       p_tcb->psp = (unsigned int*)*sp;
//       p_tcb->psp = (unsigned int*)((int)p_stack + p_taskinfo->u_stack_size);

       // uR12, uR11, ..., uR0
       for ( int j = 0; j < 13; j++ ) {
           *(--sp) = 0x0;
       }
   }


   /*---------------------------------------------------------------
    *  Step3: create task kernel initial context on kernel stack
    *
    *         14 registers listed in push order
    *         <kLR, kR0-kR12>
    * -------------------------------------------------------------*/
   if ( p_taskinfo->priv == 0 ) {
       // user thread LR: return to the SVC handler
       *(--sp) = (U32) (&SVC_RESTORE);
   } else {
       // kernel thread LR: return to the entry point of the task
       *(--sp) = (U32) (p_taskinfo->ptask);
   }

   // kernel stack R0 - R12, 13 registers
   for ( int j = 0; j < 13; j++) {
       *(--sp) = 0x0;
   }

   // kernel stack CPSR
   *(--sp) = (U32) INIT_CPSR_SVC;
   p_tcb->ksp = sp;

   return RTX_OK;
}

/**************************************************************************//**
 * @brief       switching kernel stacks of two TCBs
 * @param:      p_tcb_old, the old tcb that was in RUNNING
 * @return:     RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre:        gp_current_task is pointing to a valid TCB
 *              gp_current_task->state = RUNNING
 *              gp_crrent_task != p_tcb_old
 *              p_tcb_old == NULL or p_tcb_old->state updated
 * @note:       caller must ensure the pre-conditions are met before calling.
 *              the function does not check the pre-condition!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *
 *****************************************************************************/
__asm void k_tsk_switch(TCB *p_tcb_old)
{
        PUSH    {R0-R12, LR}
        MRS 	R1, CPSR
        PUSH 	{R1}
        STR     SP, [R0, #TCB_KSP_OFFSET]   ; save SP to p_old_tcb->ksp
        LDR     R1, =__cpp(&gp_current_task);
        LDR     R2, [R1]
        LDR     SP, [R2, #TCB_KSP_OFFSET]   ; restore ksp of the gp_current_task
        POP		{R0}
        MSR		CPSR_cxsf, R0
        POP     {R0-R12, PC}
}


/**************************************************************************//**
 * @brief       run a new thread. The caller becomes READY and
 *              the scheduler picks the next ready to run task.
 * @return      RTX_ERR on error and zero on success
 * @pre         gp_current_task != NULL && gp_current_task == RUNNING
 * @post        gp_current_task gets updated to next to run task
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * @attention   CRITICAL SECTION
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 *****************************************************************************/
int k_tsk_run_new(void)
{
#ifdef DEBUG_0
    printf("\nentering k_tsk_run_new\n");
    printf("confirm task count: %d\n", g_num_active_tasks);
    pq_print(&queue);

    printf("--------- THIS IS CURRENT TASK ----------\n");
    printf("            old: 0x%x\n", gp_current_task);
	printf("            ksp: 0x%x\n", gp_current_task->ksp);
	printf("            psp: 0x%x\n", gp_current_task->psp);
    printf("           next: 0x%x\n", gp_current_task->next);
    printf("           prev: 0x%x\n", gp_current_task->prev);
	printf("            tid: %d\n", gp_current_task->tid);
	printf("user stack_size: 0x%x\n", gp_current_task->stack_size);
	printf("           prio: %d\n", gp_current_task->prio);
	printf("          state: %d\n", gp_current_task->state);
	printf("           priv: %d\n", gp_current_task->priv);
#endif /* DEBUG_0 */

	//TCB to hold current task
    TCB *p_tcb_old = gp_current_task;
    //check if queue contains more than just the null task
    if (queue.next_dq != PRIO_NULL)
    {
    	gp_current_task = scheduler();
    	gp_current_task->state = RUNNING;
    }
    else
    {
        // If the only task left in the queue is the null task, don't remove it, just run it
    	gp_current_task = pq_peek(&queue);
    }
    // This is the case of exiting
    if(p_tcb_old->state==DORMANT || p_tcb_old->state==BLK_MSG) {
    	k_tsk_switch(p_tcb_old);
    	return RTX_OK;
    }

    // This is the case of no exit
    if (gp_current_task != p_tcb_old) {
#ifdef DEBUG_0
    printf("--------- THIS IS NEW TASK ----------\n");
    printf("            new: 0x%x\n", gp_current_task);
    printf("            ksp: 0x%x\n", gp_current_task->ksp);
    printf("            psp: 0x%x\n", gp_current_task->psp);
    printf("           next: 0x%x\n", gp_current_task->next);
    printf("           prev: 0x%x\n", gp_current_task->prev);
    printf("            tid: %d\n", gp_current_task->tid);
    printf("user stack_size: 0x%x\n", gp_current_task->stack_size);
    printf("           prio: %d\n", gp_current_task->prio);
    printf("          state: %d\n", gp_current_task->state);
    printf("           priv: %d\n", gp_current_task->priv);
    printf("\n");
#endif /* DEBUG_0 */

    	p_tcb_old->state = READY;           // change state of the to-be-switched-out tcb
        k_tsk_switch(p_tcb_old);        // switch stacks
    }
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       yield the cpu
 * @return:     RTX_OK upon success
 *              RTX_ERR upon failure
 * @pre:        gp_current_task != NULL &&
 *              gp_current_task->state = RUNNING
 * @post        gp_current_task gets updated to next to run task
 * @note:       caller must ensure the pre-conditions before calling.
 *****************************************************************************/
int k_tsk_yield(void)
{
#ifdef DEBUG_0
    print_mem();
    pq_print(&queue);
#endif /* DEBUG_0 */

	TCB* peek_ele = pq_peek(&queue);
	if (peek_ele->prio <= gp_current_task->prio)
	{
		if (gp_current_task->tid != TID_NULL)
		{
			pq_add(&queue, gp_current_task);
		}
		return k_tsk_run_new();
	}
    return RTX_OK;
}


/*
 *===========================================================================
 *                             TO BE IMPLEMETED IN LAB2
 *===========================================================================
 */

int k_tsk_create(task_t *task, void (*task_entry)(void), U8 prio, U16 stack_size)
{
#ifdef DEBUG_0
    printf("k_tsk_create: entering...\n\r");
    printf("task = 0x%x, task_entry = 0x%x, prio=%d, stack_size = %d\n\r", task, task_entry, prio, stack_size);
#endif /* DEBUG_0 */

    // fail if at max task count, stack size is too small or big, invalid prio, task or task entry is null
    if (g_num_active_tasks==MAX_TASKS || stack_size<U_STACK_SIZE || prio==PRIO_RT || prio>=PRIO_NULL || task_entry==NULL) return RTX_ERR;

    *task = NULL;
    // assign kcd tid to kcd task
    if (task_entry == &kcd_task) *task = TID_KCD;

    // find first available tid
    else
    {
        for (int i = 1; i < MAX_TASKS; i++)
        {
            if (g_tcbs[i].state == DORMANT && i != TID_KCD)
            {
                *task = i;
                break;
            }
        }
    }
    
    // check task (tid) is not NULL
    if (task == NULL) return RTX_ERR;

    // create RTX_TASK_INFO struct and TCB struct
    RTX_TASK_INFO p_taskinfo;
    p_taskinfo.ptask = task_entry;
    p_taskinfo.k_stack_hi = NULL;
    p_taskinfo.u_stack_hi = NULL;
    p_taskinfo.k_stack_size = K_STACK_SIZE;
    p_taskinfo.u_stack_size = ALIGN(stack_size);
    p_taskinfo.tid = *task;
    p_taskinfo.prio = prio;
    p_taskinfo.state = READY;
    p_taskinfo.priv = 0;

    TCB *p_tcb = (TCB*)(&(g_tcbs[*task]));
    p_tcb->prio = prio;
    p_tcb->priv = 0;
    p_tcb->stack_size = ALIGN(stack_size);
    p_tcb->mailbox = NULL;

    int res;

    // pass into k_tsk_create_new to create kernel and user stack
    res = k_tsk_create_new(&p_taskinfo, p_tcb, *task);
    if (res == RTX_ERR) return RTX_ERR;

    res = pq_add(&queue, p_tcb);
    if (res == RTX_ERR) return RTX_ERR;
    g_num_active_tasks++;
#ifdef DEBUG_0
    printf("INCREMENTING G_NUM_ACTIVE TASKS!!!!!! to: %d\n", g_num_active_tasks);
#endif /* DEBUG_0 */
    // Check if scheduler needs to be run now
    if (pq_peek(&queue)->prio < gp_current_task->prio)
    {
    	res = pq_add(&queue, gp_current_task);
    	if (res == RTX_ERR) return RTX_ERR;
        k_tsk_run_new();
    }
    return RTX_OK;
}

void k_tsk_exit(void) 
{
#ifdef DEBUG_0
    printf("k_tsk_exit: entering...\n\r");
#endif /* DEBUG_0 */
    // change current task to dormant
    gp_current_task->state = DORMANT;

    // deallocate user stack
    if (gp_current_task->priv == 0)
    {
    	is_allocating_stack = 1;
    	k_mem_dealloc((void*)((int)gp_current_task->psp - gp_current_task->stack_size));
    	is_allocating_stack = 0;
    }
    // deallocate user stack
    if (gp_current_task->mailbox != NULL)
    {
    	is_allocating_stack = 1;
    	k_mem_dealloc(gp_current_task->mailbox);
    	is_allocating_stack = 0;
    	gp_current_task->mailbox = NULL;
    }

    // decrement active tasks
    g_num_active_tasks--;
    // run k_tsk_run_new
    k_tsk_run_new();

    return;
}

int k_tsk_set_prio(task_t task_id, U8 prio) 
{
#ifdef DEBUG_0
    printf("k_tsk_set_prio: entering...\n\r");
    printf("task_id = %d, prio = %d.\n\r", task_id, prio);
#endif /* DEBUG_0 */
    // Return error if prio is not between 1 and 254 or if task_id is null task
    if (prio == PRIO_RT || prio >= PRIO_NULL || task_id == TID_NULL)
    {
    	return RTX_ERR;
    }

    // Attempt to find the task specified by task_id
    int idx = -1;
	for (int i = 1; i < MAX_TASKS; i++)
	{
		if (g_tcbs[i].tid == task_id)
		{
			idx = i;
			break;
		}
	}
    // Return error if task does not exist
	if (idx == -1)
	{
		return RTX_ERR;
	}

    // Return error if running task does not have permission to change prio
	// User task cannot change kernel task prio, all other combinations are allowed
	if (!(gp_current_task->priv) && g_tcbs[idx].priv)
	{
		return RTX_ERR;
	}

	TCB* peek_ele = pq_peek(&queue);
	if (peek_ele == NULL) return RTX_ERR;

	// Change the priority of self
	if (task_id == gp_current_task->tid)
	{
		gp_current_task->prio = prio;
		if (peek_ele->prio <= gp_current_task->prio)
		{
			// We must swap
			int res = pq_add(&queue, gp_current_task);
			if (res == RTX_ERR) return RTX_ERR;
			k_tsk_run_new();
		}
		// Keep running
		return RTX_OK;
	}

	// Change the priority of another task
	int res = pq_move(&queue, &(g_tcbs[idx]), prio);
	if (res == RTX_ERR)
	{
		return RTX_ERR;
	}

	// Check scheduler again to see if we need to swap tasks
	if (peek_ele->prio < gp_current_task->prio)
	{
		// This call also handles the call to scheduler()
		res = pq_add(&queue, gp_current_task);
		if (res == RTX_ERR) return RTX_ERR;
		k_tsk_run_new();
	}
	// Else we return from this function call and current task continues to run

    return RTX_OK;    
}

int k_tsk_get_info(task_t task_id, RTX_TASK_INFO *buffer)
{
#ifdef DEBUG_0
    printf("k_tsk_get_info: entering...\n\r");
    printf("task_id = %d, buffer = 0x%x.\n\r", task_id, buffer);
#endif /* DEBUG_0 */
    // if buffer does not exist, return error
    if (buffer == NULL) {
        return RTX_ERR;
    }

    int idx = -1;
    // Search for the associated TCB
    for (int i = 0; i < MAX_TASKS; i++) {
    	if (g_tcbs[i].tid == task_id) {
    		idx = i;
    		break;
    	}
    }
    // If not found, return error
    if (idx == -1) {
    	return RTX_ERR;
    }
    buffer->ptask = g_tcbs[idx].ptask;
    buffer->k_stack_hi = (U32)g_tcbs[idx].ksp;
    buffer->u_stack_hi = (U32)g_tcbs[idx].psp;
    buffer->k_stack_size = K_STACK_SIZE;
    buffer->u_stack_size = g_tcbs[idx].stack_size;
    buffer->tid = g_tcbs[idx].tid;
    buffer->prio = g_tcbs[idx].prio;
    buffer->state = g_tcbs[idx].state;
    buffer->priv = g_tcbs[idx].priv;
    return RTX_OK;     
}

task_t k_tsk_get_tid(void)
{
#ifdef DEBUG_0
    printf("k_tsk_get_tid: entering...\n\r");
#endif /* DEBUG_0 */ 
    return gp_current_task->tid;
}

int k_tsk_ls(task_t *buf, int count){
#ifdef DEBUG_0
    printf("k_tsk_ls: buf=0x%x, count=%d\r\n", buf, count);
#endif /* DEBUG_0 */
    return 0;
}


/*
 *===========================================================================
 *                             TO BE IMPLEMETED IN LAB4
 *===========================================================================
 */

int k_tsk_create_rt(task_t *tid, TASK_RT *task)
{
    return 0;
}

void k_tsk_done_rt(void) {
#ifdef DEBUG_0
    printf("k_tsk_done: Entering\r\n");
#endif /* DEBUG_0 */
    return;
}

void k_tsk_suspend(TIMEVAL *tv)
{
#ifdef DEBUG_0
    printf("k_tsk_suspend: Entering\r\n");
#endif /* DEBUG_0 */
    return;
}

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
