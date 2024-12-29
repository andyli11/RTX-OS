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
#include "ae_usr_tasks.h"
#include "rtx.h"
#include "Serial.h"
#include "printf.h"

extern void kcd_task(void);

task_t utid1;
task_t utid2;
task_t utid3;
task_t utid4;
task_t utid5;
task_t utid6;
task_t utid7;

#if TEST == 0
void utask1(void)
{
    task_t tid;
    RTX_TASK_INFO task_info;

    SER_PutStr (0,"utask1: entering \n\r");

    /* do something */
    tsk_create(&tid, &utask2, 175, 0x200);  /*create a user task */
    tsk_get_info(tid, &task_info);
    tsk_set_prio(tid, 200);
    /* terminating */
    tsk_exit();
}

/**
 * @brief: a dummy task2
 */
void utask2(void)
{
    SER_PutStr (0,"utask2: entering \n\r");
    /* do something */
    long int x = 0;
    int ret_val = 10;
    int i = 0;
    int j = 0;
    for (i = 1;;i++) {
		char out_char = 'a' + i%10;
		for (j = 0; j < 5; j++ ) {
			SER_PutChar(0,out_char);
		}

		SER_PutStr(0,"\n\r");

		for ( x = 0; x < 5000000; x++); // some artifical delay

		if (i % 6 == 0) {
			SER_PutStr(0,"utask2 before yielding cpu.\n\r");
			ret_val = tsk_yield();
			SER_PutStr(0,"utask2 after yielding cpu.\n\r");
			printf("utask2: ret_val=%d\n\r", ret_val);
		}
	}
}
#endif


#if TEST == 1

void utask1(void) {
	int res = -1;
	k_mem_init();

#if TEST_N == 1
	res = test_large_coalesce();
#endif
#if TEST_N == 2
	res = test_increasing_alloc_dealloc();
#endif
#if TEST_N == 3
	res = test_block_size_rounding();
#endif
#if TEST_N == 10
	res = test_coalescing_free_regions_using_count_extfrag();
#endif
#if TEST_N == 11
	res = test_mem();
#endif
#if TEST_N == 69
	res = testing_something();
#endif

	printf("Test result: %d\n", res);
	tsk_exit();
}

#endif

#if TEST == 2

volatile char turn = 1;

