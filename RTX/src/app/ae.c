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
extern void kcd_task(void);

/**************************************************************************//**
 * @brief   	ae_init
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 * @param[out]	task_info boot-time tasks struct array AE writes to
 *
 *****************************************************************************/

int ae_init(RTX_SYS_INFO *sys_info, RTX_TASK_INFO *task_info, int num_tasks) {
    if (ae_set_sys_info(sys_info) != RTX_OK) {
        return RTX_ERR;
    }

    ae_set_task_info(task_info, num_tasks);
    return RTX_OK;
}

/**************************************************************************//**
 * @brief       fill the sys_info struct with system configuration info.
 * @return		RTX_OK on success and RTX_ERR on failure
 * @param[out]  sys_info system initialization struct AE writes to
 *
 *****************************************************************************/
int ae_set_sys_info(RTX_SYS_INFO *sys_info) {
    if (sys_info == NULL) {
        return RTX_ERR;
    }

    // Scheduling sys info set up, only do DEFAULT in lab2
    sys_info->sched = DEFAULT;

    return RTX_OK;
}

/**************************************************************************//**
 * @brief       fill the tasks array with information
 * @param[out]  tasks 		An array of RTX_TASK_INFO elements to write to
 * @param[in]	num_tasks	The length of tasks array
 * @return		None
 *****************************************************************************/

void ae_set_task_info(RTX_TASK_INFO *tasks, int num_tasks) {

    if (tasks == NULL) {
    	printf("[ERROR] RTX_TASK_INFO undefined\n\r");
        return;
    }

#if TEST == 0
    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);

	tasks[1].ptask = &ktask2;
	tasks[1].priv  = 1;
	tasks[1].prio  = 100;
	tasks[1].k_stack_size = (0x200);

	tasks[2].ptask = &utask2;
	tasks[2].priv  = 0;
	tasks[2].prio  = 100;
	tasks[2].k_stack_size = (0x200);
//	tasks[2].u_stack_size = 0x200;
#endif


#if TEST == 1

    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_01!\r\n");
    printf("Info: Initializing system with a single user task!\r\n");

    tasks[0].prio = HIGH;
	tasks[0].priv = 0;
	tasks[0].ptask = &utask1;
	tasks[0].k_stack_size = 0x200;
	tasks[0].u_stack_size = 0x200;

#endif


#if TEST == 2

    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_02!\r\n");
    printf("Info: Initializing system with three user task and the KCD task!\r\n");
    printf("Info: UT1 (M), UT2 (L), KCD (H), UT4 (L)!\r\n");

    tasks[0].prio = 150;
	tasks[0].priv = 0;
	tasks[0].ptask = &utask1;
	tasks[0].k_stack_size = 0x200;
	tasks[0].u_stack_size = 0x200;

    tasks[1].prio = 175;
	tasks[1].priv = 0;
	tasks[1].ptask = &utask2;
	tasks[1].k_stack_size = 0x200;
	tasks[1].u_stack_size = 0x200;

	tasks[2].prio = 100;
	tasks[2].priv = 0;
	tasks[2].ptask = &kcd_task;
	tasks[2].k_stack_size = 0x200;
	tasks[2].u_stack_size = 0x200;

	tasks[3].prio = 200;
	tasks[3].priv = 0;
	tasks[3].ptask = &utask3;
	tasks[3].k_stack_size = 0x200;
	tasks[3].u_stack_size = 0x200;

#endif

#if TEST == 3

    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_03!\r\n");
    printf("Info: Initializing system with two user tasks (M and L priorities)!\r\n");

    tasks[0].prio = 150;
	tasks[0].priv = 0;
	tasks[0].ptask = &utask1;
	tasks[0].k_stack_size = 0x200;
	tasks[0].u_stack_size = 0x200;

    tasks[1].prio = 175;
	tasks[1].priv = 0;
	tasks[1].ptask = &utask2;
	tasks[1].k_stack_size = 0x200;
	tasks[1].u_stack_size = 0x200;

#endif


// Additional Tests

