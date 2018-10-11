/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/*****************************
 *
 * Include header files.
 *
 *****************************/

#include <stdbool.h>
#include <stdint.h>
#include <bl_common.h>
/****************************
 *
 * Define constants and types.
 *
 ****************************/


typedef struct {
    unsigned int   pattern_num;
    unsigned int   bit_num;
    unsigned int   pattern;
    unsigned int   addr;
    unsigned int   val;
} debug_array;



#define EMI_TEST_PATTERN1 0x0
#define EMI_TEST_PATTERN2 0xffffffff

#ifdef MT6261
#define ARCHI_WITH_EXTRAM
//#define ARCHI_WITH_INTRAM
#endif
//#define ARCHI_WITH_INTRAM
//#define ARCHI_WITH_EXTRAM
//#define ARCHI_WITH_SYSRAM



/********************************************
 *
 * Define external function prototype.
 *
 ********************************************/

#ifdef ARCHI_WITH_EXTRAM
#define EXT_RAM_TEST_LEN 0x10000
#endif

#ifdef ARCHI_WITH_INTRAM
#define INT_RAM_TEST_LEN 0x2000
unsigned int intram_test_buffer[INT_RAM_TEST_LEN / 4];
#endif

#ifdef ARCHI_WITH_SYSRAM
#define SYS_RAM_TEST_LEN 0x1000
unsigned int sysram_test_buffer[SYS_RAM_TEST_LEN / 4];
#endif

#define MAX_DEBUG_INFO 20
#define PATTERN_MAX 20

//#define NODE_LIMIT_MODE 0
debug_array debug_info[MAX_DEBUG_INFO];
unsigned int debug_node_counter;
unsigned int counter[PATTERN_MAX];
void debug_node_print(void);
/*************************************************************************
* FUNCTION
*
*  memory_btmt_test
*
* DESCRIPTION
*
*  This function runs basic memory test.
*
* PARAMETERS
*
* RETURNS
*
*************************************************************************/



void reset_debug_buffer(void)
{
    int i;
    debug_node_counter = 0;
    for (i = 0; i < MAX_DEBUG_INFO; i++) {
        debug_info[i].pattern_num = 0xFFFFFFFF;
        debug_info[i].bit_num = 0xFFFFFFFF;
        debug_info[i].pattern = 0xFFFFFFFF;
        debug_info[i].addr = 0xFFFFFFFF;
        debug_info[i].val = 0xFFFFFFFF;
    }
}


void flush_debug_buffer(void)
{
    debug_node_print();
    reset_debug_buffer();
}

void drain_debug_buffer(void)
{
    int i;
    int sum = 0;
    debug_node_print();
    reset_debug_buffer();
    for (i = 0; i < PATTERN_MAX; i++) {
        if (counter[i] != 0) {
            bl_print(LOG_DEBUG, "pattern %d, failed number %d\n\r", i, counter[i]);
            sum = sum + counter[i];
        }
    }
    bl_print(LOG_DEBUG, "total failed number: %d\n\r", sum);
    if (sum == 0) {
        bl_print(LOG_DEBUG, "SLT Mem test pattern pass!\n\r");
    } else {
        bl_print(LOG_DEBUG, "SLT Mem test pattern failed!\n\r");
    }
}

unsigned int add_node(unsigned int pattern_num, unsigned int bit_num, unsigned int pattern, volatile unsigned int *addr, unsigned int val)
{
    if (debug_node_counter >= MAX_DEBUG_INFO) {
        // bl_print(LOG_DEBUG, "flush node overfow debug_node_counter:%d, MAX_DEBUG_INFO %d\n\r",debug_node_counter,MAX_DEBUG_INFO);
        bl_print(LOG_DEBUG, "flush buffer...\n\r");
        flush_debug_buffer();
    }
    {
        debug_info[debug_node_counter].pattern_num = pattern_num;
        debug_info[debug_node_counter].bit_num = bit_num;
        debug_info[debug_node_counter].pattern = pattern;
        debug_info[debug_node_counter].addr = (unsigned int)addr;
        debug_info[debug_node_counter].val = val;
        debug_node_counter++;
        counter[pattern_num]++;
    }
    return 1;
}

void init_node(void)
{
    int i;
    debug_node_counter = 0;
    bl_print(LOG_DEBUG, "Init debug node\n\r");
    for (i = 0; i < PATTERN_MAX; i++) {
        counter[i] = 0;
    }
    for (i = 0; i < MAX_DEBUG_INFO; i++) {
        debug_info[i].pattern_num = 0xFFFFFFFF;
        debug_info[i].bit_num = 0xFFFFFFFF;
        debug_info[i].pattern = 0xFFFFFFFF;
        debug_info[i].addr = 0xFFFFFFFF;
        debug_info[i].val = 0xFFFFFFFF;
    }
}

