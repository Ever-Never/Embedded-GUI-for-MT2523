/*
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

#ifdef MTK_CTP_ENABLE

#include "touch_panel_custom_config.h"
#ifdef CTP_IT7258_SUPPORTED

#include <string.h>
#include "ctp.h"
#include "ctp_ite_it7xxx.h"
#include "ctp_ite_it7xxx_download.h"
#include "touch_panel_custom_ite.h"
#include "hal_log.h"
#include <assert.h>

#ifdef CTP_IT7258_400X400_SCREEN
#include "it7xxx_config_400X400.h"
#else
#include "it7xxx_config.h"
#endif

#define BUFFER_INDEX_COMMAND                0x20
#define BUFFER_INDEX_QUERY                  0x80
#define BUFFER_INDEX_COMMAND_RESPONSE       0xA0

#define MAX_BUFFER_SIZE                     256

#define QUERY_COMMAND_READY                 0x00
#define QUERY_COMMAND_BUSY                  0x01
#define QUERY_COMMAND_ERROR                 0X02

#define COMMAND_RESPONSE_SUCCESS            0x0000

#define FLASH_HIGH_ADDR                     0xD0
#define FLASH_LOW_ADDR                      0x00

// *****************************************************************************
// Function name: waitCommandDone
//
// Description:
//   This function is compared Firmware Version.
//
// ******************************************************************************
static void waitCommandDone(void)
{
    uint8_t ucQuery;
    do {
        if (!fnReadBuffer(BUFFER_INDEX_QUERY, 1, &ucQuery)) {
            ucQuery = QUERY_COMMAND_BUSY;
        }
    } while (ucQuery & QUERY_COMMAND_BUSY);
}

// *****************************************************************************
// Function name: isCommandResponse
//
// Description:
//   This function is compared Firmware Version.
//
// ******************************************************************************
bool isCommandResponse(void)
{
    uint16_t wCommandResponse;

    if (!fnReadBuffer(BUFFER_INDEX_COMMAND_RESPONSE, 2, &wCommandResponse)) {
        return false;
    }

    if (wCommandResponse != COMMAND_RESPONSE_SUCCESS) {
        return false;
    }

    return true;
}

// *****************************************************************************
// Function name: fnCompareFW
//
// Description:
//   This function is compared Firmware Version.
//
// ******************************************************************************
bool fnCompareFW(uint32_t unFirmwareLength, void *pFirmware)
{
    uint8_t pucbuffer[SIGNATURE_LENGTH];
    uint8_t i;
    uint32_t current_version = 0;
    uint32_t update_version  = 0;

    if (!fnSetStartOffset(0x0000)) {
        log_hal_info("fnCompareFW set start offset fail......");
        return false;
    }

    waitCommandDone();
    if (!fnAdvanceReadFlash(SIGNATURE_LENGTH, 0x0000, pucbuffer)) {
        return false;
    }

    /*
    for (i=0 ;i<SIGNATURE_LENGTH; i++) {
        log_hal_info("[mtk-tpd] ITE7260 flash Version:[%d] = %x", i, pucbuffer[i]);
    }*/

    current_version  = (uint32_t)(pucbuffer[8] << 24);
    current_version += (uint32_t)(pucbuffer[9] << 16);
    current_version += (uint32_t)(pucbuffer[10] << 8);
    current_version += (uint32_t)(pucbuffer[11]);

    update_version  = (uint32_t)(*((uint8_t *)pFirmware + 8) << 24);
    update_version += (uint32_t)(*((uint8_t *)pFirmware + 9) << 16);
    update_version += (uint32_t)(*((uint8_t *)pFirmware + 10) << 8);
    update_version += (uint32_t)(*((uint8_t *)pFirmware + 11));

    log_hal_info("[version]current:%x update:%x\r\n", current_version, update_version);
    for (i = 0; i < 4; i++) {
        if (pucbuffer[8 + i] != *((uint8_t *)pFirmware + 8 + i)) {
            return true;
        }
    }

    return false;
}