void utask1(void) {
	printf("[UT1] Info: Entering user task 1!\r\n");

	RTX_MSG_HDR *msg = NULL;
	task_t sender_tid = 0;

	int i = 0;
	int j = 0;
	int eflag = 0;
	char tsk2_passed = 0;
	char correct_msg = 1;
	char message[4] = {'c', 'G', 'T', 'A'};
	utid1 = tsk_get_tid();

	printf("[UT1] Info: Creating a mailbox!\r\n");

	if (mbx_create(2 * (sizeof(RTX_MSG_HDR) + sizeof(task_t) + 5)) == RTX_OK) {
		printf("[UT1] Info: Allocating memory for messages!\r\n");

		msg = mem_alloc(sizeof(RTX_MSG_HDR) + 4);

		if (msg != NULL) {
			printf("[UT1] Info: Setting up a registration message!\r\n");

			msg->length = sizeof(RTX_MSG_HDR) + 1;
			msg->type = KCD_REG;
			*((char *)(msg + 1)) = 'c';

			printf("[UT1] Info: Registering 'c' command with the KCD task!\r\n");

			if (send_msg(TID_KCD, msg) == RTX_OK) {
				printf("[UT1] Passed: Registered 'c' command with the KCD!\r\n");
				printf("- Task 1 will wait for input commands from KCD in a loop!\r\n");
				printf("- Task 1 will let task 2 run by setting a global variable after receiving 2 commands!\r\n");
				printf("- Task 1 waist for task 2 to send it a message with test results!\r\n");
				printf("- Task 2 will register 'c' and 'd' with KCD and wait for KCD to receive 4 commands!\r\n");
				printf("- Task 2 will send a message to task 1 with test results and exits\r\n");
				printf("- Task 1 will print the results!\r\n");

				for (i = 0; i < 2; i++) {
					printf("[UT1] Info: Calling receive!\r\n");

					if (recv_msg(&sender_tid, msg, sizeof(RTX_MSG_HDR) + 4) == RTX_OK) {
						printf("[UT1] Info: Checking the sender!\r\n");

						if (sender_tid == TID_KCD) {
							printf("[UT1] Info: Checking the data in the message!\r\n");
							correct_msg = 1;

							if (((msg->length) - sizeof(RTX_MSG_HDR)) == 4) {
								for (j = 0; j < 4; j++) {
									char c = *((char *)(msg + 1) + j);

									SER_PutChar(1, c);

									if (c != message[j]) {
										correct_msg = 0;
									}
								}

								SER_PutStr(1, "\r\n");
							} else {
								correct_msg = 0;
							}

							if (correct_msg == 1) {
								printf("[UT1] Passed: Received the correct message from KCD!\r\n");
							} else {
								printf("[UT1] Failed: Received a wrong message from KCD!\r\n");
								eflag++;
							}

						} else {
							printf("[UT1] Failed: Message was not sent by KCD task!\r\n");
							printf("Cannot run the rest of the test items including:\r\n");
							printf("- Checking the content of the message.\r\n");
							eflag++;
						}

					} else {
						printf("[UT1] Failed: Receiving a message caused an error!\r\n");
						printf("Cannot run the rest of the test items including:\r\n");
						printf("- Checking the sender and the content of the message.\r\n");
						eflag++;
					}
				}
			} else {
				printf("[UT1] Failed: Could not register a command with KCD!\r\n");
				printf("Cannot run the rest of the test items including:\r\n");
				printf("- Calling recv_msg in a loop and to receive input commands from KCD.\r\n");
				eflag = 3;
			}

			printf("[UT1] Info: Setting global variable to let user task 2 register 'c' command!\r\n");
			turn = 2;

			printf("[UT1] Info: Searching for TID of task 2!\r\n");

			printf("[UT1] Info: Calling receive function to wait for task 2 to be done!\r\n");

			if (recv_msg(&sender_tid, msg, sizeof(RTX_MSG_HDR) + 4) == RTX_OK) {
				printf("[UT1] Info: Checking the sender!\r\n");

				if (sender_tid == utid2) {
					printf("[UT1] Passed: A message was received from task 2!\r\n");
					printf("[UT1] Info: Checking the content of message to see how many tests were passed by task 2!\r\n");

					if (*((char *)(msg + 1)) < 10) {
						tsk2_passed = *((char *)(msg + 1));
						printf("[UT1] Passed: %d tests passed by task 2!\r\n", (int)tsk2_passed);
					} else {
						tsk2_passed = 0;
						printf("[UT1] Failed: A wrong message was received from task 2!\r\n");
					}

					eflag += (7 - tsk2_passed);

				} else {
					printf("[UT1] Failed: Received a message from a task with different TID than task 2!\r\n");
					printf("Cannot run the rest of the test items including:\r\n");
					printf("- Checking task 2's tests.\r\n");
					eflag += 8;
				}
			} else {
				printf("[UT1] Failed: Message was not successfully received!\r\n");
				printf("Cannot run the rest of the test items including:\r\n");
				printf("- Checking task 2's tests.\r\n");
				eflag += 8;
			}

		} else {
			printf("[UT1] Failed: Could not allocate memory!\r\n");
			printf("Cannot run the rest of the test items including:\r\n");
			printf("- Calling recv_msg in a loop and to receive input commands from KCD; and\r\n");
			printf("- Checking task 2's tests.\r\n");
			eflag = 11;
		}
	} else {
		printf("[UT1] Failed: Could not create a mailbox!\r\n");
		printf("Cannot run the rest of the test items including:\r\n");
		printf("- Calling recv_msg in a loop and to receive input commands from KCD; and\r\n");
		printf("- Checking task 2's tests.\r\n");
		eflag = 11;
	}

	printf("============================================\r\n");
	printf("=============Final test results=============\r\n");
	printf("============================================\r\n");
	printf("[T_02] %d out of 11 tests passed!\r\n", 11 - eflag);
	tsk_exit();
}