void debug_node_print(void)
{
    int i = 0;
    for (i = 0; i < MAX_DEBUG_INFO; i++) {
        if (debug_info[i].bit_num == 0xFFFFFFFF) {
            break;
        }
        bl_print(LOG_DEBUG, "(%2d,%2d bit,0x%08x,&0x%08x,0x%08x) format:(pattern num,bit operation,test pattern,failed addr,failed val)\n\r", debug_info[i].pattern_num, \
                 debug_info[i].bit_num, debug_info[i].pattern, debug_info[i].addr, debug_info[i].val);
    }
    return;
}

bool memory_btmt_test(void)
{
    bool rst = true;
#if defined(ARCHI_WITH_INTRAM)
    unsigned int 	intram_test_addr = (unsigned int)intram_test_buffer;
    unsigned int 	intram_test_len = INT_RAM_TEST_LEN;
#endif /* ARCHI_WITH_INTRAM */

#if defined(ARCHI_WITH_EXTRAM)
    unsigned int    extram_test_addr = (unsigned int)0x0;
    unsigned int    extram_test_len = EXT_RAM_TEST_LEN;
#endif

#if defined(ARCHI_WITH_SYSRAM)
    unsigned int 	sysram_test_addr = (unsigned int)sysram_test_buffer;
    unsigned int 	sysram_test_len	= SYS_RAM_TEST_LEN;
#endif /* ARCHI_WITH_SYSRAM */

    init_node();
#if defined(ARCHI_WITH_INTRAM)
    if (-1 == ram_basic_test(intram_test_addr, intram_test_len)) {
        rst = false;
    }
#endif /* ARCHI_WITH_INTRAM */


#if defined(ARCHI_WITH_EXTRAM)
    bl_print(LOG_DEBUG, "Test Size 0x%x\n\r", EXT_RAM_TEST_LEN);
    if (-1 == ram_basic_test(extram_test_addr, extram_test_len)) {
        rst = false;
    }
#endif /* ARCHI_WITH_INTRAM */


#if defined(ARCHI_WITH_SYSRAM)
    if (-1 == ram_basic_test(sysram_test_addr, sysram_test_len)) {
        rst = false;
    }
#endif /* ARCHI_WITH_SYSRAM */

    drain_debug_buffer();
    return rst;
}




/*************************************************************************
* FUNCTION
*
*  MTK_RAM_Basic_Test
*
* DESCRIPTION
*
*  This function runs basic memory test.
*
* PARAMETERS
*
* RETURNS
*
*************************************************************************/