// *****************************************************************************
// Function name: fnCompareCFG
//
// Description:
//   This function is compared config Version.
//
// ******************************************************************************
bool fnCompareCFG(uint32_t unConfigLength, void *pConfig)
{
    uint8_t pucbuffer[8];
    uint8_t i;
    uint8_t *p;

    waitCommandDone();

    pucbuffer[0] = 0x1; //COMMAND_INQUIRY_SENSOR
    pucbuffer[1] = 0x6; //configuration version
    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 2, pucbuffer)) {
        return false;
    }

    waitCommandDone();

    if (!fnReadBuffer(BUFFER_INDEX_COMMAND_RESPONSE, 8, &pucbuffer[0])) {
        return false;
    }


    log_hal_info("[CTP]Current touch Config Version %x %x %x %x\n",
                 pucbuffer[1], pucbuffer[2], pucbuffer[3], pucbuffer[4]);

    p = (uint8_t *)((uint32_t)pConfig + unConfigLength - 8);
    log_hal_info("[CTP]Current file Config Version %x %x %x %x\n", *p, *(p + 1), *(p + 2), *(p + 3));

    for (i = 0; i < 4; i++) {
        if (pucbuffer[1 + i] != *((uint8_t *)pConfig + unConfigLength - 8 + i)) {
            return true;
        }
    }

    return false;
}

// *****************************************************************************
// Function name: fnFirmwareDownload
//
// Description:
//   This function is used to download firmware and configuration files.
//
// Arguments:
//  unFirmwareLength -- the file length of firmware file.
//  pFirmware -- the firmware binary file.
//  unConfigLength -- the file length of configuration file.
//  pConfig -- the configuration binary file.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnFirmwareDownload(void)
{
#ifdef CTP_ITE_UPGRADE_FIRMWARE
    bool     fw_flag, cfg_flag;
    uint32_t unFirmwareLength = sizeof(it7xx_firmware_data);
    uint8_t *pFirmware;
    uint32_t unConfigLength = sizeof(it7xx_config_data);
    uint8_t *pConfig;

    pFirmware = (uint8_t *)it7xx_firmware_data;
    pConfig   = (uint8_t *)it7xx_config_data;
    if (unFirmwareLength == 0 && unConfigLength == 0) {
        // Need not to download anything
        return true;
    }

    fw_flag  = fnCompareFW(unFirmwareLength, pFirmware);
    cfg_flag = fnCompareCFG(unConfigLength, pConfig);
    if (ctp_is_force_upgrade_firmware == true) {
        fw_flag  = true;
        cfg_flag = true;
    }

    log_hal_info("[CTP][FW]fw_flag = %d,cfg_flag = %d", fw_flag, cfg_flag);

    if (fw_flag || cfg_flag) {
        if (!fnEnterFirmwareUpgradeMode()) {
            log_hal_info("[CTP][FW]Enter firmware upgrade mode fail....");
            return false;
        }
        if (unFirmwareLength != 0 && pFirmware != NULL && fw_flag) {
            // Download firmware
            if (!fnDownloadFirmware(unFirmwareLength, pFirmware)) {
                log_hal_info("[CTP][FW]Download firmware fail....");
                return false;
            }
        }

        if (unConfigLength != 0 && pConfig != NULL && cfg_flag) {
            // Download configuration
            if (!fnDownloadConfig(unConfigLength, pConfig)) {
                log_hal_info("[CTP][FW]Download config fail....");
                return false;
            }
        }
        if (!fnExitFirmwareUpgradeMode()) {
            log_hal_info("[CTP][FW]Exit firmware upgrade mode fail....");
            return false;
        }

        if (!fnFirmwareReinitialize()) {
            log_hal_info("[CTP][FW]Firmware reinitialize fail....");
            return false;
        }
        log_hal_info("[CTP][FW]Download firmware success....");
    } else {
        log_hal_info("[CTP][FW]No need to upgrade firmware....");
        return false;
    }
#endif

    return true;
}

// *****************************************************************************
// Function name: fnDownloadFirmware
//
// Description:
//   This function is used to download firmware file.
//
// Arguments:
//  unFirmwareLength -- the file length of firmware file.
//  pFirmware -- the firmware binary file.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnDownloadFirmware(uint32_t unFirmwareLength, void *pFirmware)
{
    if (!fnVerifyFirmware(unFirmwareLength, pFirmware)) {
        log_hal_info("Verify firmware fail......");
        return false;
    }

    if (!fnSetStartOffset(0x0000)) {
        log_hal_info("Firmware set start offset fail......");
        return false;
    }
    if (!fnAdvanceWriteFlash(unFirmwareLength, pFirmware)) {
        log_hal_info("Firmware advance write flash fail......");
        return false;
    }

    if (!fnCompareResult(unFirmwareLength, 0x0000, pFirmware)) {
        log_hal_info("Firmware compare result fail......");
        return false;
    }

    return true;
}



