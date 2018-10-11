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

#include "hal_usb.h"
#include "hal_usb_internal.h"
#include "hal_eint.h"
#include "hal_log.h"
#include "hal_clock.h"
#include "hal_clock_internal.h"
#include "hal_pmu_wrap_interface.h"
#include "hal_gpt.h"
#include "hal_nvic.h"

#ifdef HAL_USB_MODULE_ENABLED

static volatile USB_REGISTER_T *musb = (USB_REGISTER_T *)USB_BASE;
static void usb_pdn_enable(void);
static void usb_pdn_disable(void);

/* Exception flag*/
USB_Drv_Info g_UsbDrvInfo;

/* EP0's FIFO address is fixed from 0~63 */
static uint32_t g_FIFOadd = USB_FIFO_START_ADDRESS;
static uint8_t usb_get_dma_channel_num(uint8_t ep_num, hal_usb_endpoint_direction_t direction);
static void usb_hw_stop_dma_channel(uint32_t ep_num, hal_usb_endpoint_direction_t direction);
static void usb_ep_check(uint32_t ep_num, hal_usb_endpoint_direction_t direction, uint32_t line);
static void usb_ep_dma_running_check(uint32_t ep_num, hal_usb_endpoint_direction_t direction, uint32_t line);
static void usb_ep0en(void);
static void usb_hw_epfifowrite(uint32_t ep_num, uint16_t nBytes, void *pSrc);
static bool usb_check_dma_time_out(uint8_t dma_chan);
static void usb_dma_callback_func(uint8_t dma_chan);


/************************************************************
	USB PDN
*************************************************************/
static void usb_pdn_enable(void)
{
    hal_clock_disable(HAL_CLOCK_CG_USB_DMA);
    hal_clock_disable(HAL_CLOCK_CG_USB_BUS);
    hal_clock_disable(HAL_CLOCK_CG_USB48M);

    USB_DRV_ClearBits(0xa20107a0, 0x01); //usbsif_rstb bit

    pmu_set_vcore_voltage(PMIC_VCORE_UNLOCK, PMIC_VOL_1P2V);
    pmu_ctrl_driver_power(DRIVER_USB, PMIC_CTL_DISABLE);	/* Turn off 3.3V phy power */
}

/* USB PDN disable */
static void usb_pdn_disable(void)
{
    pmu_ctrl_driver_power(DRIVER_USB, PMIC_CTL_ENABLE); /* Turn on 3.3V phy power */
    pmu_set_vcore_voltage(PMIC_VCORE_LOCK, PMIC_VOL_1P2V);

    USB_DRV_SetBits(0xa20107a0, 0x01); //usbsif_rstb bit

    clock_mux_sel(USB_MUX_SEL, 3); // 62.4MHz, UPLL

    hal_clock_enable(HAL_CLOCK_CG_USB48M);
    hal_clock_enable(HAL_CLOCK_CG_USB_BUS);
    hal_clock_enable(HAL_CLOCK_CG_USB_DMA);
}

/************************************************************
	Dump USB RG for debug
*************************************************************/
#if 0
static void hal_dump_regs(void)
{
    volatile USBPHY_REGISTER_T *phy = (USBPHY_REGISTER_T *)USB_SIFSLV_BASE;
    volatile USB_REGISTER_T *mac = (USB_REGISTER_T *)USB_BASE;

    log_hal_info("mac->faddr                  : 0x%X\r\n", &mac->faddr);
    log_hal_info("mac->power                  : 0x%X\r\n", &mac->power);
    log_hal_info("mac->intrtx                 : 0x%X\r\n", &mac->intrtx);
    log_hal_info("mac->intrrx                 : 0x%X\r\n", &mac->intrrx);
    log_hal_info("mac->intrtxe                : 0x%X\r\n", &mac->intrtxe);
    log_hal_info("mac->intrrxe                : 0x%X\r\n", &mac->intrrxe);
    log_hal_info("mac->intrusb                : 0x%X\r\n", &mac->intrusb);
    log_hal_info("mac->intrusbe               : 0x%X\r\n", &mac->intrusbe);
    log_hal_info("mac->frame                  : 0x%X\r\n", &mac->frame);
    log_hal_info("mac->index                  : 0x%X\r\n", &mac->index);
    log_hal_info("mac->testmode               : 0x%X\r\n", &mac->testmode);
    log_hal_info("mac->txmap                  : 0x%X\r\n", &mac->txmap);
    log_hal_info("mac->txcsr                  : 0x%X\r\n", &mac->txcsr);
    log_hal_info("mac->rxmap                  : 0x%X\r\n", &mac->rxmap);
    log_hal_info("mac->rxcsr                  : 0x%X\r\n", &mac->rxcsr);
    log_hal_info("mac->rxcount                : 0x%X\r\n", &mac->rxcount);
    log_hal_info("mac->txtype                 : 0x%X\r\n", &mac->txtype);
    log_hal_info("mac->txinterval             : 0x%X\r\n", &mac->txinterval);
    log_hal_info("mac->rxtype                 : 0x%X\r\n", &mac->rxtype);
    log_hal_info("mac->rxinterval             : 0x%X\r\n", &mac->rxinterval);
    log_hal_info("mac->fifosize               : 0x%X\r\n", &mac->fifosize);
    log_hal_info("mac->fifo0                  : 0x%X\r\n", &mac->fifo0);
    log_hal_info("mac->fifo1                  : 0x%X\r\n", &mac->fifo1);
    log_hal_info("mac->devctl                 : 0x%X\r\n", &mac->devctl);
    log_hal_info("mac->pwrupcnt               : 0x%X\r\n", &mac->pwrupcnt);
    log_hal_info("mac->txfifosz               : 0x%X\r\n", &mac->txfifosz);
    log_hal_info("mac->rxfifosz               : 0x%X\r\n", &mac->rxfifosz);
    log_hal_info("mac->txfifoadd              : 0x%X\r\n", &mac->txfifoadd);
    log_hal_info("mac->rxfifoadd              : 0x%X\r\n", &mac->rxfifoadd);
    log_hal_info("mac->hwcaps		              : 0x%X\r\n", &mac->hwcaps);
    log_hal_info("mac->hwsvers	              : 0x%X\r\n", &mac->hwsvers);
    log_hal_info("mac->busperf1	              : 0x%X\r\n", &mac->busperf1);
    log_hal_info("mac->busperf2	              : 0x%X\r\n", &mac->busperf2);
    log_hal_info("mac->busperf3	              : 0x%X\r\n", &mac->busperf3);
    log_hal_info("mac->epinfo		              : 0x%X\r\n", &mac->epinfo);
    log_hal_info("mac->raminfo	              : 0x%X\r\n", &mac->raminfo);
    log_hal_info("mac->linkinfo	              : 0x%X\r\n", &mac->linkinfo);
    log_hal_info("mac->vplen		              : 0x%X\r\n", &mac->vplen);
    log_hal_info("mac->hs_eof1	              : 0x%X\r\n", &mac->hs_eof1);
    log_hal_info("mac->fs_eof1	              : 0x%X\r\n", &mac->fs_eof1);
    log_hal_info("mac->ls_eof1	              : 0x%X\r\n", &mac->ls_eof1);
    log_hal_info("mac->rst_info	              : 0x%X\r\n", &mac->rst_info);
    log_hal_info("mac->rxtog		              : 0x%X\r\n", &mac->rxtog);
    log_hal_info("mac->rxtogen	              : 0x%X\r\n", &mac->rxtogen);
    log_hal_info("mac->txtog		              : 0x%X\r\n", &mac->txtog);
    log_hal_info("mac->txtogen	              : 0x%X\r\n", &mac->txtogen);
    log_hal_info("mac->usb_l1ints             : 0x%X\r\n", &mac->usb_l1ints);
    log_hal_info("mac->usb_l1intm             : 0x%X\r\n", &mac->usb_l1intm);
    log_hal_info("mac->usb_l1intp             : 0x%X\r\n", &mac->usb_l1intp);
    log_hal_info("mac->usb_l1intc             : 0x%X\r\n", &mac->usb_l1intc);
    log_hal_info("mac->csr0		                : 0x%X\r\n", &mac->csr0);
    log_hal_info("mac->count0		              : 0x%X\r\n", &mac->count0);
    log_hal_info("mac->type0		              : 0x%X\r\n", &mac->type0);
    log_hal_info("mac->naklimt0	              : 0x%X\r\n", &mac->naklimt0);
    log_hal_info("mac->sramconfigsize	        : 0x%X\r\n", &mac->sramconfigsize);
    log_hal_info("mac->hbconfigdata           : 0x%X\r\n", &mac->hbconfigdata);
    log_hal_info("mac->configdata	            : 0x%X\r\n", &mac->configdata);
    log_hal_info("mac->tx1map		              : 0x%X\r\n", &mac->tx1map);
    log_hal_info("mac->tx1csr		              : 0x%X\r\n", &mac->tx1csr);
    log_hal_info("mac->rx1map		              : 0x%X\r\n", &mac->rx1map);
    log_hal_info("mac->rx1csr		              : 0x%X\r\n", &mac->rx1csr);
    log_hal_info("mac->rx1count	              : 0x%X\r\n", &mac->rx1count);
    log_hal_info("mac->tx1type	              : 0x%X\r\n", &mac->tx1type);
    log_hal_info("mac->tx1interval	          : 0x%X\r\n", &mac->tx1interval);
    log_hal_info("mac->rx1type	              : 0x%X\r\n", &mac->rx1type);
    log_hal_info("mac->rx1interval	          : 0x%X\r\n", &mac->rx1interval);
    log_hal_info("mac->fifosize1	            : 0x%X\r\n", &mac->fifosize1);
    log_hal_info("mac->dma_intr_status        : 0x%X\r\n", &mac->dma_intr_status);
    log_hal_info("mac->dma_intr_unmask        : 0x%X\r\n", &mac->dma_intr_unmask);
    log_hal_info("mac->dma_intr_unmask_clear  : 0x%X\r\n", &mac->dma_intr_unmask_clear);
    log_hal_info("mac->dma_intr_unmask_set    : 0x%X\r\n", &mac->dma_intr_unmask_set);
    log_hal_info("mac->dma_cntl_0		          : 0x%X\r\n", &mac->dma_cntl_0);
    log_hal_info("mac->dma_addr_0	            : 0x%X\r\n", &mac->dma_addr_0);
    log_hal_info("mac->dma_count_0	          : 0x%X\r\n", &mac->dma_count_0);
    log_hal_info("mac->dma_limiter	          : 0x%X\r\n", &mac->dma_limiter);
    log_hal_info("mac->dma_config	            : 0x%X\r\n", &mac->dma_config);
    log_hal_info("mac->ep1rxpktcount	        : 0x%X\r\n", &mac->ep1rxpktcount);
    log_hal_info("mac->t0funcaddr	            : 0x%X\r\n", &mac->t0funcaddr);
    log_hal_info("mac->t0hubaddr	            : 0x%X\r\n", &mac->t0hubaddr);
    log_hal_info("mac->t1funcaddr	            : 0x%X\r\n", &mac->t1funcaddr);
    log_hal_info("mac->t1hubaddr	            : 0x%X\r\n", &mac->t1hubaddr);
    log_hal_info("mac->r1funcaddr	            : 0x%X\r\n", &mac->r1funcaddr);
    log_hal_info("mac->r1hubaddr	            : 0x%X\r\n", &mac->r1hubaddr);
    log_hal_info("mac->tm1		                : 0x%X\r\n", &mac->tm1);
    log_hal_info("mac->hwver_date             : 0x%X\r\n", &mac->hwver_date);
    log_hal_info("mac->srama	                : 0x%X\r\n", &mac->srama);
    log_hal_info("mac->sramd	                : 0x%X\r\n", &mac->sramd);
    log_hal_info("mac->risc_size	            : 0x%X\r\n", &mac->risc_size);
    log_hal_info("mac->resreg		              : 0x%X\r\n", &mac->resreg);
    log_hal_info("mac->otg20_csrl             : 0x%X\r\n", &mac->otg20_csrl);
    log_hal_info("mac->otg20_csrh             : 0x%X\r\n", &mac->otg20_csrh);


    log_hal_info("phy->u2phyac0   : 0x%X\r\n",   &phy->u2phyac0);
    log_hal_info("phy->u2phyac1   : 0x%X\r\n",   &phy->u2phyac1);
    log_hal_info("phy->u2phyac2   : 0x%X\r\n",   &phy->u2phyac2);
    log_hal_info("phy->u2phyacr0  : 0x%X\r\n",   &phy->u2phyacr0);
    log_hal_info("phy->u2phyacr1  : 0x%X\r\n",   &phy->u2phyacr1);
    log_hal_info("phy->u2phyacr2  : 0x%X\r\n",   &phy->u2phyacr2);
    log_hal_info("phy->u2phyacr3  : 0x%X\r\n",   &phy->u2phyacr3);
    log_hal_info("phy->u2phyacr4  : 0x%X\r\n",   &phy->u2phyacr4);
    log_hal_info("phy->u2phydcr0  : 0x%X\r\n",   &phy->u2phydcr0);
    log_hal_info("phy->u2phydcr1  : 0x%X\r\n",   &phy->u2phydcr1);
    log_hal_info("phy->u2phydtm0  : 0x%X\r\n",   &phy->u2phydtm0);
    log_hal_info("phy->u2phydtm1  : 0x%X\r\n",   &phy->u2phydtm1);
    log_hal_info("phy->u2phydmon0 : 0x%X\r\n",   &phy->u2phydmon0);
    log_hal_info("phy->u2phydmon1 : 0x%X\r\n",   &phy->u2phydmon1);
    log_hal_info("phy->u2phydmon2 : 0x%X\r\n",   &phy->u2phydmon2);
    log_hal_info("phy->u1phycr0   : 0x%X\r\n",   &phy->u1phycr0);
    log_hal_info("phy->u1phycr1   : 0x%X\r\n",   &phy->u1phycr1);
    log_hal_info("phy->u1phycr2   : 0x%X\r\n",   &phy->u1phycr2);
    log_hal_info("phy->regfppc    : 0x%X\r\n",   &phy->regfppc);
    log_hal_info("phy->versionc   : 0x%X\r\n",   &phy->versionc);
    log_hal_info("phy->regfcom    : 0x%X\r\n",   &phy->regfcom);
    log_hal_info("phy->fmcr0      : 0x%X\r\n",   &phy->fmcr0);
}
#endif
/************************************************************
	DMA utilities
*************************************************************/
static uint8_t usb_get_dma_channel_num(uint8_t ep_num, hal_usb_endpoint_direction_t direction)
{
    uint8_t  dma_chan;
    dma_chan = g_UsbDrvInfo.dma_port[direction][ep_num - 1];

    if ((dma_chan == 0) || (dma_chan > HAL_USB_MAX_NUMBER_DMA) || (ep_num == 0)) {
        log_hal_error("ASSERT\r\n");
    }

    return dma_chan;
}

