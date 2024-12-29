/*
 ****************************************************************************
 *
 *                  UNIVERSITY OF WATERLOO ECE 350 RTOS LAB
 *
 *                 Copyright 2020-2021 ECE 350 Teaching Team
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

#include "ae.h"
#include "ae_priv_tasks.h"
#include "ae_usr_tasks.h"
#include "Serial.h"
#include "printf.h"
#include "k_mem.h"

task_t ktid1;
task_t ktid2;
task_t ktid3;
task_t ktid4;
task_t ktid5;
task_t ktid6;
task_t ktid7;


#if TEST == 0
/*****************************************************************************
 * @brief       a task that prints AAAAA, BBBBB, CCCCC,...., ZZZZZ on each line.
 *              It yields the cpu every 6 lines are printed.
 *****************************************************************************/
void ktask1(void)
{
    int i = 0;
    int j = 0;
    long int x = 0;

    while (1) {
        char out_char = 'A' + i % 26;
        for (j = 0; j < 5; j++ ) {
            SER_PutChar(0,out_char);
        }

        SER_PutStr(0,"\n\r");

        for ( x = 0; x < DELAY; x++);

        if ( (++i) % 6 == 0 ) {
            SER_PutStr(0,"priv_task1 before yielding cpu.\n\r");
            k_tsk_yield();
            SER_PutStr(0,"priv_task1 after yielding cpu.\n\r");
        }
    }
}

/*****************************************************************************
 * @brief:      a task that prints 00000, 11111, 22222,....,99999 on each line.
 *              It yields the cpu every 6 lines are printed
 *              before printing these lines indefinitely, it creates a
 *              new task and and obtains the task information. It then
 *              changes the newly created task's priority.
 *****************************************************************************/

void ktask2(void)
{
    long int x = 0;
    int i = 0;
    int j = 0;
    task_t tid;

    k_tsk_create(&tid, &utask1, LOW, 0x200);  /*create a user task */
    k_tsk_set_prio(tid, LOWEST);


    for (i = 1;;i++) {
        char out_char = '0' + i%10;
        for (j = 0; j < 5; j++ ) {
            SER_PutChar(0,out_char);
        }
        SER_PutStr(0,"\n\r");

        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( i%6 == 0 ) {
            SER_PutStr(0,"priv_task2 before yielding CPU.\n\r");
            k_tsk_yield();
            SER_PutStr(0,"priv_task2 after yielding CPU.\n\r");
        }
    }
}
#endif


