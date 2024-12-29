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
 * @file        ae_mem.c
 * @brief       memory lab auto-tester
 *
 * @version     V1.2021.01
 * @authors     Yiqing Huang
 * @date        2021 JAN
 *
 *****************************************************************************/

#include "rtx.h"
#include "Serial.h"
#include "printf.h"
#include "ae.h"
#include "../kernel/k_mem.h"


#if TEST == -1

int test_mem(void) {
	unsigned int start = timer_get_current_val(2);
	printf("NOTHING TO TEST.\r\n");
	unsigned int end = timer_get_current_val(2);

	// Clock counts down
	printf("This took %u us\r\n", start - end);

	printf("%x\r\n", (unsigned int) &Image$$ZI_DATA$$ZI$$Limit);
	return TRUE;
}
#endif

#if TEST_N == 10
BOOL test_coalescing_free_regions_using_count_extfrag() {
	void * p1 = k_mem_alloc(32);
	void * p2 = k_mem_alloc(32);

	unsigned int header_size = (unsigned int)p2 - (unsigned int)p1 - 32;

	void * p3 = k_mem_alloc(32);
	void * p4 = k_mem_alloc(32);
	void * p5 = k_mem_alloc(32);
	void * p6 = k_mem_alloc(32);
	void * p7 = k_mem_alloc(32);
	void * p8 = k_mem_alloc(32);
	void * p9 = k_mem_alloc(32);

	int size_33_plus_one_header = k_mem_count_extfrag(32 + header_size + 1);
	int size_97_plus_three_header = k_mem_count_extfrag(96 + 3*header_size + 1);

	if((size_33_plus_one_header!=0) || (size_97_plus_three_header!=0)) {
		printf("test_coalescing_free_regions_using_count_extfrag: 1. Either mem_alloc or mem_count_extfrag has failed.\r\n");
		k_mem_dealloc(p1);
		k_mem_dealloc(p2);
		k_mem_dealloc(p3);
		k_mem_dealloc(p4);
		k_mem_dealloc(p5);
		k_mem_dealloc(p6);
		k_mem_dealloc(p7);
		k_mem_dealloc(p8);
		k_mem_dealloc(p9);
		return FALSE;
	}

	k_mem_dealloc(p2);
	k_mem_dealloc(p4);
	k_mem_dealloc(p6);
	k_mem_dealloc(p8);

	size_33_plus_one_header = k_mem_count_extfrag(32 + header_size + 1);
	size_97_plus_three_header = k_mem_count_extfrag(96 + 3*header_size + 1);

	if((size_33_plus_one_header!=4) || (size_97_plus_three_header!=4)) {
		printf("test_coalescing_free_regions_using_count_extfrag: 2. Either mem_dealloc or coalescing has failed.\r\n");
		k_mem_dealloc(p1);
		k_mem_dealloc(p3);
		k_mem_dealloc(p5);
		k_mem_dealloc(p7);
		k_mem_dealloc(p9);
		return FALSE;
	}

	k_mem_dealloc(p3);
	k_mem_dealloc(p7);

	size_33_plus_one_header = k_mem_count_extfrag(32 + header_size + 1);
	size_97_plus_three_header = k_mem_count_extfrag(96 + 3*header_size + 1);

	if((size_33_plus_one_header!=0) || (size_97_plus_three_header!=2)) {
		printf("test_coalescing_free_regions_using_count_extfrag: 3. Either mem_dealloc or coalescing has failed.\r\n");
		k_mem_dealloc(p1);
		k_mem_dealloc(p5);
		k_mem_dealloc(p9);
		return FALSE;
	}

	k_mem_dealloc(p1);
	k_mem_dealloc(p5);
	k_mem_dealloc(p9);

	int size_289_plus_nine_header = k_mem_count_extfrag(288 + 9*header_size + 1);
	
	if(size_289_plus_nine_header!=0) {
		printf("test_coalescing_free_regions_using_count_extfrag: 4. Either mem_dealloc or coalescing has failed.\r\n");
		return FALSE;
	}

	return TRUE;
}


int test_mem(void) {

	int test_coalescing_free_regions_result = test_coalescing_free_regions_using_count_extfrag();

	return test_coalescing_free_regions_result;
}
#endif

#if TEST_N == 11

#define N 10

#define CODE_MEM_INIT -1
#define CODE_MEM_ALLOC -2
#define CODE_MEM_DEALLOC -3
#define CODE_HEAP_LEAKAGE_1 -4
#define CODE_HEAP_LEAKAGE_2 -5
#define CODE_SUCCESS 0

