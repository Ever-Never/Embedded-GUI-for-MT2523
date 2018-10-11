/*
** $Id: @(#) hal_pse_lib_rom.h $
*/

/*! \file   hal_pse_lib_rom.h
    \brief  This file export the prototype of PSE library function calls.

    Common PSE library function calls are exported in this file.

*/
/*******************************************************************************
* Copyright (c) 2012 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
********************************************************************************
*/

/*******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
********************************************************************************
*/

/*
** $Log: hal_pse_lib_rom.h $
**
** 04 10 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] Update PSE code to support share buffer new design change.
**
** 04 03 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [mt6630] [pse] add WTBL&BCN content default values as PSE init procedure
**
** 03 06 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] 
** 1. PSE module provide related lib function for WTBL and INIT usage.
** 2. Fix Emulation error for Interrupt relaed cases.
** 3. Add new PSE emulation items for enhancing coverage.
**
** 02 19 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [mt6630] follow indirect reference method to add PSE related ROM API
** for RAM code reference.
**
** 02 08 2013 pat.lu
** [BORA00002364] [WLAN] RX FPGA Emulation Verification Modification
** .
** Update RX emul code
** Fix dbg_uart_putbytes declation in wifi_rom_api_vctr.h
** Add some PSE API to wifi_rom_api_vctr.h
** Enable including of wifi_rom_api_vctr.h in precomp_emu.h
**
** 01 21 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] base on 20130102 pse datasheet to support page size 128 bytes 
** and all emulation cases in wifi ram code test
**
** 12 20 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE]  Add PSE rom base function call to table g_apfnFixedAddrRomApi for ram code refer.
**
** 12 20 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] Base on MT6630 PSE FPGA release on 2012.12.17 to sync Lib, HAL, and emulation items in firmware
**
** 11 30 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** 11 30 2012 wayne.guo
** [MT6630] Base on 2012.11.28 released PSE FPGA features to update PSE firmware code
** 
** 10 22 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** 10 22 2012 wayne.guo
** [MT6630] Add PSE ROM code support
** 
**
*/

#ifndef _HAL_PSE_LIB_ROM_H
#define _HAL_PSE_LIB_ROM_H

/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/

/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/

/*******************************************************************************
*                   F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*
========================================================================
Routine Description:
    PsePageIndexGet: This routine is used to get Page Index after init.

Arguments:
                   VOID

Return Value:
                   UINT_32

Note:
========================================================================
*/

UINT_32
PsePageIndexGet (
    VOID
    );


/*
========================================================================
Routine Description:
    PsegGetWtbl2to4Entry: This routine is used to get WTBL2, WTBL3 and  WTBL4 related FID, EID and VM address

Arguments:
					IN  UINT_8 		ucWlanIdx		: WTBL id
					
					OUT PUINT_16 	pu2Wtbl2Fid		: Fid of WTBL2
					OUT PUINT_8    	pucWtbl2Eid		: Eid of WTBL2
					OUT VOID**    	ppWtbl2VmAddr	: VM of WTBL2
					
					OUT PUINT_16    pu2Wtbl3Fid		: Fid of WTBL3
					OUT PUINT_8     pucWtbl3Eid		: Eid of WTBL3
					OUT VOID**     	ppWtbl3VmAddr	: VM of WTBL3
					
					OUT PUINT_16    pu2Wtbl4Fid		: Fid of WTBL4
					OUT PUINT_8     pucWtbl4Eid		: Eid of WTBL4
					OUT VOID**     	ppWtbl4VmAddr	: VM of WTBL4

Return Value:
                	OUT BOOLEAN   TRUE/FALSE

Note:
========================================================================
*/