/* Stop DMA channel */
static void usb_hw_stop_dma_channel(uint32_t ep_num, hal_usb_endpoint_direction_t direction)
{
    uint32_t 	savedMask;
    uint8_t 	dma_chan;

    dma_chan = usb_get_dma_channel_num(ep_num, direction);

    savedMask = SaveAndSetIRQMask();
    USBDMA_Stop(dma_chan);

    /* Clear pending DMA interrupts */
    DRV_WriteReg8(&musb->dma_intr_status, (1 << (dma_chan - 1)));
    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = false;
    g_UsbDrvInfo.dma_running[dma_chan - 1] = false;
    RestoreIRQMask(savedMask);
}


/************************************************************
	driver debug utility
*************************************************************/

static void usb_ep_check(uint32_t ep_num, hal_usb_endpoint_direction_t direction, uint32_t line)
{
    if ((ep_num == 0) || ((direction == HAL_USB_EP_DIRECTION_TX) && (ep_num > HAL_USB_MAX_NUMBER_ENDPOINT_TX)) ||
            ((direction == HAL_USB_EP_DIRECTION_RX) && (ep_num > HAL_USB_MAX_NUMBER_ENDPOINT_RX))) {
        log_hal_info("ASSERT\r\n");
    }
}


static void usb_ep_dma_running_check(uint32_t ep_num, hal_usb_endpoint_direction_t direction, uint32_t line)
{
    uint8_t	dma_chan;

    dma_chan = g_UsbDrvInfo.dma_port[direction][ep_num - 1];

    if ((dma_chan != 0) && (USBDMA_CheckRunStat(dma_chan) != 0) && (g_UsbDrvInfo.dma_dir[dma_chan - 1] == direction)) {
        /* Fix for USB compliance test program */
        usb_hw_stop_dma_channel(ep_num, direction);
    }
}

static void usb_ep0en(void)
{
    /* Default address is from 0 to 63  */
    USB_DRV_SetBits(&musb->intrtxe, USB_INTRE_EPEN);
}

/*  Write data to FIFO EP */
static void usb_hw_epfifowrite(uint32_t ep_num, uint16_t nBytes, void *pSrc)
{
    uint16_t nCount = nBytes;
    uint8_t *pby;
    uint32_t nAddr;
    uint32_t  temp_1, temp_2;
    uint32_t *pby1;

    if ((nBytes != 0) && (pSrc == NULL)) {
        log_hal_error("ASSERT\r\n");
    }

    if (pSrc == NULL) {
        log_hal_info("usb_hw_epfifowrite Error: pSrc is NULL!!\r\n");
        return;
    }

    /* not index register */
    nAddr = (uint32_t)(&musb->fifo0 + ep_num);

    if (((uint32_t)pSrc % 4) == 0) {
        /* Source address is 4 byte alignment */
        temp_1 = nCount / 4;
        temp_2 = nCount % 4;

        pby1 = (uint32_t *)pSrc;

        while (temp_1) {
            USB_DRV_WriteReg32(nAddr, *pby1++);	//lint !e613
            temp_1--;
        }

        pby = (uint8_t *)pby1;

        while (temp_2) {
            USB_DRV_WriteReg8(nAddr, *pby++);	//lint !e613
            temp_2--;
        }
    } else {
        pby = (uint8_t *)pSrc;

        /* Read byte by byte */
        while (nCount) {
            USB_DRV_WriteReg8(nAddr, *pby++);	//lint !e613
            nCount--;
        }
    }
}

hal_usb_endpoint_state_t hal_usb_check_rx_endpoint_usage(uint32_t ep_num)
{
    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    return g_UsbDrvInfo.ep_rx_enb_state[ep_num - 1];
}

hal_usb_endpoint_state_t hal_usb_check_tx_endpoint_usage(uint32_t ep_num)
{
    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    return g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1];
}

hal_usb_status_t hal_usb_configure_rx_endpoint_type(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_is_use_dma)
{
    uint32_t savedMask;
    bool b_is_switch_to_dma;


    b_is_switch_to_dma = b_is_use_dma;


    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    if (b_is_switch_to_dma == true) {
        //if(g_UsbDrvInfo.ep_rx_enb_state[ep_num-1] == HAL_USB_EP_STATE_DMA)
        //	log_hal_info("ASSERT\r\n");

        g_UsbDrvInfo.ep_rx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_DMA;

        savedMask = SaveAndSetIRQMask();
        USB_DRV_ClearBits(&musb->intrrxe, (USB_INTRE_EPEN << ep_num));
        USB_DRV_WriteReg8(&musb->index, ep_num);

        /* Must set related DMA bits, otherwise, packets arrive will result in RX ep intr */
        if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            USB_DRV_WriteReg(&musb->rxcsr, USB_DMA_RX_CSR_ISO | USB_RXCSR_RXPKTRDY);
        } else {
            USB_DRV_WriteReg(&musb->rxcsr, USB_DMA_RX_CSR | USB_RXCSR_RXPKTRDY);
        }

        RestoreIRQMask(savedMask);
    } else {
        //if(g_UsbDrvInfo.ep_rx_enb_state[ep_num-1] == HAL_USB_EP_STATE_FIFO)
        //	log_hal_info("ASSERT\r\n");

        g_UsbDrvInfo.ep_rx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_FIFO;

        savedMask = SaveAndSetIRQMask();
        USB_DRV_WriteReg8(&musb->index, ep_num);

        /* When we change ep state from DMA to FIFO, must make sure that FIFO is empty */
        /* But there may be a null packet coming here in DMA callback */
        if (USB_DRV_Reg(&musb->rxcsr)&USB_RXCSR_RXPKTRDY) {
            if (USB_DRV_Reg(&musb->rxcount) == 0) {
                USB_DRV_ClearBits(&musb->rxcsr, USB_RXCSR_RXPKTRDY);
            } else {
                log_hal_info("ASSERT\r\n");
            }
        }

        if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            //USB_DRV_WriteReg(USB_RXCSR, (USB_RXCSR_ISO|USB_RXCSR_RXPKTRDY));
            /*It may clear STALL status, need to prevent it.*/
            USB_DRV_WriteReg(&musb->rxcsr, USB_DRV_Reg(&musb->rxcsr) & (USB_RXCSR_ISO | USB_RXCSR_RXPKTRDY | USB_RXCSR_SENDSTALL | USB_RXCSR_SENTSTALL));
        } else {
            //USB_DRV_WriteReg(USB_RXCSR, USB_RXCSR_RXPKTRDY);
            /*It may clear STALL status, need to prevent it.*/
            USB_DRV_WriteReg(&musb->rxcsr, USB_DRV_Reg(&musb->rxcsr) & (USB_RXCSR_RXPKTRDY | USB_RXCSR_SENDSTALL | USB_RXCSR_SENTSTALL));
        }

        USB_DRV_SetBits(&musb->intrrxe, (USB_INTRE_EPEN << ep_num));
        RestoreIRQMask(savedMask);
    }


    return HAL_USB_STATUS_OK;
}

