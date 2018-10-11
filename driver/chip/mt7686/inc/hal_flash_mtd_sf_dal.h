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

//#include "hal_flash_opt.h"

#ifndef __FLASH_MTD_SF_DAL_H__
#define __FLASH_MTD_SF_DAL_H__

#include "hal_flash_general_types.h"
#include "hal_flash_drvflash.h"
#include "hal_flash_fs_type.h"
#include "hal_flash_sf.h"
#include "hal_flash_custom_memorydevice.h"
#include <assert.h>

#define ust_get_duration(a,b) ((a>b)?(b+(0xFFFFFFFF-a)+0x1):(b-a))

#ifdef __UBL__
#define ASSERT(expr)  { if (!(expr))  while (1); }
#define ASSERT_RET(expr, ret)  { if (!(expr))  while (1); }
#else
#define ASSERT(expr)\
   do{\
         if(!(expr))\
         {\
            Flash_ReturnReady();\
            assert(0);\
            return;\
         }\
     }while(0);
#define ASSERT_RET(expr, ret)\
   do{\
         if(!(expr))\
         {\
            Flash_ReturnReady();\
            assert(0);\
            return ret;\
         }\
     }while(0);
#endif

//-------------------------------------------------------
// IoCtrl enumeration
//-------------------------------------------------------
typedef enum {
    SF_IOCTRL_UNLOCK_BLOCK,
    SF_IOCTRL_GET_PROGRAM_FAIL_HANDLE_TYPE
} SF_MTD_IOCtrl_Action_Enum;

typedef enum {
    SFDRV_READY = 0,
    SFDRV_PROGRAMMING,
    SFDRV_ERASING
} SF_Driver_Status;

//-------------------------------------------------------
// Driver Data - Device Command
//-------------------------------------------------------
typedef struct {
    uint8_t PageProg;
    uint8_t WriteEnable;

    uint8_t Suspend;
    uint8_t Resume;
    uint8_t Read;

    uint8_t Erase;
    uint8_t Erase4K;
    uint8_t Erase32K;
    uint8_t Erase64K;

    uint8_t ReadID_SPI;
    uint8_t ReadID_QPI;

    uint8_t ReadBPReg;
    uint8_t WriteBPReg;
    uint8_t ReadSecurityReg;
    uint8_t WriteSecurityReg;
    uint8_t ReadSuspendReg;
    uint8_t ReadReadyReg;
    uint8_t ReadFailReg;

    uint8_t EnterDPD;
    uint8_t LeaveDPD;

//#ifdef __SECURITY_OTP__
    uint8_t OTPProg;
    uint8_t OTPRead;
    uint8_t OTPEnter;
    uint8_t OTPExit;
    uint8_t OTPLock;
//#endif
} SF_MTD_CMD;

//-----------------------------------------------------------------------
// Driver Data
// Note: The first part must be identical to
//       NOR_Flash_MTD_Data(FDM4.0), NOR_MTD_DATA(FDM5.0)
//       The second part must be identical to
//       NOR_Flash_MTD_Data(FDM4.0)
//-----------------------------------------------------------------------