int heap_leakage_test() {

	char *p_old[N], *p_new[N];

	// Step 1: Allocate memory
	for (int i = 0; i < N; i++) {
		p_old[i] = (char*) k_mem_alloc(i * 256 + 255);

		// pointer to allocated memory should not be null
		// starting address of allocated memory should be four-byte aligned
		if (p_old[i] == NULL || ((unsigned int) p_old[0] & 3))
			return CODE_MEM_ALLOC;

		if (i > 0) {
			// adjacent allocated memory should not conflict
			if (p_old[i - 1] + 256 * i >= p_old[i])
				return CODE_MEM_ALLOC;
		}
	}

	// Step 2: De-allocate memory
	for (int i = 0; i < N; i++) {
		if (k_mem_dealloc(p_old[i]) == -1) {
			return CODE_MEM_DEALLOC;
		}
	}

	// Step 3: Memory Leakage
	for (int i = 0; i < N; i++) {
		p_new[i] = (char*) k_mem_alloc((N - i) * 256 - 1);

		// pointer to allocated memory should not be null
		// starting address of allocated memory should be four-byte aligned
		if (p_new[i] == NULL || ((unsigned int) p_new[0] & 3))
			return CODE_HEAP_LEAKAGE_1;

		if (i > 0) {
			// adjacent allocated memory should not conflict
			if (p_new[i - 1] + 256 * (N - i + 1) >= p_new[i])
				return CODE_HEAP_LEAKAGE_1;
		}
	}

	// the total occupied area in the re-allocation
	// should be the same as in the initial allocation
	if ((p_old[0] != p_new[0])
			|| (p_old[N - 1] + N * 256 != p_new[N - 1] + 256)) {
		return CODE_HEAP_LEAKAGE_2;
	}

	for (int i = 0; i < N; i++) {
		k_mem_dealloc(p_new[i]);
	}

	return CODE_SUCCESS;
}

int test_mem(void) {

	int result = heap_leakage_test();
	switch (result) {
	case CODE_MEM_INIT:
	case CODE_MEM_ALLOC:
		printf("Err: Basic allocation fails.\r\n");
		break;
	case CODE_MEM_DEALLOC:
		printf("Err: Basic deallocation fails.\r\n");
		break;
	case CODE_HEAP_LEAKAGE_1:
		printf("Err: Reallocation fails.\r\n");
		break;
	case CODE_HEAP_LEAKAGE_2:
		printf("Err: Heap memory is leaked.\r\n");
		break;
	case CODE_SUCCESS:
		printf("No heap leakage.\r\n");
		break;
	default:
		break;
	}

	return result == CODE_SUCCESS;
}
#endif

/*
 *===========================================================================
 *                             ADDIONAL TEST CASES
 *===========================================================================
 */

#if TEST_N == 69
int testing_something() {
	void*test1 = k_mem_alloc(1);
	void*test2 = k_mem_alloc(1);
	void*test3 = k_mem_alloc(1);
	void*test4 = k_mem_alloc(1);
	k_mem_dealloc(test3);
	k_mem_dealloc(test1);
	k_mem_dealloc(test2);
	return TRUE;
}
#endif


#if TEST_N == 1

#define NUM_BLOCKS (100)
#define BLOCK_SIZE (512)

/*  TEST CASE A
    Allocate a large number of same size blocks (e.g. blocks of 512) and deallocate starting 
    from the center ~ the unallocated regions should continually be coalesced into one giant unallocated 
    block (center unallocated block always grows by a constant amount each time) 
	Note: coalescing is done in K_mem_dealloc()
*/
int test_large_coalesce()
{
	void* mem_regions[NUM_BLOCKS];

	// Allocate the blocks of BLOCK_SIZE bytes
	for(int i = 0 ; i < NUM_BLOCKS ; i++)
	{
		mem_regions[i] = k_mem_alloc(BLOCK_SIZE);
		if(mem_regions[i] == NULL)
		{
			return FALSE;
		}
	}

	// Check total size in memory matches NUM_BLOCKS * (BLOCK_SIZE + 2*sizeof(header/footer)) bytes
//	void* start_addr = mem_regions[0] - sizeof(header_footer_t);
//	void* end_addr = mem_regions[NUM_BLOCKS-1] + BLOCK_SIZE + sizeof(header_footer_t);
//	uint32_t expected_size = NUM_BLOCKS * (BLOCK_SIZE + 2*sizeof(header_footer_t));
//	if((uint32_t)(end_addr - start_addr) != expected_size)
//	{
//		return FALSE;
//	}
	
	int pos = 0;
	// Start deallocating the blocks from the center
	for(int i = 0 ; i < NUM_BLOCKS ; i++)
	{
		if(i == 0)
		{
			pos = NUM_BLOCKS/2;
		}
		else
		{
			// (i%2) ? ODD : EVEN
			pos += (i%2) ? (-i) : (i);
			printf("pos:  %d\n", pos);
		}
		
		// Dealloc
		k_mem_dealloc(mem_regions[pos]);

		// Check if we have avoided fragmentation
		if (k_mem_count_extfrag(4) != 1)
		{
			return FALSE;
		}
	}
	return TRUE;
}
#endif

