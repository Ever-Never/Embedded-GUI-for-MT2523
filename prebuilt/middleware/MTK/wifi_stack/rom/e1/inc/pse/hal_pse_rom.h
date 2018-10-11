/*
** $Id: @(#) hal_pse_rom.h $
*/

/*! \file   hal_pse_rom.h
    \brief  This file includes the prototype of PSE HAL function calls.

    This file includes the prototype of PSE HAL function calls.

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
** $Log: hal_pse_rom.h $
**
** 10 17 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [MT7636] [PSE] support disable/enable pse_hif_ffa_delta_token and pse_hif_ffa_token_vld signaling
**
** 06 21 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] modify Fid free or not free check function to prevent assert case.
**
** 04 10 2013 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] [PSE] Update PSE code to support share buffer new design change.
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
** [MT6630] [PSE] Base on MT6630 PSE FPGA release on 2012.12.17 to sync Lib, HAL, and emulation items in firmware
**
** 11 30 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** 11 30 2012 wayne.guo
** [MT6630] Base on 2012.11.28 released PSE FPGA features to update PSE firmware code
** 
** 10 24 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** [MT6630] Base on MT6630 PSE datasheet release on date 2012.10.23 to add debug port feature support.
** 
** 10 22 2012 wayne.guo
** [BORA00002164] MT6630 unified MAC
** 10 22 2012 wayne.guo
** [MT6630] Add PSE ROM code support
** 
**/

#ifndef _HAL_PSE_ROM_H
#define _HAL_PSE_ROM_H

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

/*
========================================================================
Routine Description:
    halPsePageIdxToPageSize: to get Page size by Page size_idx

Arguments:
    IN UINT_32 u4PageIdx

Return Value:
    page size

Note:   No sanity check
========================================================================
*/

extern OUT UINT_32
(*halPsePageIdxToPageSize) (
    IN UINT_32 u4PageIdx
    );

/*
========================================================================
Routine Description:
    halPseFrameIdxToFrameSize: to get Frame size by Frame size_idx

Arguments:
    IN UINT_32 uFrameIdx

Return Value:
    Frame size

Note:   No sanity check
========================================================================
*/
OUT UINT_32
halPseFrameIdxToFrameSize (
    IN UINT_32 u4FrameIdx
    );
/*
========================================================================
Routine Description:
    halPsePortOutputQueueSanityCheck: Sanity check for Output Queue

Arguments:
    IN UINT_8       ucPortid,
    IN UINT_8       ucQueueid

Return Value:
    OUT UINT_16 : if return PSE_FRAME_FAULT, means check fail,
                  if return 0, means check ok.

Note:   No sanity check
========================================================================
*/

extern OUT UINT_16
(*halPsePortOutputQueueSanityCheck) (
    IN UINT_8       ucPortid,
    IN UINT_8       ucQueueid
    );

/*
========================================================================
Routine Description:
    halPseFlowCtrlFfaFreeCntRead:

Arguments:
        OUT PUINT_16     pu2FfaCnt       : The pointer to contain number of pages, the counter counts the FFA part of flow control mechanism
        OUT PUINT_16     pu2FreePageCnt  : The pointer to contain number of pages, the counter counts the current available page for PSE physically

Return Value:
        VOID

Note:
========================================================================
*/

extern VOID (*halPseFlowCtrlFfaFreeCntRead) (
    OUT PUINT_16     pu2FfaCnt,
    OUT PUINT_16     pu2FreePageCnt
    );

/*
========================================================================
Routine Description:
    halPseCifMiscCtrl: CPU interface allocate frame byte length setting, unit in bytes

Arguments:
                   IN UINT_16       u2CifFrameByte: The required memory size in bytes
Return Value:
                   VOID

Note:   This routine does not do sanity check
========================================================================
*/

VOID halPseCifMiscCtrl (
    IN UINT_16       u2CifFrameByte
    );
/*
========================================================================
Routine Description:
    HalPseDequeuSigFromFreeQHead: to dequeue a single frame from free queue

Arguments:
                   VOID

Return Value:
                   OUT UINT_16  return the Fid

Note:   To simplify the dequue operation from free queue
========================================================================
*/

OUT UINT_16
HalPseDequeuSigFromFreeQHead (
    VOID
    );


/*
========================================================================
Routine Description:
    halPseGetQlenCtrl: This routine is used to get the queue length in page mode or frame mode

Arguments:
                    IN   UINT_8       ucPortid              : Port ID to request
                    IN   UINT_8       ucQueueid             : Queue ID to request
                    IN   BOOLEAN      fgQlenInPage          : Page Mode(PSE_QLEN_IN_PAGE_MODE = 1)/Frame Mode (PSE_QLEN_IN_FRAME_MODE =0)
                    OUT  PUINT_16     pu2QlenReturnValue    : the queue length in page mode or frame mode.

Return Value:
                    OUT BOOLEAN     TRUE/FALSE

Note:
========================================================================
*/


