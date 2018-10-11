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


/*****************************************************************************
 *
 * Filename:
 * ---------
 *    usbms_state.c
 *
 * Project:
 * --------
 *   Maui_Software
 *
 * Description:
 * ------------
 *    This file implements usb mass storage state machine
 *
 * Author:
 * -------
 *   Jensen Hu
 *   CJung Chen
 *
 *****************************************************************************/

#ifdef __USB_MASS_STORAGE_ENABLE__
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "usb_comm.h"
#include "usb.h"
#include "usbms_drv.h"
#include "usbms_adap.h"
#include "usbms_state.h"
#include "usb_custom.h"
#include "usb_msdisk.h"
#include "kal_data_types.h"
#include "syslog.h"
#include "bmd.h"
#include "memory_attribute.h"


/* sense code */
ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t senseOk[] = {0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                                          };
ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t senseNoMedia[] = {0x70, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00,
                                                                0x00, 0x00, 0x3A, 0x00, 0x00, 0x00, 0x00, 0x00
                                                               };
ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t senseMediaChanged[] = {0x70, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00,
                                                                     0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00
                                                                    };
ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t senseInvalidFieldInCDB[] = {0x70, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00,
                                                                          0x00, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00
                                                                         };
#if 0//def __P_PROPRIETARY_COPYRIGHT__
/*__align(4)*/ ATTR_RWDATA_IN_NONCACHED_RAM static uint8_t senseDataProtect[] = {0x70, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00,
                                                                                   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                                                                                  };
#endif

