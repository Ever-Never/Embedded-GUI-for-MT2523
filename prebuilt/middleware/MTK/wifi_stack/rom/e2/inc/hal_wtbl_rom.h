/*******************************************************************************
* Copyright (c) 2014 MediaTek Inc.
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
** $Log: hal_wtbl_rom.h $
**
** 05 22 2015 changyi.hsu
** [BORA00004545] [MT7637E2]
** 1. Pass all SEC SPP A-MSDU test cases.
** 2. Fix WTBL array size issue.
**
** 11 18 2014 changyi.hsu
** [BORA00000000] [MT7637]
** Initial version.
**
*/

#ifndef _HAL_WTBL_ROM_H
#define _HAL_WTBL_ROM_H

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
#define MAX_WTBL_ENTRY_NUM             32

#define WPI_ENTRY_NUM                  2
#define WEP_ENTRY_NUM                  1
#define WPI_ENTRY_SIZE                 (16 * 4)
#define WEP_ENTRY_SIZE                 (16 * 4)
#define CCMP_TKIP_ENTRY_SIZE           ( 8 * 4)

#define WTBL_TBL3_EID_UINT             ( 8 * 4)
#define WTBL_TBL4_EID_UINT             ( 8 * 4)

typedef struct _WTBL_TBL_FID_EID_T  {
    UINT_16        au2Tbl2Fid[MAX_WTBL_ENTRY_NUM];
    UINT_16        au2Tbl3Fid[MAX_WTBL_ENTRY_NUM];
    UINT_16        au2Tbl4Fid[MAX_WTBL_ENTRY_NUM];
    UINT_8         aucTbl2Eid[MAX_WTBL_ENTRY_NUM];
    UINT_8         aucTbl3Eid[MAX_WTBL_ENTRY_NUM];
    UINT_8         aucTbl4Eid[MAX_WTBL_ENTRY_NUM];
}WTBL_TBL_FID_EID_T, *P_WTBL_TBL_FID_EID_T;

typedef enum _ENUM_WTBL2_TX_COUNTER_TYPE_T
{
    RATE1_CNT = 0,
    RATEN_CNT,
    CURRENT_BW_CNT,
    OTHER_BW_CNT,
    TOTAL_TX_CNT
} ENUM_WTBL2_TX_COUNTER_TYPE_T;