void utask2(void) {
	printf("[UT2] Info: Entering user task 2!\r\n");

	RTX_MSG_HDR *msg = NULL;
	task_t sender_tid = 0;

	int i = 0;
	int j = 0;
	char sflag = 0;
	char correct_msg = 1;
	char message1[4] = {'c', 'G', 'T', 'A'};
	char message2[4] = {'d', 'G', 'T', 'A'};
	char *message;

	utid2 = tsk_get_tid();
	task_t tsk1_tid = 0;

	printf("[UT2] Info: Waiting for task 1 to be done with its first portion!\r\n");

	while(turn == 1);

	printf("[UT2] Info: Creating a mailbox!\r\n");

	if (mbx_create(2 * (sizeof(RTX_MSG_HDR) + sizeof(task_t) + 5)) == RTX_OK) {
		printf("[UT2] Info: Allocating memory for messages!\r\n");

		msg = mem_alloc(sizeof(RTX_MSG_HDR) + 4);

		if (msg != NULL) {
			printf("[UT2] Info: Setting up a registration message!\r\n");

			msg->length = sizeof(RTX_MSG_HDR) + 1;
			msg->type = KCD_REG;
			*((char *)(msg + 1)) = 'c';

			printf("[UT2] Info: Registering 'c' command with the KCD task!\r\n");

			if (send_msg(TID_KCD, msg) == RTX_OK) {
				printf("[UT2] Passed: Registered 'c' command with the KCD!\r\n");
				sflag++;

				printf("[UT2] Info: Setting up another registration message!\r\n");
				*((char *)(msg + 1)) = 'd';

				printf("[UT2] Info: Registering 'd' command with the KCD task!\r\n");

				if (send_msg(TID_KCD, msg) == RTX_OK) {
					printf("[UT2] Passed: Registered 'd' command with the KCD!\r\n");
					sflag++;

					for (i = 0; i < 4; i++) {
						printf("[UT2] Info: Calling receive!\r\n");
						printf("- KCD will send task 1 input commands!\r\n");
						if ((i % 2) == 0) {
							message = message1;
						} else {
							printf("%%dGROUP and then press enter!\r\n");
							message = message2;
						}
						printf("You will probably see the command echoed into the Putty!\r\n");

						if (recv_msg(&sender_tid, msg, sizeof(RTX_MSG_HDR) + 4) == RTX_OK) {
							printf("[UT2] Info: Checking the sender!\r\n");

							if (sender_tid == TID_KCD) {
								printf("[UT2] Info: Checking the data in the message!\r\n");
								correct_msg = 1;

								if (((msg->length) - sizeof(RTX_MSG_HDR)) == 4) {
									for (j = 0; j < 4; j++) {
										char c = *((char *)(msg + 1) + j);

										SER_PutChar(1, c);

										if (c != message[j]) {
											correct_msg = 0;
										}
									}

									SER_PutStr(1, "\r\n");
								} else {
									correct_msg = 0;
								}

								if (correct_msg == 1) {
									printf("[UT2] Passed: Received the correct message from KCD!\r\n");
									sflag++;
								} else {
									printf("[UT2] Failed: Received a wrong message from KCD!\r\n");
								}

							} else {
								printf("[UT2] Failed: Message was not sent by KCD task!\r\n");
								printf("Cannot run the rest of the test items including:\r\n");
								printf("- Checking the content of the message.\r\n");
							}

						} else {
							printf("[UT2] Failed: Receiving a message caused an error!\r\n");
							printf("Cannot run the rest of the test items including:\r\n");
							printf("- Checking the sender and the content of the message.\r\n");
						}
					}
				} else {
					printf("[UT2] Failed: Could not register a command with KCD!\r\n");
					printf("Cannot run the rest of the test items including:\r\n");
					printf("- Calling recv_msg in a loop and to receive input commands from KCD.\r\n");
				}
			} else {
				printf("[UT2] Failed: Could not register a command with KCD!\r\n");
				printf("Cannot run the rest of the test items including:\r\n");
				printf("- Calling recv_msg in a loop and to receive input commands from KCD.\r\n");
			}

			printf("[UT2] Info: Searching for TID of task 1!\r\n");
			tsk1_tid = utid1;
			if (tsk1_tid != 0) {
				printf("[UT2] Info: Changing priority of task 1 to lowest!");

				if (tsk_set_prio(tsk1_tid, 200) == RTX_OK) {
					printf("[UT2] Passed: Successfully changed priority of task 1 to lowest!\r\n");
					sflag++;

					printf("[UT2] Info: Setting up a message to be sent to test 1!\r\n");

					msg->type = DEFAULT;
					*((char *)(msg + 1)) = sflag;

					send_msg(tsk1_tid, msg);

				} else {
					printf("[UT2] Failed: Could not set priority of task 1!\r\n");
					printf("Cannot run the rest of the test items including:\r\n");
					printf("- Sending a message to task 1 to let it finish and print the test result!\r\n");
				}
			} else {
				printf("[UT2] Failed: Could not find TID of task 1!\r\n");
				printf("Cannot run the rest of the test items including:\r\n");
				printf("- Sending a message to task 1 to let it finish and print result!\r\n");
			}
		} else {
			printf("[UT2] Failed: Could not allocate memory!\r\n");
			printf("Cannot run the rest of the test items including:\r\n");
			printf("- Calling recv_msg in a loop and to receive input commands from KCD; and\r\n");
			printf("- Sending a message to task 1 and letting it print the test results.\r\n");
		}
	} else {
		printf("[UT2] Failed: Could not create a mailbox!\r\n");
		printf("Cannot run the rest of the test items including:\r\n");
		printf("- Calling recv_msg in a loop and to receive input commands from KCD; and\r\n");
		printf("- Sending a message to task 1 and letting it print the test results.\r\n");
	}

	printf("[UT2] Exiting task 2!\r\n");
	tsk_exit();
}

