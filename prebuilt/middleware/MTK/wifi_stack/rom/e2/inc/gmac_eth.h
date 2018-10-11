#ifndef __GMAC_ETH_H__
#define __GMAC_ETH_H__
#include "delay.h"

#define RT3180_BIT(x)              ((1 << x))


/* ====================================== */
#define RT3180_GDM1_DISPAD       RT3180_BIT(18)
#define RT3180_GDM1_DISCRC       RT3180_BIT(17)
#define RT3180_GDM1_STRPCRC      RT3180_BIT(16)
//GDMA1 uni-cast frames destination port
#define RT3180_GDM1_UFRC_P_CPU     (0 << 12)
#define RT3180_GDM1_UFRC_P_GDMA1   (1 << 12)
#define RT3180_GDM1_UFRC_P_ppe     (6 << 12)
#define RT3180_GDM1_UFRC_P_DROP    (7 << 12)
//GDMA1 broad-cast MAC address frames
#define RT3180_GDM1_BFRC_P_CPU     (0 << 8)
#define RT3180_GDM1_BFRC_P_GDMA1   (1 << 8)
#define RT3180_GDM1_BFRC_P_PPE     (6 << 8)
#define RT3180_GDM1_BFRC_P_DROP    (7 << 8)
//GDMA1 multi-cast MAC address frames
#define RT3180_GDM1_MFRC_P_CPU     (0 << 4)
#define RT3180_GDM1_MFRC_P_GDMA1   (1 << 4)
#define RT3180_GDM1_MFRC_P_PPE     (6 << 4)
#define RT3180_GDM1_MFRC_P_DROP    (7 << 4)
//GDMA1 other MAC address frames destination port
#define RT3180_GDM1_OFRC_P_CPU     0
#define RT3180_GDM1_OFRC_P_GDMA1   1
#define RT3180_GDM1_OFRC_P_PPE     6
#define RT3180_GDM1_OFRC_P_DROP    7
/* ====================================== */
/* ====================================== */
#define RT3180_GP1_LNK_DWN     RT3180_BIT(9) 
#define RT3180_GP1_AN_FAIL     RT3180_BIT(8) 
/* ====================================== */
/* ====================================== */
#define RT3180_PSE_RESET       RT3180_BIT(0)
/* ====================================== */
#define RT3180_PST_DRX_IDX0       RT3180_BIT(16)
#define RT3180_PST_DTX_IDX3       RT3180_BIT(3)
#define RT3180_PST_DTX_IDX2       RT3180_BIT(2)
#define RT3180_PST_DTX_IDX1       RT3180_BIT(1)
#define RT3180_PST_DTX_IDX0       RT3180_BIT(0)

#define RT3180_TX_WB_DDONE       RT3180_BIT(6)
#define RT3180_RX_DMA_BUSY       RT3180_BIT(3)
#define RT3180_TX_DMA_BUSY       RT3180_BIT(1)
#define RT3180_RX_DMA_EN         RT3180_BIT(2)
#define RT3180_TX_DMA_EN         RT3180_BIT(0)

#define RT3180_GP1_FRC_EN         RT3180_BIT(15)

#define RT3180_GP1_FC_TX        RT3180_BIT(11)
#define RT3180_GP1_FC_RX        RT3180_BIT(10)
#define RT3180_GP1_LINK_DWN     RT3180_BIT(9)
/*
     FE_INT_STATUS
*/
#define FE_TX_DONE_INT0		RT3180_BIT(8)
#define FE_TX_DONE_INT1		RT3180_BIT(9)
#define FE_RX_DONE_INT0		RT3180_BIT(2)

