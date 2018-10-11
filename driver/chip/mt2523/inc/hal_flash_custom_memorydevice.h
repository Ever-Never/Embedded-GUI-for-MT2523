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

#ifndef __CUSTOM_MEMORYDEVICE__
#define __CUSTOM_MEMORYDEVICE__

/*
 ****************************************************************************
 [README , VERY IMPORTANT NOTICE]
 --------------------------------
 After user configured this C header file, not only C compiler compile it but
 also auto-gen tool parse user's configure setting.
 Here are recommend configure convention to make both work fine.

 1. All configurations in this file form as #define MACRO_NAME MACRO_VALUE format.
    Note the #define must be the first non-space character of a line

 2. To disable the optional configurable item. Please use // before #define,
    for example: //#define MEMORY_DEVICE_TYPE

 3. Please don't use #if , #elif , #else , #endif conditional macro key word here.
    Such usage might cause compile result conflict with auto-gen tool parsing result.
    Auto-Gen tool will show error and stop.
    3.1.  any conditional keyword such as #if , #ifdef , #ifndef , #elif , #else detected.
          execpt this #ifndef __CUSTOM_MEMORYDEVICE__
    3.2.  any duplicated MACRO_NAME parsed. For example auto-gen tool got
          2nd MEMORY_DEVICE_TYPE macro value.
 ****************************************************************************
*/

/*
 ****************************************************************************
 Step 1: Specify memory device type and its complete part number
         Possible memory device type: NOR_RAM_MCP, LPSDRAM, NOR_LPSDRAM_MCP, SERIAL_FLASH
 ****************************************************************************
*/
#define MEMORY_DEVICE_TYPE    SERIAL_FLASH
/*
 ****************************************************************************
 Step 2: Specify part number for each device chosen when COMBO_MEMORY_SUPPORT is enabled
         Please note that this section will only be used when COMBO_MEMORY_SUPPORT is enabled,
         and will be ignored if COMBO_MEMORY_SUPPORT is disabled
 ****************************************************************************
*/
#define COMBO_MEM_ENTRY_COUNT 2

#define COMBO_MEM01_CS0_PART_NUMBER MX25R8035F_0
#define COMBO_MEM01_CS1_PART_NUMBER MX25R8035F_0
#define COMBO_MEM02_CS0_PART_NUMBER W25Q80EWSNIG
#define COMBO_MEM02_CS1_PART_NUMBER W25Q80EWSNIG

/*
 ****************************************************************************
 Step 3: Define the clock frequency of EMI and SFI

         By default, the highest EMI speed that can be supported by memory will be used
         (define EMI_CLK to be DEFAULT).
         By specifying EMI_CLK to be ¡§104M¡¨, ¡§52M¡¨, ¡§166M¡¨, or ¡§200M¡¨ can force EMI/memory to be configured
         running at 104MHz, 52MHz, 166MHz, or 200MHz for special purpose.
         Please note that this feature is only available on MT6253D and MT6276! All other chip must set to
         DEFAULT configuration.
         Available EMI_CLK configurations are:
         MT6253D: 104M, 52M, DEFAULT
         MT6276: 166M, 200M, DEFAULT
         other: DEFAULT

         For SFI, the highest SFI speed that can be supported by memory will be used by default
         (define SFI_CLK to be DEFAULT)
         By specifying SFI_CLK to be "130M", "104M" or "78M" can force SFI to be configured running
         at 130MHz, 104MHz or78MHz for special purpose.
         Please note that this feature is only available for platforms which support Serial Flash!
 ****************************************************************************
*/
#define SFI_CLK                 DEFAULT

/*
 ****************************************************************************
 Step 4: Specify SF related Features
 ****************************************************************************
*/

/*
  # Description:
  #   This feature option notifies EMI gen to generate SIP serial flash setting.After set this value other than NONE, need to set COMBO_MEMORY_SUPPORT to TRUE also.
  # Option Values:
  #   NONE: No serial flash SIP inside.
  #   8M_BITS: SIP 8Mb serial flash
  #   16M_BITS: SIP 16Mb serial flash
  #   32M_BITS: SIP 32Mb serial flash
*/
#define SIP_SERIAL_FLASH_SIZE 32M_BITS

/*
  # Description:
  #   This feature option below are default enabled
*/
#define __SERIAL_FLASH_EN__

#define __COMBO_MEMORY_SUPPORT__

#define __NOR_FULL_DRIVER__

#define __SINGLE_BANK_NOR_FLASH_SUPPORT__


#endif /* __CUSTOM_MEMORYDEVICE__ */