void utask3(void) {
	while(1) {
		tsk_yield();
	}
}

#endif

#if TEST == 3

volatile int sflag = 0;
int num_iterations = 8;
int num_dequeues = 8;

void utask1(void) {
	printf("[UT1] Info: Entering user task 1!\r\n");

	task_t my_tid  = 0;
	RTX_MSG_HDR *msg = NULL;

	int i = 0;
	int j = 0;
	int what_to_expect = 0;

	int message_len = 0;
	char is_sequential = 0;

	printf("[UT1] Info: Searching for TID of task 1!\r\n");

	utid1 = tsk_get_tid();
	my_tid = utid1;

	if (my_tid != 0) {
		printf("[UT1] Info: Creating a mailbox!\r\n");

		if (mbx_create(0x1FF) == RTX_OK) {
			printf("[UT1] Info: Allocating memory for the messages!\r\n");

			msg = mem_alloc(sizeof(RTX_MSG_HDR) + 4);

			if (msg != NULL) {
				printf("[UT1] Info: Calling receive to let task 2 send task 1 messages!\r\n");
				printf("1. Task 2 will change priority of task 1 to lowest!\r\n");
				printf("2. Task 2 will fill up task 1's mailbox!\r\n");
				printf("3. Task 2 will change priority of task 1 back to medium!\r\n");
				printf("4. Task 1 dequeues some messages (checking for content as well) and sets its own priority to lowest!\r\n");
				printf("5. Task 2 fills up task 1's mailbox and we repeat (3), (4), and (5) for few times!\r\n");

				for (i = 0; i < num_iterations; i++) {
					if ((i % 2) == 0) {
						message_len = 1;
					} else {
						message_len = 4;
					}

					is_sequential = 1;

					for (j = 0; j < num_dequeues; j++) {
						if (recv_msg(NULL, msg, sizeof(RTX_MSG_HDR) + message_len) == RTX_OK) {
							if ((*((int *)(msg + 1))) != what_to_expect) {
								is_sequential = 0;
								break;
							}
						} else {
							if ((i % 2) != 0) {
								is_sequential = 0;
								break;
							}
						}
						what_to_expect++;
					}

					if (is_sequential == 1) {
						printf("[UT1] Passed: Messages were sequential!\r\n");
						sflag++;
					} else {
						printf("[UT1] Failed: Messages were not sequential!\r\n");
						break;
					}

					printf("[UT1] Info: Setting the priority of task 1 to lowest\r\n");

					if (tsk_set_prio(my_tid, 200) != RTX_OK) {
						printf("[UT1] Failed: Could not set the priority of task 1 to lowest\r\n");
						break;
					}
				}
			} else {
				printf("[UT1] Failed to allocate memory for messages!\r\n");
			}
		} else {
			printf("[UT1] Failed: Could not create a mailbox!\r\n");
		}
	} else {
		printf("[UT1] Failed: Could not find TID of task 1!\r\n");
	}

	printf("============================================\r\n");
	printf("=============Final test results=============\r\n");
	printf("============================================\r\n");
	printf("[T_03] %d out of 17 tests passed!\r\n", sflag);
	tsk_exit();
}


