/**
 * @file:   k_msg.c
 * @brief:  kernel message passing routines
 * @author: Yiqing Huang
 * @date:   2020/10/09
 */

#include "k_msg.h"

//#define DEBUG_0

#ifdef DEBUG_0
#include "printf.h"
#endif /* ! DEBUG_0 */

// External Globals
extern TCB g_tcbs[];
extern TCB* gp_current_task;
extern PRIO_Q queue;
extern int is_allocating_stack;

int k_mbx_create(size_t size) {
#ifdef DEBUG_0
    printf("k_mbx_create: passed arg size = %d\n", size);
#endif /* DEBUG_0 */

    // Check for error conditions in passed parameters
    // 1. Mailbox size requested is below the minimum
    // 2. The task requesting the mailbox already owns a mailbox
    if (size < MIN_MBX_SIZE || gp_current_task->mailbox != NULL) return RTX_ERR;

#ifdef DEBUG_0
    printf("k_mbx_create: mem alloc size is %d\n", size + sizeof(CIRC_Q));
#endif

    // Allocate the memory for the task's mailbox and assign ownership to the kernel
    is_allocating_stack = 1;
    gp_current_task->mailbox = (CIRC_Q*)(k_mem_alloc(size + sizeof(CIRC_Q)));
    is_allocating_stack = 0;
    // Check if mailbox was allocated correctly
	if (gp_current_task->mailbox == NULL) return RTX_ERR;

#ifdef DEBUG_0
	printf("k_mbx_create: memory after allocating a mailbox of size = %d for tid = %d\n", size, gp_current_task->tid);
    print_mem();
#endif

    // Set offset for mailbox buffer
    void *buf = (void*)((int)gp_current_task->mailbox + sizeof(CIRC_Q));
    // Initialize the circular queue structure and check for error
    if (cq_init(gp_current_task->mailbox, buf, ALIGN4(size)) == RTX_ERR) return RTX_ERR;

#ifdef DEBUG_0
    printf("k_mbx_create: mailbox init size: %d\n",           gp_current_task->mailbox->size);
    printf("k_mbx_create: mailbox init num_msgs: %d\n",       gp_current_task->mailbox->num_msgs);
    printf("k_mbx_create: mailbox init buf: 0x%x\n",          gp_current_task->mailbox->buf);
    printf("k_mbx_create: mailbox init head: 0x%x\n",         gp_current_task->mailbox->head);
    printf("k_mbx_create: mailbox init tail: 0x%x\n",         gp_current_task->mailbox->tail);
    printf("k_mbx_create: mailbox init size remaining: %d\n", cq_count_space(gp_current_task->mailbox));
#endif
    return RTX_OK;
}