// Additional Tests
#if TEST == 10
void ktask1(void)
{
//    long int x = 0;
//    int i = 0;
//    int j = 0;
//
//    RTX_TASK_INFO task_info;
//    ktid1 = tsk_get_tid();
//    printf("222 OUTPUT OF GET TID: %d\n", ktid1);
//    tsk_get_info(1, &task_info);
//
//    // print get info
//    printf("printing get info for kernal task:\n");
//    printf("task_entry: %p\n", task_info.ptask);
//    printf("k_stack_hi: 0x%x\n",task_info.k_stack_hi);
//    printf("u_stack_hi: 0x%x\n",task_info.u_stack_hi);
//    printf("k_stack_size: 0x%x\n", task_info.k_stack_size);
//    printf("u_stack_size: 0x%x\n", task_info.u_stack_size);
//    printf("tid: %d\n", task_info.tid);
//    printf("prio: %d\n", task_info.prio);
//    printf("state: %d\n", task_info.state);
//    printf("priv: %d\n", task_info.priv);
//
//    // creates user task with higher priority -> user takes over
//    k_tsk_create(&utid1, &utask1, 51, 0x400);
//    // set its own priority to higher than user task -> does nothing
//    k_tsk_set_prio(ktid1, 50);
//    tsk_get_info(ktid1, &task_info);
//    printf("\nnew k_task prio: %d\n", task_info.prio);
//    // stay here
//    k_tsk_yield();
//    // switch to user task which will exit immediately
//    k_tsk_set_prio(ktid1, 102);
//    tsk_get_info(ktid1, &task_info);
//    printf("\nnew k_task prio: %d\n", task_info.prio);
//    // stay here
//    k_tsk_yield();
//
//    for (i = 1;;i++) {
//        char out_char = '0' + i%10;
//        for (j = 0; j < 5; j++ ) {
//            SER_PutChar(0,out_char);
//        }
//        SER_PutStr(0,"\n\r");
//
//        for ( x = 0; x < DELAY; x++); // some artifical delay
//        if ( i%6 == 0 ) {
//            SER_PutStr(0,"priv_task before yielding CPU.\n\r");
//            k_tsk_yield();
//            SER_PutStr(0,"priv_task after yielding CPU.\n\r");
//        }
//    }
	task_t tid = tsk_get_tid();
	printf("Entered task with prio %d\n", g_tcbs[tid].prio);

	int res;

	printf("\nAttempting to change null task's priority to 100\n");
	res = k_tsk_set_prio(0, 100);
	printf("Result: %d\n", res);
	printf("Current null task priority: %d\n", g_tcbs[0].prio);

	printf("\nAttempting to change own priority to 0\n");
	res = k_tsk_set_prio(tid, 0);
	printf("Result: %d\n", res);
	printf("Current priority: %d\n", g_tcbs[tid].prio);

	printf("\nAttempting to change own priority to 255\n");
	res = k_tsk_set_prio(tid, 255);
	printf("Result: %d\n", res);
	printf("Current priority: %d\n", g_tcbs[tid].prio);

	printf("\nAttempting to change own priority to 100\n");
	res = k_tsk_set_prio(tid, 100);
	printf("Result: %d\n", res);
	printf("Current priority: %d\n", g_tcbs[tid].prio);

	printf("\nAttempting to change own priority to 200\n");
	res = k_tsk_set_prio(tid, 200);
	printf("Result: %d\n", res);
	printf("Current priority: %d\n", g_tcbs[tid].prio);

	printf("\nAttempting to change other task's priority to 1\n");
	res = k_tsk_set_prio((tid == 1) ? 2 : 1, 1);
	printf("Result: %d\n", res);
	printf("Current priority: %d\n", g_tcbs[tid].prio);

	k_tsk_yield();

	printf("\nReached end of task with tid = %d\n", tid);
}
#endif

// Additional Tests
#if TEST == 11
void ktask1(void)
{
    long int x = 0;
    int i = 0;
    int j = 0;

    k_tsk_create(&utid1, &utask1, 90, 0x200);
    k_tsk_yield();

    for (i = 1;;i++) {
        char out_char = '0' + i%10;
        for (j = 0; j < 5; j++ ) {
            SER_PutChar(0,out_char);
        }
        SER_PutStr(0,"\n\r");

        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( i%6 == 0 ) {
            SER_PutStr(0,"priv_task before yielding CPU.\n\r");
            k_tsk_yield();
            SER_PutStr(0,"priv_task after yielding CPU.\n\r");
        }
    }
}
#endif

#if TEST == 12
void ktask1(void)
{
	SER_PutStr(0,"Entered kernel task\n\r");
	RTX_TASK_INFO task_info;

	tsk_get_info(0, &task_info);
	printf("printing get info for null task:\n");
	printf("task_entry: %p\n", task_info.ptask);
	printf("gkstack[1]: 0x%x\n",&g_k_stacks[1]);
	printf("k_stack_hi: 0x%x\n",task_info.k_stack_hi);
	printf("u_stack_hi: 0x%x\n",task_info.u_stack_hi);
	printf("k_stack_size: 0x%x\n", task_info.k_stack_size);
	printf("u_stack_size: 0x%x\n", task_info.u_stack_size);
	printf("tid: %d\n", task_info.tid);
	printf("prio: %d\n", task_info.prio);
	printf("state: %d\n", task_info.state);
	printf("priv: %d\n", task_info.priv);

	SER_PutStr(0,"Exiting kernel task\n\r");
    k_tsk_exit();
}
#endif

#if TEST == 13
void ktask1(void)
{
    long int x = 0;
    int i = 0;
    int j = 0;

    for (i = 1;;i++) {
        char out_char = '0' + i%10;
        for (j = 0; j < 5; j++ ) {
            SER_PutChar(0,out_char);
        }
        SER_PutStr(0,"\n\r");

        for ( x = 0; x < DELAY; x++); // some artifical delay
        if ( i%6 == 0 ) {
            SER_PutStr(0,"priv_task before yielding CPU.\n\r");
            k_tsk_yield();
            SER_PutStr(0,"priv_task after yielding CPU.\n\r");
        }
    }
}
#endif