typedef struct {
//========== First Part ===========
    uint32_t         Signature;
    uint8_t         *BaseAddr;
    kal_semid          DeviceLock;       // The device lock that keeps driver resource consistent
    kal_taskid         DeviceLockOwner;  // The owner of the deivce lock
    int8_t           DeviceLockDepth;  // The depth of the deivce lock

//========== Second Part ===========
    FlashRegionInfo   *RegionInfo;
    uint8_t         *CurrAddr;
    FlashBankInfo     *BankInfo;
    uint8_t         *CurrBankAddr;
    uint32_t         CurrBankSize;
    uint8_t         *CurrPollAddr;

#if defined(__SINGLE_BANK_NOR_FLASH_SUPPORT__)
    NOR_AddrLookup    lookup_tbl[LOOKUP_TABLE_SIZE][2];
#endif

//========== Serial Flash =========
    SF_Status             *StatusMap;
    SF_MTD_CMD            *CMD;
    uint32_t             Flag;
    uint16_t             BPRBitCount;
    uint16_t             UniformBlock;

//OTP variable, always exist because FDM will use SF_MTD_Data
//#ifdef __SECURITY_OTP__
    uint16_t             OTPLength;
    uint16_t             OTPBase;
//#endif // __SECURITY_OTP__

    uint8_t              SuspendLatency;
    uint8_t              FlashID[SF_FLASH_ID_LENGTH];
    uint8_t              DriverStatus;
    uint8_t              AddressCycle;

    //Add for Dual SF
    uint16_t             CS;
    uint32_t             sf_dal_fix;
    uint32_t             CS_Size;
//Dual SF use variable, always exist for FDM5.0 adaption
//#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    bool               NOR_FLASH_BUSY;
    bool               NOR_FLASH_SUSPENDED;
//#endif

    int (*UnlockBlock)(void *MTDData);

//OTP function pointer, always exist because FDM will use SF_MTD_Data
//#ifdef __SECURITY_OTP__
    int (*OTPLock)(void *MTDData);
    int (*OTPRead)(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
    int (*OTPWrite)(void *MTDData, uint32_t Offset, void *BufferPtr, uint32_t Length);
//#endif // __SECURITY_OTP__
    //For FDM5.0 usage, can not have compile option for FDM5.0 here, otherwise Drvflash_ldisk.c will have build error
//       NORLayoutInfo * LayoutInfo;

} SF_MTD_Data;

//-----------------------------------------------------------------------
// GPRAM 4-byte alined access union
//-----------------------------------------------------------------------
typedef union {
    uint32_t   u32;
    uint16_t   u16[2];
    uint8_t    u8[4];
} sf_uint;

//-------------------------------------------------------
// Type Enumeration
//-------------------------------------------------------
// Note: This must match with JECDEC vendor id
typedef enum {
    SF_DAL_TYPE_NONE       = 0,
    SF_DAL_TYPE_NUMONYX    = 0x20,
    SF_DAL_TYPE_NUMONYX_W  = 0x2C,
    SF_DAL_TYPE_EON        = 0x1C,
    SF_DAL_TYPE_SST        = 0xBF,
    SF_DAL_TYPE_MXIC       = 0xC2,
    SF_DAL_TYPE_GIGADEVICE = 0xC8,
    SF_DAL_TYPE_GIGADEVICE_MD = 0x51,
    SF_DAL_TYPE_WINBOND    = 0xEF,
    SF_DAL_TYPE_FIDELIX    = 0xF8,
    SF_DAL_TYPE_ESMT       = 0x8C,
    SF_DAL_TYPE_COUNT
} SF_DAL_Type;


//-----------------------------------------------------------------------------
// [DAL] Device Lock
//-----------------------------------------------------------------------------
#ifdef __UBL__
#define SF_DAL_CreateDevLock(...)
#define SF_DAL_GetDevLock(...)
#define SF_DAL_FreeDevLock(...)
#else
void SF_DAL_CreateDevLock(SF_MTD_Data *data);
void SF_DAL_GetDevLock(SF_MTD_Data *data);
void SF_DAL_FreeDevLock(SF_MTD_Data *data);
#endif

//-------------------------------------------------------
// INIT Stage APIs
//-------------------------------------------------------
int32_t SF_DAL_Mount_Driver_EMIINIT(NOR_MTD_Driver *driver, SF_MTD_Data *data, uint32_t baseaddr);
int SF_DAL_EraseBlock_EMIINIT(void *MTDData, uint32_t BlockSize, uint32_t Address);
void SF_DAL_ProgramData_EMIINIT(void *MTDData, uint32_t address, void *Data, uint32_t Size);
void SF_DAL_ReleaseSFIChannels_EMIINIT(void);

//-------------------------------------------------------
// Driver Abstract Layer Initialization Function
//-------------------------------------------------------
int32_t SF_DAL_Init_Driver(
    NOR_MTD_Driver *driver,    // Pointer to allocated driver interface
    SF_MTD_Data *data,         // Pointer to allocated driver data
    uint32_t baseaddr,       // Base address of FAT region
    uint32_t parameter       // Device Paramter (Supported Erase Unit)
);

//-----------------------------------------------------------------------------
// [DAL] First Level Abstract (Interface to upper driver)
//-----------------------------------------------------------------------------
int SF_DAL_MountDevice(void *MTDData, void *Info);
int SF_DAL_ShutDown(void *MTDData);
int SF_DAL_EraseBlock(void *MTDData, uint32_t BlockIndex);
int SF_DAL_EraseBlockWrapper(void *MTDData, uint32_t BlockIndex);
int SF_DAL_NonBlockEraseBlock(void *MTDData, uint32_t BlockIndex);
int32_t SF_DAL_CheckDeviceReady(void *MTDData, uint32_t BlockIndex);
int32_t SF_DAL_CheckReadyAndResume(void *MTDData, uint32_t addr, uint8_t data);
int SF_DAL_ResumeErase(void *MTDData, uint32_t BlockIndex);
int SF_DAL_SuspendErase(void *MTDData, uint32_t BlockIndex);
int SF_DAL_LockEraseBlkAddr(void *MTDData, void *BlkAddr, uint32_t ACTION);
int SF_DAL_IOCtrl(void *MTDData, uint32_t CtrlAction, void *CtrlData);
int SF_DAL_ProgramData(void *MTDData, void *Address, void *Data, uint32_t Length);
int SF_DAL_ProgramDataWrapper(void *MTDData, void *Address, void *Data, uint32_t Length);
int SF_DAL_OTPQueryLength(void *MTDData, unsigned int *LengthPtr);
int SF_DAL_OTPAccess(void *MTDData, int accesstype, unsigned int Offset, void *BufferPtr, unsigned int Length);
uint32_t ust_get_current_time(void);


//-------------------------------------------------------
// Sleep Mode Service Routines
//-------------------------------------------------------
void Flash_ProgramData(void *Target, void *Data, uint32_t Size);
bool Flash_CheckReady(void);

//-------------------------------------------------------
// System Context Switch Service Routines
//-------------------------------------------------------
void Flash_ReturnReady(void);

#endif  // #ifndef __FLASH_MTD_SF_H__