typedef enum _ENUM_WTBL2_RX_COUNTER_TYPE_T
{
    RCPI_CNT = 0,
    CHANNELCAPACITY_CNT,
    NOISE_CNT,
    CE_RMSD,
    TOTAL_RX_CNT
} ENUM_WTBL2_RX_COUNTER_TYPE_T;

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
	/*----------------------------------------------------------------------------*/
		/*!
		* @brief  Set PSM flag for SW to overwrite PSM value in WTBL1 DW3
		*
		* @param  ucPsm 					   PSM flag
		*
		* @return HAL_STATUS				   Indication of command status
		*/
		/*----------------------------------------------------------------------------*/
		extern HAL_STATUS
		(*halWtblSetPsm)(
			IN UINT_8 ucPsm
			);
		/*----------------------------------------------------------------------------*/
		/*!
		* @brief  Set IPSM flag for SW to overwrite IPSM value in WTBL1 DW3
		*
		* @param  ucIpsm					   IPSM flag
		*
		* @return HAL_STATUS				   Indication of command status
		*/
		/*----------------------------------------------------------------------------*/
		extern HAL_STATUS
		(*halWtblSetIpsm)(
			IN UINT_8 ucIpsm
			);
		
		/*----------------------------------------------------------------------------*/
		/*!
		* @brief Set SN to WTBL2
		* @param ucEntryIdx 				wtbl index
		* @param ucSnSource 				TID(HT/VHT STA), AC(Legacy QoS STA)
		* @param u2SN						SN value
		* @return HAL_STATUS
		* @NOTE:!!!!TMAC MUST be stopped before using this HAL!!!!
		*/
		/*----------------------------------------------------------------------------*/
		
		
		extern HAL_STATUS
		(*halWtbl2SetSN)(
			IN UINT_8					   ucEntryIdx,
			IN ENUM_WTBL2_SN_SOURCE_TYPE_T eSnSource,
			IN UINT_16					   u2SN
			);
		/*----------------------------------------------------------------------------*/
		/*!
		* @brief Config WTBL Entry Capability
		*
		* @param ucEntryIdx 		WTBL Entry Index
		* @param prWtblTxConfig 	Pointer to the P_HAL_WTBL_TX_CONFIG_T data structure
		* @param prWtblFidEid		Pointer to the P_HAL_WTBL_FID_EID_T data structure
		* @param prWtblSecConfig	Pointer to the P_HAL_WTBL_HT_CAP_T data structure
		*
		* @return (HAL_STATUS)
		*/
		/*----------------------------------------------------------------------------*/
		extern HAL_STATUS
		(*halWtblConfigTbl1EntryCap)(
			IN UINT_8				   ucEntryIdx,
			IN P_HAL_WTBL_TX_CONFIG_T  prWtblTxConfig,
			IN P_HAL_WTBL_FID_EID_T    prWtblFidEid,
			IN P_HAL_WTBL_SEC_CONFIG_T prWtblSecConfig
			);
		
		/*----------------------------------------------------------------------------*/
		/*!
		* @brief Config BA entry
		* @param ucWtblIdx					wtbl index
		* @param fgEnable					Enable BA for this TID
		* @param ucTID						TID
		* @param ucWinSizeEntryIdx			Assign window size entry index to this TID
		* @return HAL_STATUS
		*/
		/*----------------------------------------------------------------------------*/
		extern HAL_STATUS
		(*halWtbl2ConfigBaEntry)(
			IN UINT_8  ucWtblIdx,
			IN BOOLEAN fgEnable,
			IN UINT_8  ucTID,
			IN UINT_8  ucWinSizeEntryIdx
			);
		
		/*----------------------------------------------------------------------------*/
		/*!
		* @brief config antenna
		* @param ucEntryIdx 					 wtbl index
		* @param eWtbl2ClearCnt 			   clear type ,tx or rx
		* @return HAL_STATUS
		*/
		/*----------------------------------------------------------------------------*/
		extern HAL_STATUS
		(*halWtbl2ClearCounter)(
			IN UINT_8						   ucEntryIdx,
			IN ENUM_WTBL2_CLEAR_COUNTER_TYPE_T eWtbl2ClearCnt
			);
		
		/*----------------------------------------------------------------------------*/
		/*!
		* \brief halWtblGetTbl1EntryCap
		*
		* \param[in]
		*
		* \return HAL_STATUS
		*/
		/*----------------------------------------------------------------------------*/
		extern HAL_STATUS
		(*halWtblGetTbl1EntryCap)(
			IN UINT_8					ucEntryIdx,
			OUT P_HAL_WTBL_TX_CONFIG_T	prWtblTxConfig,
			OUT P_HAL_WTBL_FID_EID_T	prWtblFidEid,
			OUT P_HAL_WTBL_SEC_CONFIG_T prWtblSecConfig
			);
	
	/*----------------------------------------------------------------------------*/
	/*!
	* @brief config wtbl2 AMPDU capability (E2+ only)
	* @param ucEntryIdx 				  wtbl index
	* @param prWtblPeerCap				  the peer capability
	* @return HAL_STATUS
	*/
	/*----------------------------------------------------------------------------*/
	extern HAL_STATUS
	(*halWtbl2ConfigAmpduCap)(
		IN UINT_8				 ucEntryIdx,
		IN P_HAL_WTBL_PEER_CAP_T prWtblPeerCap
		);


#endif  // #ifndef _HAL_WTBL_ROM_H