int ram_basic_test(unsigned int test_start_addr, unsigned int test_len)
{
    volatile unsigned char *MEM8_BASE = (volatile unsigned char *)test_start_addr;
    volatile unsigned short *MEM16_BASE	= (volatile unsigned short *)test_start_addr;
    volatile unsigned int *MEM32_BASE = (volatile unsigned int *)test_start_addr;

    unsigned char pattern8;
    unsigned short pattern16;
    unsigned int 	pattern32;

    unsigned int test_4B_size;
    unsigned int readback_value;

    int i, j;
    test_4B_size = test_len >> 2;

    /* Verify the tied bits (tied high) */
    for (i = 0; i < test_4B_size; i++) {
        MEM32_BASE[i] = 0;
    }

    for (i = 0; i < test_4B_size; i++) {

        if (MEM32_BASE[i] != 0) {
            add_node(1, 32, 0, &MEM32_BASE[i], MEM32_BASE[i]);
        } else {
            MEM32_BASE[i] = 0xffffffff;
        }
    }

    /* Verify the tied bits (tied low) */
#if 1
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0xffffffff) {
            MEM32_BASE[i] = 0x00;
        } else {
            add_node(2, 32, 0xffffffff, &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }

    /* Verify pattern 1 (0x00~0xff) */
    pattern8 = 0x00;
    for (i = 0; i < test_len; i++) {
        MEM8_BASE[i] = pattern8++;
    }

    pattern8 = 0x00;
    for (i = 0; i < test_len; i++) {
        if (MEM8_BASE[i] != pattern8++) {
            add_node(3, 8, (pattern8 - 1), (volatile unsigned int *)&MEM8_BASE[i], MEM8_BASE[i]);
        }
    }


    /* Verify pattern 2 (0x00~0xff) */
    pattern8 = 0x00;
    for (i = j =  0; i < test_len; i += 2, j++) {
        if (MEM8_BASE[i] == pattern8) {
            MEM16_BASE[j] = pattern8;
        }

        if (MEM16_BASE[j] != pattern8) {
            add_node(4, 16, pattern8, (volatile unsigned int *)&MEM16_BASE[i], MEM16_BASE[i]);
        }
        pattern8 += 2;
    }


    /* Verify pattern 3 (0x00~0xffff) */
    pattern16 = 0x00;
    for (i = 0; i < (test_len >> 1); i++) {
        MEM16_BASE[i] = pattern16++;
    }

    pattern16 = 0x00;
    for (i = 0; i < (test_len >> 1); i++) {
        if (MEM16_BASE[i] != pattern16++) {
            add_node(5, 16, (pattern16 - 1), (volatile unsigned int *)&MEM16_BASE[i], MEM16_BASE[i]);
        }
    }


    /* Verify pattern 4 (0x00~0xffffffff) */
    pattern32 = 0x00;
    for (i = 0; i < (test_len >> 2); i++) {
        MEM32_BASE[i] = pattern32++;
    }

    pattern32 = 0x00;
    for (i = 0; i < (test_len >> 2); i++) {
        if (MEM32_BASE[i] != pattern32++) {
            add_node(6, 32, (pattern32 - 1), &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /* Pattern 5: Filling memory range with 0x44332211 */
    for (i = 0; i < test_4B_size; i++) {
        MEM32_BASE[i] = 0x44332211;
    }

    /* Read Check then Fill Memory with a5a5a5a5 Pattern */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0x44332211) {
            MEM32_BASE[i] = 0xa5a5a5a5;
        } else {
            add_node(7, 32, 0x44332211, &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /* Read Check then Fill Memory with 00 Byte Pattern at offset 0h */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0xa5a5a5a5) {
            MEM8_BASE[i * 4] = 0x00;
        } else {
            add_node(8, 32, 0xa5a5a5a5 , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /* Read Check then Fill Memory with 00 Byte Pattern at offset 2h */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0xa5a5a500) {
            MEM8_BASE[i * 4 + 2] = 0x00;
        } else {
            add_node(9, 32, 0xa5a5a500 , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /* Read Check then Fill Memory with 00 Byte Pattern at offset 1h */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0xa500a500) {
            MEM8_BASE[i * 4 + 1] = 0x00;
        } else {
            add_node(10, 32, 0xa500a500 , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /* Read Check then Fill Memory with 00 Byte Pattern at offset 3h */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0xa5000000) {
            MEM8_BASE[i * 4 + 3] = 0x00;
        } else {
            add_node(11, 8, 0xa5000000 , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }

    /* Read Check then Fill Memory with ffff Word Pattern at offset 1h */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0x00000000) {
            MEM16_BASE[i * 2 + 1] = 0xffff;
        } else {
            add_node(12, 8, 0x00000000 , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /* Read Check then Fill Memory with ffff Word Pattern at offset 0h */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] == 0xffff0000) {
            MEM16_BASE[i * 2] = 0xffff;
        } else {
            add_node(13, 16, 0xffff0000  , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /* Read Check */
    for (i = 0; i < test_4B_size; i++) {
        if (MEM32_BASE[i] != 0xffffffff) {
            add_node(14, 16, 0xffffffff  , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


    /**************************
    * Additional verification
    **************************/

    /* stage 1 => write 0 */
    for (i = 0; i < test_4B_size; i++) {
        MEM32_BASE[i] = EMI_TEST_PATTERN1;
    }

    /* stage 2 => read 0, write 0xF */
    for (i = 0; i < test_4B_size; i++) {
        readback_value = MEM32_BASE[i];

        if (readback_value != EMI_TEST_PATTERN1) {
            add_node(15, 32, EMI_TEST_PATTERN1  , &MEM32_BASE[i], MEM32_BASE[i]);
        }

        MEM32_BASE[i] = EMI_TEST_PATTERN2;
    }

    /* stage 3 => read 0xF, write 0 */
    for (i = 0; i < test_4B_size; i++) {
        readback_value = MEM32_BASE[i];

        if (readback_value != EMI_TEST_PATTERN2) {
            add_node(16, 32, EMI_TEST_PATTERN2  , &MEM32_BASE[i], MEM32_BASE[i]);
        }

        MEM32_BASE[i] = EMI_TEST_PATTERN1;
    }


    /* stage 4 => read 0, write 0xF */
    for (i = 0; i < test_4B_size; i++) {
        readback_value = MEM32_BASE[i];

        if (readback_value != EMI_TEST_PATTERN1) {
            add_node(17, 32, EMI_TEST_PATTERN1  , &MEM32_BASE[i], MEM32_BASE[i]);
        }

        MEM32_BASE[i] = EMI_TEST_PATTERN2;
    }


    /* stage 5 => read 0xF, write 0 */
    for (i = 0; i < test_4B_size; i++) {
        readback_value = MEM32_BASE[i];

        if (readback_value != EMI_TEST_PATTERN2) {
            add_node(18, 32, EMI_TEST_PATTERN2  , &MEM32_BASE[i], MEM32_BASE[i]);
        }

        MEM32_BASE[i] = EMI_TEST_PATTERN1;
    }


    /* stage 6 => read 0 */
    for (i = 0; i < test_4B_size; i++) {
        readback_value = MEM32_BASE[i];

        if (readback_value != EMI_TEST_PATTERN1) {
            add_node(19, 32, EMI_TEST_PATTERN1  , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }

    /* stage 6 => read 0 */
    for (i = 0; i < test_4B_size; i++) {
        MEM32_BASE[i] = 0x0;
        MEM32_BASE[i] = 0x61E11E49;
        readback_value = MEM32_BASE[i];

        if (readback_value != 0x61E11E49) {
            add_node(20, 32, 0x61E11E49  , &MEM32_BASE[i], MEM32_BASE[i]);
        }
    }


#endif
    return 0;
}