#if TEST == 14
void ktask1(void)
{
    SER_PutStr(0,"this is ktask1.\n\r");
    SER_PutStr(0,"I am killing myself now.\n\r");

    k_tsk_yield();
}
void ktask2(void)
{
    SER_PutStr(0,"this is ktask2.\n\r");
    SER_PutStr(0,"I am killing myself now.\n\r");
    k_tsk_yield();
}
void ktask3(void)
{
    SER_PutStr(0,"this is ktask3.\n\r");
    SER_PutStr(0,"I am killing myself now.\n\r");
    k_tsk_yield();
}
void ktask4(void)
{
    SER_PutStr(0,"this is ktask4.\n\r");
    SER_PutStr(0,"I am killing myself now.\n\r");
    k_tsk_yield();
}
void ktask5(void)
{
    SER_PutStr(0,"this is ktask5.\n\r");
    SER_PutStr(0,"I am killing myself now.\n\r");
    k_tsk_yield();
}
#endif

#if TEST == 15
void ktask1(void)
{
	SER_PutStr (0,"ktask1: entering \n\r");
	while(1)
	{
		k_tsk_yield();
		printf("I am looping in kernel task\n");
	}
	SER_PutStr (0,"ktask1: exiting \n\r");
//    k_tsk_exit();
}
#endif

#if TEST == 20
void ktask1(void)
{
	void* bigbongarrr[10];

	SER_PutStr (0,"ktask1: entering \n\r");
    for (int i = 0; i < 10; i++)
    {
        SER_PutStr (0,"ktask1: alloc \n\r");
        bigbongarrr[i] = mem_alloc(100);
        print_mem();
        k_tsk_yield();
    }
	SER_PutStr (0,"ktask1: finished \n\r");
	for (int i = 0 ; i < 10 ; i++)
	{
		mem_dealloc(bigbongarrr[i]);
	}
	print_mem();

	k_tsk_set_prio(1, 120);
}
#endif

#if TEST == 21
void ktask1(void)
{
    // 1
    int prio = 1;
    while(prio <= 254 - 5)
    {
        SER_PutStr (0,"ktask1: entering \n\r");
        prio += 5;
        k_tsk_set_prio(1, prio);
        k_tsk_yield();
    }
}
void ktask2(void)
{
    // 2
    int prio = 2;
    while(prio <= 254 - 5)
    {
        SER_PutStr (0,"ktask2: entering \n\r");
        prio += 5;
        k_tsk_set_prio(2, prio);
        k_tsk_yield();
    }
}
void ktask3(void)
{
    // 3
    int prio = 3;
    while(prio <= 254 - 5)
    {
        SER_PutStr (0,"ktask3: entering \n\r");
        prio += 5;
        k_tsk_set_prio(3, prio);
        k_tsk_yield();
    }
}
void ktask4(void)
{
    // 4
    int prio = 4;
    while(prio <= 254 - 5)
    {
        SER_PutStr (0,"ktask4: entering \n\r");
        prio += 5;
        k_tsk_set_prio(4, prio);
        k_tsk_yield();
    }
}
void ktask5(void)
{
    // 5
    int prio = 5;
    while(prio <= 254 - 5)
    {
        SER_PutStr (0,"ktask5: entering \n\r");
        prio += 5;
        k_tsk_set_prio(5, prio);
        k_tsk_yield();
    }
}
#endif

#if TEST == 22
void ktask1(void)
{
    task_t u_tid1;
    k_tsk_create(&u_tid1, &utask1, 50, 0x200);
    task_t u_tid2;
    k_tsk_create(&u_tid2, &utask2, 50, 0x200);
    task_t u_tid3;
    k_tsk_create(&u_tid3, &utask3, 50, 0x200);
    task_t u_tid4;
    k_tsk_create(&u_tid4, &utask4, 50, 0x200);
    task_t u_tid5;
    k_tsk_create(&u_tid5, &utask5, 50, 0x200);
    task_t u_tid6;
    k_tsk_create(&u_tid6, &utask6, 50, 0x200);
    task_t u_tid7;
    k_tsk_create(&u_tid7, &utask7, 50, 0x200);
    SER_PutStr (0,"ktask1: finished creating user tasks \n\r");

    k_tsk_set_prio(ktask1, 100);
    k_tsk_yield();
}
 