extern OUT BOOLEAN
(*PsegGetWtbl2to4Entry)(
	IN  UINT_8 		ucWlanIdx,
	
	OUT PUINT_16 	pu2Wtbl2Fid,
	OUT PUINT_8    	pucWtbl2Eid,
	OUT VOID**    	ppWtbl2VmAddr,
	
	OUT PUINT_16    pu2Wtbl3Fid,
	OUT PUINT_8     pucWtbl3Eid,
	OUT VOID**     	ppWtbl3VmAddr,
	
	OUT PUINT_16    pu2Wtbl4Fid,
	OUT PUINT_8     pucWtbl4Eid,
	OUT VOID**     	ppWtbl4VmAddr	
	);


/*
========================================================================
Routine Description:
    PseMaxAllocFrameSizeLimit: the MAX size for CPU Interface to allocate PSU Buffer.

Arguments:
                   VOID
Return Value:
                   OUT UINT_16  the Max Size Limitation in Bytes

Note:
========================================================================
*/

OUT UINT_16
PseMaxAllocFrameSizeLimit(
    VOID
    );
/*
========================================================================
Routine Description:
    PseFrameIDtoVmAddr: to transfer the Frame ID to Virtual address

Arguments:
                   IN UINT_16 u2FrameID : Frame ID
Return Value:
                   OUT VOID *
 
Note:
========================================================================
*/
extern OUT VOID *
(*PseFrameIDtoVmAddr) (
    IN UINT_16 u2FrameID
    );
/*
========================================================================
Routine Description:
    PseVmAddrtoFrameID: to transfer Virtual address to Frame ID

Arguments:
                   IN VOID * pu2VmAddr: the address of frame ID
Return Value:
                   Frame ID.

Note:
======================================================================== 
*/

OUT UINT_16
PseVmAddrtoFrameID (
    IN VOID * pu2VmAddr
    );
/*
========================================================================
Routine Description:
    PseAllocFrameBuf: base on the byte count required from user to allocate buffer from PSE and return the Frame ID

Arguments:
                   IN UINT_16  u2CifFrameByte   : number of bytes required, must be smaller than VM_FRAME_MAX_LEN

Return Value:
                   OUT UINT_16  return the Fid

Note:
========================================================================
*/

OUT UINT_16
PseAllocFrameBuf (
    IN UINT_16  u2CifFrameByte
    );
/*
========================================================================
Routine Description:
    PseFreeFrameBuf: Free Frame buf to P#3_Q#8 by Frame ID

Arguments:
                   IN UINT_16 u2FrameID : the frame id to be free to free queue

Return Value:
                   VOID

Note:
========================================================================
*/

OUT VOID
PseFreeFrameBuf (
    IN UINT_16 u2FrameID
    );
/*
========================================================================
Routine Description:
    PseFreeQueueUsableCntNum: to read the number of pages usable to be dequeue from Cif

Arguments:
                   VOID

Return Value:
                   OUT UINT_16 : number of page usable to be dequeue from Cif

Note:
========================================================================
*/
extern OUT UINT_16
(*PseFreeQueueUsableCntNum) (
    VOID
    );

/*
========================================================================
Routine Description:
    PseFreeQueuePoolPageCnt: to read the number of pages in free queue

Arguments:
                   VOID

Return Value:
                   OUT UINT_16 : number of page in free queue

Note:
========================================================================
*/
OUT UINT_16
PseFreeQueuePoolPageCnt (
    VOID
    );    
#ifdef IOT_PROCESS_PORTING

/*
========================================================================
Routine Description:
    PseCheckFidFreed:    This routine is used to check if a Fid has already beed freed

Arguments:
			    IN BOOLEAN     	enable/disable the setting

Return Value:
                OUT BOOLEAN     TRUE/FALSE, if fid freed return TRUE, otherwise return FALSE.

Note: 
========================================================================
*/

OUT BOOLEAN
PseCheckFidFreed(
    IN UINT_16     	u2FrameID
 	);

void PseLibInit(void);

#endif/*IOT_PROCESS_PORTING*/

#endif  /* _HAL_PSE_LIB_ROM_H */