extern void DRV_RESET(void);
static void USB_Ms_Recv_Data(usb_ms_rec_done_conf_struct *buf);
static void USB_Ms_Transmit_Data(void);
static void USB_Ms_Generate_Csw(UsbMs_CSW *CSW, UsbMs_CBW *CBW);
static void USB_Ms_Build_Tx_CWS(UsbMs_CSW *CSW, UsbMs_CBW *CBW);
static void USB_Ms_MemInverse(void *src, void *dst, uint16_t len);
static void USB_Ms_Cmd(void *data);
//static void USB_Ms_Cmd_Format(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_State_Checkmedia_Exist(uint8_t LUN);
static void USB_Ms_Cmd_Inquiry(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Test_Unit_Ready(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Prev_Media_Removal(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Read_Capacity(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Read_FormatCapacity(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_RequestSense(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Verify(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_ModeSense(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_ModeSense6(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Start_Stop_Unit(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Synchronize_Cache_10(UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Unknown(UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Read(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Handle_Read(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Read_Fail_Handler(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Cmd_Write(uint8_t LUN, UsbMs_CBW *CBW);
static void USB_Ms_Handle_Write(uint8_t LUN, UsbMs_CBW *CBW, uint8_t sector_num);
#if 0//def __P_PROPRIETARY_COPYRIGHT__
static bool	USB_Ms_Search_Item(uint32_t *data_items, uint32_t data_size, uint32_t search_value, uint32_t *p_index);
static void USB_Ms_Sort_Index_Items(uint32_t *items, uint32_t num);
static bool	USB_Ms_Check_Read_Protect(uint8_t LUN, uint32_t LBA, uint32_t sec_num);
static bool	USB_Ms_Check_Second_Read_Protect(uint8_t LUN, uint32_t LBA, uint32_t sec_num);
static void USB_Ms_Update_Read_Protect(uint8_t LUN, uint32_t LBA, uint32_t sec_num);
#endif

//#pragma arm section zidata = "NONCACHEDZI", rwdata = "NONCACHEDRW"
/* allocate a non-cacheable buffer for D-cacheable region */
/*__align(4)*/ ATTR_RWDATA_IN_NONCACHED_RAM static UsbMs_CSW g_UsbMS_CSW={0};
//#pragma arm section zidata, rwdata


hal_usb_endpoint_state_t hal_usb_check_rx_endpoint_usage(uint32_t ep_num);
hal_usb_status_t hal_usb_configure_rx_endpoint_type(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_is_use_dma);


/************************************************************
        Tx/Rx path state machine functions
*************************************************************/
void USB_Ms_State_Main(USB_Ms_State event, usb_ms_rec_done_conf_struct *local_para_ptr)
{
    UsbMs_CBW *CBW = (UsbMs_CBW *)g_UsbMS.CBWdata;

    //if USB reset just happen,do nothing if message is not MSG_ID_USB_MSDRV_RESET_IND.(ignore previous message.)
    if ((event != MSG_ID_USB_MSDRV_RESET_IND) && ((g_UsbMS.usb_reset == true) || (g_UsbMS.ms_is_reset == true))) { //receive reset
        LOG_E(hal, "USB_Ms_State_Main, do nothing return\n");
        return;
    }

    switch (event) {
        case MSG_ID_USB_MSDRV_REC_DONE_CONF:
            /* Receive data from DMA callback */
            USB_Ms_Recv_Data(/*p_recv_ilm->*/local_para_ptr);
            break;

        case MSG_ID_USB_MSDRV_TRX_DONE_CONF:
            /* Transmitted data from DMA callback */
            USB_Ms_Transmit_Data();
            break;

        case MSG_ID_USB_MSDRV_CLEAR_STALL_REQ:
            g_UsbMS.nState = USBMS_IDLE;
            g_UsbMS.CSWStatusError = 0x01;
            if (hal_usb_check_rx_endpoint_usage(g_UsbMS.rxpipe->byEP) == HAL_USB_EP_STATE_DMA) {
                hal_usb_configure_rx_endpoint_type(g_UsbMS.rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, false);
            }
            USB_Ms_Generate_Csw(&g_UsbMS_CSW, CBW);
            USB_Ms_BuildTx(&g_UsbMS_CSW, 13);
            break;

        case MSG_ID_USB_MSDRV_RESET_IND:
            /* Reset from reset signal or class specific EP0 RESET command */
            g_UsbMS.nState = USBMS_IDLE;
            g_UsbMS.usb_reset = false;
            g_UsbMS.ms_is_reset = false;
            g_UsbMS.is_send_usb_reset_ilm = true;
            break;

        default:
            //ASSERT(0);
            LOG_E(hal, "ASSERT\n");
            break;
    }

}

/* RX path state machine */
static void USB_Ms_Recv_Data(usb_ms_rec_done_conf_struct *buf)
{
    UsbMs_CBW   *CBW;
    uint8_t   *CBWCB;
    uint8_t   sector_num;
    usb_ms_rec_done_conf_struct *ptr = (usb_ms_rec_done_conf_struct *)buf;

    switch (g_UsbMS.nState) {
        case USBMS_IDLE:
            if (ptr->nBytesRecv != 31) {
                LOG_E(hal, "ASSERT, nBytesRecv = %d\n", ptr->nBytesRecv);
            }
            USB_Ms_Cmd(g_UsbMS.CBWdata);
            break;

        case USBMS_RX:
            CBW = (UsbMs_CBW *)g_UsbMS.CBWdata;
            CBWCB = (uint8_t *)CBW->CBWCB;

            if ((CBWCB[0] == USBMS_WRITE10) || (CBWCB[0] == USBMS_WRITE12)) {
                sector_num = ptr->nBytesRecv / 512;
                USB_Ms_Handle_Write(g_UsbMS.current_LUN, CBW, sector_num);
            } else { /*if (CBWCB[0] == USBMS_CMD_Write)*/
                LOG_E(hal, "ASSERT, CBW = %d\n", (uint32_t)CBW->CBWCB[0]);
            }
            break;

//	case USBMS_GETNEXTCMD:
        case USBMS_ACK:
        case USBMS_TX:
        default:
            LOG_E(hal, "ASSERT, g_UsbMS.nState = %d\n", g_UsbMS.nState);
            break;
    }
}


/* TX path state machine */
static void USB_Ms_Transmit_Data(void)
{
    UsbMs_CBW *CBW = (UsbMs_CBW *)g_UsbMS.CBWdata;

    switch (g_UsbMS.nState) {
        case USBMS_ACK:
            /*Send CSW*/
            g_UsbMS.nState = USBMS_IDLE;
            USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
            break;

        case USBMS_TX:
            if ((CBW->CBWCB[0] == USBMS_READ10) || (CBW->CBWCB[0] == USBMS_READ12)) {
                USB_Ms_Handle_Read(g_UsbMS.current_LUN, CBW);
            } else {
                //EXT_ASSERT(0, (uint32_t)CBW->CBWCB[0], 0, 0);

                LOG_E(hal, "ASSERT\n");
            }
            break;

        case USBMS_IDLE:
        case USBMS_RX:
        default:
            LOG_E(hal, "ASSERT, g_UsbMS.nState = %d\n", g_UsbMS.nState);
            break;
    }
}


/************************************************************
        utility functions
*************************************************************/
/* Generate CSW element according to CBW and keep status */
static void USB_Ms_Generate_Csw(UsbMs_CSW *CSW, UsbMs_CBW *CBW)
{
    CSW->dCSWSignature = 0x53425355;
    CSW->dCSWTag = CBW->dCBWTag;
    CSW->dCSWDataResidue = g_UsbMS.CSWDataResidue;
    CSW->bmCSWStatus = g_UsbMS.CSWStatusError;
    g_UsbMS.CSWStatusError = 0x00;
    g_UsbMS.CSWDataResidue = 0;
}


/* inverse scr to dst, total len*/
static void USB_Ms_MemInverse(void *src, void *dst, uint16_t len)
{
    uint32_t  index;
    uint8_t *li_data = (uint8_t *)src;
    uint8_t *bi_data = (uint8_t *)dst;
    li_data += (len - 1);

    for (index = 0; index < len; index++) {
        *(bi_data + index) = *(li_data - index);
    }
}

/* Use this function to save ROM size
 * Action1: Switch RX endpoint state to use FIFO
 * Action2: Build 13 bytes CWS
 */
static void USB_Ms_Build_Tx_CWS(UsbMs_CSW *CSW, UsbMs_CBW *CBW)
{
    hal_usb_configure_rx_endpoint_type(g_UsbMS.rxpipe->byEP, HAL_USB_EP_TRANSFER_BULK, false);

    USB_Ms_Generate_Csw(&g_UsbMS_CSW, CBW);
    USB_Ms_BuildTx(&g_UsbMS_CSW, 13);
}



/************************************************************
        CBW parse command functions
*************************************************************/

/* parse CBW command */
static void USB_Ms_Cmd(void *data)
{
    UsbMs_CBW *CBW = (UsbMs_CBW *)data;
    uint8_t   *CBWCB;


    CBWCB = (uint8_t *)CBW->CBWCB;

//	if((CBW->dCBWSignature != 0x43425355)||(CBW->bCBWLUN > g_UsbMS.max_LUN) ||(USB_Get_DMA_Rx_Left_Count(g_UsbMS.rxpipe->byEP) != 0))
    if ((CBW->dCBWSignature != 0x43425355) || (CBW->bCBWLUN > g_UsbMS.max_LUN)) {
        g_UsbMS.cbw_is_vaild = false;
        hal_usb_set_endpoint_stall(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, true);
        hal_usb_set_endpoint_stall(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, true);
        return;
    }

    /* If we accept this packet, it means it is a valid CBW */
    g_UsbMS.cbw_is_vaild = true;

    /* keep current LUN */
    g_UsbMS.current_LUN = CBW->bCBWLUN;

    /* determine state */
    if (CBW->dCBWDataTransferLength != 0) {
        if (CBW->bmCBWFlags & USBMS_DIR_IN) {
            g_UsbMS.nState = USBMS_TX;
        } else {
            g_UsbMS.nState = USBMS_RX;
        }
    }

    /* parse command */
    switch (CBWCB[0]) {
//	case USBMS_FORMAT_UNIT:
//		USB_Ms_Cmd_Format(CBW->bCBWLUN, CBW);
//		break;
        case USBMS_INQUIRY:
            USB_Ms_Cmd_Inquiry(CBW->bCBWLUN, CBW);
            break;
        case USBMS_TEST_UNIT_READY:
            USB_Ms_Cmd_Test_Unit_Ready(CBW->bCBWLUN, CBW);
            break;
        case USBMS_PREVALLOW_MEDIA_REMOVL:
            USB_Ms_Cmd_Prev_Media_Removal(CBW->bCBWLUN, CBW);
            break;
        case USBMS_WRITE10:
            USB_Ms_MemInverse(&CBWCB[7], &g_UsbMS.rw_cmd.BlkLen, 2);
            USB_Ms_Cmd_Write(CBW->bCBWLUN, CBW);
            break;
        case USBMS_WRITE12:
            USB_Ms_MemInverse(&CBWCB[6], &g_UsbMS.rw_cmd.BlkLen, 4);
            USB_Ms_Cmd_Write(CBW->bCBWLUN, CBW);
            break;
        case USBMS_READ10:
            USB_Ms_MemInverse(&CBWCB[7], &g_UsbMS.rw_cmd.BlkLen, 2);
            USB_Ms_Cmd_Read(CBW->bCBWLUN, CBW);
            break;
        case USBMS_READ12:
            USB_Ms_MemInverse(&CBWCB[6], &g_UsbMS.rw_cmd.BlkLen, 4);
            USB_Ms_Cmd_Read(CBW->bCBWLUN, CBW);
            break;
        case USBMS_READ_CAPACITY:
            USB_Ms_Cmd_Read_Capacity(CBW->bCBWLUN, CBW);
            break;
        case USBMS_READ_FORMATCAPACITY:
            USB_Ms_Cmd_Read_FormatCapacity(CBW->bCBWLUN, CBW);
            break;
        case USBMS_REQUESTSENSE:
            USB_Ms_Cmd_RequestSense(CBW->bCBWLUN, CBW);
            break;
        case USBMS_VERIFY:
            USB_Ms_Cmd_Verify(CBW->bCBWLUN, CBW);
            break;
        case USBMS_MODE_SENSE:      /* Mode Sense */
            USB_Ms_Cmd_ModeSense(CBW->bCBWLUN, CBW);
            break;
        case USBMS_MODE_SENSE6:      /*Mode Sense*/
            USB_Ms_Cmd_ModeSense6(CBW->bCBWLUN, CBW);
            break;
        case USBMS_START_STOP_UNIT:
            USB_Ms_Cmd_Start_Stop_Unit(CBW->bCBWLUN, CBW);
            break;
        case USBMS_SYNCHRONIZE_CACHE_10:
            USB_Ms_Cmd_Synchronize_Cache_10(CBW);
            break;
        case USBMS_MODE_SELECT:
        case USBMS_MODE_SELECT6:
        default:
            USB_Ms_Cmd_Unknown(CBW);
            break;
    }
}

#if 0
/* handle CBW USBMS_FORMAT_UNIT command */
static void USB_Ms_Cmd_Format(uint8_t LUN, UsbMs_CBW *CBW)
{
    bool msdc_status;


    USB_Ms_State_Checkmedia_Exist(LUN);
    if (g_UsbMS.CSWStatusError == 0x00) {
        msdc_status = USB_Ms_Format(LUN);
        if (msdc_status == false) {
            USB_Ms_State_Checkmedia_Exist(LUN);
        }
    }

    g_UsbMS.nState = USBMS_GETNEXTCMD;
    USB_Ms_Generate_Csw(&g_UsbMS_CSW, CBW);
    USB_Ms_BuildTx(&g_UsbMS_CSW, 13);
}
#endif


/* check media status and generate correspongind status for CSW */
static void USB_Ms_State_Checkmedia_Exist(uint8_t LUN)
{
    USB_STORAGE_DEV_STATUS   msdc_status;
    msdc_status = USB_STORAGE_DEV_STATUS_OK;//USB_Ms_Checkmedia_Exist(LUN);

    if (g_UsbMS.power_off_state[LUN] == false) {
        switch (msdc_status) {
            case USB_STORAGE_DEV_STATUS_OK:
            case USB_STORAGE_DEV_STATUS_WP:
                g_UsbMS.sensePtr = (uint8_t *)senseOk;
                g_UsbMS.CSWStatusError = 0x00;
                break;
            case USB_STORAGE_DEV_STATUS_MEDIA_CHANGE:
                g_UsbMS.sensePtr = (uint8_t *)senseMediaChanged;
                g_UsbMS.CSWStatusError = 0x01;
                break;
            case USB_STORAGE_DEV_STATUS_NOMEDIA:
                g_UsbMS.sensePtr = (uint8_t *)senseNoMedia;
                g_UsbMS.CSWStatusError = 0x01;
                break;
            default:
                LOG_E(hal, "Checkmedia_Exist:ASSERT\n");
                break;
        }
    } else {
        g_UsbMS.sensePtr = (uint8_t *)senseNoMedia;
        g_UsbMS.CSWStatusError = 0x01;
    }
}


/* handle CBW USBMS_INQUIRY command */
static void USB_Ms_Cmd_Inquiry(uint8_t LUN, UsbMs_CBW *CBW)
{
    if (CBW->dCBWDataTransferLength == 0) {
        USB_Ms_State_Checkmedia_Exist(LUN);

        g_UsbMS.nState = USBMS_IDLE;

        g_UsbMS.CSWDataResidue = 0;
        USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
    } else {
        /*Windows Vista and 7 will allocate 0xFF byte directly. We shouldn't STALL it*/
        if (CBW->dCBWDataTransferLength > g_UsbMS.ms_param->inquire_size) {
            CBW->dCBWDataTransferLength = g_UsbMS.ms_param->inquire_size;
        }

        //if(CBW->dCBWDataTransferLength <= ((uint32_t)g_UsbMS.ms_param->inquire_size))
        {
            g_UsbMS.sensePtr = (uint8_t *)senseOk;
            g_UsbMS.CSWStatusError = 0x00;

            g_UsbMS.nState = USBMS_ACK;
            g_UsbMS.CSWDataResidue = 0;
            {
                USB_Ms_BuildTx((uint8_t *)g_UsbMS.ms_param->inquire_data, CBW->dCBWDataTransferLength);
            }
        }
    }
}


/* handle CBW USBMS_TEST_UNIT_READY command */
static void USB_Ms_Cmd_Test_Unit_Ready(uint8_t LUN, UsbMs_CBW *CBW)
{
    if (CBW->dCBWDataTransferLength != 0) {
        if (g_UsbMS.nState == USBMS_RX) {
            hal_usb_set_endpoint_stall(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, true);
        } else {
            hal_usb_set_endpoint_stall(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, true);
        }
        g_UsbMS.CSWDataResidue = CBW->dCBWDataTransferLength;
    } else {
        USB_Ms_State_Checkmedia_Exist(LUN);

        g_UsbMS.nState = USBMS_IDLE;

        USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);

    }
}


/* handle CBW USBMS_PREVALLOW_MEDIA_REMOVL command */
static void USB_Ms_Cmd_Prev_Media_Removal(uint8_t LUN, UsbMs_CBW *CBW)
{
    bool msdc_status;

    if (USB_Ms_Dev_Type(LUN) == USB_STORAGE_DEV_CDROM) {
        USB_Ms_State_Checkmedia_Exist(LUN);

        if (g_UsbMS.CSWStatusError == 0x00) {
            msdc_status = USB_Ms_Prevmedia_Removal(LUN, true);

            if (msdc_status == false) {
                USB_Ms_State_Checkmedia_Exist(LUN);
            }
        }
    } else {
        g_UsbMS.sensePtr = (uint8_t *)senseInvalidFieldInCDB;
        g_UsbMS.CSWStatusError = 0x01;

    }

    g_UsbMS.nState = USBMS_IDLE;

    USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
}


/* handle CBW USBMS_READ_CAPACITY command */
static void USB_Ms_Cmd_Read_Capacity(uint8_t LUN, UsbMs_CBW *CBW)
{
    uint32_t LBA;
    uint32_t len;
    bool msdc_status;
    Read_Capacity_Info *Capacity_Info = (Read_Capacity_Info *)g_UsbMS.disk_buffer;

    USB_Ms_State_Checkmedia_Exist(LUN);

    if (g_UsbMS.CSWStatusError == 0x00) {
        msdc_status = USB_Ms_Read_Capacity(LUN, &LBA, &len);

        if (msdc_status == false) {
            /* device exist but not ready yet, must return error */
            g_UsbMS.CSWStatusError = 0x01;

            /*Fail case*/
            LBA = 0xFFFFFFFF;
            len = 0x200;
        }
    } else {
        /* Fail case */
        LBA = 0xFFFFFFFF;
        len = 0x200;
    }

    USB_Ms_MemInverse(&LBA, &Capacity_Info->LastBA, 4);
    USB_Ms_MemInverse(&len, &Capacity_Info->BlkLen, 4);

    g_UsbMS.nState = USBMS_ACK;
    USB_Ms_BuildTx(Capacity_Info, 8);
}


/* handle CBW USBMS_READ_FORMATCAPACITY command */
static void USB_Ms_Cmd_Read_FormatCapacity(uint8_t LUN, UsbMs_CBW *CBW)
{
    uint32_t msdc_blk_no;
    uint32_t msdc_blk_len;
    uint32_t len;
    uint8_t  *ReadformatCapacity_Data = (uint8_t *)g_UsbMS.disk_buffer;
    uint8_t *CBWCB = CBW->CBWCB;


    len = (uint8_t)CBWCB[7];
    len <<= 8;
    len |= (uint8_t)CBWCB[8];

    memset(ReadformatCapacity_Data, 0x00, 12);

    *(ReadformatCapacity_Data + 3) = 0x08;

    msdc_blk_no = 0xFFFFFFFF;
    msdc_blk_len = 0x03000200;
    g_UsbMS.CSWStatusError = 0x00;

    USB_Ms_MemInverse(&msdc_blk_no, (ReadformatCapacity_Data + 4), 4);
    USB_Ms_MemInverse(&msdc_blk_len, (ReadformatCapacity_Data + 8), 4);

    g_UsbMS.nState = USBMS_ACK;

    if (len > 12) {
        len = 12;
    }
    USB_Ms_BuildTx(ReadformatCapacity_Data, len);
}


/* handle CBW USBMS_REQUESTSENSE command */
static void USB_Ms_Cmd_RequestSense(uint8_t LUN, UsbMs_CBW *CBW)
{
    uint32_t dCBWDataTransferLength = CBW->dCBWDataTransferLength;

    if (dCBWDataTransferLength == 0) {
        USB_Ms_State_Checkmedia_Exist(LUN);

        g_UsbMS.nState = USBMS_IDLE;

        g_UsbMS.CSWDataResidue = 0;
        USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
    } else {
        g_UsbMS.CSWStatusError = 0x00;

        g_UsbMS.nState = USBMS_ACK;

        g_UsbMS.CSWDataResidue = 0;

        if (dCBWDataTransferLength > 18) {
            dCBWDataTransferLength = 18;
        }
        USB_Ms_BuildTx(g_UsbMS.sensePtr, dCBWDataTransferLength);
    }
}


/* handle CBW USBMS_VERIFY command */
static void USB_Ms_Cmd_Verify(uint8_t LUN, UsbMs_CBW *CBW)
{
    g_UsbMS.nState = USBMS_IDLE;
    USB_Ms_State_Checkmedia_Exist(LUN);
    USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
}


/* handle CBW USBMS_MODE_SENSE6 command */
static void USB_Ms_Cmd_ModeSense(uint8_t LUN, UsbMs_CBW *CBW)
{
    USB_STORAGE_DEV_STATUS result;
    uint8_t *ModeSense_Data = (uint8_t *)g_UsbMS.disk_buffer;


    USB_Ms_State_Checkmedia_Exist(LUN);
    result = USB_Ms_Checkmedia_Exist(LUN);

    *(ModeSense_Data) = 0x00;
    *(ModeSense_Data + 1) = 0x06;
    *(ModeSense_Data + 2) = 0x00;

    *(ModeSense_Data + 3) = (result == USB_STORAGE_DEV_STATUS_WP) ? 0x80 : 0x00;

    *(ModeSense_Data + 4) = 0x00;
    *(ModeSense_Data + 5) = 0x00;
    *(ModeSense_Data + 6) = 0x00;
    *(ModeSense_Data + 7) = 0x00;

    g_UsbMS.nState = USBMS_ACK;

    USB_Ms_BuildTx(ModeSense_Data, 8);
}


/* Handle CBW USBMS_MODE_SENSE6 command */
static void USB_Ms_Cmd_ModeSense6(uint8_t LUN, UsbMs_CBW *CBW)
{
    USB_STORAGE_DEV_STATUS result;
    uint8_t *ModeSense6_Data = (uint8_t *)g_UsbMS.disk_buffer;


    USB_Ms_State_Checkmedia_Exist(LUN);
    result = USB_Ms_Checkmedia_Exist(LUN);

    *(ModeSense6_Data) = 0x03;
    *(ModeSense6_Data + 1) = 0x00;
    if (result == USB_STORAGE_DEV_STATUS_WP) {
        *(ModeSense6_Data + 2) = 0x80;
    } else {
        *(ModeSense6_Data + 2) = 0x00;
    }
    *(ModeSense6_Data + 3) = 0x00;

    g_UsbMS.nState = USBMS_ACK;

    USB_Ms_BuildTx(ModeSense6_Data, 4);
}


static void USB_Ms_Cmd_Start_Stop_Unit(uint8_t LUN, UsbMs_CBW *CBW)
{
    g_UsbMS.nState = USBMS_IDLE;
    USB_Ms_State_Checkmedia_Exist(LUN);
    USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);


    //00 - Stop motor    = 0x00
    //01 - Start motor   = 0x01
    //10 - Eject media   = 0x02
    //11 - Load media    = 0x03
    if ((CBW->CBWCB[4] & 0x03) == 0x01) {
        g_UsbMS.power_off_state[LUN] = false;	//start
    } else {
        g_UsbMS.power_off_state[LUN] = true;	//Off
    }
}

static void USB_Ms_Cmd_Synchronize_Cache_10(UsbMs_CBW *CBW)
{
    USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
}

/* handle CBW unknown command */
static void USB_Ms_Cmd_Unknown(UsbMs_CBW *CBW)
{
    uint8_t count = 0;
    bool bEPEmpty;

    g_UsbMS.nState = USBMS_IDLE;
    g_UsbMS.sensePtr = (uint8_t *)senseInvalidFieldInCDB;
    g_UsbMS.CSWStatusError = 0x01;

    /* check if need to send out a NULL packet */
    if ((CBW->dCBWDataTransferLength != 0) && (CBW->bmCBWFlags & USBMS_DIR_IN)) {
        NVIC_DisableIRQ(USB20_IRQn);
        hal_usb_write_endpoint_fifo(g_UsbMS.txpipe->byEP, 0, NULL);
        hal_usb_set_endpoint_tx_ready(g_UsbMS.txpipe->byEP);

        NVIC_EnableIRQ(USB20_IRQn);

        do {
            count++;
            bEPEmpty = hal_usb_is_endpoint_tx_empty(g_UsbMS.txpipe->byEP);
        } while ((bEPEmpty == false) && (count != (uint8_t)USBMS_UNKNOW_CMD_TIMEOUT) && (USB_Get_Device_Type() != USB_UNKOWN));
    }

    if ((count != (uint8_t)USBMS_UNKNOW_CMD_TIMEOUT) && (USB_Get_Device_Type() != USB_UNKOWN))
    {
        USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
    }
}


/* handle CBW USBMS_READ command */
static void USB_Ms_Cmd_Read(uint8_t LUN, UsbMs_CBW *CBW)
{
    uint8_t *CBWCB = CBW->CBWCB;
    g_UsbMS.ms_read_write_reset = false;

    USB_Ms_State_Checkmedia_Exist(LUN);

    USB_Ms_MemInverse(&CBWCB[2], &g_UsbMS.rw_cmd.LBA, 4);

    if (g_UsbMS.nState == USBMS_RX) {

        g_UsbMS.CSWDataResidue = CBW->dCBWDataTransferLength;
        hal_usb_set_endpoint_stall(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, true);
        return;
    }

    if (CBW->dCBWDataTransferLength == 0) {

        /* Host expects no data transfers */
        g_UsbMS.CSWStatusError = 0x02;
        g_UsbMS.nState = USBMS_IDLE;
        g_UsbMS.CSWDataResidue = 0;
        USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
    } else if (g_UsbMS.rw_cmd.BlkLen == 0) {
        /* Device expects no data transfers */
        g_UsbMS.CSWDataResidue = CBW->dCBWDataTransferLength;
        hal_usb_set_endpoint_stall(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, true);
    } else {

        if (g_UsbMS.CSWStatusError == 0x00) {
            if ((g_UsbMS.rw_cmd.BlkLen * 512) != CBW->dCBWDataTransferLength) {
                hal_usb_set_endpoint_stall(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, true);
                g_UsbMS.CSWDataResidue = CBW->dCBWDataTransferLength;
            } else {
                g_UsbMS.rw_cmd.rwindex = 0;
                g_UsbMS.rw_cmd.msdc_rwindex = 0;
                g_UsbMS.rw_cmd.rw_buffer_index = 0;
                g_UsbMS.rw_cmd.rw_error_status = false;

                USB_Ms_Handle_Read(LUN, CBW);

                //USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
            }
        } else {

            USB_Ms_Read_Fail_Handler(LUN, CBW);
        }
    }
}


static void USB_Ms_Handle_Read(uint8_t LUN, UsbMs_CBW *CBW)
{
    static bool 	read_status;
    static uint32_t	readlen = 0;


    if (g_UsbMS.rw_cmd.rwindex == 0) {	/* the first time to read */
        if ((g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex) <= USBMS_TX_MAX_SECTOR) {
            readlen = g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex;
        } else {
            readlen = USBMS_TX_MAX_SECTOR;
        }

#if 0//def __P_PROPRIETARY_COPYRIGHT__
        if (USB_Ms_Check_Second_Read_Protect(LUN,
                                             (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen) == true) {
            g_UsbMS.b_read_protect = true;
            USB_Ms_Read_Fail_Handler(LUN, CBW);
            return;
        } else {
            if (USB_Ms_Check_Read_Protect(LUN,
                                          (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen) == true) {
                /* fill data as 0 */
                memset((uint8_t *)g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index], 0, readlen * 512);
                read_status = true;
            } else {
                read_status = USB_Ms_Read_Write(USB_MS_READ, LUN, g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index],
                                                (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen);
            }
            g_UsbMS.b_read_protect = false;
        }
#else
        read_status = USB_Ms_Read_Write(USB_MS_READ, LUN, g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index],
                                        (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen);
#endif   /* __P_PROPRIETARY_COPYRIGHT__ */

    }/*(g_UsbMS.rw_cmd.rwindex == 0)*/

    if (read_status == true) {
        g_UsbMS.rw_cmd.msdc_rwindex += readlen;
        USB_Ms_BuildTx((void *)g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index], (512 * readlen));
        g_UsbMS.rw_cmd.rwindex += readlen;

        if (g_UsbMS.rw_cmd.rwindex == g_UsbMS.rw_cmd.BlkLen) {

            g_UsbMS.nState = USBMS_ACK;
            return;
        }
    } else {

        USB_Ms_Read_Fail_Handler(LUN, CBW);
        return;
    }

    if ((g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex) <= USBMS_TX_MAX_SECTOR) {
        readlen = g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex;
    } else {
        readlen = USBMS_TX_MAX_SECTOR;
    }

    g_UsbMS.rw_cmd.rw_buffer_index ^= 1;

#if 0//def __P_PROPRIETARY_COPYRIGHT__
    if (USB_Ms_Check_Second_Read_Protect(LUN,
                                         (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen) == true) {
        g_UsbMS.b_read_protect = true;
        USB_Ms_Read_Fail_Handler(LUN, CBW);
        return;
    } else {
        if (USB_Ms_Check_Read_Protect(LUN,
                                      (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen) == true) {
            /* fill data as 0 */
            memset((uint8_t *)g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index], 0, readlen * 512);
            read_status = true;
        } else {
            read_status = USB_Ms_Read_Write(USB_MS_READ, LUN, g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index],
                                            (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen);
        }

        g_UsbMS.b_read_protect = false;
    }
#else
    read_status = USB_Ms_Read_Write(USB_MS_READ, LUN, g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index],
                                    (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), readlen);
#endif  /* __P_PROPRIETARY_COPYRIGHT__ */
}


/* Handle read error condition */
static void USB_Ms_Read_Fail_Handler(uint8_t LUN, UsbMs_CBW *CBW)
{
    bool bEPEmpty;
    uint16_t count = 0;

    /* Make sure previous packets are already sent out
       maybe last DMA data has triggered DMA done but data are still in FIFO*/
    //	bEPEmpty = USB_Is_EP_Tx_Empty(g_UsbMS.txpipe->byEP, USB_ENDPT_BULK);
    bEPEmpty = hal_usb_is_endpoint_tx_empty(g_UsbMS.txpipe->byEP);

    /* If cable plug out at this time, add timeout to avoid looping here */
    while ((bEPEmpty == false) && (count != USBMS_READ_FAIL_TIMEOUT) && (USB_Get_Device_State() == DEVSTATE_CONFIG)) {
        count++;
        bEPEmpty = hal_usb_is_endpoint_tx_empty(g_UsbMS.txpipe->byEP);
    }

    /* send out a NULL packet */
    NVIC_DisableIRQ(USB20_IRQn);//IRQMask(IRQ_USB_CODE);

    if ((g_UsbMS.ms_read_write_reset == false) && (count != USBMS_READ_FAIL_TIMEOUT)) {
        hal_usb_write_endpoint_fifo(g_UsbMS.txpipe->byEP, 0, NULL);
        hal_usb_set_endpoint_tx_ready(g_UsbMS.txpipe->byEP);
    }

    //USB_UnMask_Irq(USB_MASK_OWNER_MS);
    {
        //IRQUnmask(IRQ_USB_CODE);
        NVIC_EnableIRQ(USB20_IRQn);
    }

    /* make sure previous NULL pkt has been sent out
       avoid next DMA data in FIFO sent out instead of previos NULL pkt.*/
    /* To avoid "IN token received after next DMA(below part) has moved data into FIFO"
    If this happens, MTK device will send CSW of 13 bytes out to PC,
    and it will cause PC to wait for next data(about 10 seconds) and reset USB after that.
    Disk will show on PC until this condition disappear*/
    if (count != USBMS_READ_FAIL_TIMEOUT) {
        //bEPEmpty = USB_Is_EP_Tx_Empty(g_UsbMS.txpipe->byEP, USB_ENDPT_BULK);
        bEPEmpty = hal_usb_is_endpoint_tx_empty(g_UsbMS.txpipe->byEP);

        /* If cable plug out at this time, add timeout to avoid looping here */
        count = 0;
        while ((bEPEmpty == false) && (count != USBMS_READ_FAIL_TIMEOUT) && (USB_Get_Device_State() == DEVSTATE_CONFIG)) {
            count++;
            bEPEmpty = hal_usb_is_endpoint_tx_empty(g_UsbMS.txpipe->byEP);
        }
    }

#if 0//def __P_PROPRIETARY_COPYRIGHT__
    if (g_UsbMS.b_read_protect == true) {
        g_UsbMS.b_read_protect = false;
        g_UsbMS.sensePtr = (uint8_t *)senseDataProtect;
        g_UsbMS.CSWStatusError = 0x01;
    } else {
        USB_Ms_State_Checkmedia_Exist(LUN);
    }
#else
    USB_Ms_State_Checkmedia_Exist(LUN);
#endif

    g_UsbMS.nState = USBMS_IDLE;

    if ((USB_Get_Device_State() == DEVSTATE_CONFIG) && (hal_usb_check_rx_endpoint_usage(g_UsbMS.rxpipe->byEP) == HAL_USB_EP_STATE_DMA)) {
        /* If the FIFO state is not in DMA, this means that PC already reset USB, and re-enumerate again */
        g_UsbMS.CSWDataResidue = (g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.msdc_rwindex) * 512;
        g_UsbMS.CSWStatusError = 0x01;

        NVIC_DisableIRQ(USB20_IRQn);//IRQMask(IRQ_USB_CODE);

        if ((g_UsbMS.ms_read_write_reset == false) && (count != USBMS_READ_FAIL_TIMEOUT)) {
            USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
        }

        //USB_UnMask_Irq(USB_MASK_OWNER_MS);
        //if(USB_Get_UnMask_Irq() == true)
        {
            //IRQUnmask(IRQ_USB_CODE);
            NVIC_EnableIRQ(USB20_IRQn);
        }
    }
}


/* handle CBW USBMS_WRITE command */
static void USB_Ms_Cmd_Write(uint8_t LUN, UsbMs_CBW *CBW)
{
    uint8_t   *CBWCB = CBW->CBWCB;
    USB_STORAGE_DEV_STATUS   msdc_status;

    g_UsbMS.ms_read_write_reset = false;
    msdc_status = USB_STORAGE_DEV_STATUS_OK;//USB_Ms_Checkmedia_Exist(LUN);

    USB_Ms_MemInverse(&CBWCB[2], &g_UsbMS.rw_cmd.LBA, 4);

    if (g_UsbMS.nState == USBMS_TX) {
        hal_usb_set_endpoint_stall(g_UsbMS.txpipe->byEP, HAL_USB_EP_DIRECTION_TX, true);

        g_UsbMS.CSWDataResidue = CBW->dCBWDataTransferLength;
        return;
    }

    if (CBW->dCBWDataTransferLength == 0) {
        g_UsbMS.CSWStatusError = 0x02;
        g_UsbMS.nState = USBMS_IDLE;
        g_UsbMS.CSWDataResidue = 0;
        /* Switch RX endpoint state to use FIFO */
        if (g_UsbMS.ms_read_write_reset == false) {
            USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
        }
    } else if (g_UsbMS.rw_cmd.BlkLen == 0) {
        /* Device expects no data transfers */
        hal_usb_set_endpoint_stall(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, true);
        g_UsbMS.CSWDataResidue = CBW->dCBWDataTransferLength;
    } else {

        if ((g_UsbMS.rw_cmd.BlkLen * 512) != CBW->dCBWDataTransferLength) {
            //USB_CtrlEPStall(g_UsbMS.rxpipe->byEP, USB_EP_RX_DIR, true);
            hal_usb_set_endpoint_stall(g_UsbMS.rxpipe->byEP, HAL_USB_EP_DIRECTION_RX, true);
            g_UsbMS.CSWDataResidue = CBW->dCBWDataTransferLength;
        } else {
            g_UsbMS.rw_cmd.rwindex = 0;
            g_UsbMS.rw_cmd.msdc_rwindex = 0;
            g_UsbMS.rw_cmd.rw_buffer_index = 0;

            if (msdc_status == USB_STORAGE_DEV_STATUS_OK) {
                g_UsbMS.rw_cmd.rw_error_status = false;
            } else {
                g_UsbMS.rw_cmd.rw_error_status = true;
            }

            USB_Ms_Handle_Write(LUN, CBW, 0);
        }
    }
}


static void USB_Ms_Handle_Write(uint8_t LUN, UsbMs_CBW *CBW, uint8_t sector_num)
{
    uint32_t writelen;
    bool write_status;
    bool write_data = false;

    /* Determine if it has data to write, if yes and msdc status ok, then write it */
    if ((g_UsbMS.rw_cmd.rwindex != 0) && (g_UsbMS.rw_cmd.rw_error_status == false)) {
        write_data = true;
    }

    /* Write the last one data */
    if (g_UsbMS.rw_cmd.rwindex == g_UsbMS.rw_cmd.BlkLen) {
        g_UsbMS.rw_cmd.rw_buffer_index ^= 1;
        if (write_data == true) {

            write_status = USB_Ms_Read_Write(USB_MS_WRITE, g_UsbMS.current_LUN, g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index],
                                             (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), sector_num);

            if (write_status == true) {
#if 0//def __P_PROPRIETARY_COPYRIGHT__
                USB_Ms_Update_Read_Protect(g_UsbMS.current_LUN, (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), sector_num);
#endif
                g_UsbMS.rw_cmd.msdc_rwindex += sector_num;

            } else {
                g_UsbMS.rw_cmd.rw_error_status = true;

            }
        }

        /* RX done */
        NVIC_DisableIRQ(USB20_IRQn);//IRQMask(IRQ_USB_CODE);
        if (g_UsbMS.nState != USBMS_IDLE) {

            g_UsbMS.nState = USBMS_IDLE;
            /*Send CSW*/
            g_UsbMS.CSWDataResidue = (g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.msdc_rwindex) * 512;
            USB_Ms_Checkmedia_Exist(g_UsbMS.current_LUN);

            if (g_UsbMS.CSWDataResidue != 0) {
                g_UsbMS.CSWStatusError = 0x01;
            }

            g_UsbMS.rw_cmd.rw_error_status = false;
            /* Switch RX endpoint state to use FIFO */
            if (g_UsbMS.ms_read_write_reset == false) {
                USB_Ms_Build_Tx_CWS(&g_UsbMS_CSW, CBW);
            }
        }
        NVIC_EnableIRQ(USB20_IRQn);
    } else {	/* more data to receive, or ready to receive the first one data packet*/
#if 1
        /* determine the length to be received*/
        if ((g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex) <= USBMS_RX_MAX_SECTOR) {
            writelen = g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex;

        } else {
            writelen = USBMS_RX_MAX_SECTOR;
        }

        USB_Ms_BuildRx((void *)g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index], (512 * writelen));
        g_UsbMS.rw_cmd.rwindex += writelen;

        /* write flash while receive another packet*/
        g_UsbMS.rw_cmd.rw_buffer_index ^= 1;  /*  toggle buffer index */
#endif

        if (write_data == true) {
            write_status = USB_Ms_Read_Write(USB_MS_WRITE, g_UsbMS.current_LUN, g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index],
                                             (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), sector_num);
            if (write_status == true) {
#if 0//def __P_PROPRIETARY_COPYRIGHT__
                USB_Ms_Update_Read_Protect(g_UsbMS.current_LUN, (g_UsbMS.rw_cmd.LBA + g_UsbMS.rw_cmd.msdc_rwindex), sector_num);
#endif
                g_UsbMS.rw_cmd.msdc_rwindex += sector_num;

            } else {
                g_UsbMS.rw_cmd.rw_error_status = true;

            }
        }
#if 0
        /* Determine the length to be received */
        if ((g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex) <= USBMS_RX_MAX_SECTOR) {
            writelen = g_UsbMS.rw_cmd.BlkLen - g_UsbMS.rw_cmd.rwindex;

        } else {
            writelen = USBMS_RX_MAX_SECTOR;
        }

        USB_Ms_BuildRx((void *)g_UsbMS.disk_buffer->ms_buffer[g_UsbMS.rw_cmd.rw_buffer_index], (512 * writelen));
        g_UsbMS.rw_cmd.rwindex += writelen;
#endif
    }
}


#if 0//def __P_PROPRIETARY_COPYRIGHT__

/* translate fd type to usb dev type*/
USB_STORAGE_DEV_TYPE USB_Ms_Trans_Fs_Type(FS_DEVICE_TYPE_ENUM type)
{
    USB_STORAGE_DEV_TYPE dev_type;

    switch (type) {
        case FS_DEVICE_TYPE_NOR:
            dev_type = USB_STORAGE_DEV_NOR;
            break;
        case FS_DEVICE_TYPE_NAND:
            dev_type = USB_STORAGE_DEV_NAND;
            break;
        case FS_DEVICE_TYPE_CARD:
            dev_type = USB_STORAGE_DEV_CARD;
            break;
        default:
            //EXT_ASSERT(0, (uint32_t)type, 0, 0);

            LOG_E(hal, "EXT_ASSERT\n");
            break;
    }
    return dev_type;
}


/* The return value means whether the item is found or not. parameter *p_index is the item index in this array*/
/* If the return value is false, no item is found, and *p_index is the first one just bigger than searched one*/
/* If all items in this array are bigger than the searched one, *p_index will be larger than array size*/
static bool	USB_Ms_Search_Item(uint32_t *data_items, uint32_t data_size,
                                   uint32_t search_value, uint32_t *p_index)
{
    signed long long low_index = -1;
    signed long long high_index = (signed long long)data_size;
    signed long long index = 0;
    bool	ret_value = false;

    if (data_size == 0) {
        *p_index = 0;
        return false;
    }

    while ((high_index - low_index) > 1) {
        index = (high_index + low_index) / 2;
        if (search_value == data_items[index]) {
            high_index = index;
            low_index = index;
        } else if (search_value < data_items[index]) {
            high_index = index;
        } else {
            low_index = index;
        }
    }

    if (search_value == data_items[index]) {
        ret_value = true;
    } else {
        ret_value = false;
        if (search_value > data_items[index]) {
            index++;
        }
        /* If search value is larger than any one, index will be eqaul to data_size but data_size[data_size] not exist*/
        if ((index < data_size) && (search_value > data_items[index]))
            //EXT_ASSERT(0, search_value, data_items[index], (uint32_t)index);
        {
            LOG_E(hal, "EXT_ASSERT\n");
        }
    }

    *p_index = index;
    return ret_value;
}


/* Sort the protected items got from file system */
void USB_Ms_Sort_Protect_Items(uint32_t *data_item, uint32_t *second_data_item)
{
    uint32_t index = 0;
    uint32_t *p_sort_buff = (uint32_t *)g_UsbMS.disk_buffer;
    bool	b_found = false;
    uint32_t find_index;

    /* After sorting, data item is sorted array*/
    g_UsbMS.read_protect_num = data_item[0];

    /* sorting data_item*/
    data_item[0] = data_item[1];
    second_data_item[0] = second_data_item[1];

    /* sort list 1*/
    for (index = 1; index < g_UsbMS.read_protect_num; index++) {
        b_found = USB_Ms_Search_Item(data_item, index, data_item[index + 1], &find_index);
        /* Should not have two same itmes*/
        if (b_found == true)
            //EXT_ASSERT(0, index, g_UsbMS.read_protect_num, data_item[index+1]);
        {
            LOG_E(hal, "EXT_ASSERT\n");
        }
        /* update list 1*/
        memcpy(p_sort_buff, &data_item[find_index], (index - find_index)*sizeof(uint32_t));
        data_item[find_index] = data_item[index + 1];
        memcpy(&data_item[find_index + 1], p_sort_buff, (index - find_index)*sizeof(uint32_t));

        /* update list 2*/
        memcpy(p_sort_buff, &second_data_item[find_index], (index - find_index)*sizeof(uint32_t));
        second_data_item[find_index] = second_data_item[index + 1];
        memcpy(&second_data_item[find_index + 1], p_sort_buff, (index - find_index)*sizeof(uint32_t));
    }

    /* initailize index 1 and index 2*/
    for (index = 0; index < g_UsbMS.read_protect_num; index++) {
        g_UsbMS.read_protect_index[index] = index;
    }
    for (index = 0; index < g_UsbMS.read_protect_num; index++) {
        g_UsbMS.read_protect_second_index[index] = index;
    }

    /* sort list 2 and update index 2*/
    for (index = 1; index < g_UsbMS.read_protect_num; index++) {
        b_found = USB_Ms_Search_Item(second_data_item, index, second_data_item[index], &find_index);
        /* If the file dose not have second cluster, the values are all 0xffffffff*/
        //if(b_found==true)
        //	EXT_ASSERT(0, index, g_UsbMS.read_protect_num, second_data_item[index]);
        /* update list 2 */
        memcpy(p_sort_buff, &second_data_item[find_index], (index - find_index)*sizeof(uint32_t));
        second_data_item[find_index] = second_data_item[index];
        memcpy(&second_data_item[find_index + 1], p_sort_buff, (index - find_index)*sizeof(uint32_t));

        /* update index 2 */
        memcpy(p_sort_buff, &g_UsbMS.read_protect_second_index[find_index], (index - find_index)*sizeof(uint32_t));
        g_UsbMS.read_protect_second_index[find_index] = g_UsbMS.read_protect_second_index[index];
        memcpy(&g_UsbMS.read_protect_second_index[find_index + 1], p_sort_buff, (index - find_index)*sizeof(uint32_t));
    }

    /* update index1 */
    for (index = 0; index < g_UsbMS.read_protect_num; index++) {
        g_UsbMS.read_protect_index[g_UsbMS.read_protect_second_index[index]] = index;
    }

    //g_UsbMS.read_protect_items = data_item;
    //g_UsbMS.read_protect_second_items = second_data_item;
}


/* This is used in run time for sorting index list */
static void USB_Ms_Sort_Index_Items(uint32_t *items, uint32_t num)
{
    uint32_t index = 0;
    bool	b_found = false;
    uint32_t find_index;
    uint32_t sort_index;
    uint32_t sort_data;

    if (num == 1) {
        return;
    }

    /* sort list*/
    for (index = 1; index < num; index++) {
        b_found = USB_Ms_Search_Item(items, index, items[index], &find_index);
        /* Should not have two same itmes*/
        if (b_found == true)
            //EXT_ASSERT(0, index, num, items[index]);
        {
            LOG_E(hal, "EXT_ASSERT\n");
        }

        /* update list 1*/
        sort_data = items[index];
        for (sort_index = (index - 1); sort_index >= find_index; sort_index--) {
            items[sort_index + 1] = items[sort_index];
            if (sort_index == 0) {
                break;
            }
        }
        items[find_index] = sort_data;
    }
}


/* check if it is read protected sectors */
static bool	USB_Ms_Check_Read_Protect(uint8_t LUN, uint32_t LBA, uint32_t sec_num)
{
    uint32_t index;
    bool b_found = false;

    if (USB_Ms_Dev_Type(LUN) == g_UsbMS.read_protect_dev_type) {
        b_found = USB_Ms_Search_Item(g_UsbMS.read_protect_items, g_UsbMS.read_protect_num, LBA, &index);

        if (b_found == false) {
            /* If match this conditino, read should also be protected */
            if ((index < g_UsbMS.read_protect_num) && ((LBA + sec_num - 1) >= g_UsbMS.read_protect_items[index])) {
                b_found = true;
            }
        }
    }

    return b_found;
}


/* check if it is second read protected sectors */
static bool	USB_Ms_Check_Second_Read_Protect(uint8_t LUN, uint32_t LBA, uint32_t sec_num)
{
    uint32_t index;
    bool b_found = false;

    if (USB_Ms_Dev_Type(LUN) == g_UsbMS.read_protect_dev_type) {
        b_found = USB_Ms_Search_Item(g_UsbMS.read_protect_second_items, g_UsbMS.read_protect_num, LBA, &index);

        if (b_found == false) {
            /* If match this conditino, read should also be protected */
            if ((index < g_UsbMS.read_protect_num) && ((LBA + sec_num - 1) >= g_UsbMS.read_protect_second_items[index])) {
                b_found = true;
            }
        }
    }

    return b_found;
}


/* update read protect list when wrtiting these sectors */
static void USB_Ms_Update_Read_Protect(uint8_t LUN, uint32_t LBA, uint32_t sec_num)
{
    uint32_t index_min, index_max;
    bool b_found_min = false;
    bool b_found_max = false;
    uint32_t update_length;
    uint32_t index, index_2;
    uint32_t protect_num;

    /* If media type is not removable, update the read protect region if it falls in this region */
    if (USB_Ms_Dev_Type(LUN) == g_UsbMS.read_protect_dev_type) {
        /*******************************/
        /*******update the first list*********/
        /*******************************/
        b_found_min = USB_Ms_Search_Item(g_UsbMS.read_protect_items, g_UsbMS.read_protect_num, LBA, &index_min);
        b_found_max = USB_Ms_Search_Item(g_UsbMS.read_protect_items, g_UsbMS.read_protect_num, LBA + sec_num - 1, &index_max);

        update_length = index_max - index_min;
        if (b_found_max == true) {
            update_length++;
            index_max++;
        }

        if (update_length > 0) {
            /****** update list 2 and index 2 *****/
            protect_num = g_UsbMS.read_protect_num;

            /* determine index 2 id */
            for (index = 0; index < update_length; index++) {
                /* decrement index 2*/
                for (index_2 = 0; index_2 < protect_num; index_2++) {
                    if (g_UsbMS.read_protect_second_index[index_2] >
                            g_UsbMS.read_protect_second_index[g_UsbMS.read_protect_index[index_min + index]]) {
                        g_UsbMS.read_protect_second_index[index_2]--;
                    }
                }
            }
            /* *sort index list, and delete list2/index2 by sequence */
            USB_Ms_Sort_Index_Items(&g_UsbMS.read_protect_index[index_min], update_length);
            for (index = 0; index < update_length; index++) {
                memcpy(&g_UsbMS.read_protect_second_items[g_UsbMS.read_protect_index[index_min + index] - index],
                            &g_UsbMS.read_protect_second_items[g_UsbMS.read_protect_index[index_min + index] - index + 1],
                            (protect_num - (g_UsbMS.read_protect_index[index_min + index] - index) - 1)*sizeof(uint32_t));
                memcpy(&g_UsbMS.read_protect_second_index[g_UsbMS.read_protect_index[index_min + index] - index],
                            &g_UsbMS.read_protect_second_index[g_UsbMS.read_protect_index[index_min + index] - index + 1],
                            (protect_num - (g_UsbMS.read_protect_index[index_min + index] - index) - 1)*sizeof(uint32_t));
                protect_num--;
            }


            /****** update list 1 and index 1 *****/
            protect_num = g_UsbMS.read_protect_num;

            /* determine index 1 id */
            for (index = 0; index < update_length; index++) {
                /* decrement index 1*/
                for (index_2 = 0; index_2 < protect_num; index_2++) {
                    if (g_UsbMS.read_protect_index[index_2] > g_UsbMS.read_protect_index[index_min + index]) {
                        g_UsbMS.read_protect_index[index_2]--;
                    }
                }
            }

            /* delete list1/index1 */
            memcpy(&g_UsbMS.read_protect_items[index_min], &g_UsbMS.read_protect_items[index_max],
                        (g_UsbMS.read_protect_num - index_max)*sizeof(uint32_t));
            memcpy(&g_UsbMS.read_protect_index[index_min], &g_UsbMS.read_protect_index[index_max],
                        (g_UsbMS.read_protect_num - index_max)*sizeof(uint32_t));

            g_UsbMS.read_protect_num -= update_length;
        }


        /*******************************/
        /*******update the second list******/
        /*******************************/
        b_found_min = USB_Ms_Search_Item(g_UsbMS.read_protect_second_items, g_UsbMS.read_protect_num, LBA, &index_min);
        b_found_max = USB_Ms_Search_Item(g_UsbMS.read_protect_second_items, g_UsbMS.read_protect_num, LBA + sec_num - 1, &index_max);

        update_length = index_max - index_min;
        if (b_found_max == true) {
            update_length++;
            index_max++;
        }

        if (update_length > 0) {
            /****** update list 1 and index 1 *****/
            protect_num = g_UsbMS.read_protect_num;

            /* determine index 1 id */
            for (index = 0; index < update_length; index++) {
                /* decrement index 1*/
                for (index_2 = 0; index_2 < protect_num; index_2++) {
                    if (g_UsbMS.read_protect_index[index_2] >
                            g_UsbMS.read_protect_index[g_UsbMS.read_protect_second_index[index_min + index]]) {
                        g_UsbMS.read_protect_index[index_2]--;
                    }
                }
            }
            /* *sort index list, and delete list1/index1 by sequence */
            USB_Ms_Sort_Index_Items(&g_UsbMS.read_protect_second_index[index_min], update_length);
            for (index = 0; index < update_length; index++) {
                memcpy(&g_UsbMS.read_protect_items[g_UsbMS.read_protect_second_index[index_min + index] - index],
                            &g_UsbMS.read_protect_items[g_UsbMS.read_protect_second_index[index_min + index] - index + 1],
                            (protect_num - (g_UsbMS.read_protect_second_index[index_min + index] - index) - 1)*sizeof(uint32_t));
                memcpy(&g_UsbMS.read_protect_index[g_UsbMS.read_protect_second_index[index_min + index] - index],
                            &g_UsbMS.read_protect_index[g_UsbMS.read_protect_second_index[index_min + index] - index + 1],
                            (protect_num - (g_UsbMS.read_protect_second_index[index_min + index] - index) - 1)*sizeof(uint32_t));
                protect_num--;
            }

            /****** update list 2 and index 2 *****/
            protect_num = g_UsbMS.read_protect_num;

            /* determine index 2 id */
            for (index = 0; index < update_length; index++) {
                /* decrement index 2*/
                for (index_2 = 0; index_2 < protect_num; index_2++) {
                    if (g_UsbMS.read_protect_second_index[index_2] > g_UsbMS.read_protect_second_index[index_min + index]) {
                        g_UsbMS.read_protect_second_index[index_2]--;
                    }
                }
            }

            /* delete list2/index2 */
            memcpy(&g_UsbMS.read_protect_second_items[index_min], &g_UsbMS.read_protect_second_items[index_max],
                        (g_UsbMS.read_protect_num - index_max)*sizeof(uint32_t));
            memcpy(&g_UsbMS.read_protect_second_index[index_min], &g_UsbMS.read_protect_second_index[index_max],
                        (g_UsbMS.read_protect_num - index_max)*sizeof(uint32_t));

            g_UsbMS.read_protect_num -= update_length;
        }
    }
}

#endif /* __P_PROPRIETARY_COPYRIGHT__ */

#endif //__USB_MASS_STORAGE_ENABLE__