int k_send_msg(task_t receiver_tid, const void *buf) {
#ifdef DEBUG_0
    printf("k_send_msg: receiver_tid = %d, buf=0x%x\r\n", receiver_tid, buf);
#endif /* DEBUG_0 */
    U32 buffer_length = ((RTX_MSG_HDR*) buf)->length;

#ifdef DEBUG_0
    U32 buffer_type = ((RTX_MSG_HDR*) buf)->type;
    printf("buffer_length = %d\n", buffer_length);
    printf("buffer_type = %d\n", buffer_type);

    printf("k_send_msg: receiver_tid: %d\n",               receiver_tid);
    printf("k_send_msg: g_tcbs[receiver_tid].state: %d\n", g_tcbs[receiver_tid].state);
    printf("k_send_msg: g_tcbs[receiver_tid].mailbox: 0x%x\n", g_tcbs[receiver_tid].mailbox);
    printf("k_send_msg: buf: 0x%x\n", buf);
    printf("k_send_msg: buffer_length: %d\n", buffer_length);
    printf("k_send_msg: g_tcbs[receiver_tid].tid: %d\n", g_tcbs[receiver_tid].tid);
    printf("k_send_msg: gp_current task: %p\n", gp_current_task);

#endif /* DEBUG_0 */
    // Check for error conditions on parameters
    if (receiver_tid >= MAX_TASKS
    		|| g_tcbs[receiver_tid].state == DORMANT
			|| g_tcbs[receiver_tid].mailbox == NULL
    		|| buf == NULL
			|| buffer_length < MIN_MSG_SIZE + sizeof(RTX_MSG_HDR))
    	return RTX_ERR;

    // Add in the sender tid to the message
    if (((RTX_MSG_HDR*)buf)->sender_tid != TID_UART_IRQ)
    {
    	((RTX_MSG_HDR*)buf)->sender_tid = gp_current_task->tid;
    }

    // Copy the message from the buf to the receiver task's mailbox
    if (cq_write(g_tcbs[receiver_tid].mailbox, buffer_length, (unsigned char*)buf) == RTX_ERR) return RTX_ERR;

#ifdef DEBUG_0
    printf("k_send_msg: The state of the receiver task: %s\n",
    		(g_tcbs[receiver_tid].state == DORMANT) ? "DORMANT" :
    		(g_tcbs[receiver_tid].state == READY)   ? "READY"   :
			(g_tcbs[receiver_tid].state == RUNNING) ? "RUNNING" :
			(g_tcbs[receiver_tid].state == BLK_MSG) ? "BLK_MSG" :
			 "UNKOWN_STATE");
#endif

    // This is the case where the task sends a message to its own mailbox
    if (gp_current_task->tid == receiver_tid)
    {
        // We don't change its state and we don't add it to the prio queue... it just continues to run
        return RTX_OK;
    }
    // Check if the receiving task is in a blocked state
    else if (g_tcbs[receiver_tid].state == BLK_MSG)
    {
        // Change the receiver task's state to READY
        g_tcbs[receiver_tid].state = READY;

#ifdef DEBUG_0
            printf("k_send_msg: state of ready queue before unblocked task to queue\n");
            pq_print(&queue);
#endif

        // Check if receiver prio is higher prio (lower number) than current task's prio
        if (g_tcbs[receiver_tid].prio < gp_current_task->prio)
        {
            // Change current task to READY state and add both tasks to the ready queue
            gp_current_task->state = READY;
            pq_add(&queue, gp_current_task);
            pq_add(&queue, &g_tcbs[receiver_tid]);
#ifdef DEBUG_0
            printf("k_send_msg: state of ready queue after adding current and unblocked task to queue\n");
            pq_print(&queue);
#endif
            // Allow scheduler to run the next highest priority task
            k_tsk_run_new();
        }
        else
        {
        	// Current task's priority is higher, so current task continues to run
        	// Add the receiver task back to the ready queue
            pq_add(&queue, &g_tcbs[receiver_tid]);
#ifdef DEBUG_0
            printf("k_send_msg: state of ready queue after adding unblocked task to queue\n");
            pq_print(&queue);
#endif
        }
    }
    return RTX_OK;
}

int k_recv_msg(task_t *sender_tid, void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg: (ptr) sender_tid  = 0x%x, buf=0x%x, len=%d\r\n", sender_tid, buf, len);
#endif /* DEBUG_0 */

    // Check for error conditions on inputs
	if (gp_current_task->mailbox == NULL || buf == NULL) return RTX_ERR;

#ifdef DEBUG_0
	printf("k_recv_msg: number of messages in mailbox: %d\n", gp_current_task->mailbox->num_msgs);
#endif

	// Check if the mailbox is empty
	if (gp_current_task->mailbox->num_msgs == 0)
	{
		// No need to remove the current task from the ready queue since it is the current task (already removed)
		// Set current task to blocked state and run scheduler
		gp_current_task->state = BLK_MSG;
		k_tsk_run_new();
	}

	// Copy the message contents from mailbox into provided buf
	if (cq_read(gp_current_task->mailbox, len, buf) == RTX_ERR) return RTX_ERR;

#ifdef DEBUG_0
	printf("k_recv_msg: tid of sender is %d\n", ((RTX_MSG_HDR*) buf)->sender_tid));
#endif

	// Assign sender_tid if receiving task requires the sender tid
	if (sender_tid != NULL)
	{
		*sender_tid = ((RTX_MSG_HDR*) buf)->sender_tid;
	}
	return RTX_OK;
}

// ************************* NOT IN USE ******************************
int k_recv_msg_nb(task_t *sender_tid, void *buf, size_t len) {
#ifdef DEBUG_0
    printf("k_recv_msg_nb: sender_tid  = 0x%x, buf=0x%x, len=%d\r\n", sender_tid, buf, len);
#endif /* DEBUG_0 */
    return 0;
}

// ************************* NOT IN USE ******************************
int k_mbx_ls(task_t *buf, int count) {
#ifdef DEBUG_0
    printf("k_mbx_ls: buf=0x%x, count=%d\r\n", buf, count);
#endif /* DEBUG_0 */
    return 0;
}