extern OUT BOOLEAN
(*halPseGetQlenCtrl) (
    IN   UINT_8       ucPortid,
    IN   UINT_8       ucQueueid,
    IN   BOOLEAN      fgQlenInPage,
    OUT  PUINT_16     pu2QlenReturnValue
    );

/*
========================================================================
Routine Description:
    halPseQLenRead: This routine can read the queue length

Arguments:
                    IN UINT_8       ucPortid        :   port id
                    IN UINT_8       ucQueueid       :   queue id
                    OUT PUINT_16     pu2QLenRead    :   the pointer to read back the quue length
Return Value:
                    OUT BOOLEAN     TRUE/FALSE

Note:
        For Port3_Queue8, this routine returns the page counts.
        For Other Port#_Queue#, this routine returns the frame counts.

========================================================================
*/

extern OUT BOOLEAN
(*halPseQLenRead) (
    IN UINT_8       ucPortid,
    IN UINT_8       ucQueueid,
    OUT PUINT_16     pu2QLenRead
    );
/*
========================================================================
Routine Description:
    halPseCifEnqSig: to enqueue a frame to the head or tail of a specific queue port pair.

Arguments:
                    IN UINT_8       ucEnPortid,         : the dest. port to input
                    IN UINT_8       ucEnQueueid,        : the dest. queue to input
                    IN UINT_16      u2EnSigFid,         : Enqueue frame id
                    IN BOOLEAN      fgEnqReverse        : 1'b0: enqueue from the tail
                                                          1'b1: enqueue from the head
Return Value:
                   OUT BOOLEAN TURE/FALSE

Note:
========================================================================
*/

OUT BOOLEAN
halPseCifEnqSig(
    IN UINT_8       ucEnPortid,
    IN UINT_8       ucEnQueueid,
    IN UINT_16      u2EnSigFid,
    IN BOOLEAN      fgEnqReverse
    );
/*
========================================================================
Routine Description:
    halPseCifDeqSig: to get a head or tail frameID from a specific queue and port

Arguments:
                    IN UINT_8       ucDePortid,
                    IN UINT_8       ucDeQueueid,
                    IN BOOLEAN      fgGetReverse    1'b0: dequeue from the head
                                                    1'b1: dequeue from the tail

Return Value:
                   OUT UINT_16  return the Fid

Note:
========================================================================
*/

extern OUT UINT_16
(*halPseCifDeqSig)(
    IN UINT_8       ucDePortid,
    IN UINT_8       ucDeQueueid,
    IN BOOLEAN      fgGetReverse
    );

#ifdef IOT_PROCESS_PORTING
/*
========================================================================
Routine Description:
    halPseCifGetFirstFidinPort3Q8: to get a head or tail frameID from Port3 Queue8

Arguments:
                    IN UINT_8       ucGetPid,
                    IN UINT_8       ucGetQid,
                    IN BOOLEAN      fgGetReverse:   1'b0: get the head FID of the queue
                                                    1'b1: get the tail FID of the queue

Return Value:
                   OUT UINT_16  return the Fid

Note:
========================================================================
*/

OUT UINT_16
halPseCifGetFirstFidinPort3Q8 (
    IN UINT_8       ucGetPid,
    IN UINT_8       ucGetQid,
    IN BOOLEAN      fgGetReverse
    );


/*
========================================================================
Routine Description:
    halPseRelayInfoGet: get relay information by frame ID.

Arguments:

        IN  UINT_16     u2Fid           :   Frame ID
        OUT PBOOLEAN    pfgFidValid       :   the pointer of container for storing fid_vld field.
        OUT PBOOLEAN    pfgPauseQ         :   the pointer of container for storing pause field.
        OUT PUCHAR      pucPageNum        :   the pointer of container for storing page_num field.
        OUT PUCHAR      pucSrcPidQidTag   :   the pointer of container for storing srcpidqid_tag field.
        OUT PBOOLEAN    pfgDstValid       :   the pointer of container for storing dst_vld field.
        OUT PUCHAR      pucDstPid         :   the pointer of container for storing dst_pid field.
        OUT PUCHAR      pucDstQid         :   the pointer of container for storing dst_qid field.

Return Value:
        VOID

Note:
========================================================================
*/


VOID
halPseRelayInfoGet (
    IN     UINT_16     u2Fid,
    OUT    PBOOLEAN    pfgFidValid,
    OUT    PBOOLEAN    pfgPauseQ,
    OUT    PUCHAR      pucPageNum,
    OUT    PUCHAR      pucSrcPidQidTag,
    OUT    PBOOLEAN    pfgDstValid,
    OUT    PUCHAR      pucDstPid,
    OUT    PUCHAR      pucDstQid
    );

#endif/*IOT_PROCESS_PORTING*/

#endif /* _HAL_PSE_ROM_H */