hal_usb_status_t hal_usb_configure_tx_endpoint_type(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_is_use_dma)
{
    uint32_t savedMask;
    bool b_is_switch_to_dma;


    b_is_switch_to_dma = b_is_use_dma;


    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    if (b_is_switch_to_dma == true) {
        if (g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1] == HAL_USB_EP_STATE_DMA) {
            log_hal_info("ASSERT\r\n");
        }
        g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_DMA;

        savedMask = SaveAndSetIRQMask();
        USB_DRV_ClearBits(&musb->intrtxe, (USB_INTRE_EPEN << ep_num));
        USB_DRV_WriteReg8(&musb->index, ep_num);

        if (USB_DRV_Reg(&musb->txcsr)&USB_TXCSR_FIFONOTEMPTY) {
            log_hal_info("ASSERT\r\n");
        }

        if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            USB_DRV_WriteReg(&musb->txcsr, USB_TXCSR_ISO);
        } else {
            USB_DRV_WriteReg(&musb->txcsr, 0x00);
        }

        /* Only is configured as multiple packet DMA TX mode */
        /*
        		if(ep_type == USB_ENDPT_ISO)
        		{
        			USB_DRV_WriteReg(USB_TXCSR, USB_DMA_TX_CSR_ISO);
        		}
        		else
        		{
        			USB_DRV_WriteReg(USB_TXCSR, USB_DMA_TX_CSR);
        		}
        */
        RestoreIRQMask(savedMask);
    } else {
        if (g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1] == HAL_USB_EP_STATE_FIFO) {
            log_hal_info("ASSERT\r\n");
        }

        g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_FIFO;

        savedMask = SaveAndSetIRQMask();
        USB_DRV_WriteReg8(&musb->index, ep_num);

        /* From DMA to FIFO, it may be not empty because this is DMA callback */
        //		if(USB_DRV_Reg(USB_TXCSR)&USB_TXCSR_FIFONOTEMPTY)
        //			ASSERT(0);

        /* When change ep state from DMA to FIFO, must make sure that FIFO is empty */
        if (USB_DRV_Reg(&musb->txcsr)&USB_TXCSR_TXPKTRDY) {
            log_hal_info("ASSERT\r\n");
        }

        if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            USB_DRV_WriteReg(&musb->txcsr, USB_TXCSR_ISO);
        } else {
            USB_DRV_WriteReg(&musb->txcsr, 0x00);
        }

        USB_DRV_SetBits(&musb->intrtxe, (USB_INTRE_EPEN << ep_num));
        RestoreIRQMask(savedMask);
    }


    return HAL_USB_STATUS_OK;
}

/************************************************************
	HISR/LISR   interrupt handler
*************************************************************/
static void usb_eint_hisr(void)
{
    /*log_hal_info("usb_eint_hisr\n");*/
}

void usb_hisr(void)
{
    uint8_t   IntrUSB;
    uint16_t  IntrTx;
    uint16_t  TXCSR;
    uint16_t  RXCSR;
    uint16_t  IntrRx;
    uint16_t  IntrRxE;
    uint32_t  ep_num;
    //uint8_t    USBMode;
    uint32_t savedMask;
    uint32_t  	DMAIntr;
    //uint32_t  	DMAQIntr;
    static uint32_t  	index;

    // read and clear
    IntrUSB = USB_DRV_Reg8(&musb->intrusb);
    USB_DRV_WriteReg8(&musb->intrusb, IntrUSB);

    // Write 1 clear
    IntrTx = musb->intrtx;
    USB_DRV_WriteReg(&musb->intrtx, IntrTx);

    // Write 0 clear
    IntrRx = musb->intrrx;
    IntrRxE = musb->intrrxe;
    IntrRx = IntrRx & IntrRxE;
    USB_DRV_WriteReg(&musb->intrrx, IntrRx);

    //USBMode = musb->power;

    // Reset must be the highest priority ISR
    /* Check for reset interrupt */
    if (IntrUSB & USB_INTRUSB_RESET) {
        /*log_hal_info("USB_INTRUSB_RESET\r\n");*/
        g_UsbDrvInfo.power_state = HAL_USB_POWER_STATE_NORMAL;
        USB_DRV_WriteReg8(&musb->faddr, 0);

        if (g_UsbDrvInfo.reset_hdlr == NULL) {
            log_hal_error("ASSERT\r\n");
        }

        /* Set SW_RST bit here, so we do not care about any following ep interrupts */
        g_UsbDrvInfo.reset_hdlr();
    }

    /* Check for resume from suspend mode */
    if (IntrUSB & USB_INTRUSB_RESUME) {
        /*log_hal_info("USB_INTRUSB_RESUME\r\n");*/
        g_UsbDrvInfo.power_state = HAL_USB_POWER_STATE_NORMAL;

        if ((musb->power & USB_POWER_HSMODE) != 0) {
            g_UsbDrvInfo.Is_HS_mode = true;
        } else {
            g_UsbDrvInfo.Is_HS_mode = false;
        }

        g_UsbDrvInfo.resume_hdlr();
    }

    /* Check for endpoint 0 interrupt */
    if (IntrTx & USB_INTRTX_EP0) {
        /*log_hal_info("USB_INTRTX_EP0\r\n");*/
        if (g_UsbDrvInfo.ep0_hdlr == NULL) {
            log_hal_error("ASSERT\r\n");
        }

        g_UsbDrvInfo.ep0_hdlr();
    }

    /* Check for Bulk TX interrupt */
    /* TX interrupt : 1.TxPktRdy is clear, 2. SENTSTALL is set */
    for (ep_num = 1; ep_num <= HAL_USB_MAX_NUMBER_ENDPOINT_TX; ep_num++) {
        if (IntrTx & (0x01 << ep_num)) {
            savedMask = SaveAndSetIRQMask();
            USB_DRV_WriteReg8(&musb->index, ep_num);
            TXCSR = USB_DRV_Reg(&musb->txcsr);
            RestoreIRQMask(savedMask);
            if ((TXCSR & USB_TXCSR_SENTSTALL) != 0) {
                /* Only clear SENTSTALL bit */
                TXCSR &= ~USB_TXCSR_SENTSTALL;
                savedMask = SaveAndSetIRQMask();
                USB_DRV_WriteReg8(&musb->index, ep_num);
                USB_DRV_WriteReg(&musb->txcsr, TXCSR);
                RestoreIRQMask(savedMask);
            } else {
                if (g_UsbDrvInfo.ep_tx_hdlr[ep_num - 1] != NULL) {
                    g_UsbDrvInfo.ep_tx_hdlr[ep_num - 1]();
                } else {
                    if (g_UsbDrvInfo.ep_tx_flush_intr[ep_num - 1] == true) {
                        g_UsbDrvInfo.ep_tx_flush_intr[ep_num - 1] = false;
                    }
                }
            }
        }
    }

    /* Check for Bulk RX interrupt */
    /* RX interrupt : 1.RxPktRdy is set, 2. SENTSTALL is set */
    for (ep_num = 1; ep_num <= HAL_USB_MAX_NUMBER_ENDPOINT_RX; ep_num++) {
        if (IntrRx & (0x01 << ep_num)) {
            savedMask = SaveAndSetIRQMask();  /*prevent race condition with putisrbyte */
            USB_DRV_WriteReg8(&musb->index, ep_num);
            RXCSR = USB_DRV_Reg(&musb->rxcsr);
            RestoreIRQMask(savedMask);

            if (g_UsbDrvInfo.power_state != HAL_USB_POWER_STATE_NORMAL) {
                RXCSR = 0x00;
            }

            if ((RXCSR & USB_RXCSR_SENTSTALL) != 0) {
                RXCSR &= ~USB_RXCSR_SENTSTALL;
                savedMask = SaveAndSetIRQMask();  /*prevent race condition with putisrbyte */
                USB_DRV_WriteReg8(&musb->index, ep_num);
                USB_DRV_WriteReg(&musb->rxcsr, RXCSR);
                RestoreIRQMask(savedMask);
            } else if ((RXCSR & USB_RXCSR_RXPKTRDY) != 0) {
                if (g_UsbDrvInfo.ep_rx_hdlr[ep_num - 1] != NULL) {
                    g_UsbDrvInfo.ep_rx_hdlr[ep_num - 1]();
                } else {
                    /* No handler to receive this packet */
                    USB_DRV_ClearBits(&musb->rxcsr, USB_RXCSR_RXPKTRDY);
                }
            } else {
                if (g_UsbDrvInfo.ep_rx_flush_intr[ep_num - 1] == true) {
                    g_UsbDrvInfo.ep_rx_flush_intr[ep_num - 1] = false;
                }
            }
        }
    }

    /* When cable plugged out, it will trigger disconn interrupt */
    if (IntrUSB & USB_INTRUSB_DISCON) {
        log_hal_info("USB_INTRUSB_DISCON\r\n");
    }

    /* Check for suspend mode */
    if (IntrUSB & USB_INTRUSB_SUSPEND) {
        /*log_hal_info("USB_INTRUSB_SUSPEND\r\n");*/
        g_UsbDrvInfo.power_state = HAL_USB_POWER_STATE_SUSPEND;
        g_UsbDrvInfo.Is_HS_mode = false;
        g_UsbDrvInfo.suspend_hdlr();
    }


    /*******************/
    /*DMA_HISR        */
    /*******************/
    DMAIntr = USB_DRV_Reg8(&musb->dma_intr_status);
    while (DMAIntr != 0) {
        for (index = 0; index < HAL_USB_MAX_NUMBER_DMA; index++) {
            if ((DMAIntr & (1 << index)) != 0) {
                // write 1 clear first, then do call back function, order is very important !!!
                USB_DRV_WriteReg8(&musb->dma_intr_status, (0x01 << index));

                /* DMA interrupt still occur if stop DMA channel and DMA do a transfer done at the same time.
                 * Must skip it.
                 */
                if (g_UsbDrvInfo.dma_running[index] == true) {
                    /* DMA channel index+1 have interrupt */
                    //if(USB_DRV_Reg32(USB_DMA_REALCNT(index+1)) != 0)
                    if (1) {
                        usb_dma_callback_func(index + 1);
                    } else {
                        if (usb_check_dma_time_out(index + 1) == true) {
                            usb_dma_callback_func(index + 1);
                        } else {
                            log_hal_error("ASSERT\r\n");
                        }
                    }
                }
            }
        }

        /* Read register again to see if any interrupt is generated again */
        /* if pong pong buffer serve done, then we do call back function here */
        DMAIntr = USB_DRV_Reg8(&musb->dma_intr_status);
    }

    /* Clear interrupt and unmask interrupt if application agree on it */
    if (g_UsbDrvInfo.is_unMaskUSB == true) {
        hal_nvic_enable_irq(USB20_IRQn);
    }
}