#define RT3180_CNT_PPE_AF       RT3180_BIT(31)     
#define RT3180_CNT_GDM1_AF      RT3180_BIT(29)
#define RT3180_SDIO_TX_STATUS RT3180_BIT(26)
#define RT3180_PSE_P1_FC        RT3180_BIT(22)
#define RT3180_PSE_P0_FC        RT3180_BIT(21)
#define RT3180_PSE_FQ_EMPTY     RT3180_BIT(20)
#define RT3180_GE1_STA_CHG      RT3180_BIT(18)
#define RT3180_TX_COHERENT      RT3180_BIT(17)
#define RT3180_RX_COHERENT      RT3180_BIT(16)
/*
#define RT3180_TX_DONE_INT1     RT3180_BIT(9)
#define RT3180_TX_DONE_INT0     RT3180_BIT(8)
#define RT3180_RX_DONE_INT0     RT3180_BIT(2)
*/
#define RT3180_TX_DLY_INT       RT3180_BIT(1)
#define RT3180_RX_DLY_INT       RT3180_BIT(0)

#define RT3180_PDMA_RELATED        0x0100

#define RT3180_TX_BASE_PTR0        (BASE_MAC + 0x990)
#define RT3180_TX_MAX_CNT0         (BASE_MAC + 0x994)
#define RT3180_TX_CTX_IDX0         (BASE_MAC + 0x998)
#define RT3180_TX_DTX_IDX0         (BASE_MAC + 0x99C)
#if 0 // not MT7662
#define RT3180_PDMA_GLO_CFG        (RT3180_PDMA_RELATED + 0x00)
#define RT3180_PDMA_RST_IDX        (RT3180_PDMA_RELATED + 0x04)
#define RT3180_PDMA_SCH_CFG        (RT3180_PDMA_RELATED + 0x08)
#define RT3180_DELAY_INT_CFG       (RT3180_PDMA_RELATED + 0x0C)

#define RT3180_TX_BASE_PTR1        (RT3180_PDMA_RELATED + 0x20)
#define RT3180_TX_MAX_CNT1         (RT3180_PDMA_RELATED + 0x24)
#define RT3180_TX_CTX_IDX1         (RT3180_PDMA_RELATED + 0x28)
#define RT3180_TX_DTX_IDX1         (RT3180_PDMA_RELATED + 0x2C)
#endif

#define RT3180_RX_BASE_PTR0        (BASE_MAC + 0x9a0)
#define RT3180_RX_MAX_CNT0         (BASE_MAC + 0x9a4)
#define RT3180_RX_CALC_IDX0        (BASE_MAC + 0x9a8)
#define RT3180_RX_DRX_IDX0         (BASE_MAC + 0x9aC)

#define RT3180_FE_INT_STATUS		0x200
#define RT3180_FE_INT_ENABLE		0x204

//SDIO Interrupt Register (base: 0xnnnn.0200) ¡V For CPU Access
#define SD_PDMA_INT             0x200  //default: 0x00 SDIO and PMDA Interrupt
#define SD_PDMA_INT_EN          0x204  //default: 0x00 SDIO and PMDA Interrupt Enable

#define SD_MAIL_BOX             0x300 
#define SOC_HST_MAIL0         (SD_MAIL_BOX + 0x10)
#define SOC_HST_DB               (SD_MAIL_BOX + 0x24)
#define SOC_HST_DB0_BIT      (1 << 0)
#define SOC_HST_DB1_BIT      (1 << 1)

/*=========================================
      PDMA RX Descriptor Format define
=========================================*/

//-------------------------------------------------
typedef struct _PDMA_RXD_INFO1_  PDMA_RXD_INFO1_T;

struct _PDMA_RXD_INFO1_ {
	unsigned int    PDP0;
};
//-------------------------------------------------
typedef struct _PDMA_RXD_INFO2_    PDMA_RXD_INFO2_T;

struct _PDMA_RXD_INFO2_ {
	unsigned int    PLEN1                 : 14;
	unsigned int    LS1                   : 1;
	unsigned int    BURST                 : 1;
	unsigned int    PLEN0                 : 14;
	unsigned int    LS0                   : 1;
	unsigned int    DDONE_bit             : 1;
};
//-------------------------------------------------
typedef struct _PDMA_RXD_INFO3_  PDMA_RXD_INFO3_T;

struct _PDMA_RXD_INFO3_ {
	unsigned int    PDP1;
};
//-------------------------------------------------
typedef struct _PDMA_RXD_INFO4_    PDMA_RXD_INFO4_T;

struct _PDMA_RXD_INFO4_ {    
    unsigned int      PktLen:14;
    unsigned int      Rsv:11;
    unsigned int      QSel:2;
    unsigned int      S_port:3;
    unsigned int      InfoType:2;
#if 0    
	unsigned int		BA:1;
	unsigned int		DATA:1;
	unsigned int		NULLDATA:1;
	unsigned int		FRAG:1;
	unsigned int		U2M:1;              // 1: this RX frame is unicast to me
	unsigned int		Mcast:1;            // 1: this is a multicast frame
	unsigned int		Bcast:1;            // 1: this is a broadcast frame	
	unsigned int		MyBss:1;  	// 1: this frame belongs to the same BSSID	
	unsigned int		Crc:1;              // 1: CRC error
	unsigned int		CipherErr:2;        // 0: decryption okay, 1:ICV error, 2:MIC error, 3:KEY not valid
	unsigned int		AMSDU:1;		// rx with 802.3 header, not 802.11 header.
	unsigned int		HTC:1;
	unsigned int		RSSI:1;
	unsigned int		L2PAD:1;
	unsigned int		AMPDU:1;	
	unsigned int		Decrypted:1;	// this frame is being decrypted.
	unsigned int		PlcpSignal:1;		// To be moved
	unsigned int		PlcpRssil:1;// To be moved
	unsigned int		Rsv1:13;
#endif    
};


struct PDMA_rxdesc {
	PDMA_RXD_INFO1_T rxd_info1;
	PDMA_RXD_INFO2_T rxd_info2;
	PDMA_RXD_INFO3_T rxd_info3;
	PDMA_RXD_INFO4_T rxd_info4;
};


/*=========================================
      PDMA TX Descriptor Format define
=========================================*/
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO1_  PDMA_TXD_INFO1_T;

struct _PDMA_TXD_INFO1_ {
	unsigned int    SDP0;
};
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO2_    PDMA_TXD_INFO2_T;

struct _PDMA_TXD_INFO2_ {
	unsigned int    SDL1                  : 14;
	unsigned int    LS1_bit               : 1;
	unsigned int    BURST_bit             : 1;
	unsigned int    SDL0                  : 14;
	unsigned int    LS0_bit               : 1;
	unsigned int    DDONE_bit             : 1;
};
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO3_  PDMA_TXD_INFO3_T;

struct _PDMA_TXD_INFO3_ {
	unsigned int    SDP1;
};
//-------------------------------------------------
typedef struct _PDMA_TXD_INFO4_    PDMA_TXD_INFO4_T;

struct _PDMA_TXD_INFO4_ {
#if 1 // MT7662
    unsigned int      PktLen:14;
    unsigned int      Rsv:11;
    unsigned int      QSel:2;
    unsigned int      S_port:3;
    unsigned int      InfoType:2;
#else
	unsigned int		len:15;
	unsigned int		rsv2:9;
	unsigned int		WIV:1;	// Wireless Info Valid. 1 if Driver already fill WI,  o if DMA needs to copy WI to correctposition
	unsigned int		QSEL:2;	// select on-chip FIFO ID for 2nd-stage output scheduler.0:MGMT, 1:HCCA 2:EDCA
	unsigned int		rsv:2;
	unsigned int		TCO:1;	//  
	unsigned int		UCO:1;	//  
	unsigned int		ICO:1;	//
#endif    
};


struct PDMA_txdesc {
	PDMA_TXD_INFO1_T txd_info1;
	PDMA_TXD_INFO2_T txd_info2;
	PDMA_TXD_INFO3_T txd_info3;
	PDMA_TXD_INFO4_T txd_info4;
};

#endif /* __GMAC_ETH_H__ */
