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

#include "hal_platform.h"

#if defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED)

#if defined(MTK_VM_LOG_ENABLE)

#include "hal_audio_fw_sherif.h"
#include "hal_audio_internal_afe.h"
#include "hal_audio_internal_vm.h"

#include "assert.h"
#include <string.h>

epl_frame_t eplfrm_t;
vm_frame_t vmfrm_t;

static uint16_t vmCodecBuf[VM_BUFFER_SIZE];
static uint16_t eplInputBuf[FIVE_PCM_SAVE_SIZE];


#include "ff.h"
FATFS vm_fatfs;
FIL vm_file_handle;


#include "serial_port.h"
serial_port_open_para_t vm_port_open_para;
static serial_port_handle_t vm_log_port_handle;

#include "hal_gpt.h"

uint8_t   vm_counter;

static struct {
    uint16_t        control;    /* control word for FR/HR/EFR                         */
    uint16_t        control_1;
    uint16_t        control_2;
    uint16_t        *vmBuf;    /* point to buffer for VM                              */
    uint16_t        *pcmBuf;   /* point to buffer for PCM data         */
    uint16_t        debug_info; /* 0  : vm (speech codec data) only                   */
    /* b1 : record 1st set of UL-DL PCM buffer for SPE    */
    /* b2 : record 2nd set of UL-DL PCM buffer for SPE    */
    uint8_t         state;
    uint8_t         vm_lost;
    uint8_t         vm_lost_count;
    uint8_t         pcm_lost_count;
    uint8_t         sc_mode;
    uint8_t         sd_mode;

    bool            isVmLOn; // only use under call AM_IsSpeechOn()
    uint32_t        pcm_save_size; // EPL PCM buffer size
    vm_save_or_print_method_t   save_or_print_method;
} vm;

static const uint8_t speech_pattern_length[] = {
    (uint8_t)((260 + 15) / 16),           /*  FR       */
    (uint8_t)((112 + 15) / 16 + 1),       /*  HR       */
    (uint8_t)((244 + 15) / 16),           /* EFR       */
    (uint8_t)((244 + 15) / 16),           /* AMR 12.2K */
    (uint8_t)((204 + 15) / 16),           /* AMR 10.2K */
    (uint8_t)((159 + 15) / 16),           /* AMR 7.95K */
    (uint8_t)((148 + 15) / 16),           /* AMR 7.4K  */
    (uint8_t)((134 + 15) / 16),           /* AMR 6.7K  */
    (uint8_t)((118 + 15) / 16),           /* AMR 5.9K  */
    (uint8_t)((103 + 15) / 16),           /* AMR 5.15K */
    (uint8_t)((95 + 15) / 16),            /* AMR 4.75K */
    (uint8_t)((132 + 15) / 16),           /* AMR-WB 6.60  */
    (uint8_t)((177 + 15) / 16),           /* AMR-WB 8.85  */
    (uint8_t)((253 + 15) / 16),           /* AMR-WB 12.65 */
    (uint8_t)((285 + 15) / 16),           /* AMR-WB 14.25 */
    (uint8_t)((317 + 15) / 16),           /* AMR-WB 15.85 */
    (uint8_t)((365 + 15) / 16),           /* AMR-WB 18.25 */
    (uint8_t)((397 + 15) / 16),           /* AMR-WB 19.85 */
    (uint8_t)((461 + 15) / 16),           /* AMR-WB 23.05 */
    (uint8_t)((477 + 15) / 16),           /* AMR-WB 23.85 */
};