// *****************************************************************************
// Function name: fnVerifyFirmware
//
// Description:
//   This function is used to verify firmware file.
//
// Arguments:
//  unFirmwareLength -- the file length of firmware file.
//  pFirmware -- the firmware binary file.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnVerifyFirmware(uint32_t unFirmwareLength, void *pFirmware)
{
    uint16_t wFlashSize;
    uint16_t wSize;
    uint8_t pucBuffer[SIGNATURE_LENGTH];

    // Check signature
    if (*((uint8_t *)pFirmware + 0) != 'I' ||
            *((uint8_t *)pFirmware + 1) != 'T' ||
            *((uint8_t *)pFirmware + 2) != '7' ||
            *((uint8_t *)pFirmware + 3) != '2' ||
            *((uint8_t *)pFirmware + 4) != '6' ||
            *((uint8_t *)pFirmware + 5) != '0' ||
            *((uint8_t *)pFirmware + 6) != 'F' ||
            *((uint8_t *)pFirmware + 7) != 'W') {
        return false;
    }
    // Check overlap with config file
    if (!fnGetFlashSize(&wFlashSize)) {
        return false;
    }
    if (!fnAdvanceReadFlash(SIGNATURE_LENGTH, wFlashSize - SIGNATURE_LENGTH, pucBuffer)) {
        return false;
    }
    if (pucBuffer[0] == 'C'
            && pucBuffer[1] == 'O'
            && pucBuffer[2] == 'N'
            && pucBuffer[3] == 'F'
            && pucBuffer[4] == 'I'
            && pucBuffer[5] == 'G') {
        memcpy(&wSize, pucBuffer + 6, sizeof(uint16_t));
        if ((wSize + (uint16_t)unFirmwareLength) >= wFlashSize) {
            log_hal_info("Check overlap with config file fail....");
            return false;
        }
    } else {
        log_hal_info("not found config file....");
    }
    return true;
}

// *****************************************************************************
// Function name: fnDownloadConfig
//
// Description:
//   This function is used to download configuration file.
//
// Arguments:
//  unConfigLength -- the file length of configuration file.
//  pConfig -- the configuration binary file.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnDownloadConfig(uint32_t unConfigLength, void *pConfig)
{
    uint16_t wFlashSize;

    if (!fnVerifyConfig(unConfigLength, pConfig)) {
        log_hal_info("Config verify fail......");
        return false;
    }

    if (!fnGetFlashSize(&wFlashSize)) {
        log_hal_info("Config get flash size fail......");
        return false;
    }
    if (!fnSetStartOffset(wFlashSize - (uint16_t)unConfigLength)) {
        log_hal_info("Config set start offset fail......");
        return false;
    }

    if (!fnAdvanceWriteFlash(unConfigLength, pConfig)) {
        log_hal_info("Config advance write flash fail......");
        return false;
    }

    if (!fnCompareResult(unConfigLength, (wFlashSize - (uint16_t)unConfigLength), pConfig)) {
        log_hal_info("Config compare result fail......");
        return false;
    }
    return true;
}