void utask2(void) {
	printf("[UT2] Info: Entering task 2!\r\n");

	RTX_MSG_HDR *msg = NULL;
	task_t tsk1_tid = 0;

	int what_to_send = 0;
	int tsk1_mbx_cap = 0;

	int i = 0;
	int j = 0;

	char lost_cap = 0;

	printf("[UT2] Info: Searching for TID of task 1!\r\n");
	tsk1_tid = utid1;

	if (tsk1_tid != 0) {
		printf("[UT2] Info: Setting priority of task 1 to lowest!\r\n");

		if(tsk_set_prio(tsk1_tid, 200) == RTX_OK) {
			printf("[UT2] Info: Allocating memory for messages!\r\n");
			msg = mem_alloc(sizeof(RTX_MSG_HDR) + 4);

			if (msg != NULL) {
				printf("[UT2] Info: Setting up sequence of messages to be sent to task 1!\r\n");

				msg->type = DEFAULT;
				msg->length = sizeof(RTX_MSG_HDR) + 4;
				*((int *)(msg + 1)) = what_to_send;

				printf("[UT2] Info: Filling up task 1's mailbox!\r\n");
				while(send_msg(tsk1_tid, msg) == RTX_OK) {
					tsk1_mbx_cap++;
					*((int *)(msg + 1)) = (++what_to_send);
				}

				if (tsk1_mbx_cap > num_dequeues) {
					printf("[UT2] Passed: Filled up task 1's mailbox with more than %d messages!\r\n", num_dequeues);
					sflag++;

					for (i = 0; i < num_iterations; i++) {
                                                printf("[UT2] Info: Setting priority of task 1 back to medium!\r\n");

						if (tsk_set_prio(tsk1_tid, 150) == RTX_ERR) {
							printf("[UT2] Failed: Could not change priority of task 1 to medium!\r\n");
							break;
						}

						printf("[UT2] Info: Filling up task 1's mailbox again with %d more messages!\r\n", num_dequeues);

						lost_cap = 0;

						for (j = 0; j < num_dequeues; j++) {
							*((int *)(msg + 1)) = what_to_send;

							if (send_msg(tsk1_tid, msg) == RTX_ERR) {
								lost_cap = 1;
								break;
							} else {
								what_to_send++;
							}
						}

						if(lost_cap == 0) {
							printf("[UT2] Passed: Filled task1's mailbox again with %d messages!\r\n", num_dequeues);
							sflag++;
						} else {
							printf("[UT2] Failed: Task 1's mailbox got full with less than %d messages!\r\n", num_dequeues);
							break;
						}
					}
				} else {
					printf("[UT2] Failed: Could not send more than 4 messages to task 2 with huge capacity!\r\n");
				}
			} else {
				printf("[UT2] Failed: Could not allocate memory for messages!\r\n");
			}
		} else {
			printf("[UT2] Failed: Could not set priority of task 1 to lowest!\r\n");
		}
	} else {
		printf("[UT2] Failed: Could not find TID of task 1!\r\n");
	}

	printf("[UT2] Info: Exiting task 2!\r\n");
	tsk_exit();
}