static void vm_port_service_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    switch(event) {
        case SERIAL_PORT_EVENT_READY_TO_READ:
            log_hal_info("vm_port_service_callback: SERIAL_PORT_EVENT_READY_TO_READ");
        break;

        case SERIAL_PORT_EVENT_READY_TO_WRITE:
            log_hal_info("vm_port_service_callback: SERIAL_PORT_EVENT_READY_TO_WRITE");
        break;

        case SERIAL_PORT_EVENT_USB_CONNECTION:
            log_hal_info("vm_port_service_callback: SERIAL_PORT_EVENT_USB_CONNECTION");
        break;

        case SERIAL_PORT_EVENT_USB_DISCONNECTION:
            log_hal_info("vm_port_service_callback: SERIAL_PORT_EVENT_USB_DISCONNECTION");
        break;

        default:
        break;
    }
}


static uint16_t vm_get_speech_pattern_length(uint8_t speech_mode)
{
    if (speech_mode >= 0x20) {
        speech_mode = speech_mode - 0x20 + 0x0B;
    }
    if (speech_mode < (sizeof(speech_pattern_length) / sizeof(speech_pattern_length[0]))) {
        return (uint16_t)speech_pattern_length[speech_mode];
    } else {
        return 0;
    }
}

#define AA88_GET_VM_LENGTH(vmctrl_1, vmctrl_2) vm_get_speech_pattern_length((vmctrl_1>> 1) & 0x3F) \
+ ((vmctrl_2 & 1)?(vm_get_speech_pattern_length((vmctrl_2 >> 1) & 0x3F)):0) + 7

static bool vm_write_to_file(uint32_t len, uint16_t *pBuf)
{
    if (vm.save_or_print_method == VM_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
        UINT written;
        uint8_t *tempPtr;
        FRESULT result;

        len = len * sizeof(uint16_t);
        written = 0;
        tempPtr = (uint8_t *)(pBuf);

        while (len > 0) {
            result = f_write(&vm_file_handle, tempPtr + written, len, &written);
            if (FR_OK != result) {
                return false;
            }
            tempPtr += written;
            len = len - written;
        }
    } else if (vm.save_or_print_method == VM_SAVE_OR_PRINT_METHOD_WRITE_USB_DEBUG_PORT){
        serial_port_status_t status;
        serial_port_ctrl_para_t send_data;

        send_data.serial_port_write_data.data = (uint8_t*)pBuf;
        send_data.serial_port_write_data.size = len * sizeof(uint16_t);

        uint32_t send_fail_count = 0;
        while(1) {
            status = serial_port_control(vm_log_port_handle, SERIAL_PORT_CMD_WRITE_DATA, &send_data);
            if (status == SERIAL_PORT_STATUS_OK) {
                //log_hal_info("send vm log SERIAL_PORT_STATUS_OK, status=%d, send_data.serial_port_write_data.size=%d, send_data.serial_port_write_data.ret_size=%d\r\n", status, send_data.serial_port_write_data.size, send_data.serial_port_write_data.ret_size);
                send_data.serial_port_write_data.size -= send_data.serial_port_write_data.ret_size;
                send_data.serial_port_write_data.data += send_data.serial_port_write_data.ret_size;
            } else {
                //log_hal_info("send vm log fail, status=%d, send_data.serial_port_write_data.size=%d, send_data.serial_port_write_data.ret_size=%d\r\n", status, send_data.serial_port_write_data.size, send_data.serial_port_write_data.ret_size);

                // usb buffer may full, add delay.
                hal_gpt_delay_us(200);

                if (send_fail_count > 10) {
                    log_hal_info("send_fail_count > 10, break, it will miss data\r\n");
                    break;
                }

                send_fail_count++;
            }

            if (send_data.serial_port_write_data.size == 0) {
                log_hal_info("send vm log successed\r\n");
                break;
            }
        }
    }

    return true;
}