// *****************************************************************************
// Function name: fnVerifyConfig
//
// Description:
//   This function is used to verify configuration file.
//
// Arguments:
//  unConfigLength -- the file length of configuration file.
//  pConfig -- the configuration binary file.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnVerifyConfig(uint16_t unConfigLength, void *pConfig)
{
    uint16_t wFlashSize;
    uint16_t wSize;
    uint8_t pucBuffer[SIGNATURE_LENGTH];

    // Check signature
    if (*((uint8_t *)pConfig + unConfigLength - 16) != 'C' ||
            *((uint8_t *)pConfig + unConfigLength - 15) != 'O' ||
            *((uint8_t *)pConfig + unConfigLength - 14) != 'N' ||
            *((uint8_t *)pConfig + unConfigLength - 13) != 'F' ||
            *((uint8_t *)pConfig + unConfigLength - 12) != 'I' ||
            *((uint8_t *)pConfig + unConfigLength - 11) != 'G') {
        log_hal_info("Verify config name fail....");
        return false;
    }

    // Check overlap with config file
    if (!fnGetFlashSize(&wFlashSize)) {
        log_hal_info("Verify config fnGetFlashSize fail....");
        return false;
    }

    /*
    if (!fnReadFlash(SIGNATURE_LENGTH / 2, wFlashSize, pucBuffer)) {
        log_hal_info("Verify config fnReadFlash fail....");
        return false;
    }*/


    if (!fnReadFlash(SIGNATURE_LENGTH / 2, 0x0000, pucBuffer)) {
        log_hal_info("Verify config fnReadFlash fail....");
        return false;
    }

    if (pucBuffer[0] == 'I' &&
            pucBuffer[1] == 'T' &&
            pucBuffer[2] == '7' &&
            pucBuffer[3] == '2' &&
            pucBuffer[4] == '6' &&
            pucBuffer[5] == '0' &&
            pucBuffer[6] == 'F' &&
            pucBuffer[7] == 'W') {
        memcpy(&wSize, pucBuffer + 12, sizeof(uint16_t));
        if (wSize + (uint16_t)unConfigLength >= wFlashSize) {
            log_hal_info("Verify config wSize fail....");
            return false;
        }
    }

    return true;
}

// *****************************************************************************
// Function name: fnEnterFirmwareUpgradeMode
//
// Description:
//   This function is used to enter firmware upgrade mode.
//
// Arguments:
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnEnterFirmwareUpgradeMode(void)
{
    uint8_t pucBuffer[MAX_BUFFER_SIZE];

    waitCommandDone();

    pucBuffer[0] = 0x60;
    pucBuffer[1] = 0x00;
    pucBuffer[2] = 'I';
    pucBuffer[3] = 'T';
    pucBuffer[4] = '7';
    pucBuffer[5] = '2';
    //pucBuffer[6] = '6';
    //pucBuffer[7] = '0';
    //pucBuffer[8] = 0x55;
    //pucBuffer[9] = 0xAA;

    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 6, pucBuffer)) {
        return false;
    }

    waitCommandDone();

    return isCommandResponse();
}

// *****************************************************************************
// Function name: fnExitFirmwareUpgradeMode
//
// Description:
//   This function is used to exit firmware upgrade mode.
//
// Arguments:
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnExitFirmwareUpgradeMode(void)
{
    uint8_t pucBuffer[MAX_BUFFER_SIZE];

    waitCommandDone();

    pucBuffer[0] = 0x60;
    pucBuffer[1] = 0x80;
    pucBuffer[2] = 'I';
    pucBuffer[3] = 'T';
    pucBuffer[4] = '7';
    pucBuffer[5] = '2';
    //pucBuffer[6] = '6';
    //pucBuffer[7] = '0';
    //pucBuffer[8] = 0xAA;
    //pucBuffer[9] = 0x55;

    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 6, pucBuffer)) {
        return false;
    }

    waitCommandDone();

    return isCommandResponse();
}

// *****************************************************************************
// Function name: fnGetFlashSize
//
// Description:
//   This function is used to get IT7260/50 flash size.
//
// Arguments:
//  pwFlashSize -- the flash size of IT7260/50.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnGetFlashSize(uint16_t *pwFlashSize)
{
    uint8_t pucBuffer[MAX_BUFFER_SIZE];
    uint8_t pucCommandResponse[3];

    if (pwFlashSize == NULL) {
        return false;
    }

    waitCommandDone();

    pucBuffer[0] = 0x01;
    pucBuffer[1] = 0x03;

    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 2, pucBuffer)) {
        return false;
    }

    waitCommandDone();

    if (!fnReadBuffer(BUFFER_INDEX_COMMAND_RESPONSE, 3, pucCommandResponse)) {
        return false;
    }

    *pwFlashSize = (pucCommandResponse[2] << 8) | pucCommandResponse[1]; //*(uint16_t*)(pucCommandResponse+1);

    log_hal_info("Get flash size : %x ", *pwFlashSize);

    return true;
}