static hal_usb_status_t hal_usb_drv_create_isr(void)
{
    hal_eint_config_t eint_config;
    hal_eint_status_t result;

    /*interrupt*/
    hal_nvic_register_isr_handler((hal_nvic_irq_t)USB20_IRQn, (hal_nvic_isr_t)usb_hisr);
    hal_nvic_enable_irq(USB20_IRQn);

    /*eint*/
    eint_config.trigger_mode = HAL_EINT_LEVEL_LOW;
    eint_config.debounce_time = 0;
    hal_eint_mask(HAL_EINT_USB);
    result = hal_eint_init(HAL_EINT_USB, &eint_config);
    if (result != HAL_EINT_STATUS_OK) {
        log_hal_info("hal_eint_init fail: %d\r\n", result);
        return HAL_USB_STATUS_ERROR;
    }
    result = hal_eint_register_callback((hal_eint_number_t)HAL_EINT_USB, (hal_eint_callback_t)usb_eint_hisr, NULL);
    if (result != HAL_EINT_STATUS_OK) {
        log_hal_info("hal_eint_register_callback fail: %d\r\n", result);
        return HAL_USB_STATUS_ERROR;
    }
    //hal_eint_unmask(HAL_EINT_USB);

    return HAL_USB_STATUS_OK;
}

/* enable system global interrupt*/
static void usb_en_sys_intr(void)
{
    USB_DRV_WriteReg8(&musb->intrusbe, ~USB_INTRUSBE_SOF);
}


/* EP TX data prepared ready, set ready bit */
static void usb_hw_ep_tx_ready(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    savedMask = SaveAndSetIRQMask();  //DMA_Setup : clear UnderRun bit & Set TX_PkyReady at the same time
    USB_DRV_WriteReg8(&musb->index, ep_num);

    if (USB_DRV_Reg(&musb->txcsr)&USB_TXCSR_TXPKTRDY) { // check TX_PktReady bit
        log_hal_error("ASSERT\r\n");
    }

    USB_DRV_SetBits(&musb->txcsr, USB_TXCSR_TXPKTRDY);
    RestoreIRQMask(savedMask);

}


/* EP RX data already read out, clear the data */
static void usb_hw_ep_rx_ready(uint32_t ep_num)
{
    uint32_t savedMask;
    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);

    if ((USB_DRV_Reg(&musb->rxcsr)&USB_RXCSR_RXPKTRDY) == 0) { // check RX_PktReady bit
        log_hal_error("ASSERT\r\n");
    }

    USB_DRV_ClearBits(&musb->rxcsr, USB_RXCSR_RXPKTRDY);
    RestoreIRQMask(savedMask);
}


/* Get status. See if ep in fifo is empty.
   If false, it means some data in fifo still wait to send out */
static bool usb_hw_is_ep_tx_empty(uint32_t ep_num)
{
    uint32_t savedMask;
    uint16_t  	CSR;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    CSR = USB_DRV_Reg(&musb->txcsr);
    RestoreIRQMask(savedMask);

    if (CSR & USB_TXCSR_FIFONOTEMPTY) {
        return  false;
    } else {
        return  true;
    }

}


/************************************************************
	DMA control functions
*************************************************************/
static void usb_dma_callback_func(uint8_t dma_chan)
{
    hal_usb_dma_handler_t callback = g_UsbDrvInfo.dma_callback[dma_chan - 1];
    uint8_t ep_num;

    if (dma_chan == 0) {
        log_hal_error("ASSERT\r\n");
    }

    g_UsbDrvInfo.dma_callback[dma_chan - 1] = NULL;

    if (g_UsbDrvInfo.dma_dir[dma_chan - 1] == HAL_USB_EP_DIRECTION_TX) {
        ep_num = g_UsbDrvInfo.dma_tx_ep_num[dma_chan - 1];
    } else {
        ep_num = g_UsbDrvInfo.dma_rx_ep_num[dma_chan - 1];
    }


    if (usb_check_dma_time_out(dma_chan) == false) {
        /* ep0 do not do this */
        if (g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] == true) {
            if (g_UsbDrvInfo.dma_dir[dma_chan - 1] == HAL_USB_EP_DIRECTION_TX) {
                usb_hw_ep_tx_ready(ep_num, HAL_USB_EP_TRANSFER_BULK);

            } else if (g_UsbDrvInfo.dma_dir[dma_chan - 1] == HAL_USB_EP_DIRECTION_RX) {
                usb_hw_ep_rx_ready(ep_num);
            }
        }
    }

    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = false;

    /* if application callback function does not request to clear drv_running set by itself, clear running state here*/
    if (g_UsbDrvInfo.dma_callback_upd_run[dma_chan - 1] == false) {
        g_UsbDrvInfo.dma_running[dma_chan - 1] = false;
    }

    if (callback != NULL) {
        callback();
    }
}

#ifdef  __DMA_UNKNOWN_RX__
static void usb_enable_dma_timer_count(uint8_t dma_chan, bool enable, uint8_t timer_ticks)
{
    if (enable == true) {
        USB_DRV_WriteReg(USB_DMA_TIMER(dma_chan), USB_DMA_TIMER_ENTIMER | (timer_ticks & USB_DMA_TIMER_TIMEOUT_MASK));
    } else {
        // Disable DMA timer
        USB_DRV_WriteReg(USB_DMA_TIMER(dma_chan), 0);
    }
}
#endif

static bool usb_check_dma_time_out(uint8_t dma_chan)
{
    if ((USB_DRV_Reg(USB_DMA_TIMER(dma_chan))&USB_DMA_TIMER_TIMEOUT_STATUS) != 0) {
        return true;
    } else {
        return false;
    }
}

/*******************************************************************************
	USB Power Down Related
 *******************************************************************************/
static hal_usb_status_t hal_usb_pdn_mode(bool pdn_en)
{
    if (pdn_en == true) {
        /* Power down */
        hal_nvic_disable_irq(USB20_IRQn);
        USB_DRV_ClearBits8(&musb->power, USB_POWER_SOFTCONN);

        USB_DRV_WriteReg(&musb->intrtxe, 0);
        USB_DRV_WriteReg(&musb->intrrxe, 0);
        USB_DRV_WriteReg8(&musb->intrusbe, 0);
        USB_DRV_Reg8(&musb->intrusb);
        USB_DRV_Reg(&musb->intrtx);

        USB_DRV_WriteReg(&musb->intrrx, 0xFFFF); // write 1 clear
        hal_usbphy_save_current();
    } else {
        /* Enable sequence: 1. UPLL, 2.PDN_USB(USB power), 3.USB(USB register), 4.GPIO(D+) */
        usb_pdn_disable();
        hal_usbphy_poweron_initialize();
        hal_usbphy_recover();
        hal_usbphy_slew_rate_calibration();

        USB_DRV_WriteReg(&musb->intrtxe, 0);
        USB_DRV_WriteReg(&musb->intrrxe, 0);
        USB_DRV_WriteReg8(&musb->intrusbe, 0);
        USB_DRV_Reg8(&musb->intrusb);
        USB_DRV_Reg(&musb->intrtx);
        USB_DRV_WriteReg(&musb->intrrx, 0xFFFF); // write 1 clear
        USB_DRV_WriteReg8(&musb->devctl, 0x00);
    }
    return HAL_USB_STATUS_OK;
}

/*******************************************************************************
	USB SLT/DVT related
 *******************************************************************************/
bool usb_dma_test_ep0_loopback(uint8_t *tx_buf, uint8_t *rx_buf)
{
    uint16_t dma_ctrl;
    uint8_t dma_chan = 2;
    static uint32_t dma_burst_mode = 0;
    uint8_t ep_num = 0;
    uint32_t i;
    bool cmp_result = true;


    // initial buffer
    for (i = 0; i < 64; i++) {
        tx_buf[i] = i;
        rx_buf[i] = 0;
    }

    /* Disable INTR Setup */
    hal_nvic_disable_irq(USB20_IRQn);

    /* Init Setup */
    dma_burst_mode = 0;
    {
        DRV_WriteReg32(USB_DMAADDR(dma_chan), tx_buf);
        DRV_WriteReg32(USB_DMACNT(dma_chan), 64);
        dma_ctrl = USB_DMACNTL_INTEN | (ep_num << 4);
        dma_ctrl |= ((dma_burst_mode & 0x03) << 9) | USB_DMACNTL_DMAEN;
        dma_ctrl |= USB_DMACNTL_DMADIR;
        DRV_WriteReg(USB_DMACNTL(dma_chan), dma_ctrl);
    }

    hal_gpt_delay_ms(20);
    if (DRV_Reg(USB_DMACNTL(dma_chan)) & 0x01) {
        log_hal_info("%s: dma fail\r\n", __func__);
        return false;
    }
    USB_DRV_WriteReg(&musb->csr0, USB_CSR0_TXPKTRDY);
    USB_DRV_WriteReg8(&musb->testmode, USB_TESTMODE_FIFOACCESS | USB_TESTMODE_FORCEHS);


    //RX
    {
        dma_ctrl = 0;
        DRV_WriteReg32(USB_DMAADDR(dma_chan), rx_buf);
        DRV_WriteReg32(USB_DMACNT(dma_chan), 64);
        dma_ctrl = USB_DMACNTL_INTEN | (ep_num << 4);
        dma_ctrl |= ((dma_burst_mode & 0x03) << 9) | USB_DMACNTL_DMAEN;
        DRV_WriteReg(USB_DMACNTL(dma_chan), dma_ctrl);
    }

    hal_gpt_delay_ms(20);
    for (i = 0; i < 64; i++) {
        if (tx_buf[i] != rx_buf[i]) {
            cmp_result = false;
            log_hal_info("%s: compare data fail\r\n", __func__);
            break;
        }
    }
    USB_DRV_WriteReg8(&musb->testmode, 0);
    return cmp_result;
}


hal_usb_status_t hal_usb_dcm_enable(void)
{
    /*
     * bit16: mcu dcm
     * bit18: usbip dcm
     */
    DRV_WriteReg32(&musb->resreg,  musb->resreg & (~(0x05 << 16)));
    log_hal_info("hal_usb_dcm_enable: 0x%x\r\n", musb->resreg);

    return HAL_USB_STATUS_OK;
}


/*******************************************************************************
	Control functions for USB_DRV
 *******************************************************************************/

/* Initialize usb driver SW information, called at USB_Init() */
hal_usb_status_t hal_usb_configure_driver(void)
{
    int32_t index;

    g_UsbDrvInfo.power_state = HAL_USB_POWER_STATE_NORMAL;
    g_UsbDrvInfo.is_unMaskUSB = true;
    g_UsbDrvInfo.reset_hdlr = NULL;
    g_UsbDrvInfo.ep0_hdlr = NULL;

    g_UsbDrvInfo.Is_HS_mode = false;

    for (index = (HAL_USB_MAX_NUMBER_ENDPOINT_TX - 1); index >= 0; index--) {
        g_UsbDrvInfo.ep_tx_enb_state[index] = HAL_USB_EP_STATE_DISABLE;
        g_UsbDrvInfo.ep_tx_hdlr[index] = NULL;
        g_UsbDrvInfo.ep_tx_stall_status[index] = false;
        g_UsbDrvInfo.ep_tx_flush_intr[index] = false;
    }

    for (index = (HAL_USB_MAX_NUMBER_ENDPOINT_RX - 1); index >= 0; index--) {
        g_UsbDrvInfo.ep_rx_enb_state[index] = HAL_USB_EP_STATE_DISABLE;
        g_UsbDrvInfo.ep_rx_hdlr[index] = NULL;
        g_UsbDrvInfo.ep_rx_stall_status[index] = false;
        g_UsbDrvInfo.ep_rx_flush_intr[index] = false;
    }

    for (index = (HAL_USB_MAX_NUMBER_ENDPOINT - 1); index >= 0; index--) {
        g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_TX][index] = 0;
        g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_RX][index] = 0;
    }
    g_UsbDrvInfo.dma_channel = 0;

    for (index = (HAL_USB_MAX_NUMBER_DMA - 1); index >= 0; index--) {
        g_UsbDrvInfo.dma_pktrdy[index] = false;
        g_UsbDrvInfo.dma_running[index] = false;
        g_UsbDrvInfo.dma_callback_upd_run[index] = false;
        g_UsbDrvInfo.dma_callback[index] = NULL;
        g_UsbDrvInfo.dma_tx_ep_num[index] = 0;
        g_UsbDrvInfo.dma_rx_ep_num[index] = 0;
    }

    return HAL_USB_STATUS_OK;
}