static void vm_get_frame(uint8_t rat, vm_frame_t *vmfrm)
{
    vmfrm->enc_mode     = *DSP_SPH_Encoder_Used_Mode;
    vmfrm->dec_mode     = *DSP_SPH_Decoder_Used_Mode;
    vmfrm->dbgInfo_addr = DSP_SPH_VM_DBG_INFO;

    if (rat == 0) {
        // 2g
        vmfrm->enc_hdr     = *DSP_SPH_2G_SE_DATA_HDR;
        vmfrm->enc_hb_addr = DSP_SPH_2G_SE_DATA_HB;
        vmfrm->dec_hdr     = *DSP_SPH_2G_SD_DATA_HDR;
        vmfrm->dec_hb_addr = DSP_SPH_2G_SD_DATA_HB;
    } else if (rat == 1) {
        // 3g
        vmfrm->enc_hdr     = *DSP_SPH_3G_SE_DATA_HDR;
        vmfrm->enc_hb_addr = DSP_SPH_3G_SE_DATA_HB;
        vmfrm->dec_hdr     = *DSP_SPH_3G_SD_DATA_HDR;
        vmfrm->dec_hb_addr = DSP_SPH_3G_SD_DATA_HB;
    }

    vmfrm->enh_dbgInfo_addr = 0;    //DSP_DM_ADDR(6, *DSP_SPH_ENH_DEBUG_ADDR);

    return;
}

static void audio_vm_record_hisr(void *param)
{

    volatile uint16_t   *addr;

    uint16_t sc_mode, sd_mode, sc_len, sd_len;
    uint16_t *vmBuf;
    uint32_t I, J;

    if (vm.state != VM_STATE_RECORD) {
        return;
    }
    vmBuf = vm.vmBuf;

    if (vmBuf[0] == VM_MAGIC_HEADER) {
        // Already buffer VM
        I = (uint16_t)(vmBuf[1] >> 3)
            + AA88_GET_VM_LENGTH(vm.control_1, vm.control_2);
        // + vm_get_speech_pattern_length((vmBuf[5]>> 1) & 0x3F)
        // + ((vm.control_2 & 1)?(vm_get_speech_pattern_length((vmBuf[6] >> 1) & 0x3F)):0)
        // + 7; // vm header len

        // write
        if (false == vm_write_to_file(I, vm.vmBuf))	{
            vm.vm_lost_count ++;
        } else {
            vm.vm_lost_count = 0;
        }

        vm.vmBuf[0] = 0;
        vm.vmBuf[1] = 0;
        vm.vmBuf[2] = 0;
    }


    // begin put information to vmBuf
    vmBuf = vm.vmBuf;

    *vmBuf = VM_MAGIC_HEADER;
    vmBuf++;

    J = 0;
    *vmBuf = ((uint16_t)(((VM_DBGINFO_LEN + VM_ENH_DBGINFO_LEN + VM_DRV_DBGINFO_LEN) << 3) | J));
    vmBuf++;
    *vmBuf = VM_VM_RECORD_FLAG;
    vmBuf++;


    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &J);
    J = (J >> 10); // i.e. j= j/1024 around ms
    *vmBuf = ((uint16_t)(J & 0xFFFF));
    vmBuf++;
    *vmBuf = ((uint16_t)((J >> 16) + ((uint32_t)vm_counter << 8)));
    vmBuf++;

    vm_counter++;
    // ======= end ======

    vm_get_frame(0, &vmfrm_t);
    sc_mode = vmfrm_t.enc_mode;
    sd_mode = vmfrm_t.dec_mode;

    assert((sc_len = vm_get_speech_pattern_length(sc_mode)) > 0);
    assert((sd_len = vm_get_speech_pattern_length(sd_mode)) > 0);

    vm.control_1 = (sc_mode << 1) | (vm.control_1 & 1);
    vm.control_2 = (sd_mode << 1) | (vm.control_2 & 1);


    if (sc_mode > 2) {
        // I = vmfrm_t.enc_hdr & 3;//bit0, bit1
        vm.control_1 = vm.control_1 | ((vmfrm_t.enc_hdr & 3) << 7); //Tx
    } else {
        // I = (vmfrm_t.enc_hdr & 2);//sp_flag
        vm.control_1 = vm.control_1 | ((vmfrm_t.enc_hdr & 2) << 10);
        //add 3G_Mode here, where is dsp's 3g mode indicator
    }


    if (sd_mode > 2) {
        // I = (vmfrm_t.dec_hdr & 0xe) >> 1;//bit1, bit2, bit3
        vm.control_2 = vm.control_2 | (((vmfrm_t.dec_hdr & 0xe) >> 1) << 7); // Rx
    } else {
        // I = (vmfrm_t.dec_hdr & 0x3E);
        vm.control_2 = vm.control_2 | ((vmfrm_t.dec_hdr & 0x3E) << 10);
    }

    *vmBuf++ = vm.control_1;
    *vmBuf++ = vm.control_2;


    if (vm.control_1 & 1) {
        addr = vmfrm_t.enc_hb_addr;
        // vmLen += sc_len;
        for (I = 0; I < sc_len; I++) {
            *vmBuf++ = *addr++;
        }
    }

    if (vm.control_2 & 1) {
        addr = vmfrm_t.dec_hb_addr;
        // vmLen += sc_len;
        for (I = 0; I < sd_len; I++) {
            *vmBuf++ = *addr++;
        }
    }

    addr = vmfrm_t.dbgInfo_addr;
    for (I = 0; I < VM_DBGINFO_LEN; I++) {
        *vmBuf++ = *addr++;
    }

    //addr = vmfrm_t.enh_dbgInfo_addr;
    for (I = 0; I < VM_ENH_DBGINFO_LEN; I++) {
        *vmBuf++ = 0;
    }

    for (I = 0; I < VM_DRV_DBGINFO_LEN; I++) {
        *vmBuf++ = 0;
    }
}