// *****************************************************************************
// Function name: fnSetStartOffset
//
// Description:
//   This function is used to set start offset of read/write flash.
//
// Arguments:
//  wOffset -- the start offset of read/write flash.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnSetStartOffset(uint16_t wOffset)
{
    uint8_t pucBuffer[MAX_BUFFER_SIZE];

    waitCommandDone();

    pucBuffer[0] = 0x61;
    pucBuffer[1] = 0;
    memcpy(pucBuffer + 2, &wOffset, sizeof(uint16_t));

    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 4, pucBuffer)) {
        return false;
    }

    waitCommandDone();

    return isCommandResponse();
}

// *****************************************************************************
// Function name: fnFirmwareReinitialize
//
// Description:
//   This function is used to reset IT7260/50 firmware.
//
// Arguments:
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnFirmwareReinitialize(void)
{
    uint8_t pucBuffer[MAX_BUFFER_SIZE];

    waitCommandDone();

    pucBuffer[0] = 0x6F;

    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 1, pucBuffer)) {
        return false;
    }
    waitCommandDone();

    return isCommandResponse();
}

// *****************************************************************************
// Function name: fnWriteFlash
//
// Description:
//   This function is used to write flash data.
//
// Arguments:
//  unLength -- the length of send flash data (Max length 128).
//  pData -- the send flash data.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnWriteFlash(uint32_t unLength, void *pData)
{
    uint8_t pucBuffer[MAX_BUFFER_SIZE];
    uint32_t ucLoop;

    if (unLength != 128 || pData == NULL) {
        return false;
    }
    /*Write 128 bytes into IC buffer by using command 0xF0 32 times*/
    for (ucLoop = 0; ucLoop < unLength; ucLoop += 4) {
        waitCommandDone();
        pucBuffer[0] = 0xF0;
        pucBuffer[1] = ucLoop;
        memcpy(pucBuffer + 2, (uint8_t *)pData + ucLoop, 4 * sizeof(uint8_t)) ;

        if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 6, pucBuffer)) {
            return false;
        }

        waitCommandDone();

        if (isCommandResponse() == false) {
            return false;
        }
    }

    /*Write 128 bytes into flash from IC buffer by using command 0xF1*/
    pucBuffer[0] = 0xF1;

    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 1, pucBuffer)) {
        return false;
    }

    waitCommandDone();

    return isCommandResponse();
}

// *****************************************************************************
// Function name: fnAdvanceWriteFlash
//
// Description:
//   This function is used to write flash data.
//
// Arguments:
//  unLength -- the length of send flash data.
//  pData -- the send flash data.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnAdvanceWriteFlash(uint32_t unLength, void *pData)
{
    uint32_t unCurRoundLength;
    uint32_t unRemainderLength = unLength;

    while (unRemainderLength > 0) {
        if (unRemainderLength > 128) {
            unCurRoundLength = 128;
        } else {
            unCurRoundLength = unRemainderLength;
        }

        if (!fnWriteFlash(unCurRoundLength, (uint8_t *)pData + (unLength - unRemainderLength))) {
            return false;
        }

        unRemainderLength -= unCurRoundLength;
    }

    return true;
}

// *****************************************************************************
// Function name: fnReadFlash
//
// Description:
//   This function is used to read flash data.
//
// Arguments:
//  unLength -- the length of receive flash data (Max length 4)..
//  pData -- the receive flash data.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnReadFlash(uint32_t unLength, uint32_t unOffset, void *pData)
{
    uint8_t pucBuffer[MAX_BUFFER_SIZE];

    if (unLength > 8 || pData == NULL) {
        return false;
    }

    if (!fnSetStartOffset((uint16_t)unOffset)) {
        return false;
    }

    waitCommandDone();

    /*Read from flash:
      command: 0xE1, count, 0x01 mean byte, High Offset, Low Offset, 0x00, 0xD0
    */
    // pucBuffer[0] = 0x63;
    // pucBuffer[1] = (uint8_t)unLength;
    pucBuffer[0] = 0xE1;
    pucBuffer[1] = (uint8_t)unLength;
    pucBuffer[2] = 0x01; //byte
    pucBuffer[3] = (uint8_t)(unOffset & 0x00FF);
    pucBuffer[4] = (uint8_t)((unOffset & 0xFF00) >> 8);
    pucBuffer[5] = FLASH_LOW_ADDR;
    pucBuffer[6] = FLASH_HIGH_ADDR;

    if (!fnWriteBuffer(BUFFER_INDEX_COMMAND, 7, pucBuffer)) {
        return false;
    }

    waitCommandDone();

    if (!fnReadBuffer(BUFFER_INDEX_COMMAND_RESPONSE, unLength, pData)) {
        return false;
    }

    return true;
}