#endif

#if TEST == 23
void ktask1(void)
{
    task_t u_tid1;
    k_tsk_create(&u_tid1, &utask1, 50, 0x001);
    task_t u_tid2;
    k_tsk_create(&u_tid2, &utask2, 50, 0x002);
    task_t u_tid3;
    k_tsk_create(&u_tid3, &utask3, 50, 0x004);
    task_t u_tid4;
    k_tsk_create(&u_tid4, &utask4, 50, 0x008);
    task_t u_tid5;
    k_tsk_create(&u_tid5, &utask5, 50, 0x010);

    // Total stack size should add up to 11111 in binary.
    
    SER_PutStr (0,"ktask1: finished creating user tasks \n\r");

    while(1){
        // teehee
    }
}
#endif

#if TEST == 24
void ktask1(void)
{
    while(1)
    {
        SER_PutStr (0,"ktask1 running \n\r");
        k_tsk_yield();
    }
}

void ktask2(void)
{
    while(1)
    {
        SER_PutStr (0,"ktask2 running \n\r");
        k_tsk_yield();
    }
}
#endif

#if TEST == 25
void ktask1(void)
{
    void *p1 = mem_alloc(10);
    SER_PutStr (0,"ktask1: allocated memory \n\r");

    task_t u_tid1;
    k_tsk_create(&u_tid1, &utask1, 40, 0x200);

    SER_PutStr (0,"ktask1: created user task 1 \n\r");

    k_tsk_yield();

    int created = 0;
    while(1)
    {
        if (!created)
        {
            mem_dealloc(p1);
            task_t u_tid3;
            k_tsk_create(&u_tid3, &utask3, 30, 0x200);
            created = 1;
            SER_PutStr (0,"ktask1: created user task 3 \n\r");

            k_tsk_yield();
        }
        
    }
}
#endif

#if TEST == 26
void utask1(void)
{
	// This task will immediately terminate
	task_t tid = tsk_get_tid();
	printf("\nEntered suicidal task with tid = %d\n", tid);
	for (int i = 0 ; i < DELAY ; i++)
	{
		int x = 1 + 1;
	}
	printf("Exiting suicidal task with tid = %d\n", tid);
	k_tsk_exit();
}

void utask2(void)
{
	task_t child_tid;
	// This task will create a new task
	task_t tid = tsk_get_tid();
	printf("\nEntered generating task with tid = %d\n", tid);

	k_tsk_create(&child_tid, &utask1, 254, 0x1111);
	printf("Created task with tid = %d\n", child_tid);
	for (int i = 0 ; i < DELAY ; i++)
	{
		int x = 1 + 1;
	}
	printf("Exiting generating task with tid = %d\n", tid);
	while(1)
	{
		k_tsk_yield();
	}
}
#endif

#if TEST == 27
void ktask2(void)
{
	task_t tid = tsk_get_tid();
//	task_t child_tid;
	printf("Entered task with tid = %d\n", tid);

//	RTX_TASK_INFO task_info;
//	tsk_get_info(0, &task_info);

//	printf("printing get info for null task:\n");
//	printf("task_entry: %p\n", task_info.ptask);
//	printf("gkstack[1]: 0x%x\n",&g_k_stacks[1]);
//	printf("k_stack_hi: 0x%x\n",task_info.k_stack_hi);
//	printf("u_stack_hi: 0x%x\n",task_info.u_stack_hi);
//	printf("k_stack_size: 0x%x\n", task_info.k_stack_size);
//	printf("u_stack_size: 0x%x\n", task_info.u_stack_size);
//	printf("tid: %d\n", task_info.tid);
//	printf("prio: %d\n", task_info.prio);
//	printf("state: %d\n", task_info.state);
//	printf("priv: %d\n", task_info.priv);

//	for (int i = 0 ; i < 158 ; i++)
//	{
//		k_tsk_create(&child_tid, &ktask1, 200, 0x1111-i);
//		printf("Created task with tid = %d\n", child_tid);
//	}
//	while (1)
//	{
//		k_tsk_yield();
//		printf("Re-entered task with tid = %d\n", tid);
//	}
}