static void epl_get_frame(epl_frame_t *eplfrm)
{
    eplfrm->ul_pre_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_UL_PRE_BUF);
    eplfrm->ul_pos_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_UL_POS_BUF);
    eplfrm->dl_pre_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_DL_PRE_BUF);
    eplfrm->dl_pos_buf  = DSP_DM_ADDR(7, *DSP_SPH_EPL_DL_POS_BUF);
    eplfrm->ul2_pos_buf = DSP_DM_ADDR(5, *DSP_DM_ADDR(6, DSP_SPH_SE2_PTR));

    eplfrm->ul_pre_len  = *DSP_SPH_EPL_BND & 0x0001 ? 320 : 160;
    eplfrm->ul_pos_len  = *DSP_SPH_EPL_BND & 0x0002 ? 320 : 160;
    eplfrm->dl_pre_len  = *DSP_SPH_EPL_BND & 0x0004 ? 320 : 160;
    eplfrm->dl_pos_len  = *DSP_SPH_EPL_BND & 0x0008 ? 320 : 160;
    eplfrm->ul2_pos_len = *DSP_SPH_EPL_BND & 0x0010 ? 320 : 160;

    return;
}


static void audio_vm_epl_record_hisr(void *param)
{
    uint32_t I, tmp = 0;
    uint16_t *buf;
    uint16_t pcmsize = 0;
    uint16_t *pcmBuf;

    if (vm.state != VM_STATE_RECORD) {
        return;
    }

    // get the size
    epl_get_frame(&eplfrm_t);
    pcmsize += eplfrm_t.ul_pre_len + eplfrm_t.dl_pre_len;
    pcmsize += eplfrm_t.ul_pos_len + eplfrm_t.dl_pos_len;

    // for second mic
    // pcmsize += eplfrm_t.ul2_pos_len;

    // vm Buf header process
    buf = vm.vmBuf;
    if (buf[0] == VM_MAGIC_HEADER) {
        // Already buffer VM
        tmp = (buf[1] >> 3) + pcmsize;  // Debug size
        I = tmp + AA88_GET_VM_LENGTH(vm.control_1, vm.control_2);
    } else {
        tmp = pcmsize;          // Debug size(only PCM)
        I = pcmsize + 3 + 2;    // Plus sync word and format and 2 timestamp
    }
    I -= pcmsize; // vm header + en/de debugInfo

    buf[2] &= 0xfe0f;   // Clean the epl band flag
    if (eplfrm_t.ul_pre_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_UL_PRE;
    }

    if (eplfrm_t.ul_pos_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_UL_POS;
    }

    if (eplfrm_t.dl_pre_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_DL_PRE;
    }

    if (eplfrm_t.dl_pos_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_DL_POS;
    }

    if (eplfrm_t.ul2_pos_len == 320) {
        buf[2] |= VM_PCM_BAND_FLAG_UL2_POS;
    }

    // EPL PCM buffer handling
    pcmBuf = vm.pcmBuf;

    buf[0] = VM_MAGIC_HEADER;
    buf[1] = (uint16_t)((VM_DBGINFO_LEN + VM_ENH_DBGINFO_LEN + VM_DRV_DBGINFO_LEN) << 3) | 0;
    buf[2] |= (vm.debug_info & (VM_PCM_REFMIC_RECORD_FLAG + VM_PCM_1ST_SET_RECORD_FLAG + VM_PCM_2ND_SET_RECORD_FLAG));

    vm_write_to_file(I, buf);

    // Header for record 1st set of UL-DL PCM data
    *pcmBuf++ = (vm.vm_lost_count << 8) + vm.pcm_lost_count;
    *pcmBuf++ = vm_counter;

    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.ul_pre_buf, eplfrm_t.ul_pre_len);	// Uplink
    pcmBuf += (eplfrm_t.ul_pre_len);
    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.dl_pre_buf, eplfrm_t.dl_pre_len);	// Downlink
    pcmBuf += (eplfrm_t.dl_pre_len);

    // Header for record 2nd set of UL-DL PCM data
    *pcmBuf++ = (vm.vm_lost_count << 8) + vm.pcm_lost_count;
    *pcmBuf++ = vm_counter;

    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.ul_pos_buf, eplfrm_t.ul_pos_len);	// Uplink
    pcmBuf += (eplfrm_t.ul_pos_len);
    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.dl_pos_buf, eplfrm_t.dl_pos_len);	// Downlink
    pcmBuf += (eplfrm_t.dl_pos_len);

    // Header for second mic
    *pcmBuf++ = (vm.vm_lost_count << 8) + vm.pcm_lost_count;
    *pcmBuf++ = vm_counter ;

    audio_idma_read_from_dsp(pcmBuf, eplfrm_t.ul2_pos_buf, eplfrm_t.ul2_pos_len);	// Uplink2
    pcmBuf += (eplfrm_t.ul2_pos_len);

    *pcmBuf++ = *DSP_SPH_AGC_SW_GAIN1;
    *pcmBuf++ = *DSP_SPH_AGC_SW_GAIN2;

    // write all
    vm_write_to_file((2 + eplfrm_t.ul_pre_len + eplfrm_t.dl_pre_len + 2 + eplfrm_t.ul_pos_len + eplfrm_t.dl_pos_len + 4 + eplfrm_t.ul2_pos_len), vm.pcmBuf);

    // Reset
    vm.vmBuf[0] = 0;
    vm.vmBuf[1] = 0;
    vm.vmBuf[2] = 0;
    vm.pcm_lost_count = 0;

    return;
}


