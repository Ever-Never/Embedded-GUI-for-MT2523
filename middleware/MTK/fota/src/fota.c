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

#include "fota.h"
#ifdef MTK_FOTA_ON_7687
#include "flash_map.h"
#else
#include "memory_map.h"
#endif
#ifdef FOTA_FS_ENABLE
#include "ff.h"
#endif
#ifndef FOTA_FS_ENABLE
#include "hal_flash.h"
#ifdef MTK_FOTA_ON_7687
/* trigger flag address. Don't modify it please */
#define FOTA_RESERVED_PARTITION_END_ADDRESS    (FOTA_BASE + FOTA_LENGTH)
#else
#ifndef ROM_BASE
#define FOTA_RESERVED_PARTITION_END_ADDRESS    (FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - BL_BASE)
#else
#define FOTA_RESERVED_PARTITION_END_ADDRESS    (FOTA_RESERVED_BASE + FOTA_RESERVED_LENGTH - ROM_BASE)
#endif
#endif

/* fota magic pattern */
#define FOTA_HEADER_MAGIC_PATTERN      0x004D4D4D  // "MMM"
#define FOTA_TRIGGER_FLAG_ADDRESS    (FOTA_RESERVED_PARTITION_END_ADDRESS - 512)
#else
/* fota magic pattern */
#define FOTA_HEADER_MAGIC_PATTERN      0x4D4D4D  // "MMM"
#define FOTA_TRIGGER_FLAG_FILE         "SD:/update"
#endif

#ifdef BL_FOTA_DEBUG
#include "hw_uart.h"
#define FOTA_API_PRINT_I hw_uart_printf
#define FOTA_API_PRINT_W hw_uart_printf
#define FOTA_API_PRINT_E hw_uart_printf
#else
#include "syslog.h"
log_create_module(fota_module_api, PRINT_LEVEL_INFO);
#define FOTA_API_PRINT_I(_message,...)  LOG_I(fota_module_api, (_message), ##__VA_ARGS__)
#define FOTA_API_PRINT_W(_message,...)  LOG_W(fota_module_api, (_message), ##__VA_ARGS__)
#define FOTA_API_PRINT_E(_message,...)  LOG_E(fota_module_api, (_message), ##__VA_ARGS__)
#endif


#ifndef FOTA_FS_ENABLE
fota_ret_t fota_trigger_update(void)
{
#ifndef MTK_DUAL_IMAGE_FOTA_ENABLE
    hal_flash_status_t ret;
    uint32_t pattern = FOTA_HEADER_MAGIC_PATTERN;
    ret = hal_flash_init();
    if (ret < HAL_FLASH_STATUS_OK)
    {
        FOTA_API_PRINT_E("[FOTA_SDK]Hal flash initialize fail, ret = %d\n", ret);
        return FOTA_TRIGGER_FAIL;
    }

    /* erase the last 4k block in fota reserved partition */
    ret = hal_flash_erase(FOTA_RESERVED_PARTITION_END_ADDRESS - 4096, HAL_FLASH_BLOCK_4K);
    if (ret < HAL_FLASH_STATUS_OK)
    {
        FOTA_API_PRINT_E("[FOTA_SDK]erase 4k block fail, ret = %d\n", ret);
        return FOTA_TRIGGER_FAIL;
    }

    /* write pattern into 512 bytes ahead of the reserved partition end address */
    FOTA_API_PRINT_I("[FOTA_SDK]flag address is 0x%x\n", FOTA_TRIGGER_FLAG_ADDRESS);
    ret = hal_flash_write(FOTA_TRIGGER_FLAG_ADDRESS, (const uint8_t*)&pattern, sizeof(uint32_t));
    if (ret < HAL_FLASH_STATUS_OK)
    {
        FOTA_API_PRINT_I("[FOTA_SDK]Trigger update result is %d\n", ret);
        return FOTA_TRIGGER_FAIL;
    }
    else
    {
        FOTA_API_PRINT_I("[FOTA_SDK]Trigger update result is %d\n", ret);
        return FOTA_TRIGGER_SUCCESS;
    }
#endif /* MTK_DUAL_IMAGE_FOTA_ENABLE*/

}
#else
fota_ret_t fota_trigger_update(void)
{
  FRESULT res;
  FIL fdst;
  UINT length_written = 0U;
  uint32_t pattern = FOTA_HEADER_MAGIC_PATTERN;
  fota_ret_t result = FOTA_TRIGGER_SUCCESS;
  res = f_open(&fdst, _T(FOTA_TRIGGER_FLAG_FILE), FA_CREATE_ALWAYS | FA_WRITE | FA_READ);
  if (FR_OK != res) {
      FOTA_API_PRINT_E("[FOTA] Failed to create fota trigger file, error = \n\r", (uint32_t)(res));
      result = FOTA_TRIGGER_FAIL;
  }
  res = f_write(&fdst, (const uint8_t*)&pattern, sizeof(uint32_t)-1, &length_written);
  if (FR_OK != res) {
      FOTA_API_PRINT_E("[FOTA] Failed to trigger flag to file, error = %d\n\r", (uint32_t)res);
      result = FOTA_TRIGGER_FAIL;
  }

  res = f_close(&fdst);
  if (FR_OK != res) {
      FOTA_API_PRINT_E("[FOTA] Failed to close fota trigger file!\n\r");
      result = FOTA_TRIGGER_FAIL;
  }

  return result;
}

bool readAndClearBlFotaUpdateInfo(bl_fota_update_info_t* pInfo)
{
  bool result = true;
  FRESULT res;
  FIL fdst;
  UINT length_read = 0U;

  if (pInfo == NULL){
      return false;
  }

  res = f_open(&fdst, _T(FOTA_TRIGGER_FLAG_FILE), FA_READ);
  if (FR_OK != res) {
      FOTA_API_PRINT_E("[FOTA] Failed to open trigger file, error =%d \n\r", (uint32_t)(res));
      result = false;
  }

  res = f_read(&fdst, (void*)pInfo, sizeof(bl_fota_update_info_t), &length_read);
  if((FR_OK != res) || (length_read != sizeof(bl_fota_update_info_t))){
     FOTA_API_PRINT_E("[FOTA] Failed to read update info res =%d, length_read=%d \n\r", (uint32_t)(res), length_read);
     result = false;
  }

  res = f_close(&fdst);
  if (FR_OK != res) {
      FOTA_API_PRINT_E("[FOTA] Failed to close update info file!\n\r");
      result = false;
  }

  res = f_unlink(_T(FOTA_TRIGGER_FLAG_FILE));
  if (FR_OK != res) {
      FOTA_API_PRINT_E("[FOTA] Failed to delete update info file!\n\r");
      result = false;
  }

  return result;

}

bool isFotaExecuted(void)
{
  FRESULT res;
  FILINFO fno;
  res = f_stat(_T(FOTA_TRIGGER_FLAG_FILE), &fno);
  if(FR_OK == res){
     return true;
  }
  return false;
}
#endif
