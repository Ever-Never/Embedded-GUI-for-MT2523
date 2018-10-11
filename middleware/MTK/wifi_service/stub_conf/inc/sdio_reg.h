#ifndef __SDIO_REG_H__
#define __SDIO_REG_H__

/* FIFO SIZE DEFINITION */

#define DEV_TX_FIFO_SZ          2080
#define DEV_RX_FIFO_SZ          2304

/*4*16 = 64Byte*/
#define DEV_DEFAULT_TX_THOLD    (4)

#ifdef BIT
#undef BIT
#endif

#define BIT(n) (1 << (n))


/*--------------------- PIO Driver Domain Control Register --------------------*/
#define SDIO_CCIR           0x0000      /* Chip ID */

#define SDIO_CHLPCR         0x0004      /* HIF Low Power Control */

#define SDIO_CSDIOCSR       0x0008      /* SDIO Status Register */

#define SDIO_CHCR           0x000C      /* HIF Control Register */



#define SDIO_CHISR          0x0010      /* HIF Interrupt Status */

#define SDIO_CHIER          0x0014      /* HIF Interrupt Enable */



#define SDIO_CTDR           0x0018      /* Tx Data Port */

#define SDIO_CRDR           0x001C      /* Rx Data Port */



#define SDIO_CTFSR          0x0020      /* Tx FIFO Status */

#define SDIO_CRPLR          0x0024      /* Rx Packet Length */



#define SDIO_CTMDR          0x00B0      /* Test Mode Data Port Register */

#define SDIO_CTMCR          0x00B4      /* Test Mode Control Register */

#define SDIO_CTMDPCR0       0x00B8      /* Test Mode Data Pattern 0 */

#define SDIO_CTMDPCR1       0x00BC      /* Test Mode Data Pattern 1 */



#define SDIO_CSR            0x00D8      /* Snapshot Register */



#define SDIO_CLKIOCR        0x0100

#define SDIO_CMDIOCR        0x0104

#define SDIO_DAT0IOCR       0x0108

#define SDIO_DAT1IOCR       0x010C

#define SDIO_DAT2IOCR       0x0110

#define SDIO_DAT3IOCR       0x0114

#define SDIO_CLKDLYCR       0x0118

#define SDIO_CMDDLYCR       0x011C

#define SDIO_ODATDLYCR      0x0120

#define SDIO_IDATDLYCR1     0x0124

#define SDIO_IDATDLYCR2     0x0128

#define SDIO_ILHCR          0x012C



#define SDIO_CCIR_G_FUNC_RDY            BIT(23)

#define SDIO_CCIR_F_FUNC_RDY            BIT(22)

#define SDIO_CCIR_B_FUNC_RDY            BIT(21)

#define SDIO_CCIR_POR_INDICATOR         BIT(20)

#define SDIO_CCIR_REVISION_ID           0x000F0000

#define SDIO_CCIR_CHIP_ID               0x0000FFFF



#define SDIO_CHLPCR_FW_OWN_REQ_CLR      BIT(9)  // Get ownership from FW

#define SDIO_CHLPCR_FW_OWN_REQ_SET      BIT(8)  // Give ownership to FW

#define SDIO_CHLPCR_INT_EN_CLR          BIT(1)  // Clr will disable interrupt out to host

#define SDIO_CHLPCR_INT_EN_SET          BIT(0)  // Set will enable interrupt out to host



#define SDIO_CSDIOCSR_PB_CMD7_RESELECT_DIS BIT(3)  // Enable/disable response to CMD7 re-select

#define SDIO_CSDIOCSR_SDIO_INT_CTL      BIT(2)  // Enable/disable Async interrupt

#define SDIO_CSDIOCSR_SDIO_BUSY_EN      BIT(1)  // Enable/disable write busy signal

#define SDIO_CSDIOCSR_SDIO_RE_INIT_EN   BIT(0)  // If set, it will let CMD5 reset SDIO IP



#define SDIO_CHCR_INT_CLR_CTRL          BIT(1)  // Control Read-Clear or Write-1-Clear



/* CHISR int ststus */

#define SDIO_CHISR_RX_PKT_LEN           0xFFFF0000

#define SDIO_CHISR_FIRMWARE_INT         0x0000FE00