void audio_vm_Start(vm_save_or_print_method_t save_or_print_method)
{
    log_hal_info("%s() enter", __func__);

    if (VM_STATE_RECORD == vm.state) { //already on
        return;
    }

    assert(true == afe_is_audio_enable());


    if (save_or_print_method == VM_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
        // open file handler
        FRESULT result;

        result = f_mount(&vm_fatfs, _T("0:"), 1);
        if (!result) {
            log_hal_info("fmount ok \n");
            result = f_open(&(vm_file_handle), _T("SD:/VMLOG.vm"), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
            if (!result) {
                log_hal_info("fopen ok \n");
            } else {
                log_hal_info("fopen error \n");
                return;
            }
        } else {
            log_hal_info("fmount error \n");
            return;
        }

        vm.save_or_print_method = VM_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE;
    } else if (save_or_print_method == VM_SAVE_OR_PRINT_METHOD_WRITE_USB_DEBUG_PORT){
        vm_port_open_para.callback = vm_port_service_callback;
        if (serial_port_open(SERIAL_PORT_DEV_USB_COM2, &vm_port_open_para, &vm_log_port_handle) != SERIAL_PORT_STATUS_OK) {
            log_hal_info("serial_port_open open fail\r\n");
            return;
        }

        vm.save_or_print_method = VM_SAVE_OR_PRINT_METHOD_WRITE_USB_DEBUG_PORT;

    }


    vm.state = VM_STATE_RECORD;

    vm.vmBuf = vmCodecBuf;
    memset(vm.vmBuf, 0, sizeof(uint16_t)*VM_BUFFER_SIZE);

    vm.control = 0x0003;
    vm.control_1 = 0x0001;
    vm.control_2 = 0x0001;
    vm.debug_info = 0x0F;

    // config EPLs buffer
    vm.pcm_save_size = FIVE_PCM_SAVE_SIZE;
    memset(eplInputBuf, 0 , sizeof(uint16_t)*vm.pcm_save_size);
    vm.pcmBuf = eplInputBuf;
    vm.pcm_lost_count = 0;
    // vm.vm_lost  = 1; // to force save VM first
    vm.vm_lost_count = 0;

    // turn on and hook hisr
    audio_service_hook_isr(DSP_VMEPL_REC_INT, audio_vm_epl_record_hisr, 0);
    *DSP_SPH_PCM_REC_CTRL |= 0x0002;

    audio_service_hook_isr(DSP_VM_REC_INT, audio_vm_record_hisr, 0);
    *DSP_SPH_COD_CTRL |= 0x0800;
    *DSP_SPH_SCH_IMPROVE_CTRL |= 0x08;

    log_hal_info("%s() done", __func__);

}


void audio_vm_Stop(void)
{
    log_hal_info("%s() enter", __func__);

    if (VM_STATE_RECORD == vm.state) {
        vm.state = VM_STATE_RECORD_STOP;
    } else {
        log_hal_info("%s(): stop without start", __func__);
        return;
    }

    assert(true == afe_is_audio_enable());

    // turn off
    *DSP_SPH_PCM_REC_CTRL &= ~0x0002;
    audio_service_unhook_isr(DSP_VMEPL_REC_INT);

    *DSP_SPH_COD_CTRL &= ~0x0800;
    *DSP_SPH_SCH_IMPROVE_CTRL &= ~0x08;
    audio_service_unhook_isr(DSP_VM_REC_INT);

    // buffer clean
    vm.pcmBuf = NULL;
    vm.vmBuf = NULL;
    vm.debug_info = 0;

    if (vm.save_or_print_method == VM_SAVE_OR_PRINT_METHOD_WRITE_TO_FILE) {
        FRESULT result;

        // close file
        result = f_close(&vm_file_handle);
        if (!result) {
            log_hal_info("fclose success \n");
        } else {
            log_hal_info("fclose error \n");
        }
    }

    if (vm.save_or_print_method == VM_SAVE_OR_PRINT_METHOD_WRITE_USB_DEBUG_PORT) {
        if (serial_port_close(vm_log_port_handle) != SERIAL_PORT_STATUS_OK) {
            log_hal_info("serial_port_open close fail\r\n");
        }
    }


    vm.save_or_print_method = VM_SAVE_OR_PRINT_METHOD_NONE;

    vm.state = VM_STATE_IDLE;
}

#endif /*MTK_VM_LOG_ENABLE*/
#endif /* defined(HAL_I2S_MODULE_ENABLED) || defined(HAL_AUDIO_MODULE_ENABLED) */