#if TEST_N == 2

#define NUM_BLOCKS (10)
#define INITIAL_BLOCK_SIZE (4)

/*  TEST CASE B
    Allocate blocks of increasing size where each block is double the size of the previous block. Deallocate
	every second block. Start allocating another set of blocks starting from the original size. This results
	in memory where there are 2 allocated blocks of the same size, then an empty block of the same size. The
	next two allocated blocks are quadruple the size of the prior, etc...
	Run the fragment counter and the expected result is that the fragmentation is a sum of the increasingly
	sized blocks
*/

int test_increasing_alloc_dealloc()
{
	void* mem_regions[NUM_BLOCKS];

	unsigned int curr_blk_size = INITIAL_BLOCK_SIZE;

	// Allocate blocks of doubling size
	for(int i = 0 ; i < NUM_BLOCKS ; i++)
	{
		mem_regions[i] = k_mem_alloc(curr_blk_size);
		if (mem_regions[i] == NULL)
		{
			return FALSE;
		}
		curr_blk_size *= 2;
	}

	curr_blk_size = INITIAL_BLOCK_SIZE*2;
	unsigned int frag_count = 0;

	// Deallocate every second block
	for(int i = 0 ; i < NUM_BLOCKS ; i+=2)
	{
		if (k_mem_dealloc(mem_regions[i]) == RTX_ERR)
		{
			return FALSE;
		}
		frag_count += curr_blk_size;
		curr_blk_size *= 4;
	}

	// Check if external fragmentated bytes add up to correct amount
//	if(k_mem_count_totalfree() != frag_count)
//	{
//		return FALSE;
//	}

	curr_blk_size = INITIAL_BLOCK_SIZE;
	frag_count = 0;

	// Allocate another set of blocks
	for(int i = 1 ; i < NUM_BLOCKS ; i+=2)
	{
		mem_regions[i] = k_mem_alloc(curr_blk_size);
		if (mem_regions[i] == NULL)
		{
			return FALSE;
		}
		curr_blk_size *= 4;
		frag_count += curr_blk_size;
	}

	// Check if external fragmentated bytes add up to correct amount
//	if(k_mem_count_totalfree() != frag_count)
//	{
//		return FALSE;
//	}

	return TRUE;
}
#endif

#if TEST_N == 3

#define NUM_BLOCKS	 	 (50)
#define BLOCK_SIZES_MAX  (128)
#define BLOCK_SIZES_MIN  (BLOCK_SIZES_MAX/2 + 1)
#define NUM_SETS         (BLOCK_SIZES_MAX/2)

/*  TEST CASE C
    Allocate NUM_BLOCKS blocks of different sizes that add up to a known size. Deallocate all of them. Repeat for
	a lot of repetitions. Compare all sets of end addresses to see that no memory is leaked. Block sizes should be
	rounded to the next power of 2 in bytes (e.g. 65-127 will be given 128 bytes).
*/
int test_block_size_rounding()
{
	void* mem_regions[NUM_BLOCKS];
	void* p_temp = NULL;

	// Outer loop is allocating a single set of NUM_BLOCKS blocks
	for(int i = 0 ; i < NUM_SETS ; i++)
	{
		// Inner loop will allocate a single block within a single block of size BLOCK_SIZES_MIN + j
		for(int j = 0 ; j < NUM_BLOCKS ; j++)
		{
			if (j == NUM_BLOCKS - 1)
			{
				mem_regions[i] = k_mem_alloc(BLOCK_SIZES_MIN + j);
				if (mem_regions[i] == NULL)
				{
					return FALSE;
				}
			}
			else
			{
				p_temp = k_mem_alloc(BLOCK_SIZES_MIN + j);
				if (p_temp == NULL)
				{
					return FALSE;
				}
			}
		}
		// Check if the pointers are all equal
		if (mem_regions[0] != mem_regions[i])
		{
			return FALSE;
		}
	}

	return TRUE;
}
#endif

/*
 *===========================================================================
 *                             END OF FILE
 *===========================================================================
 */