#endif


// Additional Tests

#if TEST == 10
void utask1(void) {
	task_t utid01;
    RTX_TASK_INFO task_info2;

    SER_PutStr (0,"utask1: entering \n\r");

    /* do something */
    utid01 = tsk_get_tid();
    printf("111 OUTPUT OF GET TID: %d\n", utid01);
    int status = tsk_get_info(utid01, &task_info2);
    // print get info
    printf("printing get info for user task:\n");
    printf("task_entry: %p\n", task_info2.ptask);
    printf("k_stack_hi: 0x%x\n",task_info2.k_stack_hi);
    printf("u_stack_hi: 0x%x\n",task_info2.u_stack_hi);
    printf("k_stack_size: 0x%x\n", task_info2.k_stack_size);
    printf("u_stack_size: 0x%x\n", task_info2.u_stack_size);
    printf("tid: %d\n", task_info2.tid);
    printf("prio: %d\n", task_info2.prio);
    printf("state: %d\n", task_info2.state);
    printf("priv: %d\n", task_info2.priv);
    // user sets itself to lower priority -> goes back to kernal
    tsk_set_prio(utid01, 101);
    tsk_get_info(utid01, &task_info2);
    printf("\nnew u_task prio: %d\n", task_info2.prio);
	SER_PutStr (0,"utask1: exiting \n\r");

    /* terminating */
    tsk_exit();
}
#endif

#if TEST == 11
void utask1(void) {
    SER_PutStr (0,"utask1: entering \n\r");
    task_t utid2;
	tsk_create(&utid2, &utask2, 80, 0x200);
    tsk_yield();
	SER_PutStr (0,"utask1: exiting \n\r");
    /* terminating */
    tsk_exit();
}

void utask2(void) {
    SER_PutStr (0,"utask2: entering \n\r");

	tsk_yield();
	SER_PutStr (0,"utask2: exiting \n\r");

    /* terminating */
    tsk_exit();
}
#endif

#if TEST == 14
void utask1(void)
{
	SER_PutStr (0,"utask1: entering \n\r");
    tsk_yield();
	SER_PutStr (0,"utask1: exiting \n\r");
	tsk_exit();
}
void utask2(void)
{
	SER_PutStr (0,"utask2: entering \n\r");
    tsk_yield();
	SER_PutStr (0,"utask2: exiting \n\r");
	tsk_exit();
}
void utask3(void)
{
	SER_PutStr (0,"utask3: entering \n\r");
    tsk_yield();
	SER_PutStr (0,"utask3: exiting \n\r");
	tsk_exit();
}
void utask4(void)
{
	SER_PutStr (0,"utask4: entering \n\r");
    tsk_yield();
	SER_PutStr (0,"utask4: exiting \n\r");
	tsk_exit();
}
void utask5(void)
{
	SER_PutStr (0,"utask5: entering \n\r");
    tsk_yield();
	SER_PutStr (0,"utask5: exiting \n\r");
	tsk_exit();
}
#endif

#if TEST == 15
void utask1(void)
{
	SER_PutStr (0,"utask1: entering \n\r");
    tsk_yield();
	SER_PutStr (0,"utask1: exiting \n\r");
	tsk_exit();
}
#endif

#if TEST == 20
void utask1(void)
{
	void* bigbongarr[10];

	SER_PutStr (0,"utask1: entering \n\r");
    for (int i = 0; i < 10; i++)
    {
		SER_PutStr (0,"utask1: alloc \n\r");
		bigbongarr[i] = mem_alloc(100);
		print_mem();
        tsk_yield();
    }
	SER_PutStr (0,"utask1: exiting \n\r");

	for (int i = 0 ; i < 10 ; i++)
	{
		mem_dealloc(bigbongarr[i]);
	}
	print_mem();
	tsk_exit();

}
#endif