hal_usb_status_t hal_usb_register_driver_callback(hal_usb_driver_handler_type_t type, uint32_t ep_num, hal_usb_driver_interrupt_handler_t hdlr)
{
    switch (type) {
        case HAL_USB_DRV_HDLR_RESET:
            g_UsbDrvInfo.reset_hdlr = hdlr;
            break;
        case HAL_USB_DRV_HDLR_SUSPEND:
            g_UsbDrvInfo.suspend_hdlr = hdlr;
            break;
        case HAL_USB_DRV_HDLR_RESUME:
            g_UsbDrvInfo.resume_hdlr = hdlr;
            break;
        case HAL_USB_DRV_HDLR_EP0:
            g_UsbDrvInfo.ep0_hdlr = hdlr;
            break;
        case HAL_USB_DRV_HDLR_EP_TX:
            usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
            g_UsbDrvInfo.ep_tx_hdlr[ep_num - 1] = hdlr;
            break;
        case HAL_USB_DRV_HDLR_EP_RX:
            usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);
            g_UsbDrvInfo.ep_rx_hdlr[ep_num - 1] = hdlr;
            break;
        default:
            log_hal_error("ASSERT\r\n");
            break;
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_reset_drv_info(void)
{
    int32_t index;
    g_UsbDrvInfo.power_state = HAL_USB_POWER_STATE_NORMAL;
    g_UsbDrvInfo.is_unMaskUSB = true;

    if ((musb->power & USB_POWER_HSMODE) != 0) {
        g_UsbDrvInfo.Is_HS_mode = true;
    } else {
        g_UsbDrvInfo.Is_HS_mode = false;
    }

    for (index = (HAL_USB_MAX_NUMBER_ENDPOINT_TX - 1); index >= 0; index--) {
        g_UsbDrvInfo.ep_tx_enb_state[index] = HAL_USB_EP_STATE_DISABLE;
        g_UsbDrvInfo.ep_tx_stall_status[index] = false;
        g_UsbDrvInfo.ep_tx_flush_intr[index] = false;
    }

    for (index = (HAL_USB_MAX_NUMBER_ENDPOINT_RX - 1); index >= 0; index--) {
        g_UsbDrvInfo.ep_rx_enb_state[index] = HAL_USB_EP_STATE_DISABLE;
        g_UsbDrvInfo.ep_rx_stall_status[index] = false;
        g_UsbDrvInfo.ep_rx_flush_intr[index] = false;
    }

    for (index = (HAL_USB_MAX_NUMBER_DMA - 1); index >= 0; index--) {
        USBDMA_Stop(index + 1);
    }

    //clear interrupt. write 1 clear
    DRV_WriteReg8(&musb->dma_intr_status, 0xFF);
    for (index = (HAL_USB_MAX_NUMBER_DMA - 1); index >= 0; index--) {
        g_UsbDrvInfo.dma_pktrdy[index] = false;
        g_UsbDrvInfo.dma_running[index] = false;
        g_UsbDrvInfo.dma_callback_upd_run[index] = false;
        g_UsbDrvInfo.dma_callback[index] = NULL;
    }

    return HAL_USB_STATUS_OK;
}


bool hal_usb_get_endpoint_stall_status(uint32_t  ep_num, hal_usb_endpoint_direction_t direction)
{
    bool result;

    usb_ep_check(ep_num, direction, 0);
    if (direction == HAL_USB_EP_DIRECTION_TX) {
        result = (bool)g_UsbDrvInfo.ep_tx_stall_status[ep_num - 1];
    } else {
        result = (bool)g_UsbDrvInfo.ep_rx_stall_status[ep_num - 1];
    }

    return result;
}

hal_usb_status_t hal_usb_pull_up_dp_line(void)
{
    /* For: Clear all interrupt before DP pull high
     * Reason: Windows MTP class won't trigger new reset signal if cable plugout and re-plugin.
     *         We need a reset interrupt to reset IP else force clear all interrupt before DP pull high.
     */
    // read and clear
    USB_DRV_Reg8(&musb->intrusb);
    USB_DRV_Reg(&musb->intrtx);
    USB_DRV_Reg(&musb->intrtx);
    USB_DRV_WriteReg(&musb->intrrx, USB_DRV_Reg(&musb->intrrx));// Write 1 clear
    /*Pull up DP here!!*/
    USB_DRV_SetBits8(&musb->power, USB_POWER_SOFTCONN);

    g_UsbDrvInfo.usb_disconnect = false;
    log_hal_info("usb DP pull high\r\n");
    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_reset_hardware(void)
{
    /* Enable software reset, USB IP only can be reset by SW when detecting reset signal from bus */
    USB_DRV_WriteReg(&musb->busperf3, USB_BUSPERF3_DISUSBRESET);

    USB_DRV_WriteReg8(&musb->rst_info, 0x0A0);
    /* Because softconn has be decided to set or not */
    /* Set USB_POWER_ISOUPDATE will make ISO pipe with DMA abnormally */
    USB_DRV_SetBits8(&musb->power, (USB_POWER_ENABLESUSPENDM));
    USB_DRV_SetBits8(&musb->power, USB_POWER_HSENAB);

    return HAL_USB_STATUS_OK;
}

hal_usb_status_t hal_usb_deinit(void)
{
    hal_usb_pdn_mode(true);
    usb_pdn_enable();
    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_set_address(uint8_t addr, hal_usb_set_address_state_t state)
{
    if (state == HAL_USB_SET_ADDR_STATUS) {
        USB_DRV_WriteReg8(&musb->faddr, addr);
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_init_tx_endpoint(uint32_t ep_num, uint16_t data_size, hal_usb_endpoint_transfer_type_t type, bool double_fifo)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    /* Double buffers, so flush twice */
    USB_DRV_WriteReg(&musb->txcsr, 0x00);
    USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
    USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
    USB_DRV_WriteReg(&musb->txmap, data_size);

    /* Set FIFO address here */
    if (type == HAL_USB_EP_TRANSFER_BULK) {
        if (double_fifo == true) {
            //double  buffer
            USB_DRV_WriteReg8(&musb->txfifosz, (USB_FIFOSZ_SIZE_512 | USB_FIFOSZ_DPB));
            USB_DRV_WriteReg(&musb->txfifoadd, (g_FIFOadd / 8));
            g_FIFOadd += USB_BULK_FIFO_UNIT_SIZE * 2;
        } else {
            //single buffer
            USB_DRV_WriteReg8(&musb->txfifosz, USB_FIFOSZ_SIZE_512);
            USB_DRV_WriteReg(&musb->txfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_BULK_FIFO_UNIT_SIZE;
        }
    } else if (type == HAL_USB_EP_TRANSFER_INTR) {
        if (double_fifo == true) {
            USB_DRV_WriteReg8(&musb->txfifosz, (USB_FIFOSZ_SIZE_32 | USB_FIFOSZ_DPB));
            USB_DRV_WriteReg(&musb->txfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_INTR_FIFO_UNIT_SIZE * 2;
        } else {
            USB_DRV_WriteReg8(&musb->txfifosz, USB_FIFOSZ_SIZE_32);
            USB_DRV_WriteReg(&musb->txfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_INTR_FIFO_UNIT_SIZE; //32
        }
    } else if (type == HAL_USB_EP_TRANSFER_ISO) {
        if (double_fifo == true) {
            USB_DRV_WriteReg8(&musb->txfifosz, (USB_FIFOSZ_SIZE_1024 | USB_FIFOSZ_DPB));
            USB_DRV_WriteReg(&musb->txfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_ISO_FIFO_UNIT_SIZE * 2;
        } else {
            USB_DRV_WriteReg8(&musb->txfifosz, USB_FIFOSZ_SIZE_1024);
            USB_DRV_WriteReg(&musb->txfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_ISO_FIFO_UNIT_SIZE;
        }
    }

    RestoreIRQMask(savedMask);

    if (g_FIFOadd > USB_MAX_FIFO_SIZE) {
        log_hal_error("ASSERT\r\n");
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_init_rx_endpoint(uint32_t ep_num, uint16_t data_size, hal_usb_endpoint_transfer_type_t type, bool double_fifo)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    USB_DRV_WriteReg(&musb->rxcsr, USB_RXCSR_RXPKTRDY);
    USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
    USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
    USB_DRV_WriteReg(&musb->rxmap, data_size);

    /* Set FIFO address here */
    if (type == HAL_USB_EP_TRANSFER_BULK) {
        if (double_fifo == true) {
            //double buffer
            USB_DRV_WriteReg8(&musb->rxfifosz, (USB_FIFOSZ_SIZE_512 | USB_FIFOSZ_DPB));
            USB_DRV_WriteReg(&musb->rxfifoadd, (g_FIFOadd / 8));
            g_FIFOadd += USB_BULK_FIFO_UNIT_SIZE * 2;
        } else {
            //single buffer
            USB_DRV_WriteReg8(&musb->rxfifosz, USB_FIFOSZ_SIZE_512);
            USB_DRV_WriteReg(&musb->rxfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_BULK_FIFO_UNIT_SIZE;
        }
    } else if (type == HAL_USB_EP_TRANSFER_ISO) {
        if (double_fifo == true) {
            USB_DRV_WriteReg8(&musb->rxfifosz, (USB_FIFOSZ_SIZE_1024 | USB_FIFOSZ_DPB));
            USB_DRV_WriteReg(&musb->rxfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_ISO_FIFO_UNIT_SIZE * 2;
        } else {
            USB_DRV_WriteReg8(&musb->rxfifosz, USB_FIFOSZ_SIZE_1024);
            USB_DRV_WriteReg(&musb->rxfifoadd, (uint16_t)(g_FIFOadd / 8));
            g_FIFOadd += USB_ISO_FIFO_UNIT_SIZE;
        }
    } else {
        log_hal_error("ASSERT\r\n");
    }

    RestoreIRQMask(savedMask);

    if (g_FIFOadd > USB_MAX_FIFO_SIZE) {
        log_hal_error("ASSERT\r\n");
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_enable_tx_endpoint(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, hal_usb_dma_usage_t dma_usage_type, bool is_flush)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    savedMask = SaveAndSetIRQMask();

    USB_DRV_WriteReg8(&musb->index, ep_num);

    /* Double buffers, so we flush twice */
    if (is_flush == true) {
        if ((USB_DRV_Reg(&musb->txcsr)&USB_TXCSR_FIFONOTEMPTY) != 0) {
            g_UsbDrvInfo.ep_tx_flush_intr[ep_num - 1] = true;
        }

        USB_DRV_WriteReg(&musb->txcsr, 0x00);
        USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
        USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
    }


    if (dma_usage_type == HAL_USB_EP_USE_ONLY_DMA) {
        g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_DMA;
        USB_DRV_ClearBits(&musb->intrtxe, (USB_INTRE_EPEN << ep_num));

        /* Only is configured as multiple packet DMA TX mode */
        if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            USB_DRV_WriteReg(&musb->txcsr, USB_DMA_TX_CSR_ISO);
        } else {
            USB_DRV_WriteReg(&musb->txcsr, USB_DMA_TX_CSR);
        }
    } else if (dma_usage_type == HAL_USB_EP_USE_NO_DMA) {
        /* EP default uses FIFO */
        g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_FIFO;
        if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            USB_DRV_WriteReg(&musb->txcsr, USB_TXCSR_ISO);
        } else {
            USB_DRV_WriteReg(&musb->txcsr, 0x00);
        }
        USB_DRV_SetBits(&musb->intrtxe, (USB_INTRE_EPEN << ep_num));
    }

    RestoreIRQMask(savedMask);
    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_disable_tx_endpoint(uint32_t ep_num)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    g_UsbDrvInfo.ep_tx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_DISABLE;

    savedMask = SaveAndSetIRQMask();
    USB_DRV_ClearBits(&musb->intrtxe, (USB_INTRE_EPEN << ep_num));

    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_enable_rx_endpoint(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, hal_usb_dma_usage_t dma_usage_type, bool is_flush)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();

    if (dma_usage_type == HAL_USB_EP_USE_ONLY_DMA) {
        g_UsbDrvInfo.ep_rx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_DMA;
        USB_DRV_ClearBits(&musb->intrrxe, (USB_INTRE_EPEN << ep_num));
    } else if (dma_usage_type == HAL_USB_EP_USE_NO_DMA) {
        /* EP default uses FIFO */
        g_UsbDrvInfo.ep_rx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_FIFO;
        USB_DRV_SetBits(&musb->intrrxe, (USB_INTRE_EPEN << ep_num));
    }

    RestoreIRQMask(savedMask);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);

    /* Maybe double buffer, so flush twice */
    if (is_flush == true) {
        USB_DRV_WriteReg(&musb->rxcsr, USB_RXCSR_RXPKTRDY);
        USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
        USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
    }

    if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
        USB_DRV_WriteReg(&musb->rxcsr, USB_RXCSR_ISO);
    }
    USB_DRV_WriteReg(&musb->rxcsr, 0x00);

    RestoreIRQMask(savedMask);
    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_disable_rx_endpoint(uint32_t ep_num)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    g_UsbDrvInfo.ep_rx_enb_state[ep_num - 1] = HAL_USB_EP_STATE_DISABLE;

    savedMask = SaveAndSetIRQMask();
    USB_DRV_ClearBits(&musb->intrrxe, (USB_INTRE_EPEN << ep_num));
    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_clear_tx_endpoint_data_toggle(uint32_t ep_num)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    USB_DRV_SetBits(&musb->txcsr, USB_TXCSR_CLRDATATOG);
    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_clear_rx_endpoint_data_toggle(uint32_t ep_num)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    USB_DRV_SetBits(&musb->rxcsr, USB_RXCSR_CLRDATATOG);
    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}

hal_usb_status_t hal_usb_read_endpoint_fifo(uint32_t ep_num, uint16_t nBytes, void *pDst)
{
    uint32_t  nAddr;
#ifdef FOUR_BYTE_ACCESS
    uint32_t  temp_1, temp_2;
    uint32_t *pby1;
#endif
    uint16_t  nCount;
    uint8_t   *pby;

    nCount = nBytes;

    if ((nBytes != 0) && (pDst == NULL)) {
        log_hal_error("ASSERT\r\n");
    }

    if (pDst == NULL) {
        return HAL_USB_STATUS_ERROR;
    }

    /* not indexed register */
    nAddr = (uint32_t)(&musb->fifo0 + ep_num);

#ifdef FOUR_BYTE_ACCESS
    if (((uint32_t)pDst % 4) == 0) {
        /* Destination address is 4 byte alignment */
        temp_1 = nCount / 4;
        temp_2 = nCount % 4;

        pby1 = (uint32_t *)pDst;

        while (temp_1) {
            *pby1++ = USB_DRV_Reg32(nAddr);	//lint !e613
            temp_1--;
        }

        pby = (uint8_t *)pby1;

        while (temp_2) {
            *pby++ = USB_DRV_Reg8(nAddr);	//lint !e613
            temp_2--;
        }
    } 
    else
#endif
    {
        pby = (uint8_t *)pDst;

        /* Read byte by byte */
        while (nCount) {
            *pby++ = USB_DRV_Reg8(nAddr);	//lint !e613
            nCount--;
        }
    }

    //fifo dump
    //if(nBytes>10)
    //    log_hal_dump("hal_usb_fifo dump\n", pDst, nBytes);

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_write_endpoint_fifo(uint32_t ep_num, uint16_t nBytes, void *pSrc)
{
    usb_hw_epfifowrite(ep_num, nBytes, pSrc);
    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_set_endpoint_stall(uint32_t ep_num, hal_usb_endpoint_direction_t direction, bool stall_enable)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, direction, 0);
    usb_ep_dma_running_check(ep_num, direction, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);

    /* Stall endpoint */
    if (direction == HAL_USB_EP_DIRECTION_RX) {
        USB_DRV_WriteReg(&musb->rxcsr, USB_RXCSR_RXPKTRDY);
        USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
        USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
        g_UsbDrvInfo.ep_rx_stall_status[ep_num - 1] = stall_enable;

        if (stall_enable == true) {
            USB_DRV_WriteReg(&musb->rxcsr, USB_RXCSR_SENDSTALL);
        }
    } else {
        /* For webcam case, when PC send clear featuere, we must reset toggle */
        if ((USB_DRV_Reg(&musb->txcsr)&USB_TXCSR_FIFONOTEMPTY) != 0) {
            g_UsbDrvInfo.ep_tx_flush_intr[ep_num - 1] = true;
        }

        USB_DRV_WriteReg(&musb->txcsr, 0x00);
        USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
        USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
        g_UsbDrvInfo.ep_tx_stall_status[ep_num - 1] = stall_enable;

        if (stall_enable == true) {
            USB_DRV_WriteReg(&musb->txcsr, USB_TXCSR_SENDSTALL);
        }
    }

    RestoreIRQMask(savedMask);


    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_get_endpoint_0_status(bool *p_transaction_end, bool *p_sent_stall)
{
    uint32_t savedMask;
    uint16_t  CSR0;

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, 0);
    CSR0 = USB_DRV_Reg(&musb->csr0);
    RestoreIRQMask(savedMask);

    if (CSR0 & USB_CSR0_SENTSTALL) {
        *p_sent_stall = true;
    } else {
        *p_sent_stall = false;
    }

    if (CSR0 & USB_CSR0_SETUPEND) {
        *p_transaction_end = true;
    } else {
        *p_transaction_end = false;
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_update_endpoint_0_state(hal_usb_endpoint_0_driver_state_t state, bool stall, bool end)
{
    uint32_t savedMask;
    uint8_t   reg_state;

    /* Clear sent stall */
    if (state == HAL_USB_EP0_DRV_STATE_CLEAR_SENT_STALL) {
        savedMask = SaveAndSetIRQMask();
        USB_DRV_WriteReg8(&musb->index, 0);
        USB_DRV_ClearBits(&musb->csr0, USB_CSR0_SENTSTALL);
        RestoreIRQMask(savedMask);
        return HAL_USB_STATUS_OK;
    }

    /* clear transaction end*/
    if (state == HAL_USB_EP0_DRV_STATE_TRANSACTION_END) {
        savedMask = SaveAndSetIRQMask();
        USB_DRV_WriteReg8(&musb->index, 0);
        USB_DRV_WriteReg(&musb->csr0, USB_CSR0_SERVICEDSETUPEND);
        RestoreIRQMask(savedMask);
        return HAL_USB_STATUS_OK;
    }

    /* ep0 read end or write ready*/
    if (state == HAL_USB_EP0_DRV_STATE_READ_END) {
        reg_state = USB_CSR0_SERVICEDRXPKTRDY;
    } else {
        reg_state = USB_CSR0_TXPKTRDY;
    }

    /* error occured, sent stall*/
    if (stall == true) {
        reg_state |= USB_CSR0_SENDSTALL;
    }

    /* last data for this transaction, set data end bit*/
    if (end == true) {
        reg_state |= USB_CSR0_DATAEND;
    }

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, 0);
    USB_DRV_WriteReg(&musb->csr0, reg_state);
    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}


uint32_t hal_usb_ep0_pkt_len(void)
{
    uint32_t savedMask;
    uint32_t nCount = 0;
    uint16_t  CSR0;

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, 0);
    CSR0 = USB_DRV_Reg(&musb->csr0);

    if (CSR0 & USB_CSR0_RXPKTRDY) {
        nCount = (uint32_t)USB_DRV_Reg8(&musb->count0);
    }
    RestoreIRQMask(savedMask);

    return nCount;
}

uint32_t hal_usb_ep_pkt_len(uint32_t ep_num)
{
    uint32_t savedMask;
    uint16_t CSR;
    uint32_t nCount = 0;


    //prep_ctrl_func_3 = &data->rEP_Ctrl_Func_3;
    //ep_num = (kal_uint32)prep_ctrl_func_3->u4ep_num;

    //USB_EP_Check(ep_num, USB_EP_RX_DIR, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    CSR = USB_DRV_Reg(&musb->rxcsr);

    if (CSR & USB_RXCSR_RXPKTRDY) {
        nCount = (uint32_t)USB_DRV_Reg(&musb->rxcount);
    }
    RestoreIRQMask(savedMask);

    //prep_ctrl_func_3->u4result = (DCL_UINT32)nCount;
    return nCount;
}


hal_usb_status_t hal_usb_set_endpoint_tx_ready(uint32_t ep_num)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    savedMask = SaveAndSetIRQMask();  //DMA_Setup : clear UnderRun bit & Set TX_PkyReady at the same time
    USB_DRV_WriteReg8(&musb->index, ep_num);

    if (USB_DRV_Reg(&musb->txcsr)&USB_TXCSR_TXPKTRDY) { // check TX_PktReady bit
        log_hal_error("ASSERT\r\n");
    }

    USB_DRV_SetBits(&musb->txcsr, USB_TXCSR_TXPKTRDY);
    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}


uint32_t hal_usb_get_rx_packet_length(uint32_t ep_num)
{
    uint32_t savedMask;
    uint16_t CSR;
    uint32_t nCount = 0;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    CSR = USB_DRV_Reg(&musb->rxcsr);

    if (CSR & USB_RXCSR_RXPKTRDY) {
        nCount = (uint32_t)USB_DRV_Reg(&musb->rxcount);
    }
    RestoreIRQMask(savedMask);

    return nCount;
}


hal_usb_status_t hal_usb_set_endpoint_rx_ready(uint32_t ep_num)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);

    USB_DRV_ClearBits(&musb->rxcsr, USB_RXCSR_RXPKTRDY);
    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}


bool hal_usb_is_endpoint_tx_empty(uint32_t ep_num)
{
    return usb_hw_is_ep_tx_empty(ep_num);
}

bool hal_usb_is_endpoint_rx_empty(uint32_t ep_num)
{
    uint32_t savedMask;
    uint16_t CSR;
    bool result;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);
    CSR = USB_DRV_Reg(&musb->rxcsr);
    RestoreIRQMask(savedMask);

    if (CSR & USB_RXCSR_RXPKTRDY) {
        result = false;
    } else {
        result = true;
    }

    return result;
}


hal_usb_status_t hal_usb_clear_tx_endpoint_fifo(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_reset_toggle)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_TX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);

    if ((USB_DRV_Reg(&musb->txcsr)&USB_TXCSR_FIFONOTEMPTY) != 0) {
        g_UsbDrvInfo.ep_tx_flush_intr[ep_num - 1] = true;
    }

    /* FLUSHFIFO only works with TXPKTRDY is 0, so we cannot "or" */
    /* DMAReqEnab must be "0" before FLUSHFIFO, otherwise DMA will complete and issue DMA interrupt */
    if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
        // To clear all DMA bits
        USB_DRV_WriteReg(&musb->txcsr, USB_TXCSR_ISO);

        if (b_reset_toggle == true) {
            USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG | USB_TXCSR_ISO));
            USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG | USB_TXCSR_ISO));
        } else {
            USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_ISO));
            USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_ISO));
        }
    } else {
        // To clear all DMA bits
        USB_DRV_WriteReg(&musb->txcsr, 0x00);

        if (b_reset_toggle == true) {
            USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
            USB_DRV_WriteReg(&musb->txcsr, (USB_TXCSR_FLUSHFIFO | USB_TXCSR_CLRDATATOG));
        } else {
            USB_DRV_WriteReg(&musb->txcsr, USB_TXCSR_FLUSHFIFO);
            USB_DRV_WriteReg(&musb->txcsr, USB_TXCSR_FLUSHFIFO);
        }
    }
    RestoreIRQMask(savedMask);

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_clear_rx_endpoint_fifo(uint32_t ep_num, hal_usb_endpoint_transfer_type_t ep_type, bool b_reset_toggle)
{
    uint32_t savedMask;

    usb_ep_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);
    usb_ep_dma_running_check(ep_num, HAL_USB_EP_DIRECTION_RX, 0);

    savedMask = SaveAndSetIRQMask();
    USB_DRV_WriteReg8(&musb->index, ep_num);

    if ((USB_DRV_Reg(&musb->rxcsr)&USB_RXCSR_RXPKTRDY) != 0) {
        g_UsbDrvInfo.ep_rx_flush_intr[ep_num - 1] = true;
    }

    if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
        // To clear all DMA bits
        USB_DRV_WriteReg(&musb->rxcsr, USB_RXCSR_ISO | USB_RXCSR_RXPKTRDY);

        if (b_reset_toggle == true) {
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_ISO | USB_RXCSR_RXPKTRDY));
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_ISO | USB_RXCSR_RXPKTRDY));
        } else {
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_ISO | USB_RXCSR_RXPKTRDY));
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_ISO | USB_RXCSR_RXPKTRDY));
        }
    } else {
        // To clear all DMA bits
        USB_DRV_WriteReg(&musb->rxcsr, USB_RXCSR_RXPKTRDY);

        if (b_reset_toggle == true) {
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_CLRDATATOG | USB_RXCSR_RXPKTRDY));
        } else {
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_RXPKTRDY));
            USB_DRV_WriteReg(&musb->rxcsr, (USB_RXCSR_FLUSHFIFO | USB_RXCSR_RXPKTRDY));
        }
    }

    RestoreIRQMask(savedMask);
    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_get_dma_channel(uint32_t ep_tx_num, uint32_t ep_rx_num, hal_usb_endpoint_direction_t direction, bool same_chan)
{
    /* Should not allow re-entry */
    static bool race_check = false;

    if (race_check == true) {
        log_hal_error("ASSERT\r\n");
    }
    race_check = true;

    g_UsbDrvInfo.dma_channel++;

    if (g_UsbDrvInfo.dma_channel > HAL_USB_MAX_NUMBER_DMA) {
        log_hal_error("ASSERT\r\n");
    }

    if (same_chan == true) {
        usb_ep_check(ep_tx_num, HAL_USB_EP_DIRECTION_TX, 0);
        usb_ep_check(ep_rx_num, HAL_USB_EP_DIRECTION_RX, 0);

        g_UsbDrvInfo.is_bidirection_dma[g_UsbDrvInfo.dma_channel - 1] = true;
        /* the same channel */
        g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_TX][ep_tx_num - 1] = g_UsbDrvInfo.dma_channel;
        g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_RX][ep_rx_num - 1] = g_UsbDrvInfo.dma_channel;
        g_UsbDrvInfo.dma_tx_ep_num[g_UsbDrvInfo.dma_channel - 1] = ep_tx_num;
        g_UsbDrvInfo.dma_rx_ep_num[g_UsbDrvInfo.dma_channel - 1] = ep_rx_num;
    } else {
        g_UsbDrvInfo.is_bidirection_dma[g_UsbDrvInfo.dma_channel - 1] = false;
        g_UsbDrvInfo.dma_dir[g_UsbDrvInfo.dma_channel - 1] = direction;

        if (direction == HAL_USB_EP_DIRECTION_TX) {
            usb_ep_check(ep_tx_num, HAL_USB_EP_DIRECTION_TX, 0);
            g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_TX][ep_tx_num - 1] = g_UsbDrvInfo.dma_channel;
            g_UsbDrvInfo.dma_tx_ep_num[g_UsbDrvInfo.dma_channel - 1] = ep_tx_num;
        } else {
            usb_ep_check(ep_rx_num, HAL_USB_EP_DIRECTION_RX, 0);
            g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_RX][ep_rx_num - 1] = g_UsbDrvInfo.dma_channel;
            g_UsbDrvInfo.dma_rx_ep_num[g_UsbDrvInfo.dma_channel - 1] = ep_rx_num;
        }
    }

    race_check = false;

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_stop_dma_channel(uint32_t ep_num, hal_usb_endpoint_direction_t direction)
{
    uint32_t 	savedMask;
    uint8_t 	dma_chan;
    volatile uint32_t delay;
    uint32_t left_count;
    bool  dma_pktrdy;

    dma_chan = usb_get_dma_channel_num(ep_num, direction);
    savedMask = SaveAndSetIRQMask();
    /* Stop DMA channel */
    USBDMA_Stop(dma_chan);
    /* Clear pending DMA interrupts */
    DRV_WriteReg8(&musb->dma_intr_status, (1 << (dma_chan - 1)));
    //DRV_WriteReg8(&musb->dma_intr_unmask_set, (1<<(dma_chan-1)));
    dma_pktrdy = g_UsbDrvInfo.dma_pktrdy[dma_chan - 1];
    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = false;
    g_UsbDrvInfo.dma_running[dma_chan - 1] = false;
    RestoreIRQMask(savedMask);
    for (delay = 0 ; delay < 500 ; delay++); /* wait for dma stop */

    left_count = USB_DMACNT(dma_chan); //get DMA Real CNT
    if ((left_count == 0)  || (left_count > g_UsbDrvInfo.dma_tx_length[dma_chan - 1])) {  /* check for short pkt */
        /* drop data in FIFO*/
        if (dma_pktrdy == true) {
            if (g_UsbDrvInfo.dma_dir[dma_chan - 1] == HAL_USB_EP_DIRECTION_TX) {
                savedMask = SaveAndSetIRQMask();
                USB_DRV_WriteReg8(&musb->index, ep_num);
                USB_DRV_SetBits(&musb->txcsr, USB_TXCSR_FLUSHFIFO | USB_TXCSR_TXPKTRDY);
                RestoreIRQMask(savedMask);
            }
        }
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_release_dma_channel(uint32_t ep_tx_num, uint32_t ep_rx_num, hal_usb_endpoint_direction_t direction, bool same_chan)
{
    uint8_t dma_chan;

    if (same_chan == true) {
        usb_ep_check(ep_tx_num, HAL_USB_EP_DIRECTION_TX, 0);
        usb_ep_check(ep_rx_num, HAL_USB_EP_DIRECTION_RX, 0);

        usb_hw_stop_dma_channel(ep_tx_num, HAL_USB_EP_DIRECTION_TX);
        dma_chan = usb_get_dma_channel_num(ep_tx_num, HAL_USB_EP_DIRECTION_TX);

        if (g_UsbDrvInfo.is_bidirection_dma[dma_chan - 1] == false) {
            log_hal_error("ASSERT\r\n");
        }

        g_UsbDrvInfo.dma_tx_ep_num[dma_chan - 1] = 0;
        g_UsbDrvInfo.dma_rx_ep_num[dma_chan - 1] = 0;
        g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_TX][ep_tx_num - 1] = 0;
        g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_RX][ep_rx_num - 1] = 0;
    } else {
        if (direction == HAL_USB_EP_DIRECTION_TX) {
            usb_ep_check(ep_tx_num, HAL_USB_EP_DIRECTION_TX, 0);
            usb_hw_stop_dma_channel(ep_tx_num, HAL_USB_EP_DIRECTION_TX);
            dma_chan = usb_get_dma_channel_num(ep_tx_num, HAL_USB_EP_DIRECTION_TX);

            if (g_UsbDrvInfo.is_bidirection_dma[dma_chan - 1] == true) {
                log_hal_error("ASSERT\r\n");
            }

            g_UsbDrvInfo.dma_tx_ep_num[dma_chan - 1] = 0;
            g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_TX][ep_tx_num - 1] = 0;
        } else {
            usb_ep_check(ep_rx_num, HAL_USB_EP_DIRECTION_RX, 0);
            usb_hw_stop_dma_channel(ep_rx_num, HAL_USB_EP_DIRECTION_RX);
            dma_chan = usb_get_dma_channel_num(ep_rx_num, HAL_USB_EP_DIRECTION_RX);

            if (g_UsbDrvInfo.is_bidirection_dma[dma_chan - 1] == true) {
                log_hal_error("ASSERT\r\n");
            }

            g_UsbDrvInfo.dma_rx_ep_num[dma_chan - 1] = 0;
            g_UsbDrvInfo.dma_port[HAL_USB_EP_DIRECTION_RX][ep_rx_num - 1] = 0;
        }
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_start_dma_channel(uint32_t ep_num, hal_usb_endpoint_direction_t direction, hal_usb_endpoint_transfer_type_t ep_type, void *addr, uint32_t length,
        hal_usb_dma_handler_t callback, bool callback_upd_run, hal_usb_dma_type_t dma_type)
{
    uint16_t dma_ctrl;
    uint8_t dma_chan;
    static uint32_t dma_burst_mode = 0;
    uint32_t savedMask;
    bool known_size = true;

    if (length == 0) {
        log_hal_error("ASSERT\r\n");
    }

    if (g_UsbDrvInfo.usb_disconnect == true) {
        return HAL_USB_STATUS_OK;
    }

    dma_chan = usb_get_dma_channel_num(ep_num, direction);

    if (g_UsbDrvInfo.dma_running[dma_chan - 1] != false) {
        log_hal_error("ASSERT\r\n");
    }

    if (g_UsbDrvInfo.is_bidirection_dma[dma_chan - 1] == true) {
        g_UsbDrvInfo.dma_dir[dma_chan - 1] = direction;
    } else if (g_UsbDrvInfo.dma_dir[dma_chan - 1] != direction) {
        log_hal_error("ASSERT\r\n");
    }

    g_UsbDrvInfo.dma_running[dma_chan - 1] = true;
    g_UsbDrvInfo.dma_callback[dma_chan - 1] = callback;
    g_UsbDrvInfo.dma_callback_upd_run[dma_chan - 1] = callback_upd_run;
    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = false;

    if (direction == HAL_USB_EP_DIRECTION_RX) {
        if (ep_type == HAL_USB_EP_TRANSFER_BULK) {
            if (g_UsbDrvInfo.Is_HS_mode == true) {
                if (((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED) != 0)
                        && ((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED) <= (HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED - 4))) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            } else {
                if (((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED) != 0)
                        && ((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED) <= (HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED - 4))) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            }
        } else if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            if (g_UsbDrvInfo.Is_HS_mode == true) {
                if (((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED) != 0)
                        && ((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED) <= (HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED - 4))) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            } else {
                if (((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_FULL_SPEED) != 0)
                        && ((length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_FULL_SPEED) <= (HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_FULL_SPEED - 4))) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            }
        }
    } else {
        if (ep_type == HAL_USB_EP_TRANSFER_BULK) {
            if (g_UsbDrvInfo.Is_HS_mode == true) {
                if (length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_HIGH_SPEED) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            } else {
                if (length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_BULK_FULL_SPEED) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            }
        } else if (ep_type == HAL_USB_EP_TRANSFER_INTR) {
            if (g_UsbDrvInfo.Is_HS_mode == true) {
                if (length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_HIGH_SPEED) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            } else {
                if (length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_INTERRUPT_FULL_SPEED) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            }
        } else if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
            if (g_UsbDrvInfo.Is_HS_mode == true) {
                if (length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_HIGH_SPEED) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            } else {
                if (length % HAL_USB_MAX_PACKET_SIZE_ENDPOINT_ISOCHRONOUS_FULL_SPEED) {
                    g_UsbDrvInfo.dma_pktrdy[dma_chan - 1] = true;
                }
            }
        }
    }

    if (g_UsbDrvInfo.b_enable_dma_burst_auto_chge == true) {
        dma_burst_mode++;
    } else {
        dma_burst_mode = g_UsbDrvInfo.dma_burst_mode;
    }

    /* DMA_CONFIG */
    if (dma_type == HAL_USB_DMA0_TYPE) {

    } else if (dma_type == HAL_USB_DMA1_TYPE) {
        if (direction == HAL_USB_EP_DIRECTION_TX) {
            savedMask = SaveAndSetIRQMask();
            USB_DRV_WriteReg8(&musb->index, ep_num);

            /* Only is configured as multiple packet DMA TX mode */
            if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
                USB_DRV_WriteReg(&musb->txcsr, USB_DMA_TX_CSR_ISO);
            } else {
                USB_DRV_WriteReg(&musb->txcsr, USB_DMA_TX_CSR);
            }

            RestoreIRQMask(savedMask);
            g_UsbDrvInfo.dma_tx_length[dma_chan - 1] = length;
            //log_hal_info("USB DMA Setup: leng: %d addr:0x%x, first byte: 0x%X\n", length, addr);
            USB_DRV_WriteReg32(USB_DMAADDR(dma_chan), addr);
            USB_DRV_WriteReg32(USB_DMACNT(dma_chan), length);
            //log_hal_info("USB DMA Setup: read back DMAADDR: 0x%x = 0x%x, DMACNT: 0x%x = 0x%x\n",
            //												USB_DMAADDR(dma_chan), USB_DRV_Reg32(USB_DMAADDR(dma_chan)),
            //												USB_DMACNT(dma_chan), USB_DRV_Reg32(USB_DMACNT(dma_chan)));

            dma_ctrl = USB_DMACNTL_DMADIR | USB_DMACNTL_DMAMODE | USB_DMACNTL_INTEN | (ep_num << 4);
            dma_ctrl |= ((dma_burst_mode & 0x03) << 9) | USB_DMACNTL_DMAEN;

            USB_DRV_WriteReg(USB_DMACNTL(dma_chan), dma_ctrl);
        } else if (direction == HAL_USB_EP_DIRECTION_RX) {
            /* Stop DMA channel */
            USBDMA_Stop(dma_chan);

            if (known_size == true) {
                savedMask = SaveAndSetIRQMask();
                USB_DRV_WriteReg8(&musb->index, ep_num);

                if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
                    USB_DRV_WriteReg(&musb->rxcsr, USB_DMA_RX_CSR_ISO | USB_RXCSR_RXPKTRDY);
                } else {
                    USB_DRV_WriteReg(&musb->rxcsr, USB_DMA_RX_CSR | USB_RXCSR_RXPKTRDY);
                }

                RestoreIRQMask(savedMask);
            } else {
#ifdef  __DMA_UNKNOWN_RX__
                savedMask = SaveAndSetIRQMask();
                USB_DRV_WriteReg8(&musb->index, ep_num);

                if (ep_type == HAL_USB_EP_TRANSFER_ISO) {
                    USB_DRV_WriteReg(&musb->rxcsr, USB_DMA_RX_CSR_ISO | USB_RXCSR_RXPKTRDY | USB_RXCSR_DMAREQMODE);
                } else {
                    USB_DRV_WriteReg(&musb->rxcsr, USB_DMA_RX_CSR | USB_RXCSR_RXPKTRDY | USB_RXCSR_DMAREQMODE);
                }

                RestoreIRQMask(savedMask);

                usb_enable_dma_timer_count(dma_chan, true, 0x7F);

#else   /* __DMA_UNKNOWN_RX__ */
                log_hal_error("ASSERT\r\n");
#endif  /* __DMA_UNKNOWN_RX__ */
            }

            USB_DRV_WriteReg32(USB_DMAADDR(dma_chan), addr);
            USB_DRV_WriteReg32(USB_DMACNT(dma_chan), length);

            dma_ctrl = USB_DMACNTL_DMAMODE | USB_DMACNTL_INTEN | (ep_num << 4);
            dma_ctrl |= ((dma_burst_mode & 0x03) << 9) | USB_DMACNTL_DMAEN;
            USB_DRV_WriteReg(USB_DMACNTL(dma_chan), dma_ctrl);
        }
    } else {
        log_hal_error("ASSERT\r\n");
    }
    return HAL_USB_STATUS_OK;
}


