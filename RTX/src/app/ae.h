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

#ifndef AE_
#define AE_

#include "Serial.h"
#include "printf.h"
#include "rtx.h"
#include "ae_priv_tasks.h"
#include "ae_usr_tasks.h"

/*
 *===========================================================================
 *                            DEFINES
 *===========================================================================
 */

extern task_t utid1;
extern task_t utid2;
extern task_t utid3;
extern task_t utid4;
extern task_t utid5;
extern task_t utid6;
extern task_t utid7;

extern task_t ktid1;
extern task_t ktid2;
extern task_t ktid3;
extern task_t ktid4;
extern task_t ktid5;
extern task_t ktid6;
extern task_t ktid7;


#define TEST 3

#if TEST == 0
	#define BOOT_TASKS 3
#endif

#if TEST == 1
	#define BOOT_TASKS 1
#endif

#if TEST == 2
	#define BOOT_TASKS 4
#endif

#if TEST == 3
	#define BOOT_TASKS 2
#endif

// Additional Tests

#if TEST == 10
	#define BOOT_TASKS 2
#endif

#if TEST == 11
	#define BOOT_TASKS 1
#endif

#if TEST == 12
	#define BOOT_TASKS 1
#endif

#if TEST == 13
	#define BOOT_TASKS 1
#endif

#if TEST == 14
	#define BOOT_TASKS 10
#endif

#if TEST == 15
	#define BOOT_TASKS 2
#endif

#if TEST == 20
	#define BOOT_TASKS 2
#endif

#if TEST == 21
	#define BOOT_TASKS 5
#endif

#if TEST == 22
	#define BOOT_TASKS 1
#endif

#if TEST == 23
	#define BOOT_TASKS 1
#endif

#if TEST == 24
	#define BOOT_TASKS 3
#endif

#if TEST == 25
	#define BOOT_TASKS 1
#endif

#if TEST == 26
	#define BOOT_TASKS 10
#endif

#if TEST == 27
	#define BOOT_TASKS 159
#endif

#if TEST == 30
	#define BOOT_TASKS 2
#endif

#if TEST == 31
	#define BOOT_TASKS 1
#endif

#if TEST == 33
	#define BOOT_TASKS 1
#endif

#if TEST == 34
	#define BOOT_TASKS 1
#endif

#if TEST == 35
	#define BOOT_TASKS 2
#endif

#if TEST == 36
	#define BOOT_TASKS 1
#endif

#if TEST == 37
	#define BOOT_TASKS 1
#endif

#if TEST == 38
	#define BOOT_TASKS 1
#endif

#if TEST == 39
	#define BOOT_TASKS 1
#endif

/*
 *===========================================================================
 *                            FUNCTION PROTOTYPES
 *===========================================================================
 */

int  ae_init          (RTX_SYS_INFO *sys_info, \
                       RTX_TASK_INFO *task_info, int num_tasks);
int  ae_set_sys_info  (RTX_SYS_INFO *sys_info);
void ae_set_task_info (RTX_TASK_INFO *tasks, int num_tasks);
int  ae_start(void);

int  test_mem(void);

#define TEST_N 0

#if TEST_N == 1
	int test_large_coalesce(void);
#endif
#if TEST_N == 2
	int test_increasing_alloc_dealloc(void);
#endif
#if TEST_N == 3
	int test_block_size_rounding(void);
#endif

BOOL test_coalescing_free_regions_using_count_extfrag(void);


#endif // ! AE_
/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */


