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

#include "hal_flash.h"

#ifdef HAL_FLASH_MODULE_ENABLED
#include "memory_attribute.h"
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_opt.h"
#include "hal_flash_sfi_release.h"
#include "hal_flash_general_types.h"
#include "hal_flash_mtd_internal.h"
#include "mt7686.h"
#include "hal_flash_sf.h"
#include "hal_flash_mtd_sf_dal.h"
#include "hal_flash_sfi_hw.h"


#define SFI_IsQPIMode()      (SFC->RW_SF_DIRECT_CTL & SFI_QPI_EN)
#define SFI_IsQPIMode_CS1()      (SFC->RW_SF_DIRECT_CTL2 & SFI_QPI_EN)


//This variable is to support L1 Modem Hard Real Time Enhancement
bool DelayResume = false;

//Extern APIs
extern bool CMEM_CheckValidDeviceID(uint8_t *id);


//Support MT6250 MT6260/61 Mask Code decompressor and LZMA when SFC Macro mode/ Flash busy
extern bool NOR_FLASH_BUSY;
extern bool NOR_FLASH_SUSPENDED;


/*----------------------------
 * Serial Flash Common APIs
 *----------------------------*/

#ifndef __UBL__
ATTR_TEXT_IN_RAM void SFI_BlankCheck(void *MTDData, uint32_t BlockIndex)
{
    SF_MTD_Data *D = MTDData;
    uint32_t   i, data_cnt;
    uint32_t  *p_data;

    data_cnt = BlockSize(MTDData, BlockIndex) / sizeof(uint32_t);

    for (i = 0, p_data = (uint32_t *)D->CurrAddr; i < data_cnt; ++i, ++p_data) {
        if (*p_data != 0xFFFFFFFF) {
            ASSERT(0);
            //MTD_SNOR_ASSERT(0, __LINE__, 0, 0, NOR_MTD_ERASE_COMP_BLOCK_DATA_FAIL);
        }
    }
}
#endif /* !__UBL__ */

//-----------------------------------------------------------------------------
/*!
  @brief
    Reverse byte order in a DWORD
*/
ATTR_TEXT_IN_RAM uint32_t SFI_ReverseByteOrder(uint32_t num)
{
    unsigned int ret = 0;

    ret |= ((num >> SFC_GENERIC_24_BIT_OFFSET) & SFC_GENERIC_0x000000FF_MASK );
    ret |= ((num >> SFC_GENERIC_8_BIT_OFFSET)  & SFC_GENERIC_0x0000FF00_MASK );
    ret |= ((num << SFC_GENERIC_8_BIT_OFFSET)  & SFC_GENERIC_0x00FF0000_MASK );
    ret |= ((num << SFC_GENERIC_24_BIT_OFFSET) & SFC_GENERIC_0xFF000000_MASK );

    return ret;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Enter macro mode, and set SFI_MAC_SIO_SEL if QPI mode is enabled.
*/
ATTR_TEXT_IN_TCM void SFI_MacEnable(const uint16_t CS)
{
    uint32_t val;
    //Mask SFC AHB Channel 2 and 3 during SFC Mac mode/ Flash busy
    SFI_MaskAhbChannel(1);

    val = SFC->RW_SF_MAC_CTL;
    if (CS == 0x0) {
        if (SFI_IsQPIMode()) {
            val |= SFI_MAC_SIO_SEL;
        }
    } else if (CS == 0x1) {
        if (SFI_IsQPIMode_CS1()) {
            val |= SFI_MAC_SIO_SEL;
        }
    } else {
        //MTD_SNOR_ASSERT(0, __LINE__, 0, 0, NOR_MTD_DATA_CORRUPTED);
        ASSERT(0);
    }

    val |= SFI_MAC_EN;

    //Wait SFC idle before issue Macro command
#if !defined(__UBL__) //Wait until SFC AHB Channel 2 become IDLE before enter Macro mode
    while (!(SFC->RW_SF_MISC_CTL3 & SFI_CH2_TRANS_IDLE));
    while (!(SFC->RW_SF_MISC_CTL & SFI_REQ_IDLE));
#endif

    SFC->RW_SF_MAC_CTL = val;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Leaves macro mode, and returns to direct read mode
  @remarks
    MT6251, MT6252, MT6255
*/
ATTR_TEXT_IN_TCM void SFI_MacLeave(const uint16_t CS)
{
    uint32_t val;

    // clear SF_TRIG and leave MACRO mode
    val = SFC->RW_SF_MAC_CTL;
    val &= ~(SFI_TRIG | SFI_MAC_SIO_SEL | SFI_MAC_SEL);
    SFC->RW_SF_MAC_CTL = val;

    // SW workaround for MTxxxx F2S write early response and Slave engine latency
    while ((SFC->RW_SF_MAC_CTL & SFI_WIP_READY));

    //Disable SFI_MAC_EN must after polling SFI_WIP_READY for MT6250 HW DCM
    //Because once SFI_MAC_EN is disabled, Idle bit of SFC will be set, HW DCM may gate SFCLK.
    //Then SFI_WIP_READAY will keep high because it depends on SFCLK.
    val = SFC->RW_SF_MAC_CTL;
    val &= ~(SFI_MAC_EN);
    SFC->RW_SF_MAC_CTL = val;

    // SW workaround for MTxxxx F2S write early response and Slave engine latency
    while ((SFC->RW_SF_MAC_CTL & SFI_MAC_EN));

#if !defined(__UBL__)//UnMask SFC AHB Channel 2 and 3 when Flash is not busy
    /*Notice: This variable has not been initialized during ScanSerialFlashBlcoks() in Init stage*/
    /*SFI channel2 and channel3 may be locked and unable to use in maui if NOR_FLASH_BUSY != false*/
    /*Solution: Always Unmask SFI channel2 and channel3 in the end of ScanSerialFlashBlocks()*/
    if ((NOR_FLASH_BUSY == false)) {
        SFI_MaskAhbChannel(0);
    }
#endif

}

//-----------------------------------------------------------------------------
/*!
  @brief
    Send commands placed in GPRAM (in macro mode)
*/
ATTR_TEXT_IN_TCM uint32_t SFI_MacTrigger(const uint16_t CS)
{
    uint32_t  val = 0;
    uint32_t  misc_ctl = 0;

#ifdef SF_1T_WORKAROUND
    uint32_t  j;
    uint8_t   delay1t = 0;
    misc_ctl = SFC->RW_SF_MISC_CTL;
    j   = SFC->RW_SF_MAC_INL);
    delay1t = misc_ctl & 0x80;
    if (j == 0 && delay1t != 0)   {
    val = SFC->RW_SF_MISC_CTL;
    val &= (0xFFFFFF7F);
        SFC->RW_SF_MISC_CTL = val;
    }
#endif // SF_1T_WORKAROUND

    val = SFC->RW_SF_MAC_CTL;

    // trigger SFC
    val |= (SFI_TRIG | SFI_MAC_EN);
    if (CS == 0x1) {  //for CS1
    val |= SFI_MAC_SEL;
}

SFC->RW_SF_MAC_CTL = val;

// wait for SFC ready
while (!(SFC->RW_SF_MAC_CTL & SFI_WIP_READY));
        while ((SFC->RW_SF_MAC_CTL & SFI_WIP));

            return misc_ctl;
        }

//-----------------------------------------------------------------------------
/*!
  @brief
    Set trigger and send GPRAM data toward serial Flash
    Leave macro mode when done
*/
ATTR_TEXT_IN_TCM void SFI_MacWaitReady(const uint16_t CS)
{
#ifdef SF_1T_WORKAROUND
    uint32_t  misc_ctl;
    misc_ctl = SFI_MacTrigger(CS);
    SFI_MacLeave(CS);
    SFC->RW_SF_MISC_CTL = misc_ctl;
#else
    SFI_MacTrigger(CS);
    SFI_MacLeave(CS);
#endif
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Issue generic command to serial Flash (max: 4 bytes)

  @param[in] cmd The command to be sent, the first shift out byte is MSB.
  @remarks This function is only called by bootloader.
           Other callers must be aware of interrupts during the MAC mode
*/
ATTR_TEXT_IN_TCM void SFI_Dev_Command(const uint16_t CS, const uint32_t cmd)
{
    uint32_t cmdi, cmdo;
    uint8_t  len;

    for (cmdi = cmd, len = 0, cmdo = 0; cmdi != 0 ; cmdi = cmdi >> SFC_GENERIC_8_BIT_OFFSET, len++)   {
        cmdo = (cmdo << SFC_GENERIC_8_BIT_OFFSET) | (cmdi & SFC_GENERIC_0xFF_MASK );
    }

    if (len == 0) {
        return;
    }

    SFI_MacEnable(CS);
    SFC_GPRAM->RW_SF_GPRAM_DATA = cmdo;
    SFC->RW_SF_MAC_OUTL = len;
    SFC->RW_SF_MAC_INL = 0;
    SFI_MacWaitReady(CS);

    return;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Send command and address to serial Flash

  @param[in] cmd The command to be sent
  @param[in] address The address to be sent, followed by cmd.
  @remarks This function is only called by bootloader.
           Other callers must be aware of interrupts during the MAC mode
*/
ATTR_TEXT_IN_RAM void SFI_Dev_CommandAddress(const uint16_t CS, const uint8_t cmd, const uint32_t address, const uint32_t address_bytes)
{
    uint32_t cmd1, cmd2;

    cmd2 = SFI_ReverseByteOrder(address);

    if (address_bytes == 3)   {
        cmd1 = (cmd2 & SFC_GENERIC_0xFFFFFF00_MASK ) | cmd;
    } else {
        cmd1 = (cmd2 << SFC_GENERIC_8_BIT_OFFSET) | cmd;
        cmd2 = cmd2 >> SFC_GENERIC_24_BIT_OFFSET;
    }

    SFC_GPRAM->RW_SF_GPRAM_DATA = cmd1;
    SFC_GPRAM->RW_SF_GPRAM_DATA_OF_4 = cmd2;
    SFC->RW_SF_MAC_OUTL = address_bytes + 1;
    SFC->RW_SF_MAC_INL = 0;

    SFI_MacEnable(CS);
    SFI_MacWaitReady(CS);
    return;
}

//-----------------------------------------------------------------------------
/*!
  @brief
    GPRAM access: Write 1 byte command and 3 bytes address

  @param[in] cmd The command to be sent
  @param[in] address The address followed by the command.
*/
ATTR_TEXT_IN_RAM uint32_t SFI_GPRAM_Write_C1A3(const uint32_t cmd, const uint32_t address)
{
    return ((SFI_ReverseByteOrder(address) & SFC_GENERIC_0xFFFFFF00_MASK ) | (cmd & SFC_GENERIC_0xFF_MASK ));
}

//-----------------------------------------------------------------------------
/*!
  @brief
    GPRAM access: Write 1 byte command, 4 bytes address, and 3 bytes of data

  @param[in] cmd The command to be sent
  @param[in] address The address followed by the command.
  @param[in] p_data The pointer-pointer to the data buffer.
  @param[in] length The pointer to the length of the data buffer.
  @remarks This function fills in the 2nd GPRAM register with
     LSB address, and 3 bytes (at most) of the data buffer. It also
     move the given pointer to next buffer address, and subtract the length of
     the data buffer for next GPRAM buffer copy.
*/
#if defined(__SFI_4BYTES_ADDRESS__)
ATTR_TEXT_IN_RAM uint32_t SFI_GPRAM_Write_C1A4(const uint32_t cmd, const uint32_t address, uint8_t **p_data, uint32_t *length)
{
    sf_uint    tmp;
    uint32_t cmd1, cmd2;
    uint32_t i;

    cmd2 = SFI_ReverseByteOrder(address);
    cmd1 = cmd2 << SFC_GENERIC_8_BIT_OFFSET | (cmd & SFC_GENERIC_0xFF_MASK );
    cmd2 = cmd2 >> SFC_GENERIC_24_BIT_OFFSET;

    tmp.u8[0] = cmd2 & SFC_GENERIC_0x000000FF_MASK ;
    for (i = 1; i < 4; i++) {
        tmp.u8[i] = **p_data;
        (*p_data)++;
    }
    cmd2 = tmp.u32;
    *length = (*length > 3) ? (*length - 3) : 0;
    SFC_GPRAM->RW_SF_GPRAM_DATA_OF_4 = cmd2;
    return cmd1;
}
#endif

//-----------------------------------------------------------------------------
/*!
  @brief
    GPRAM access: Write data to GPRAM

  @param[in] gpram_offset The offset of the GPRAM in terms of byte, it must be
             multiple of four.
  @param[in] buff The pointer to the data buffer
  @param[in] length The length of the data buffer.
  @remarks The given gpram offset must be 4 bytes aligned, and the function will
           copy the data buffer to the GPRAM WORD/HALF-WORD/BYTE wise
           according to the starting address of the buffer.
*/
ATTR_TEXT_IN_RAM void SFI_GPRAM_Write(uint32_t gpram_offset, void *buff, int32_t length)
{
    uint32_t i;
    uint8_t *p_data = (uint8_t *)buff;
    sf_uint tmp;

    gpram_offset += RW_SFI_GPRAM_DATA;

    // source address is 4-byte aligned, and gpram_offset is 4-bytes aligned, too
    if (0 == ((uint32_t)buff & 0x3))    {
        for (; length > 0; gpram_offset += 4, p_data += 4, length -= 4)        {
            SFI_WriteReg32(gpram_offset, *(uint32_t *)p_data);
        }
    }
    // source address is 2-byte aligned
    else if (0 == ((uint32_t)p_data & 0x1))     {
        for (; length > 0; gpram_offset += 4, p_data += 4, length -= 4)        {
            tmp.u16[0] = *(uint16_t *)p_data;
            tmp.u16[1] = *((uint16_t *)p_data + 1);
            SFI_WriteReg32(gpram_offset, tmp.u32); // save local_data to SFI GPRAM
        }
    }
    // source data is NOT 2-byte aligned
    else    {
        for (; length > 0; gpram_offset += 4, length -= 4)  {
            for (i = 0; i < 4 && i < length; i++) {
                tmp.u8[i] = *p_data++;
            }
            SFI_WriteReg32(gpram_offset, tmp.u32); // save local_data to SFI GPRAM
        }
    }
}


//-----------------------------------------------------------------------------
/*!
  @brief
    Issue generic command to serial Flash, and read results.

  @param[in] cmd Pointer to the commands that to be sent
  @param[out] data Pointer to the data buffer that to be stored
  @param[in] outl Length of commands (in bytes)
  @param[in] intl Length of read data

  @remarks This function shall only be invoked in bootloader.
           Other callers must be aware of interrupts during the MAC mode
*/
ATTR_TEXT_IN_RAM void SFI_Dev_Command_Ext(const uint16_t CS, const uint8_t *cmd, uint8_t *data, const uint16_t outl, const uint16_t inl)
{
    uint32_t      tmp;
    uint32_t      i, j;
    uint8_t       *p_data, *p_tmp;

    p_tmp = (uint8_t *)(&tmp);

    for (i = 0, p_data = ((uint8_t *)RW_SFI_GPRAM_DATA); i < outl; p_data += 4)   {
        for (j = 0, tmp = 0; i < outl && j < 4; i++, j++) {
            p_tmp[j] = cmd[i];
        }
        SFI_WriteReg32(p_data, tmp);
    }
    SFC->RW_SF_MAC_OUTL = outl;
    SFC->RW_SF_MAC_INL = inl;
    SFI_MacEnable(CS);
    SFI_MacWaitReady(CS);

    for (i = 0, p_data = ((uint8_t *)RW_SFI_GPRAM_DATA + outl); i < inl; ++i, ++data, ++p_data)    {
        *data = SFI_ReadReg8(p_data);
    }
    return;
}



//-----------------------------------------------------------------------------
/*!
  @brief
    Issue multiple generic commands to serial Flash

  @param[in] cmdlist Pointer to the list of the commands that to be sent to serial Flash.

  @remarks 1. This function is only called by bootloader.
              Other callers must be aware of interrupts during the MAC mode
           2. The Command List Format:
                cmd 1- {QPI/SPI, length in bytes, byte1, byte2, ... },
                cmd 2- {QPI/SPI, length in bytes, byte1, byte2, ... },
                ...
                end  - {SF_UNDEF}
              Example:
                {SPI, 1, 0x38}, {QPI, 2, 0xC0, 0x02}, {SF_UNDEF}
           3. If the first command is 0x05, the second byte will be the status polling bit mask.
              The function will wait till (status & mask) euqals zero.
              Example: {SPI, 1, 0x05, 0x01} - polling status register till bit0 becomes zero.

*/
ATTR_TEXT_IN_RAM void SFI_Dev_Command_List(const uint16_t CS, const uint8_t *cmdlist)
{
    uint32_t i = 0;
    uint32_t val;

    if (cmdlist == NULL) {
        return;
    }

#if defined(__DUAL_SERIAL_FLASH_SUPPORT__)
    val = (CS == 0x0) ? SFI_DevMode : SFI_DevMode_CS1; // denotes the orignal mode
#else
    val = SFI_DevMode; // denotes the orignal mode
#endif

    while (1)   {
        switch (cmdlist[i]) { // TODO : compile option for MT6255/51/52 of Direct Read change mode
            case SPI:
            case SPIQ:
                SFI_SetDevMode(CS, 0);
                break;   // Switch to SPI mode
            case QPI:
                SFI_SetDevMode(CS, SFI_QPI_EN);
                break;   // Switch to QPI mode
            default:
                SFI_SetDevMode(CS, val);
                return;  // Resume to orignal mode and returns
        }
        // When Read Status (05h) is encountered
        if (cmdlist[i + 2] == SF_CMD_READ_SR) {
            uint8_t status;
            // The second byte is the bit mask, polling till zero.
            do {
                SFI_Dev_Command_Ext(CS, &cmdlist[i + 2], &status, 1, 1);
            } while (status & cmdlist[i + 3]);
        }
        // Commands other than 05h
        else {
            SFI_Dev_Command_Ext(CS, &cmdlist[i + 2], NULL, cmdlist[i + 1], 0); // send command
        }

        i += (cmdlist[i + 1] + 2);
    }
}

//-----------------------------------------------------------------------------
/*!
  @brief
    Read 3 bytes ID (Vendor + Density + Device) in request mode (SPI/QPI)
  @param[out] id The pointer to the array that ID to be stored
  @remarks This function shall only be invoked in bootloader.
*/
ATTR_TEXT_IN_RAM void SFI_ReadDeviceID(const uint16_t CS, uint8_t *id, uint8_t cmd)
{
    //uint8_t cmd=SF_CMD_READ_ID;
    SFI_Dev_Command_Ext(CS, &cmd, id, 1, SF_FLASH_ID_LENGTH);
}


ATTR_TEXT_IN_RAM void SFI_ReadDeviceID_SPI(const uint16_t CS, uint8_t *id)
{
    SFI_ReadDeviceID(CS, id, SF_CMD_READ_ID);
}



/*!
  @brief
    L1 query this function to get SF working frequency
    software workaround.
  @retval
    SFI_CLK_78/104/130MHZ: 78/104/130 working FQ
    SFI_CLK_UNKNOWN: working FQ unknown
*/
ATTR_TEXT_IN_RAM sfi_clock_enum SFI_Dev_WorkingFrequencyQuery(void)
{

#if defined(__SFI_CLK_78MHZ__)
    return SFI_CLK_78MHZ;
#elif defined(__SFI_CLK_104MHZ__)
    return SFI_CLK_104MHZ;
#elif defined(__SFI_CLK_130MHZ__)
    return SFI_CLK_130MHZ;
#elif defined(__SFI_CLK_166MHZ__)
    return SFI_CLK_166MHZ;
#else
    return SFI_CLK_UNKNOWN;
#endif

}

/*!
  @brief
    L1 query this function to get SF Voltage
    software workaround.
  @retval
    SFI_Voltage_18/ 30: 1.8V/ 3.0V Voltage
    SFI_Voltage_UNKNOWN: working Voltage unknown
*/
ATTR_TEXT_IN_RAM sfi_voltage_enum SFI_Dev_WorkingVoltageQuery(void)
{
    /*Only query Internal SF's driving because it is equal to External SF's driving setting*/
    if ( (SFC->RW_SF_MISC_CTL3 & 0x00700000) <= 0x00200000) { /* <= 6mA for 3.0V Serial Flash*/
        return SFI_Voltage_30;
    } else { /*  >=8 mA for 1.8V Serial Flash*/
        return SFI_Voltage_18;
    }
}

/*!
  @brief
    Query this function to get SF unique ID (not gurantee unique because Vendor limitation)
  @param[in]
    buffer: Read 8 byte unique ID buffer
  @retval
    true: Read unique ID success
    false: Read uinque ID fail
*/
ATTR_TEXT_IN_RAM bool SFI_Dev_GetUniqueID(uint8_t *buffer)
{
    int i = 0;
    for (i = 0; i < 8; i++) {
        buffer[i] = 0xFF;
    }
    return true;
}



/*!
  @brief
    Sleep mode will call SF_DAL_DEV_Enter_DPD and SF_DAL_DEV_Leave_DPD when entering and leaving sleep mode respectively.
  @retval
*/
#include "hal_gpt.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"

extern SF_MTD_Data EntireDiskMtdData;
#define DPDReadyTime (100) //Enter and Leave Deep Power Down mode need 100us at most

ATTR_RWDATA_IN_TCM uint8_t  NOR_FLASH_LeaveDPDCmd;
ATTR_RWDATA_IN_TCM uint32_t NOR_FLASH_FATStartAddr;
ATTR_RWDATA_IN_TCM uint32_t NOR_FLASH_FATStartValue;
ATTR_RWDATA_IN_TCM uint32_t SFI_MAC_CTL_BACKUP;
ATTR_RWDATA_IN_TCM uint32_t SFI_DIRECT_CTL_BACKUP;
ATTR_RWDATA_IN_TCM uint32_t SFI_MISC_CTL_BACKUP;
ATTR_RWDATA_IN_TCM uint32_t SFI_MISC_CTL2_BACKUP;
ATTR_RWDATA_IN_TCM uint32_t SFI_DLY_CTL_3_BACKUP;

ATTR_TEXT_IN_RAM void SF_DAL_DEV_Enter_DPD(void)
{
    SF_MTD_Data *D = &EntireDiskMtdData;
    NOR_FLASH_LeaveDPDCmd = D->CMD->LeaveDPD;
    NOR_FLASH_FATStartAddr = ((uint32_t)D->BaseAddr); //Read first 4byte of flash
    NOR_FLASH_FATStartValue = *(volatile uint32_t *)NOR_FLASH_FATStartAddr;

    /* SFC registers is not in AO domain, will be reset in sleep mode. Back up them here*/
    SFI_MAC_CTL_BACKUP = SFC->RW_SF_MAC_CTL;
    SFI_DIRECT_CTL_BACKUP = SFC->RW_SF_DIRECT_CTL;
    SFI_MISC_CTL_BACKUP = SFC->RW_SF_MISC_CTL;
    SFI_MISC_CTL2_BACKUP = SFC->RW_SF_MISC_CTL2;
    SFI_DLY_CTL_3_BACKUP = SFC->RW_SF_DLY_CTL3;

    //NOR_FLASH_FATStartValue may keep in SFC buffer, but SFI_Dev_Command() will Enable/Disable Macro mode, causing buffer being cleaned
    SFI_Dev_Command(D->CS, D->CMD->EnterDPD);

    hal_gpt_delay_us(DPDReadyTime);
}

/* Leave DPD related codes must put into TCM coz ext-ram will be powered-off in sleep mode */
/* Leave DPD, cache is disabled in sleep mode, PLL is initialized before calling this function(flash to 78MHz) */
/* Only restore SFC registers, Driving/GPIO setting will not be cleared in sleep mode */
ATTR_TEXT_IN_TCM void SF_DAL_DEV_Leave_DPD(void)
{
    /* Restore SFC registers before issue any cmd */
    SFC->RW_SF_MAC_CTL = SFI_MAC_CTL_BACKUP;
    SFC->RW_SF_DIRECT_CTL = SFI_DIRECT_CTL_BACKUP;
    SFC->RW_SF_MISC_CTL = SFI_MISC_CTL_BACKUP;
    SFC->RW_SF_MISC_CTL2 = SFI_MISC_CTL2_BACKUP;
    SFC->RW_SF_DLY_CTL3 = SFI_DLY_CTL_3_BACKUP;

    /* SW workaround: Restore RW_SF_DLY_CTL1,  RW_SF_DLY_CTL3 has been restored in previous step */
    /* Sleep mode will reset SFC registers, thus don't need to restore RW_SF_DLY_CTL1 and RW_SF_DLY_CTL3, but MVG test does not reset SFC registers, hence, restore them here */
    SFC->RW_SF_DLY_CTL1 = SFC->RW_SF_DLY_CTL1 & (~SFC_GENERIC_DPD_SW_IO_MASK);

    /* Issue leave DPD Command */
    SFI_Dev_Command(0, NOR_FLASH_LeaveDPDCmd);

    hal_gpt_delay_us(DPDReadyTime);

    //Compare First 4 bytes before and after DPD to make sure DPD ok. Cache is disabled in sleep mode, therefore the first 4 byte read will get latest value
    if (NOR_FLASH_FATStartValue != (*(volatile uint32_t *)NOR_FLASH_FATStartAddr))
        while (1);
}


ATTR_RWDATA_IN_TCM uint32_t SFI_CLK_SWITCH_MISC_CTL_BACKUP;
ATTR_RWDATA_IN_TCM uint32_t SFI_CLK_SWITCH_DLY_CTL_3_BACKUP;

ATTR_TEXT_IN_TCM void SF_DAL_DEV_SWITCH_TO_LOW_FQ(void)
{
    /* Backup SFC delays, only backup SFC delay regs if it's not been backup yet */
    if ( SFI_CLK_SWITCH_MISC_CTL_BACKUP == 0) {
        SFI_CLK_SWITCH_MISC_CTL_BACKUP = SFC->RW_SF_MISC_CTL;
    }
    if ( SFI_CLK_SWITCH_DLY_CTL_3_BACKUP == 0 ) {
        SFI_CLK_SWITCH_DLY_CTL_3_BACKUP = SFC->RW_SF_DLY_CTL3;
    }

    /* clear SFC delays */
    SFC->RW_SF_MISC_CTL = (SFC->RW_SF_MISC_CTL & SFC_GENERIC_0xFFFFFF00_MASK );
    SFC->RW_SF_DLY_CTL3 = 0x0;
#ifdef MTK_CLK_USE_LFOSC
    /* SFC in LFOSC 26MHz */
    clock_mux_sel(CLK_SFC_SEL, 1);
#else
    /* SFC in DCXO 26MHz */
    clock_mux_sel(CLK_SFC_SEL, 0);
#endif
}
ATTR_TEXT_IN_TCM void SF_DAL_DEV_SWITCH_TO_HIGH_FQ(void)
{
    /* Restore SFC delays, only restore SFC registers if it has value */
    if ( SFI_CLK_SWITCH_MISC_CTL_BACKUP != 0 ) {
        SFC->RW_SF_MISC_CTL = SFI_CLK_SWITCH_MISC_CTL_BACKUP;
    }
    if ( SFI_CLK_SWITCH_DLY_CTL_3_BACKUP != 0) {
        SFC->RW_SF_DLY_CTL3 = SFI_CLK_SWITCH_DLY_CTL_3_BACKUP;
    }
    clock_mux_sel(CLK_SFC_SEL, 7); /* SFC in HFOSC 78MHz */
}



#else  // !HAL_FLASH_MODULE_ENABLED

#include "hal_flash_general_types.h"
#include "hal_flash_sfi_release.h"
#include "memory_attribute.h"

sfi_clock_enum SFI_Dev_WorkingFrequencyQuery(void)
{
    return SFI_CLK_UNKNOWN;
}

sfi_voltage_enum SFI_Dev_WorkingVoltageQuery(void)
{
    return SFI_Voltage_UNKNOWN;
}
bool SFI_Dev_GetUniqueID(uint8_t *buffer)
{
    return true;
}

ATTR_TEXT_IN_TCM void SF_DAL_DEV_Enter_DPD(void)
{
}

ATTR_TEXT_IN_TCM void SF_DAL_DEV_Leave_DPD(void)
{
}
ATTR_TEXT_IN_RAM void SF_DAL_DEV_SWITCH_TO_LOW_FQ(void)
{
}
ATTR_TEXT_IN_RAM void SF_DAL_DEV_SWITCH_TO_HIGH_FQ(void)
{
}

#endif //#ifdef HAL_FLASH_MODULE_ENABLED