bool hal_usb_is_dma_running(uint32_t ep_num, hal_usb_endpoint_direction_t direction)
{
    bool result;
    uint8_t   dma_chan;

    dma_chan = usb_get_dma_channel_num(ep_num, direction);
    result = g_UsbDrvInfo.dma_running[dma_chan - 1];

    return result;;
}

bool hal_usb_is_high_speed(void)
{
    return (bool)g_UsbDrvInfo.Is_HS_mode;;
}

hal_usb_status_t hal_usb_enter_test_mode(hal_usb_test_mode_type_t test_selector)
{
    volatile uint32_t delay = 0;

    if (test_selector == HAL_USB_TEST_MODE_TYPE_J) {
        USB_DRV_WriteReg8(&musb->testmode, USB_TESTMODE_TESTJ);
    } else if (test_selector == HAL_USB_TEST_MODE_TYPE_K) {
        USB_DRV_WriteReg8(&musb->testmode, USB_TESTMODE_TESTK);
    } else if (test_selector == HAL_USB_TEST_MODE_TYPE_SE0_NAK) {
        USB_DRV_WriteReg8(&musb->testmode, USB_TESTMODE_TESTSE0NAK);
    } else if (test_selector == HAL_USB_TEST_MODE_TYPE_PACKET) {
        uint8_t packet_test[] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
            0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
            0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
            0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF,
            0xEF, 0xF7, 0xFB, 0xFD, 0x7E
        };

        usb_hw_epfifowrite(0, 53, packet_test);

        for (delay = 0; delay != 1000; delay++) ;

        USB_DRV_WriteReg8(&musb->index, 0);
        USB_DRV_WriteReg8(&musb->testmode, USB_TESTMODE_TESTPACKET);

        for (delay = 0; delay != 1000; delay++) ;

        USB_DRV_WriteReg(&musb->csr0, USB_CSR0_TXPKTRDY);
    } else {
        log_hal_error("ASSERT\r\n");
    }

    return HAL_USB_STATUS_OK;
}