#define SDIO_CHISR_TX_OVERFLOW          BIT(8)

#define SDIO_CHISR_FW_INT_INDICATOR     BIT(7)

#define SDIO_CHISR_TX_CMPLT_CNT         0x00000070

#define SDIO_CHISR_TX_UNDER_THOLD       BIT(3)

#define SDIO_CHISR_TX_EMPTY             BIT(2)

#define SDIO_CHISR_RX_RDY               BIT(1)

#define SDIO_CHISR_FW_OWN_BACK          BIT(0)

/*CHIER int enable*/

#define SDIO_CHIER_FIRMWARE_INT_EN      0x0000FE00

#define SDIO_CHIER_TX_OVERFLOW_EN       BIT(8)

#define SDIO_CHIER_FW_INT_INDICATOR_EN  BIT(7)

#define SDIO_CHIER_TX_UNDER_THOLD_EN    BIT(3)

#define SDIO_CHIER_TX_EMPTY_EN          BIT(2)

#define SDIO_CHIER_RX_RDY_EN            BIT(1)

#define SDIO_CHIER_FW_OWN_BACK_EN       BIT(0)





#define SDIO_CTFSR_TX_FIFO_CNT          0x000000FF  // in unit of 16byte

#define SDIO_CRPLR_RX_PKT_LEN           0xFFFF0000  // in unit of byte



#define SDIO_CTMCR_FW_OWN               BIT(24)

#define SDIO_CTMCR_PRBS_INIT_VAL        0x00FF0000

#define SDIO_CTMCR_TEST_MODE_STATUS     BIT(8)

#define SDIO_CTMCT_TEST_MODE_SELECT     0x00000003





#if 1

/*-----------------------PIO Firmware Domain Control Register --------------------*/

#define SDIO_PIO_BASE_ADDR  0xF0040000

#define SDIO_ADDR(_addr)    ((volatile unsigned int *)(SDIO_PIO_BASE_ADDR+_addr))



#define SDIO_HCFISR         SDIO_ADDR(0x0300)   // Interrupt Status Regiter

#define SDIO_HCFIER         SDIO_ADDR(0x0304)   // Interrupt Enable Regiter

#define SDIO_HCFSCR         SDIO_ADDR(0x0308)   // Status Register

#define SDIO_HCFCR          SDIO_ADDR(0x030C)   // Control Regiter

#define SDIO_HCFTDR         SDIO_ADDR(0x0310)   // Tx Data Port

#define SDIO_HCFRDR         SDIO_ADDR(0x0314)   // Rx Data Port

#define SDIO_HCFHBR         SDIO_ADDR(0x0318)   // Buffer Control

#define SDIO_HCFTCR         SDIO_ADDR(0x031C)   // Tx Count

#define SDIO_HCFRCR         SDIO_ADDR(0x0320)   // Rx Count

#define SDIO_HCFSDBGR       SDIO_ADDR(0x0324)   // SRAM Debug Register

#define SDIO_HCFSTPR        SDIO_ADDR(0x0328)   // SRAM  Tx Pointer

#define SDIO_HCFSRPR        SDIO_ADDR(0x032C)   // SRAM Rx Pointer



#define SDIO_HCFISR_RX_DATA_INT         BIT(12)

#define SDIO_HCFISR_TX_DATA_INT         BIT(11)

#define SDIO_HCFISR_CLR_FW_OWN          BIT(9)

#define SDIO_HCFISR_SET_FW_OWN          BIT(8)



#define SDIO_HCFIER_RX_DATA_INT_EN      BIT(12)

#define SDIO_HCFIER_TX_DATA_INT_EN      BIT(11)

#define SDIO_HCFIER_CLR_FW_OWN_EN       BIT(9)

#define SDIO_HCFIER_SET_FW_OWN_EN       BIT(8)



#define SDIO_HCFSCR_FIRMWARE_INT        0x0000FE00

#define SDIO_HCFSCR_TX_CMPLT_CNT        0x00000070

#define SDIO_HCFSCR_TX_UNDER_THOLD      BIT(3)

#define SDIO_HCFSCR_TX_EMPTY            BIT(2)

#define SDIO_HCFSCR_FW_OWN_BACK         BIT(0)



#define SDIO_HCFCR_RX_PKT_LEN           0xFFFF0000