void ktask1(void)
{
	task_t tid = tsk_get_tid();
	printf("Entered task with tid = %d\n", tid);
	while (1)
	{
		for (int i = 0 ; i < DELAY/50 ; i++)
		{
			int x = 1 + 1;
		}
		k_tsk_yield();
		printf("Re-entered task with tid = %d\n", tid);
	}
}
#endif

#if TEST == 30
void ktask1(void)
{
    while(1)
    {
        k_tsk_yield();
    }
}
#endif

#if TEST == 31
void ktask1(void)
{
	while(1){
		SER_PutStr (0,"ktask1: prio NULL \n\r");
		k_tsk_yield();
    }
}

void ktask2(void)
{
	while(1){
		SER_PutStr (0,"ktask2: prio -1 \n\r");
		k_tsk_yield();
    }
}

void ktask3(void)
{
	while(1){
		SER_PutStr (0,"ktask3: prio 10000000 \n\r");
		k_tsk_yield();
    }
}
#endif

#if TEST == 33
void ktask1(void)
{
    task_t u_tid1;
    SER_PutStr (0,"ktask1: creating user task with stacksize 65535 \n\r");
    int fail_count = 0;
    print_mem();
    for (int i = 0; i < 1000; i++)
    {
    	int res = k_tsk_create(&u_tid1, &utask1, 60, 65527);
    	if(res == -1) fail_count++;
//    	printf ("result: %d\n", res);
    	if (i % 1000 == 0){
			printf ("ktask1: creating user task number %d\n\r", i);
    	}
    }
    print_mem();
    printf("number of fails %d\n", fail_count);
    SER_PutStr (0,"ktask1: created user task with stacksize 65535 \n\r");

    while(1)
    {
        // SER_PutStr (0,"ktask1: stacksize 0x10000000000000 \n\r");
		k_tsk_yield();
    }
}
#endif

#if TEST == 34
void ktask1(void)
{
    task_t u_tid1;
    SER_PutStr (0,"ktask1: creating user task with NULL parameter \n\r");
    int result = k_tsk_create(&u_tid1, NULL, 30, 0x200);
    printf ("ktask1: created user task with NULL parameter: %d\n\r", result);

    while(1)
    {
        // SER_PutStr (0,"ktask1: stacksize 0x200 \n\r");
		k_tsk_yield();
    }
}
#endif

#if TEST == 35
void ktask1(void)
{
    task_t u_tid1;
    SER_PutStr (0,"ktask1: creating user task with NULL parameter \n\r");
    k_tsk_create(&u_tid1, NULL, 30, 0x200);
    SER_PutStr (0,"ktask1: created user task with NULL parameter \n\r");

    while(1)
    {
        // SER_PutStr (0,"ktask1: stacksize 0x200 \n\r");
		k_tsk_yield();
    }
}
#endif

#if TEST == 36
void ktask1(void)
{
    task_t u_tid1;
    SER_PutStr (0,"entering kernel task \n\r");
    for (int i = 0; i < 3 ; i++)
    {
		k_tsk_create(&u_tid1, &utask1, 101, 0x200);
    }
    print_mem();
    k_tsk_set_prio(1, 101);
    print_mem();
    k_tsk_set_prio(1, 102);
    print_mem();

    SER_PutStr (0,"kernel task in yield loop now\n\r");
    while(1)
    {
        // SER_PutStr (0,"ktask1: stacksize 0x200 \n\r");
		k_tsk_yield();
    }
}
#endif

#if TEST == 37
#endif

#if TEST == 38
void ktask1(void)
{
    task_t u_tid1;
    SER_PutStr (0,"1. creating task 1 w priority 100 - should preempt\n");
    k_tsk_create(&u_tid1, &utask1, 99, 0x234);
    printf("17. ktask1 yielding\n");
    k_tsk_yield();
    printf("18. ktask1 resume and final exit\n");
    k_tsk_exit();
}
#endif

#if TEST == 39
void ktask1(void)
{
    task_t u_tid1;
    task_t child_tid;
    k_tsk_create(&child_tid, &utask1, 101, 0x234);
    printf ("Entered ktask1, creating utask 1 with tid = %d\n", child_tid);
    printf("1. ktask1 yielding\n");
    k_tsk_yield();
    printf("2. ktask1 resume and final exit\n");
    k_tsk_exit();
}
#endif
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