hal_usb_status_t hal_usb_reset_fifo(void)
{
    g_FIFOadd = USB_FIFO_START_ADDRESS;
    return HAL_USB_STATUS_OK;
}

/************************************************************
	Functions that is used whether USB_ENABLE is turned on or not
*************************************************************/

hal_usb_status_t hal_usb_init(void)
{
    hal_usb_pdn_mode(false);

    USB_DRV_WriteReg(&musb->busperf3, USB_DRV_Reg(&musb->busperf3) | (USB_BUSPERF3_DISUSBRESET | USB_BUSPERF3_SWRST));
    /* Enable system interrupts, but disable all ep interrupts */
    usb_en_sys_intr();
    USB_DRV_WriteReg8(&musb->index, 0);
    /* Flush ep0 FIFO */
    USB_DRV_WriteReg(&musb->csr0, USB_CSR0_FLUSHFIFO);
    /* enable EP0 interrupt */
    usb_ep0en();
    /* Unmask USB L1 interrupt */
    DRV_WriteReg(&musb->usb_l1intm, (USB_L1INTM_DMA_INT_UNMASK | USB_L1INTM_TX_INT_UNMASK | USB_L1INTM_RX_INT_UNMASK | USB_L1INTM_USBCOM_INT_UNMASK | USB_L1INTM_PSR_INT_UNMASK) | USB_L1INTM_QINT_UNMASK | USB_L1INTM_QHIF_INT_UNMASK);
    DRV_WriteReg8(&musb->dma_intr_unmask_set,  0xFF);

    hal_usb_drv_create_isr();

    return HAL_USB_STATUS_OK;
}


#endif /*HAL_USB_MODULE_ENABLED*/