// *****************************************************************************
// Function name: fnAdvanceReadFlash
//
// Description:
//   This function is used to read flash data.
//
// Arguments:
//  unLength -- the length of receive flash data (Max length 128)..
//  pData -- the receive flash data.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnAdvanceReadFlash(uint32_t unLength, uint32_t unStartOffset, void *pData)
{
    uint32_t unCurRoundLength;
    uint32_t unRemainderLength = unLength;

    while (unRemainderLength > 0) {
        if (unRemainderLength > 8) {
            unCurRoundLength = 8;
        } else {
            unCurRoundLength = unRemainderLength;
        }

        if (!fnReadFlash(unCurRoundLength, unStartOffset, (void *)((uint8_t *)pData + (unLength - unRemainderLength)))) {
            return false;
        }

        unStartOffset += 8;
        unRemainderLength -= unCurRoundLength;
    }

    return true;
}

// *****************************************************************************
// Function name: fnCompareResult
//
// Description:
//   This function is used to compare the download file and data which reading from flash.
//
// Arguments:
//  unFileLength -- the file length
//  pFile -- the binary file.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnCompareResult(uint32_t unFileLength, uint32_t unStartOffset, void *pFile)
{
    uint32_t unCurRoundLength;
    uint32_t unRemainderLength;
    uint8_t  pucBuffer[MAX_BUFFER_SIZE];
    uint8_t  pFileBuffer[MAX_BUFFER_SIZE];
    uint32_t unLoop;
    uint8_t  *pfirm;

    unRemainderLength = unFileLength;
    pfirm = (uint8_t *)pFile;

    while (unRemainderLength > 0) {
        if (unRemainderLength > 128) {
            unCurRoundLength = 128;
        } else {
            unCurRoundLength = unRemainderLength;
        }
        if (!fnAdvanceReadFlash(unCurRoundLength,  unStartOffset  + (unFileLength  -  unRemainderLength), (void *)pucBuffer)) {
            return false;
        }

        for (unLoop = 0; unLoop < unCurRoundLength; unLoop++) {
            pFileBuffer[unLoop] = (uint8_t)(*(pfirm + (unFileLength - unRemainderLength + unLoop)));
            if (pFileBuffer[unLoop] != pucBuffer[unLoop]) {
                log_hal_info("[mtk-tpd] ITE7260 compare Firmware fail:[%d] %x != %x\r\n", unFileLength - unRemainderLength + unLoop, pFileBuffer[unLoop], pucBuffer[unLoop]);
                return false;
            }
        }
        unRemainderLength -= unCurRoundLength;
    }


    return true;
}

// *****************************************************************************
// Function name: fnWriteBuffer
//
// Description:
//   This function is used to send data to IT7260/50.
//
// Arguments:
//  ucBuffeIndex -- the buffer index of IT7260/50
//  unLength -- the file length of send data.
//  pData -- the send data.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnWriteBuffer(uint8_t ucBuffeIndex, uint32_t unLength, void *pData)
{
    // Sudo code, not implement here
    return CTP_I2C_send(ucBuffeIndex, (uint8_t *)pData, unLength);
}

// *****************************************************************************
// Function name: fnReadBuffer
//
// Description:
//   This function is used to receive data from IT7260/50.
//
// Arguments:
//  ucBuffeIndex -- the buffer index of IT7260/50
//  unLength -- the file length of receive data.
//  pData -- the receive data.
//
// Return Value:
//  Return true if the function execute succefully, otherwise return false.
//
// *****************************************************************************
bool fnReadBuffer(uint8_t ucBuffeIndex, uint32_t unLength, void *pData)
{
    // Sudo code, not implement here
    return CTP_I2C_read(ucBuffeIndex, (uint8_t *)pData, unLength);
}

#endif /*CTP_IT7258_SUPPORTED*/
#endif /*MTK_CTP_ENABLE*/