#define SDIO_HCFCR_SDBG_HOST_ACCESS     BIT(10)

#define SDIO_HCFCR_SDBG_SADRD_SEL       BIT(9)

#define SDIO_HCFCR_SDBG_MOD_E           BIT(8)

#define SDIO_HCFCR_RX_PKT_RDY           BIT(4)

#define SDIO_HCFCR_G_FUNC_RDY           BIT(2)

#define SDIO_HCFCR_F_FUNC_RDY           BIT(1)

#define SDIO_HCFCR_B_FUNC_RDY           BIT(0)



#define SDIO_HCFHBR_TX_BUF_EN           BIT(29)

#define SDIO_HCFHBR_RX_BUF_EN           BIT(28)

#define SDIO_HCFHBR_TX_BUF_IDLE_INT_TYP BIT(27)

#define SDIO_HCFHBR_TX_IDLE_CNT_EN      BIT(26)

#define SDIO_HCFHBR_MAX_TX_IDLE_TIME    0x03FF0000

#define SDIO_HCFHBR_TX_DATA_THOLD       0x000000FF



#define SDIO_HCFTCR_TX_BUF_EMPTY        BIT(16)

#define SDIO_HCFTCR_TX_DATA_CNT         0x0000FFFF



#define SDIO_HCFRCR_RX_BUF_EMPTY        BIT(16)

#define SDIO_HCFRCR_RX_DATA_CNT         0x0000FFFF

#endif





/*----------------------- DMA Driver Domain Control Register ----------------------*/

#define SDIO_IP_WCIR			(0x0000)

#define SDIO_IP_WHLPCR			(0x0004)

#define SDIO_IP_WSDIOCSR		(0x0008)

#define SDIO_IP_WHCR			(0x000C)

#define SDIO_IP_WHISR			(0x0010)

#define SDIO_IP_WHIER			(0x0014)



#define SDIO_IP_WASR			(0x0020)

#define SDIO_IP_WSICR			(0x0024)

#define SDIO_IP_WTSR0			(0x0028)

#define SDIO_IP_WTSR1			(0x002C)



#define SDIO_IP_WTDR0			(0x0030)

#define SDIO_IP_WTDR1			(0x0034)



#define SDIO_IP_WRDR0			(0x0050)

#define SDIO_IP_WRDR1			(0x0054)

#define SDIO_IP_WRDR2			(0x0058)

#define SDIO_IP_WRDR3			(0x005C)



#define SDIO_IP_H2DSM0R			(0x0070)

#define SDIO_IP_H2DSM1R			(0x0074)

#define SDIO_IP_D2HRM0R			(0x0078)

#define SDIO_IP_D2HRM1R			(0x007C)

#define SDIO_IP_D2HRM2R			(0x0080)

#define SDIO_IP_WRPLR			(0x0090)

#define SDIO_IP_WRPLR1			(0x0094)

#define SDIO_IP_SWPCDBGR        (0x0154)  // add for debug use to read cpu pc vaule



#define SDIO_IP_WTMDR       	(0x00B0)

#define SDIO_IP_WTMCR       	(0x00B4)

#define SDIO_IP_WTMDPCR0    	(0x00B8)	//Read pattern

#define SDIO_IP_WTMDPCR1    	(0x00BC)	//Write pattern



//SDIO_IP_WHLPCR

#define W_INT_EN_SET		(0x1)

#define W_INT_EN_CLR		(0x2)

#define W_FW_OWN_REQ_SET	(0x100)

#define W_DRV_OWN_STATUS	(0x100)

#define W_FW_OWN_REQ_CLR	(0x200)



//WSDIOCSR

#define DB_WR_BUSY_EN		(0x8)

#define DB_RD_BUSY_EN		(0x4)

#define SDIO_INT_CTL		(0x2)



//WHCR

#define RX_ENHANC_MODE		(0x10000)

#define MAX_HIF_RX_LEN_NUM	(0xF0)

#define RECV_MAIL_BOX_RD_CLR_EN	(0x4)

#define W_INT_CLR_CTRL		(0x2)



//WCIR

#define POR_INDICATOR		(0x100000)

#define W_FUNC_RDY			(0x200000)

#define REVISION_ID			(0xF0000)

#define CHIP_ID				(0xFFFF)



#endif