// ST0
#if TEST == 10
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_10!\r\n");
    printf("Info: Initializing system with 1 kernel task\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
	tasks[0].u_stack_size = 0x999;

    tasks[1].ptask = &ktask1;
    tasks[1].priv  = 0;
    tasks[1].prio  = 200;
	tasks[1].k_stack_size = (0x200);
	tasks[1].u_stack_size = (0x777);

#endif

// ST1
#if TEST == 11
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_11!\r\n");
    printf("Info: Initializing system with 1 kernel task\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
#endif

// ST2
#if TEST == 12
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_12!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

	tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
#endif

// ST3
#if TEST == 13
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_13!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

	tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
#endif

// ST4
#if TEST == 14
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_14!\r\n");
    printf("Info: Initializing system with 5 kernel tasks and 5 user tasks!\r\n");

    // Kernel Tasks
	tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &ktask2;
    tasks[1].priv  = 1;
    tasks[1].prio  = 100;
	tasks[1].k_stack_size = (0x200);

    tasks[2].ptask = &ktask3;
    tasks[2].priv  = 1;
    tasks[2].prio  = 100;
	tasks[2].k_stack_size = (0x200);

    tasks[3].ptask = &ktask4;
    tasks[3].priv  = 1;
    tasks[3].prio  = 100;
	tasks[3].k_stack_size = (0x200);

    tasks[4].ptask = &ktask5;
    tasks[4].priv  = 1;
    tasks[4].prio  = 100;
	tasks[4].k_stack_size = (0x200);

    // User Tasks
    tasks[5].ptask = &utask1;
    tasks[5].priv  = 0;
    tasks[5].prio  = 95;
	tasks[5].k_stack_size = (0x200);
	tasks[5].u_stack_size = (0x200);


    tasks[6].ptask = &utask2;
    tasks[6].priv  = 0;
    tasks[6].prio  = 94;
	tasks[6].k_stack_size = (0x200);
	tasks[6].u_stack_size = (0x200);


    tasks[7].ptask = &utask3;
    tasks[7].priv  = 0;
    tasks[7].prio  = 93;
	tasks[7].k_stack_size = (0x200);
	tasks[7].u_stack_size = (0x200);


    tasks[8].ptask = &utask4;
    tasks[8].priv  = 0;
    tasks[8].prio  = 92;
	tasks[8].k_stack_size = (0x200);
	tasks[8].u_stack_size = (0x200);


    tasks[9].ptask = &utask5;
    tasks[9].priv  = 0;
    tasks[9].prio  = 91;
	tasks[9].k_stack_size = (0x200);
	tasks[9].u_stack_size = (0x200);


    printf("num_tasks: %d\n", num_tasks);
#endif

// ST5
#if TEST == 15 // NOT SURE HOW TO CHECK IF EVERYTHING IS FREED 
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_15!\r\n");
    printf("Info: Initializing system with 1 kernel tasks and 1 user tasks!\r\n");

	tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &utask1;
    tasks[1].priv  = 0;
    tasks[1].prio  = 100;
	tasks[1].k_stack_size = (0x200);
	tasks[1].u_stack_size = (0x200);
#endif

// MT0
#if TEST == 20
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_20!\r\n");
    printf("Info: Initializing system with 1 kernel tasks and 1 user tasks!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 50;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &utask1;
    tasks[1].priv  = 0;
    tasks[1].prio  = 50;
	tasks[1].k_stack_size = (0x200);
	tasks[1].u_stack_size = (0x201);

#endif

// MT1
#if TEST == 21
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_21!\r\n");
    printf("Info: Initializing system with 5 kernel tasks!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 1;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &ktask2;
    tasks[1].priv  = 1;
    tasks[1].prio  = 2;
	tasks[1].k_stack_size = (0x200);

    tasks[2].ptask = &ktask3;
    tasks[2].priv  = 1;
    tasks[2].prio  = 3;
	tasks[2].k_stack_size = (0x200);

    tasks[3].ptask = &ktask4;
    tasks[3].priv  = 1;
    tasks[3].prio  = 4;
	tasks[3].k_stack_size = (0x200);

    tasks[4].ptask = &ktask5;
    tasks[4].priv  = 1;
    tasks[4].prio  = 5;
	tasks[4].k_stack_size = (0x200);
#endif

// MT2
#if TEST == 22
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_22!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 10;
	tasks[0].k_stack_size = (0x200);

#endif

// MT3
#if TEST == 23
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_23!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 10;
	tasks[0].k_stack_size = (0x200);
#endif

// MT4
#if TEST == 24
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_24!\r\n");
    printf("Info: Initializing system with 2 kernel tasks and 1 user task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 50;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &ktask2;
    tasks[1].priv  = 1;
    tasks[1].prio  = 50;
	tasks[1].k_stack_size = (0x200);

    tasks[2].ptask = &utask1;
    tasks[2].priv  = 0;
    tasks[2].prio  = 100;
	tasks[2].k_stack_size = (0x200);
    tasks[2].u_stack_size = (0x200);
#endif

// MT5
#if TEST == 25
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_25!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 50;
	tasks[0].k_stack_size = (0x200);
#endif


#if TEST == 26
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_26!\r\n");
    printf("Info: Initializing system with 10 user tasks!\r\n");

    // Explodes
    tasks[0].ptask = &utask1;
    tasks[0].priv  = 0;
    tasks[0].prio  = 50;
	tasks[0].k_stack_size = (0x200);
	tasks[0].u_stack_size = (0x225);

    tasks[1].ptask = &utask2;
    tasks[1].priv  = 0;
    tasks[1].prio  = 60;
	tasks[1].k_stack_size = (0x200);
	tasks[1].u_stack_size = (0x667);

    tasks[2].ptask = &utask1;
    tasks[2].priv  = 0;
    tasks[2].prio  = 50;
	tasks[2].k_stack_size = (0x200);
	tasks[2].u_stack_size = (0x225);

    tasks[3].ptask = &utask2;
    tasks[3].priv  = 0;
    tasks[3].prio  = 60;
	tasks[3].k_stack_size = (0x200);
	tasks[3].u_stack_size = (0x667);

    tasks[4].ptask = &utask1;
    tasks[4].priv  = 0;
    tasks[4].prio  = 50;
	tasks[4].k_stack_size = (0x200);
	tasks[4].u_stack_size = (0x225);

    tasks[5].ptask = &utask2;
    tasks[5].priv  = 0;
    tasks[5].prio  = 60;
	tasks[5].k_stack_size = (0x200);
	tasks[5].u_stack_size = (0x667);

    tasks[6].ptask = &utask1;
    tasks[6].priv  = 0;
    tasks[6].prio  = 50;
	tasks[6].k_stack_size = (0x200);
	tasks[6].u_stack_size = (0x225);

    tasks[7].ptask = &utask2;
    tasks[7].priv  = 0;
    tasks[7].prio  = 60;
	tasks[7].k_stack_size = (0x200);
	tasks[7].u_stack_size = (0x667);

    tasks[8].ptask = &utask1;
    tasks[8].priv  = 0;
    tasks[8].prio  = 50;
	tasks[8].k_stack_size = (0x200);
	tasks[8].u_stack_size = (0x225);

    tasks[9].ptask = &utask2;
    tasks[9].priv  = 0;
    tasks[9].prio  = 60;
	tasks[9].k_stack_size = (0x200);
	tasks[9].u_stack_size = (0x667);
#endif

#if TEST == 27
	printf("============================================\r\n");
	printf("============================================\r\n");
	printf("Info: Starting T_27!\r\n");
//	printf("Info: Initializing system with 1 kernel tasks\r\n");

	for (int i = 0 ; i < 159 ; i ++)
	{
	    tasks[i].ptask = &ktask1;
	    tasks[i].priv  = 0;
	    tasks[i].prio  = 200;
		tasks[i].k_stack_size = (0x200);
		tasks[i].u_stack_size = (0x667+7*i);
	}
#endif

// ET0
#if TEST == 30
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_30!\r\n");
    printf("Info: Initializing system with 1 kernel tasks and 1 user tasks!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &utask1;
    tasks[1].priv  = 0;
    tasks[1].prio  = 50;
	tasks[1].k_stack_size = (0x200);
    tasks[1].u_stack_size = (0x200);
#endif

// ET1
#if TEST == 31
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_31!\r\n");
    printf("Info: Initializing system with 2 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = NULL;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &ktask1;
    tasks[1].priv  = 1;
    tasks[1].prio  = -1;
	tasks[1].k_stack_size = (0x200);

    tasks[1].ptask = &ktask2;
    tasks[1].priv  = 1;
    tasks[1].prio  = 10000000;
	tasks[1].k_stack_size = (0x200);
#endif

// ET2
#if TEST == 32
// Not really possible to test
#endif

// ET3
#if TEST == 33
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_33!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 50;
	tasks[0].k_stack_size = (0x200);
#endif

// ET4
#if TEST == 34
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_34!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 50;
	tasks[0].k_stack_size = (0x200);
#endif

// ET5
#if TEST == 35
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_35!\r\n");
    printf("Info: Initializing system with 1 kernel tasks and 1 user tasks!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);

    tasks[1].ptask = &utask1;
    tasks[1].priv  = 0;
    tasks[1].prio  = 50;
	tasks[1].k_stack_size = (0x200);
    tasks[1].u_stack_size = (0x200);
#endif

#if TEST == 36
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_36!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
#endif

#if TEST == 37
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_37!\r\n");
    printf("Info: Initializing system with 1 user task!\r\n");

    tasks[0].ptask = &utask1;
    tasks[0].priv  = 0;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
	tasks[0].u_stack_size = (0x542);
#endif

#if TEST == 38
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_38!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
#endif

#if TEST == 39
    printf("============================================\r\n");
    printf("============================================\r\n");
    printf("Info: Starting T_38!\r\n");
    printf("Info: Initializing system with 1 kernel task!\r\n");

    tasks[0].ptask = &ktask1;
    tasks[0].priv  = 1;
    tasks[0].prio  = 100;
	tasks[0].k_stack_size = (0x200);
#endif
}


/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