#if TEST == 22
void utask1(void)
{
	SER_PutStr (0,"utask1: exiting \n\r");
	tsk_exit();
}
void utask2(void)
{
	SER_PutStr (0,"utask2: exiting \n\r");
	tsk_exit();
}
void utask3(void)
{
	SER_PutStr (0,"utask3: exiting \n\r");
	tsk_exit();
}
void utask4(void)
{
	SER_PutStr (0,"utask4: exiting \n\r");
	tsk_exit();
}
void utask5(void)
{
	SER_PutStr (0,"utask5: exiting \n\r");
	tsk_exit();
}
void utask6(void)
{
	SER_PutStr (0,"utask6: exiting \n\r");
	tsk_exit();
}
void utask7(void)
{
	SER_PutStr (0,"utask7: exiting \n\r");
	tsk_exit();
}
#endif

#if TEST == 23
void utask1(void)
{
	SER_PutStr (0,"utask1: exiting \n\r");
	tsk_exit();
}
void utask2(void)
{
	SER_PutStr (0,"utask2: exiting \n\r");
	tsk_exit();
}
void utask3(void)
{
	SER_PutStr (0,"utask3: exiting \n\r");
	tsk_exit();
}
void utask4(void)
{
	SER_PutStr (0,"utask4: exiting \n\r");
	tsk_exit();
}
void utask5(void)
{
	SER_PutStr (0,"utask5: exiting \n\r");
	tsk_exit();
}
#endif

#if TEST == 24
void utask1(void)
{
	while(1)
	{
		SER_PutStr (0,"utask1: HAHAHAHAHAHAHAHAA I'M RUNNINGGGGGGGGGG \n\r");
	}
	tsk_exit();
}
#endif

#if TEST == 25
void utask1(void)
{
	void *p2 = mem_alloc(100);
	SER_PutStr (0,"utask1: allocated memory \n\r");

	task_t u_tid2;
    tsk_create(&u_tid2, &utask2, 30, 0x200);

	SER_PutStr (0,"utask1: created user task 2 \n\r");

	tsk_yield();

	while(1){
        mem_dealloc(p2);
		tsk_set_prio(utask1, 100);
		tsk_yield();
    }
}
void utask2(void)
{
	mem_alloc(1000);
	SER_PutStr (0,"utask2: allocated memory \n\r");
	tsk_set_prio(utask2, 100);

	tsk_yield();

	while(1){
        // teehee
    }
}
void utask3(void)
{
	SER_PutStr (0,"utask3: running \n\r");
	while(1){
        // teehee
    }
}
#endif

#if TEST == 30
void utask1(void)
{
	while(1){
		SER_PutStr (0,"utask1: attempting to set prio for kernel task \n\r");
		tsk_set_prio(ktask1, 10);
		SER_PutStr (0,"utask1: successfully set prio for kernel task \n\r");
    }
}
#endif

#if TEST == 33
void utask1(void)
{
	print_mem();
	while(1){
		SER_PutStr (0,"utask1: running with massive stack size \n\r");
    }
}
#endif

#if TEST == 34
void utask1(void)
{
	while(1){
		SER_PutStr (0,"utask1: running with passed NULL pointer \n\r");
	}
}
#endif

#if TEST == 35
void utask1(void)
{
	while(1){
		SER_PutStr (0,"utask1: attempting to dealloc for kernel task \n\r");
		tsk_set_prio(ktask1, 10);
		SER_PutStr (0,"utask1: successfully deallocated for kernel task \n\r");	}
}
#endif

#if TEST == 36
void utask1(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();
	printf("entering user task with tid = %d\n", tid);
	for (int i = 0 ; i < 3 ; i++)
	{
		tsk_create(&child_tid, &utask2, 102, (i+1)*0x122);
		printf("created task with tid = %d\n", child_tid);
	}
	printf("exiting task with tid = %d\n", tid);
	tsk_exit();
}

void utask2(void)
{
	task_t tid;
	tid = tsk_get_tid();
	printf("entering user task with tid = %d\n", tid);
	printf("exiting with tid = %d\n", tid);
	tsk_exit();
}
#endif

#if TEST == 37
void utask1(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();
	printf("entering user task with tid = %d\n", tid);

	printf("created task with tid = 1\n");
	int ret = tsk_create(&child_tid, &utask2, 101, 0x234);

	printf("1. setting prio of user task 2 from 101 to 99, should preempt\n");
	ret = tsk_set_prio(2 , 99);

	printf("3. setting prio of user task 2 from 99 to 98, should stay\n");
	ret = tsk_set_prio(2 , 98);

	printf("4. setting prio of user task 1 from 98 to 99, should preempt\n");
	ret = tsk_set_prio(1 , 99);

	printf("6. at end of task 1\n");
	tsk_exit();
}

void utask2(void)
{
	task_t tid;
	tid = tsk_get_tid();
	printf("entering user task with tid = %d\n", tid);

	printf("2. setting prio of user task 1 from 100 to 98, should preempt\n");
	int ret = tsk_set_prio(1, 98);

	printf("5. at end of task 2\n");
	tsk_exit();
}
#endif

#if TEST == 38
void utask1(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();
	printf("2. entering user task with tid = %d, create task with tid = 3\n", tid);
	int ret = tsk_create(&child_tid, &utask2, 98, 0x234);

	printf("15. exiting task with tid = %d\n", tid);
	tsk_exit();
}

void utask2(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	printf("3. entering user task with tid = %d, create task with tid = 4\n", tid);
	int ret = tsk_create(&child_tid, &utask3, 97, 0x234);

	printf("14. utask2 yielding\n");
	tsk_yield();

	printf("16. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask3(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	printf("4. entering user task with tid = %d, create task with tid = 5\n", tid);
	int ret = tsk_create(&child_tid, &utask4, 96, 0x234);

	printf("12. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask4(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	printf("5. entering user task with tid = %d, create task with tid = 6\n", tid);
	int ret = tsk_create(&child_tid, &utask5, 95, 0x234);

	printf("11. utask4 yielding\n");
	tsk_yield();

	printf("13. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask5(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	printf("6. entering user task with tid = %d, create task with tid = 7\n", tid);
	int ret = tsk_create(&child_tid, &utask6, 94, 0x234);

	printf("9. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask6(void)
{
	task_t tid;
	tid = tsk_get_tid();
	printf("7. entering user task with tid = %d\n", tid);

	printf("8. utask6 yielding\n");
	tsk_yield();
	tsk_set_prio(tid, 102);

	printf("10. exiting with tid = %d\n", tid);
	tsk_exit();
}
#endif

#if TEST == 39
void utask1(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	int ret = tsk_create(&child_tid, &utask2, 102, 0x234);
	printf("3. entering user task with tid = %d, create task with tid = %d\n", tid, child_tid);

	printf("4. exiting task with tid = %d\n", tid);
	tsk_exit();
}

void utask2(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	int ret = tsk_create(&child_tid, &utask3, 103, 0x234);
	printf("5. entering user task with tid = %d, create task with tid = %d\n", tid, child_tid);

	printf("6. utask2 yielding\n");
	tsk_yield();

	printf("7. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask3(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	int ret = tsk_create(&child_tid, &utask4, 104, 0x234);
	printf("8. entering user task with tid = %d, create task with tid = %d\n", tid, child_tid);

	printf("9. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask4(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	int ret = tsk_create(&child_tid, &utask5, 105, 0x234);
	printf("10. entering user task with tid = %d, create task with tid = %d\n", tid, child_tid);

	printf("11. utask4 yielding\n");
	tsk_yield();

	printf("12. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask5(void)
{
	task_t tid;
	task_t child_tid;
	tid = tsk_get_tid();

	int ret = tsk_create(&child_tid, &utask6, 106, 0x234);
	printf("13. entering user task with tid = %d, create task with tid = %d\n", tid, child_tid);

	printf("14. exiting with tid = %d\n", tid);
	tsk_exit();
}

void utask6(void)
{
	task_t tid;
	tid = tsk_get_tid();
	printf("15. entering user task with tid = %d\n", tid);

	printf("16. utask6 yielding\n");
	tsk_yield();

	printf("17. exiting with tid = %d\n", tid);
	tsk_exit();
}
#endif
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */

