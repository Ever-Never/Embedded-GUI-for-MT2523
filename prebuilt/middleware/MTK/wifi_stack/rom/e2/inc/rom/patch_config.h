/*
** $BORA/BRANCHES/MT7637_E2_MP/mcu/wifi_uni_mac_7636/include/patch_config.h $
*/

/*! \file   "Patch_config.h"
    \brief  This file includes the compilation flags for the bug fixes. 
*/

/*******************************************************************************
* Copyright (c) 2007 MediaTek Inc.
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
** $Log: config.h $
 *
 * 06 29 2016 navaneeth.krishnan
 * [BORA00006890] [MT7637E2][IOT][FW] N9 crash during Opmode switch
 * 
 * 1) Purpose:N9 Crash during opmode switch endurance test
 * 
 * 2) Changed function name:hemExtCmdAPStop,OBSSScanTimeout
 * 
 * 3) Code change description brief:hemExtCmdAPStop - OBSS timer is stoped at AP stop		
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 06 24 2016 navaneeth.krishnan
 * [BORA00006848] [MT7637E2][IOT][FW] Roaming From 11G to 11A
 * 
 * 1) Purpose:Roaming from 11G to 11A
 * 
 * 2) Changed function name:patch__BssTableSearch
 * 
 * 3) Code change description brief:patch__BssTableSearch - Removed Channel conditional checks
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 06 22 2016 navaneeth.krishnan
 * [BORA00006835] [MT7637E2][IOT][FW] Retry Changed to 30 and CCA to ED
 * 
 * 1) Purpose:Retry Changed to 30 and CCA to ED
 * 
 * 2) Changed function name:patch__RTMPWriteTxWI_Data,txmInit
 * 
 * 3) Code change description brief:txmInit - set CCA to ED.patch__RTMPWriteTxWI_Data - Retry count to 30
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 06 21 2016 navaneeth.krishnan
 * [BORA00005376] [MT7637E2][IoT][FW] Remove Extended capability info IE from Probe and Assoc req/resp frame when STA operating in 11bg mode
 * 
 * 1) Purpose:With ASUS RT-N66U AP 5G has IOT connection issue - Resolved
 * 
 * 2) Changed function name:patch__MakeOutgoingFrame
 * 
 * 3) Code change description brief:Modified the coditional check to verify the wireless mode
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 06 21 2016 navaneeth.krishnan
 * [BORA00006799] [MT7637E2][IOT][FW]Remove Memcpy in APCLI RX path
 * 
 * 1) Purpose:Remove Memcpy in APCLI Rx Path
 * 
 * 2) Changed function name:patch__bridge_rx_handle
 * 
 * 3) Code change description brief:condtion for rom_memcpy is changed sothat througput increases >20Mbps
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 06 21 2016 george.koruthu
 * [BORA00006617] [MT7637E2][IOT][FW]Reason Code to CM4
 * 
 * 1) Purpose:
 * Enable Reason Code Feature
 * 
 * 2) Changed function name:
 * 
 * 3) Code change description brief:
 * 
 * 4) Unit Test Result:
 * PASS
 * 
 * <CRID START> <CRID END>
**
** 06 15 2016 avishad.verma
** [BORA00006764] [MT7687][IOT][FW] send ownership back to  CM4 if ownership is with N9
** 	
** 	1) Purpose:
** 	Send Ownership back to CM4 if ownership is with N9 before sending any packet to CM4
** 	2) Changed function name:
** 	patch_connsys_send_packets
** 	3) Code change description brief:
** 	Send Ownership back to CM4 if ownership is with N9 before sending any packet to CM4
** 	4) Unit Test Result:
** 	PASS
** 	
** 	
** 	<CRID START> <CRID END>
 *
 * 06 09 2016 navaneeth.krishnan
 * [BORA00006736] [MT7637E2][IOT][FW]Memory Leak bug fix
 * 1) Purpose:Fix Memory Leaks
 * 
 * 2) Changed function name:patch__PseKickOutTx,patch__ap_cmm_peer_assoc_req_action
 * 
 * 3) Code change description brief:Memory Leaks are fixed
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 06 02 2016 anil.vankayala
 * [BORA00006676] [MT7637E2][IOT][FW] Support for new inband command(0x24)
 * 
 * 1) Purpose:
 * Code changes for Management pkt filter
 * 
 * 2) Changed function name:
 * patch__STAHandleRxMgmtFrame
 * hemExtCmdEnableMgmtPktFilter
 * 
 * 3) Code change description brief:
 * Added new inband command 0x24
 * Added new event id 0x53
 * 
 * 4) Unit Test Result:
 * PASS
 * 
 * <CRID START> <CRID END>
**
** 05 24 2016 avishad.verma
** [BORA00006507] [MT7687][IOT][FW] Enter PSM need 5.8 second
** 	
** 	1) Purpose:
** 	Low Power stability issue fix.
** 	P2P bug fix.
** 	BLE COEX  Fix for remaining window
** 	2) Changed function name:
** 	txminit
** 	bssEnqBcnPktFromCmd
** 	3) Code change description brief:
** 	initialize the PID pool in TXM init
** 	add check for P2p role for rate selection in beacon
** 	updated the Macro in HW patch.  
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
 *
 * 05 20 2016 navaneeth.krishnan
 * [BORA00006606] [MT7637E2][IOT][FW]Correct Disassoc Define in APCLI Mode
 * 
 * 1) Purpose:Send Correct Disconnect Event
 * 
 * 2) Changed function name:patch__RtmpOSWrielessEventSend
 * 
 * 3) Code change description brief:patch__RtmpOSWrielessEventSend - Changed the Macro 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 05 20 2016 navaneeth.krishnan
 * [BORA00006617] [MT7637E2][IOT][FW]Reason Code to CM4
 * 
 * 1) Purpose:ReasonCode Reporting
 * 
 * 2) Changed function name:patch__RspByInBands,patch__RTMPSendWirelessEvent,patch__LinkUp,patch__PeerDeauthSanity,patch__PeerAuthSanity,patch__PeerDisassocSanity,patch__PeerAssocRspSanity,patch__IterateOnBssTab
 * 
 * 3) Code change description brief:patch__RspByInBands-  For handling existing events,patch__RTMPSendWirelessEvent - For adding AP MAC,(patch__PeerDeauthSanity,patch__PeerAuthSanity,patch__PeerDisassocSanity,patch__PeerAssocRspSanity,patch__IterateOnBssTab) - Patches for identifying the reason code
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
**
** 05 03 2016 avishad.verma
** [BORA00006164] P2P support in N9
** 	
** 	1) Purpose:
** 	Enabled P2P Feature and Dbg Log to Host Feature
** 	2) Changed function name:
** 	Patch_config.h
** 	3) Code change description brief:
** 	Enable features
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 05 02 2016 avishad.verma
** [BORA00006508] [MT7687][IOT][FW] 5G Support in AP Mode
** 	
** 	1) Purpose:
** 	5G support in AP Mode
** 	2) Changed function name:
** 	bssEnqBcnPktFromCmd
** 	patch__RTMPSetPhyMode
** 	SetFullScanChList
** 	SetActivePassiveChList
** 	FillSmartConnectScanChList
** 	FillPartialScanChList
** 	hemCmdStartScan
** 	3) Code change description brief:
** 	Added support for 5g in AP mode, partial scan for 5G also added
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 04 29 2016 avishad.verma
** [BORA00006507] [MT7687][IOT][FW] Enter PSM need 5.8 second
** [BORA00006506] [MT7687][IOT][FW]TX Power inconsistent between QA-Tool and Normal mode
** 	
** 	1) Purpose:
** 	Bug Fix:
** 	 Enter PSM need 5.8 second 
** 	TX Power inconsistent between QA-Tool and Normal mode
** 	2) Changed function name:
** 	patch__OffloadPmStateCtrl
** 	patch__STAHandleRxDataFrame
** 	patch__MlmeHardTransmitMgmtRing
** 	txmHandleMacTxDone
** 	setOpMode
** 	3) Code change description brief:
** 	Modified code to get the TXS of the NULL Data
** 	Removed code for modifying the TX power in opmode command
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
 *
 * 04 29 2016 navaneeth.krishnan
 * [BORA00006503] [MT7637E2][IOT][FW]AP Only and STA only interface
 * 
 * 1) Purpose:AP Only and STA only Interface
 * 
 * 2) Changed function name:patch__RspByInBands,patch_connsys_send_packets,apply_iot_patch_after_init
 * 
 * 3) Code change description brief:patch__RspByInBands - In AP mode use interface 1,patch__RspByInBands - Send on interface 0 for STA and 1 for AP,apply_iot_patch_after_init - Enable patch_connsys_send_packets
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 04 29 2016 navaneeth.krishnan
 * [BORA00006502] [MT7637E2][IOT][FW] APCLI LINK DOWN EVENT
 * 
 * 1) Purpose:APCLI Link down Event
 * 
 * 2) Changed function name:patch__wireless_send_event,patch__ApCliLinkDown
 * 
 * 3) Code change description brief:patch__wireless_send_event - allow aplci link down to go to CM4,patch__ApCliLinkDown - Send Link down event
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 04 29 2016 navaneeth.krishnan
 * [BORA00006178] [MT7637E2][IOT][FW]Patch Optimizations
 * 
 * 1) Purpose:Patch Optimizations	
 * 
 * 2) Changed function name:Mutilple files and Functions are optimized	
 * 
 * 3) Code change description brief:Define changes are done,Duplicate code,Unused features,code paths which are not invoked are removed
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 04 28 2016 george.koruthu
 * [BORA00006496] [MT7637E2][IOT][FW]STA not connecting to AP that has better Rssi.
 * 
 * 1) Purpose:
 * Fix for STA not connecting to AP with better RSSI. 
 * 2) Changed function name:
 * patch__ConvertToRssi
 * 3) Code change description brief:
 * Rssi returned without subtracting LNA and RSSI offset.
 * 4) Unit Test Result:
 * PASS
 * 
 * <CRID START> <CRID END>
 *
 * 04 28 2016 navaneeth.krishnan
 * [BORA00006494] [MT7637E2][IOT][FW] Rx Vector Enable
 * 
 * 1) Purpose:Enable Rx Vector group 3
 * 
 * 2) Changed function name:arHifCmdExtTableIoT 
 * 
 * 3) Code change description brief:arHifCmdExtTableIoT - Added new Inband command hemExtCmdEnableRxVector
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 04 28 2016 navaneeth.krishnan
 * [BORA00006495] [MT7637E2][IOT][FW]RSSI Threshold Filter
 * 
 * 1) Purpose:RSSI Threshold filter
 * 
 * 2) Changed function name:patch__STAHandleRxMgmtFrame,ap_cmm_peer_assoc_req_action,arHifCmdExtTableIoT
 * 
 * 3) Code change description brief:patch__STAHandleRxMgmtFrame - Rssi is added to all mgmt frames,ap_cmm_peer_assoc_req_action -  If rssi is less than the threshold AP connection won't be allowed,arHifCmdExtTableIoT - added new Inband command hemExtCmdRssiThreshold}
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 04 26 2016 navaneeth.krishnan
 * [BORA00006480] [MT7637E2][IOT][FW]20/40 Scan Fix for AP/STA
 * 
 * 1) Purpose:20/40 Scan Fix for AP and STA
 * 
 * 2) Changed function name: patch__ScanNextChannel,patch__RTMPSetProfileParameters
 * 
 * 3) Code change description brief: patch__ScanNextChannel - If 20/40 scan don't build the scan list again,patch__RTMPSetProfileParameters- Dynamically read BW value according to STA or AP mode			
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
**
** 04 21 2016 michael.rong
** [BORA00004254] [MT7637E2][IOT][FW]
** 1) Purpose:
**    Update ConfigFree code/data size total = 1560 bytes
** 	
** 	2) Changed function name:
** 	
** 	3) Code change description brief:
** 	
** 	4) Unit Test Result:
** 	
** 	
** 	<CRID START> <CRID END>
 *
 * 04 18 2016 navaneeth.krishnan
 * [BORA00006432] [MT7637E2][IOT][FW]APCLI LINK STATUS
 * 
 * 1) Purpose:APCLI Link Up
 * 
 * 2) Changed function name:hemCmdGetLinkStatus
 * 
 * 3) Code change description brief:hemCmdGetLinkStatus - Added APCLI check
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
**
** 04 18 2016 avishad.verma
** [BORA00006165] Bootup time optimization
** 	
** 	1) Purpose:
** 	Removed bootup prints on MT7697
** 	2) Changed function name:
** 	bcmConfigBTProfile
** 	bcmReportCoexMode
** 	bcmceExtCmdCoexWiFiStatusUpdate
** 	BootUp_Time_Optimization
** 	iot_bcmceExtCmdCoexWiFiStatusUpdate
** 	3) Code change description brief:
** 	removed bootup prints of Mt7697
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
 *
 * 04 17 2016 navaneeth.krishnan
 * [BORA00006431] [MT7637E2][IOT][FW]Bandwidth adjust with channel above/below
 * 
 * 1) Purpose:Bandwidth adjust with channel above/below
 * 
 * 2) Changed function name:hemCmdBw
 * 
 * 3) Code change description brief:hemCmdBw - Bandwidth adjust with channel above/below
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
**
** 04 14 2016 avishad.verma
** [BORA00006413] [MT7687][IOT][FW] Memory Optimization of Bootup code
** 	
** 	1) Purpose:
** 	Moved bootup function to Heap
** 	2) Changed function name:
** 	apply_iot_patch_before_init
** 	wifi_normal_operation_entry
** 	3) Code change description brief:
** 	Moved the function only getting used on bootup to heap
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 04 13 2016 avishad.verma
** [BORA00005800] [MT7637E2][IOT][FW] build script for MT7687 and MT7697
** 	
** 	1) Purpose:
** 	Enabled the MediaTek proprietary WiFi zero config feature
** 	2) Changed function name:
** 	patch_Config.h
** 	3) Code change description brief:
** 	MediaTek proprietary WiFi zero config feature enabled
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 04 11 2016 avishad.verma
** [BORA00006349] Country region is getting changed after wireless mode if country region is 31~33
** 	
** 	1) Purpose:
** 	Storing the channel list of the country regions not present in N9 from CM4
** 	2) Changed function name:
** 	hemExtCmdCountryRegion
** 	3) Code change description brief:
** 	storing the channel list of the country regions in N9 on dynamic country region change
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 04 08 2016 avishad.verma
** [BORA00006330] [MT7687][IOT][FW] Dbg Log to Host Support in N9
** 	
** 	1) Purpose:
** 	N9 console log to CM4 support
** 	2) Changed function name:
** 	hemExtCmdN9ConsoleLog
** 	patch__RspByInBands
** 	patch__hex_dump
** 	wifi_normal_operation_entry
** 	hemExtAssertDumpEvent
** 	kalPrint
** 	3) Code change description brief:
** 	Added support for sending the N9 console logs to CM4 via event
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 04 07 2016 avishad.verma
** [BORA00005800] [MT7637E2][IOT][FW] build script for MT7687 and MT7697
** 	
** 	1) Purpose:
** 	Disable the MediaTek proprietary WiFi zero config feature
** 	2) Changed function name:
** 	Patch_config.h
** 	3) Code change description brief:
** 	Disable the MediaTek proprietary WiFi zero config feature
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 04 01 2016 avishad.verma
** [BORA00006349] Country region is getting changed after wireless mode if country region is 31~33
** 	
** 	1) Purpose:
** 	Fix the issue of unable to change country region dynamically
** 	2) Changed function name:
** 	patch__RT_CfgSetCountryRegion
** 	Iot_init_profile
** 	3) Code change description brief:
** 	BUG Fix unable to dynamically update country region
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
 *
 * 03 29 2016 navaneeth.krishnan
 * [BORA00006320] [MT7637E2][IOT][FW]Mic Error Attack Issue
 * 
 * 1) Purpose:Fix Mic Error attack issue
 * 
 * 2) Changed function name:hemCmdSetKey
 * 
 * 3) Code change description brief:Disabled the key print
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
**
** 03 21 2016 avishad.verma
** [BORA00006164] P2P support in N9
** 	
** 	1) Purpose:
** 	P2P support added
** 	2) Changed function name:
** 	setopmode
** 	txmHandleMacTxDone
** 	patch__RTMPWriteTxWI
** 	patch__MlmeHardTransmitMgmtRing
** 	patch__MiniportMMRequest
** 	patch__ScanNextChannel
** 	patch__STAHandleRxMgmtFrame
** 	patch__ComposeWPSIE
** 	patch__APPeerProbeReqAction
** 	hemCmdChannel
** 	uninitialize_ext_ie_val
** 	hemCmdSwitchMode
** 	hemExtCmdExtIE
** 	hemExtCmdExtIE
** 	hemExtCmdTxRawPkts
** 	3) Code change description brief:
** 	P2P Support added
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 03 17 2016 avishad.verma
** [BORA00006165] Bootup time optimization
** 	
** 	1) Purpose:
** 	Boot up Time optimization
** 	2) Changed function name:
** 	Wifi_normal_operation_entry, 
** 	setopmode
** 	3) Code change description brief:
** 	Disable prints on bootup,
** 	send probe request on all channels on bootup
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 03 10 2016 anthony.liu
** 1) Purpose: Enable Configure Free
** 	
** 	2) Changed function name: Configure Free
** 	
** 	3) Code change description brief: Enable Configure Free
** 	
** 	4) Unit Test Result: OK
** 	
** 	
** 	BORA00006139
**
** 03 10 2016 anthony.liu
** [BORA00006139] Configure Free - new feature
** 	
** 	1) Purpose: Add new feature: Configure Free - new feature (added but disabled)
** 	
** 	2) Changed function name: Configure Free
** 	
** 	3) Code change description brief: Added Configure Free
** 	
** 	4) Unit Test Result: OK
 *
 * 02 25 2016 raghav.monga
 * [BORA00006058] [MT7687][IOT][FW] TC#5.2.27 Fail at step 5,6,7,8
 * Fix for WMM test cases:
 * 1) STUT was checking its own phy mode instead of checking that of the AP
 * 2) Fix for VI/VO internal differentiation
**
** 02 25 2016 avishad.verma
** [BORA00006071] [MT7687][IOT][FW] TC#4.2.25-> Not able to ping from STA1 to STA2 and vice versa
** 	
** 	1) Purpose:
** 	TC#4.2.25-> Not able to ping from STA1 to STA2 and vice versa
** 	2) Changed function name:
** 	Patch__pseKickout
** 	patch__Announce_or_Forward_802_3_Packet
** 	3) Code change description brief:
** 	Fix the Memory leak issue while forwarding of QoS to Non QoS sta Packet in AP Mode
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
 *
 * 02 25 2016 navaneeth.krishnan
 * [BORA00006069] [MT7637E2][IOT][FW] AP Need to send Disassoc when data Packets are recieved from Non-Connected Clients
 * 
 * 1) Purpose:AP Sends Disassoc to data packets from non-connected clients	
 * 
 * 
 * 2) Changed function name:patch__STAHandleRxDataFrame
 * 
 * 
 * 3) Code change description brief:Sends Disassoc to data packets from non-connected clients
 * 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * 
 * <CRID START> <CRID END>
**
** 02 24 2016 avishad.verma
** [BORA00005950] [MT7637E2][IOT][FW]MBSS Feature
** 	
** 	1) Purpose:
** 	Enabled MBSS Feature,Increase ILM Size
** 	2) Changed function name:
** 	Patch_Config.h
** 	3) Code change description brief:
** 	Enabled MBSS Feature,Increase ILM Size
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 02 24 2016 raju.singh
** [BORA00006054] [MT7637E2][IoT][FW]APCLI fail to connect after OpMode switch from STA to APCLI
** 1) Purpose: ApCli fails to connect to remote-AP. Incorrect Ssid advertised in probe request frame.
** 		1- Fix build error introduced in changelist-38518
** 	2) Changed function name:
** 		- update_profile_ssid
** 	3) Code change description brief: Due to bug SsidLen is incremented
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
**
** 02 24 2016 raju.singh
** [BORA00006036] [MT7637E2][IoT][FW] Set/Get Tx power API support
** 1) Purpose: To support set/get Tx power.
** 	
** 	2) Changed function name:
** 	
** 	3) Code change description brief: Set/Get Tx power API implemented.
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
**
** 02 19 2016 raju.singh
** [BORA00006024] [MT7637E2][IoT][FW] AP Send De-Auth to all associated client when switch to other mode
** 1) Purpose: Fix opmode switch issue + add feature to send De-Auth to peer STA.
** 	
** 	2) Changed function name:
** 		- hemCmdSwitchMode
** 		- hemExtCmdAPStop
** 		- hemCmdSsid
** 		- ap_disconnect
** 	3) Code change description brief: De-Auth support added when opmode switch happen.
** 	
** 	4) Unit Test Result: PASS
**
** 02 05 2016 raju.singh
** [BORA00005852] [MT7637E2][IoT][FW]-[Homekit][N9] Lost response from N9 with switch opmode repeatedly
** 1) Purpose: Send PSE reset indication to CM4 in in-band command event
** 	
** 	2) Changed function name:
** 		- patch__RspByInBands
** 		- nicPseInit
** 	3) Code change description brief: Set g_bPseReset to indicate  PSE reset
** 	
** 	4) Unit Test Result: PASS
**
** 02 04 2016 raju.singh
** [BORA00005974] [MT7637E2][IoT][FW] Smartconnect feature in SoftAP mode
** 1) Purpose: SmartConnect support in softAP mode
** 	
** 	2) Changed function name:
** 		- FillSmartConnectScanChList
** 		- hemCmdStartScan
** 		- IsConnectionInProgress (new function)
** 		- iotFsmEventHifCmd
** 		- patch__ScanNextChannel
** 		- patch__MlmeScanReqSanity
** 		- patch__ApScanTimeoutAction
** 	3) Code change description brief: Feature added
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
**
** 02 02 2016 avishad.verma
** [BORA00005950] [MT7637E2][IOT][FW]MBSS Feature
** 	
** 	1) Purpose:
** 	MBSS Feature Support
** 	2) Changed function name:
** 	patch__bridgerxHandle
** 	patch__RTMPWriteTxWI_Data
** 	patch__STAHandleRxDataFrame
** 	patch__AsicBssInfoUpdate
** 	Iot_init_profile
** 	patch__MacTableDeleteEntry
** 	patch__CFG80211OS_NewSta
** 	patch__RTMPAllocAdapterBlock
** 	iot_mbss_deint
** 	iot_mbss_init
** 	patch__AsicStopBcnFrame
** 	patch__AsicSendBcnFrame
** 	patch__MlmeHandler
** 	apply_iot_patch_before_init
** 	hemCmdBssid
** 	setOpMode
** 	hemExtCmdAPStop
** 	hemExtCmdAPStart
** 	hemExtCmdGetMbssid
** 	nicPreTBTTInterruptForBcnUpdate
** 	txmComposeDesc
** 	3) Code change description brief:
** 	Added MBSS Feature
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 01 29 2016 raju.singh
** [BORA00005933] [MT7637][AP][APCLI test] Security is mixed mode, DUT APCLI some case connect will FAIL.
** 1) Purpose: Fix Mixed mode security issue in APCLI mode.
** 	
** 	2) Changed function name:
** 		- patch__PeerBeaconAndProbeRspSanity
** 		- patch__ApCliLinkDown
** 		- patch__ApCliLinkUp
** 	
** 	3) Code change description brief: In Mixed mode security, APCLI dynamically set AuthMode and EncrypType
** 	
** 	4) Unit Test Result: PASS
**
** 01 29 2016 avishad.verma
** [BORA00005738] [MT7637E2][IOT][FW] 5GHz support in Station Mode
** 	
** 	1) Purpose:
** 	5Ghz Changes
** 	2) Changed function name:
** 	hemcmdstartscan
** 	patch_scannextchannel
** 	patch_miniportmmRequest
** 	3) Code change description brief:
** 	Added POwer Capability ie and Suported channel IE for TGN test case 5.2.3
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
 *
 * 01 18 2016 navaneeth.krishnan
 * [BORA00005874] [MT7637E2][IOT][FW] APCLI Security Releated Changes
 * 
 * 
 * 
 * 1) Purpose:APCLI Security 
 * 
 * 
 * 
 * 2) Changed function name: patch__STAHandleRxDataFrame, patch__RTMPSendWirelessEvent,Patch__Indicate_EAPOL_Packet,patch__ApCliLinkUp ,hemCmdSetKey ,hemExtCmdSetWepKey
 * 
 * 
 * 3) Code change description brief:patch__STAHandleRxDataFrame - > Handle AP and APCLI encrypted packtes,patch__RTMPSendWirelessEvent - Add Remote AP IE and QOS bit to CM4 in the case of APCLI,Patch__Indicate_EAPOL_Packet - Handle APCLI EAPOL, patch__ApCliLinkUp - Add WEP key to Hardware for APCLI,hemCmdSetKey - To handle WPA APCLI key ,hemExtCmdSetWepKey-  Handling WEP key for APCLI
 * 
 * 
 * 
 * <CRID START> <CRID END>
**
** 01 13 2016 raju.singh
** [BORA00005865] [MT7637E2][IoT][FW] Tx Raw 802.11 packet
** 1) Purpose: To add support Tx of Raw 802.11 packets. 
** 	
** 	2) Changed function name:
** 	   - patch_connsys_rx_handle
** 	   - IoTWriteTxWI
** 	   - hemExtCmdTxRawPkts
** 	3) Code change description brief: 802.11 Tx Raw API support added
** 	
** 	4) Unit Test Result: PASS
**
** 01 12 2016 raju.singh
** [BORA00005852] [MT7637E2][IoT][FW]-[Homekit][N9] Lost response from N9 with switch opmode repeatedly
** 1) Purpose: To fix RTMPAllocAdapter fail issue.
** 	
** 	2) Changed function name:
** 	   - patch__RTMPSetProfileParameters
** 	   - setOpMode
** 	3) Code change description brief: Free Adapter if SetOpMode(STA) called after APStop in-band cmd.
** 	
** 	4) Unit Test Result: PASS
**
** 01 08 2016 avishad.verma
** [BORA00005738] [MT7637E2][IOT][FW] 5GHz support in Station Mode
** 	
** 	1) Purpose:
** 	5G Support
** 	2) Changed function name:
** 	BuildchannelList,peerbeaconandproberesponsesanity
** 	3) Code change description brief:
** 	Added Support for 5G
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 12 29 2015 raju.singh
** [BORA00005791] [MT7637E2][IoT][FW] On some RFBs throughput got stuck in AP mode
** 1) Purpose: PSE read queue length issue fixed
** 	
** 	2) Changed function name:
** 	   - patch__PseKickOutTx
** 	   - IoT_PseQueueLenRead (new function)
** 	
** 	3) Code change description brief: Counting P2 Q0/1 length from first fid till last fid.
** 	
** 	4) Unit Test Result: PASS
**
** 12 25 2015 anthony.liu
** [BORA00005743] [MT7687][E2][IOT] eFuse access API over inband
** 1) Purpose: Reduce code size of eFuse write.
** 
** 2) Changed function name: eFuse access function.
** 
** 3) Code change description brief: Rewrite byte check and enclose kalPrint with debug macros.
** 
** 4) Unit Test Result: Bin size reduced 1468 bytes. eFuse access function takes 1340 if debug message not specifically turned on.
**
** 12 22 2015 anthony.liu
** [BORA00005743] [MT7687][E2][IOT] eFuse access API over inband
** 	
** 	1) Purpose:
** 	a. Add conditional compile to eFuse access over inband command.
** 	b. fix free block counting code to check physical block availability in addition to mapping table.
** 	
** 	2) Changed function name:
** 	a. EfuseGetFreeBlock().
** 	
** 	3) Code change description brief:
** 	a. Add conditional compile flag MT7687_BORA00005743 to eFuse access over inband command.
** 	b. fix free block counting code to check physical block availability in addition to mapping table.
** 	
** 	4) Unit Test Result:
** 	R&D UT ok.
** 	
** 	<CRID START> <CRID END>
**
** 12 22 2015 avishad.verma
** [BORA00005562] [MT7637E2][IOT][FW] APCLI fixes for SQC
** 	
** 	1) Purpose:
** 	APCLI Interfacing Enabled
** 	2) Changed function name:
** 	Eanbled Macro for APCLI Interface changes
** 	3) Code change description brief:
** 	Enabled Macro for APCLI Interface Changes
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 12 18 2015 raju.singh
** [BORA00005728] [MT7637E2][IoT][FW]Issue :country region update dynamically support
** 1) Purpose: Build channel list after setting country region by in-band command.
** 	
** 	2) Changed function name:
** 	   - hemExtCmdCountryRegion
** 	3) Code change description brief: set IoTSysCfg->bg_band_entry_num = 0;
** 	
** 	4) Unit Test Result: PASS
**
** 12 15 2015 avishad.verma
** [BORA00005701] [MT7637E2][IOT][FW] WMM suport
** 	
** 	1) Purpose:
** 	Wmm support
** 	2) Changed function name:
** 	Patch__psekickoutTx
** 	3) Code change description brief:
** 	check the AC0/AC1 before queuing new packet
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
 *
 * 12 03 2015 navaneeth.krishnan
 * [BORA00005606] [MT7637E2][IOT][FW] Dual Interface Related Changes
 * 
 * 
 * 1) Purpose:Dual interface related changes are kept inside a different flag
 * 
 * 
 * 2) Changed function name: convert_to_hif_packet, patch__RspByInBands, patch__wireless_send_event,     patch__RTMPSendWirelessEvent, convert_from_hif_packet, patch_connsys_rx_handle, hemCmdSsid,hemCmdBssid
 * 
 * 
 * 3) Code change description brief: Interface related changes are kept under a seperate Flag
 * 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 12 03 2015 navaneeth.krishnan
 * [BORA00005601] [MT7637E2][IOT][FW] STA sending Qos frame to non-WMM AP
 * 
 * 
 * 1) Purpose:Issue -STA Sending Qos Frames to non-WMM AP
 * 
 * 
 * 2) Changed function name: LinkDown - WMM Client flag reset
 * 
 * 
 * 3) Code change description brief: LinkDown - WMM Client flag reset
 * 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * 
 * <CRID START> <CRID END>
**
** 12 01 2015 raju.singh
** [BORA00005537] [MT7637E2][IoT][FW]Throughput too low in frame forwarding
** 1) Purpose: To make N9 backward compitable to older CM4 after Rx-Zero copy forwarding support enabled.
** 	Issues fixed:
** 	1- Beaconing not happening
** 	2- Compatability issue in STA mode.
** 	3- Low throughput issue in STA mode.
** 
** 	2) Changed function name:
** 	   - Iot_init_profile
** 	   - bssEnqBcnPktFromCmd
** 	   - txmAllocPkt
** 	   - txmGetPktBuffer
** 	   - txmGetPktBufferInPse
** 	   - txmComposeDesc	
** 	3) Code change description brief: Rx-zero copy forwarding will be enabled based on NVRAM configuration.
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
 *
 * 11 27 2015 navaneeth.krishnan
 * [BORA00005562] [MT7637E2][IOT][FW] APCLI fixes for SQC
 * 
 * 
 * 1) Purpose: Enable Checks are added for APCLI
 * 
 * 
 * 
 * 2) Changed function name:1.convert_to_hif_packet 2.patch__RspByInBands 3.patch__wireless_send_event 4.patch__RTMPSendWirelessEvent 5.patch_connsys_rx_handle 6.Iot_init_profile 
 * 
 * 
 * 
 * 3) Code change description brief: Added checks in All the functions for APCLI mode
 * 
 * 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 11 27 2015 navaneeth.krishnan
 * [BORA00005562] [MT7637E2][IOT][FW] APCLI fixes for SQC
 * 
 * 
 * 1) Purpose:APLCI Code updates
 * 
 * 
 * 2) Changed function name:1.hemCmdBssid,2.convert_from_hif_packet
 * 
 * 
 * 3) Code change description brief:1.hemCmdBssid - Define added ,2.convert_from_hif_packet - Typo error for APLCI
 * 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 11 26 2015 navaneeth.krishnan
 * [BORA00005417] [MT7637E2][IOT][FW] Find eFuse presence
 * 
 * 
 * 1) Purpose: Check the presence of Efuse by reading the Efuse-ChipID
 * 
 * 
 * 2) Changed function name:wifi_normal_operation_entry()
 * 
 * 
 * 3) Code change description brief:wifi_normal_operation_entry - Efuse Chip-ID is read out and if this matches with 7637,then Efuse is considered as present
 * 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
 * 
 * <CRID START> <CRID END>
 *
 * 11 26 2015 raghav.monga
 * [BORA00005562] [MT7637E2][IOT][FW] APCLI fixes for SQC
 * Add APCLI Fixes for SQC:
 * 1) Rate adaptation
 * 2) Wrong mac address in ADDBA
 * 3) No Beacon when running throughput
 * 4) Assert when removing repeater entry.
**
** 11 24 2015 raju.singh
** [BORA00005537] [MT7637E2][IoT][FW]Throughput too low in frame forwarding
** 1) Purpose: a- Beaconing issue in 40MHz due to Rx-zero copy changes.
** 	    b- Rx-Zero copy for intra-BSS forwarding disable as of noe
** 	
** 	2) Changed function name:
** 	   - OBSSScanTimeout
** 	3) Code change description brief: Configuring DMA_ADCR1 to enable Rx-Zero copy in 40MHz
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
**
** 11 24 2015 raju.singh
** [BORA00005537] [MT7637E2][IoT][FW]Throughput too low in frame forwarding
** 	
** 	1) Purpose: Implement Rx-Zero copy in intra-bass forwarding
** 	
** 	2) Changed function name:
** 	   - patch__PseKickOutTx
** 	   - patch__RTMPWriteTxWI
** 	   - patch__RTMPWriteTxWI_Data
** 	   - patch__RTMPWriteTxWI_Cache
** 	   - patch__MlmeHardTransmitMgmtRing
** 	   - patch__MiniportMMRequest		
** 	   - patch__APRxFowardHandle
** 	   - patch__Announce_or_Forward_802_3_Packet
** 	   - setOpMode
** 	3) Code change description brief: RxD configured to 12DW so that it can accomodated 8DW TxD and 4DW padding. By this configuration Rx-Zero copy is achieved in intra-bss forwarding.
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
 *
 * 11 20 2015 hasanali.stationwala
 * [BORA00005521] [MT7637E2][IoT][FW]Coonection to an AP fails in 40Mhz if previously connected AP was on some other channel
 * Configure into 20Mhz mode before going for join.
**
** 11 19 2015 raju.singh
** [BORA00005413] [MT7637E2][IoT][FW]Mic failure event for counter measure
** 1) Purpose: Mic failure event for counter measure
** 	
** 	2) Changed function name:
** 	   - patch__RTMPCheckRxError
** 	   - patch__WpaSendMicFailureToWpaSupplicant
** 	   - IoT_APRxErrorHandle
** 	   - WpaSendMicFailureEvent
** 	   - rtmp_chk_tkip_mic
** 	3) Code change description brief: Added support for MIC error event as per defined format
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
**
** 11 19 2015 raju.singh
** [BORA00005515] [MT7637E2][IoT][FW] TGn-5.2.30 and TGn-5.2.33: Shortslot time used in CCK mode instead of Longslot time.
** 1) Purpose: Fix for TGn-5.2.30 and TGn-5.2.33
** 	
** 	2) Changed function name:
** 	   - patch__OffloadSlotTimeSet
**            - patch__AsicSetSlotTime
** 	3) Code change description brief: Set longslot time when associated AP advertising longslot time in its beacon
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
**
** 11 19 2015 raju.singh
** [BORA00005476] [MT7637E2][IoT][FW] Set EDCA parameter for AP in 11b only mode
** 1) Purpose: Set 11b only mode EDCA parameter
** 	
** 	2) Changed function name:
** 	   - patch__RTMPSetProfileParameters
**            - hemExtCmdAPStart
** 	3) Code change description brief: In 11b only mode- TXOP set for AC3 and AC4
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
 *
 * 11 05 2015 raghav.monga
 * [BORA00005420] [MT7637E2][IOT][FW]AP set wpa2psk aes, phone to connect, tera term ping phone 10000,ping loss >20%
 * Increase Maximum packets that can be sent by APUT after receiving NULL packet from 2 to 8.
**
** 11 04 2015 avishad.verma
** [BORA00005408] [MT7637E2][IOT][FW] Ping loss More than 20% in Powersave Mode with Trendnet AP WPA2PSK AES
** 1) Purpose:
** 	With Trendnet ap 10000 bytes ping fail in Powersave
** 	2) Changed function name:
** 	patch__AsicPmStateCtrl
** 	apply_iot_patch_before_init
** 	3) Code change description brief:
** 	Rx filter store and restore modified in Powersave
** 	4) Unit Test Result:
** 	PASS
** 	
** 	<CRID START> <CRID END>
**
** 11 04 2015 raju.singh
** [BORA00005406] [MT7637E2][IoT][FW]Phone: Sony Xperia Z2, MT7687 set wpa2psk aes, phone to connect, MT7687 AP ping out small then big size to Z2, ping out can not finish and N9 crashed
** 1) Purpose: A- Fix N9 crash
** 	    B- Scan log disable
** 	
** 	2) Changed function name:
** 	   - RTMPDeFragmentDataFrame
** 	3) Code change description brief: pRxBlk->RxWi set properly now.
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
 *
 * 11 03 2015 navaneeth.krishnan
 * [BORA00005109] [MT7637E2][IOT][FW] IOT WPS RELATED CHANGES
 * 
 * 
 * 
 * 1) Purpose:WPS Functionality for both AP and STA mode
 * 
 * 
 * 
 * 2) Changed function name:BssTableSsidSort(), ComposeWPSIE(), peerProbeReqSanity(), Patch__MiniportMMRequest(), patch__ScanNextChannel(), apply_iot_patch_before_init(), hemCmdSsid(), setOpMode(), emExtCmdAPStop(), arHifCmdExtTableIoT
 * 
 * 
 * 
 * 3) Code change description brief: Patches and changes are made for the following functions to transmit IE values in different management frames BssTableSsidSort(),ComposeWPSIE(),PeerProbeReqSanity(),Patch__MiniportMMRequest () .Configured IE values are cleared in the following functions setOpMode(),hemExtCmdAPStop().Changes are done in the follwowing function to enable WPS hemCmdSsid().arHifCmdExtTableIoT -> To handle different IE values sent from CM4,this function is used
 * 
 * 
 * 
 * 4) Unit Test Result:PASS

 *
 * 11 02 2015 navaneeth.krishnan
 * [BORA00005377] [MT7637E2][IOT][FW] PS Command from CM4
 * 
 * 
 * 1) Purpose: Handling of PS Mode command from CM4
 * 
 * 
 * 2) Changed function name:Iot_init_profile(),HIF_IOT_CMD_T
 * 
 * 
 * 3) Code change description brief:Iot_init_profile() - Default Power mode is not taken from Nvram.Default value is kept as zero.
 * HIF_IOT_CMD_T - Extra command is added is handle PS mode command from CM4
 * 
 * 
 * 4) Unit Test Result: PASS
 * 
 * 
**
** 10 30 2015 raju.singh
** [BORA00005376] [MT7637E2][IoT][FW] Remove Extended capability info IE from Probe and Assoc req/resp frame when STA operating in 11bg mode
** 1) Purpose: 11bg mode remove ExtCapIe from Probe and assoc req/resp frame.
** 	
** 	2) Changed function name:
** 	   - patch__MakeOutgoingFrame
** 	
** 	3) Code change description brief: In 11bg mode prevent STA from filling ExtCapIe to the frame.
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
**
** 10 30 2015 raju.singh
** [BORA00005370] [MT7637E2][IoT][FW] AP booting in 11bg mode with 40MHz BW setting
** 1) Purpose: A- To prevent AP from ASSERT even when booting in 11bg mode
** 	    B- Stop sending Coex2040 scan event to CM4.
** 
** 	2) Changed function name:
** 	   - Iot_init_profile
** 	   - patch__ScanNextChannel
** 	
** 	3) Code change description brief: Reset BW to 20MHz when AP boots up in 11bg mode.
** 	
** 	4) Unit Test Result: PASS
** 	
** 	
** 	<CRID START> <CRID END>
 *
 * 10 29 2015 navaneeth.krishnan
 * [BORA00005355] [MT7637E2][IOT][FW] FW Support for Get Rssi Inband Command
 * 
 * 
 * 1) Purpose: Added Handling of get RSSI command handling under define(bora -CR ID)
 * 
 * 
 * 2) Changed function name: patch__STAHandleRxMgmtFrame,patch__STAHandleRxMgmtFrame
 * 
 * 
 * 3) Code change description brief: In both functions the get RSSI command handling was put under define(bora -CR ID)
 * 
 * 
 * 4) Unit Test Result:PASS
 * 
 * 
**
** 10 23 2015 avishad.verma
** [BORA00004984] [MT7637E2][IOT][FW]Power Save Implementation
** 	
** 	1) Purpose:
** 	Wake up CM4 on Link Down
** 	2) Changed function name:
** 	patch__RTMPSendWirelessEvent
** 	3) Code change description brief:
** 	Wake up CM4  if station got disconnected
** 	
** 	4) Unit Test Result:
** 	PASS
**
** 10 21 2015 raghav.monga
** [BORA00005322] [MT7637E2][IOT][FW] TGn 4.2.10 APUT sending continuous MC data even if TIB =0
** 1) Purpose:
** 	to send MCBC packets that arrive after pre-tbtt after the next beacon
** 	2) Changed function name:
** 	APSendPacket
** 	3) Code change description brief:
** 	to send MCBC packets that arrive after pre-tbtt after the next beacon
** 	4) Unit Test Result:PASS
**
** 10 21 2015 avishad.verma
** [BORA00005319] [MT7637E2][IOT][FW]Increase retry limit for Management Frames
** 	1) Purpose:
** 	Retry limit increased for Management frames
** 	2) Changed function name:
** 	patch__RTMPWriteTxWI
** 	3) Code change description brief:
** 	Retry limit increased for Management frames from 1 to 15
** 	4) Unit Test Result:
** 	PASS
**
** 10 20 2015 avishad.verma
** [BORA00005308] [MT7637E2][IOT][FW] increase the periodic exec time from 100ms to 1 second
** 	1) Purpose:
** 	Inreased to the periodic timer to reduce no of wake ups for LP
** 	2) Changed function name:
** 	patch__MlmePeriodicExec
** 	Start_periodic_exec
** 	apply_iot_patch_after_init
** 	3) Code change description brief:
** 	chnaged the periodic exec timer from 100ms to 1 second
** 	4) Unit Test Result:
** 	PASS
**
** 10 20 2015 raju.singh
** [BORA00005306] [MT7637E2][IOT][FW] RIFS support in AP mode
** 1) Purpose: Enable RIFS mode in AP
** 	
** 	2) Changed function name:
** 	   - hemExtCmdAPStart
** 	   - hemExtCmdAPStop
** 
** 	3) Code change description brief: RIFS mode enabled
** 	
** 	4) Unit Test Result: PASS
**
** 10 19 2015 raghav.monga
** [BORA00005300] [MT7637E2][IOT][FW] Ping not working with RT2870 station in AP mode
** 1) Purpose:
** 	Only QoS single frames should be considered as AMPDU if BA session is established
** 
** 	2) Changed function name:
** 	patch__STAHandleRxDataFrame
** 	3) Code change description brief:
** 	move the code to mark a frame as AMPDU to a check that only when frame is a QoS frame, mark it as AMPDU
** 	4) Unit Test Result: PASS
**
** 10 19 2015 raju.singh
** [BORA00005292] [MT7637E2][IOT][FW] Tx blocks while running TGn-4.2.8 testcase.
** 1) Purpose: TGn-4.2.8
** 	
** 	2) Changed function name:
** 	   - hemExtCmdAPStop
** 	
** 	3) Code change description brief: Increase number of buffer of size 1600 bytes
** 	
** 	4) Unit Test Result: PASS
**
** 10 15 2015 raju.singh
** [BORA00005270] [MT7637E2][IOT][FW] AP(20MHz, channel 6) asserts when receiving coex-2040 mgmt frame from associated STA
** 1) Purpose: AP(20MHz, channel 6) asserts when receiving coex-2040 mgmt frame from associated STA
** 	
** 	2) Changed function name:
** 	   - patch__ap_phy_rrm_init_byRf
**            - patch__PeerPublicAction
**            - IoT_APStartUp
** 	3) Code change description brief: Fallback not required in case of 20MHz
** 	
** 	4) Unit Test Result: PASS
**
** 10 14 2015 avishad.verma
** [BORA00004781] [MT7637E2][IOT][FW]Invalid in-band command sent by CM4
** 	1) Purpose:
** 	Issue while sending 9 stations list to CM4
** 	2) Changed function name:
** 	patch__RspByInBands
** 	iotFsmEventHifCmd
** 	3) Code change description brief:
** 	Changed the buffer size of response to accomondate 9 stations
** 	
** 	4) Unit Test Result:
** 	PASS
**
** 10 13 2015 raghav.monga
** [BORA00005253] [MT7637E2][IOT][FW]AP set open/wpa2psk aes security, run iperf 60s (intel -c/ap console -s), sometimes short throughput value show 0Mbits sta ping ap time out
** 1) Purpose:
** 	To solve interoperablity issue is Intel STA
** 	2) Changed function name:
** 		txm_init()
** 	
** 	3) Code change description brief:
** 		Fix hardware issue of Sequence number miss.
** 	4) Unit Test Result: PASS
 *
 * 10 13 2015 hasanali.stationwala
 * [BORA00005255] [MT7637E2][IoT][FW]Low throughput in case of data forwarding
 * Fix for low throughput in case of data forwarding.
**
** 10 13 2015 raju.singh
** [BORA00005251] [MT7637E2][IOT][FW] Specific channel scan in STA mode
** 1) Purpose: Specific channel scan support in STA mode.
** 	
** 	2) Changed function name:
** 	   - patch__SendBeaconToHost
** 	   - hemCmdStartScan
** 	3) Code change description brief:
** 	   - Overlapping channel beacon not sent to Host
** 	   - Trigger NORMAL scan based on scan parameter(if valid).
** 	
** 	4) Unit Test Result: PASS
**
** 10 09 2015 raju.singh
** [BORA00005235] [MT7637E2][IOT][FW]Stop sending beacon to host during Coex2040 scan by STA.
** 1) Purpose: To stop sending Beacon/Probe response to host during Coex2040 scan. 
** 	
** 	2) Changed function name:
** 	   - patch__SendBeaconToHost
** 	
** 	3) Code change description brief: In patch__SendBeaconToHost if Coex2040 scan in progress then return without sending beacon/probe response frame to host.
** 	
** 	4) Unit Test Result: PASS
**
** 10 09 2015 raju.singh
** [BORA00005234] [MT7637E2][IOT][FW]In 11bg mode STA sending HT capability in Assoc request
** 1) Purpose: Stop 11bg STA from adding HT_IE in assoc request
** 	
** 	2) Changed function name:
** 	   - patch__RTMPSetPhyMode
**            - patch__ScanNextChannel
**            - patch__InitChannelRelatedValue
**            - patch__cmm_assoc_reassoc_action
**            - patch__MlmeAssocReqAction
** 	
** 	3) Code change description brief: conversion from 
** wmode to cfgmode at some places
** 	
** 	4) Unit Test Result: PASS
 *
 * 10 08 2015 anil.vankayala
 * [BORA00005211] [MT7637E2][IOT][FW] Support for new inband commands
 * 
 * 1) Purpose:
 *     Added support for inband-commands
 * 
 * 2) Changed function name:
 *     
 *  - apply_iot_patch_before_init
 *     
 *  - orig__MakeExtCapIE
 *     
 *  - patch__MakeExtCapIE
 *    
 *  - uninitialize_homekit_ie
 *    
 *  - setOpMode
 *     
 * - hemExtCmdAPStop
 *     
 * - hemExtCmdHomekitIE
 *    
 *  - hemExtCmdAirplayEN
 *      
 * - hemExtCmdWPSIE
 * 
 * 3) Code change description brief:
 *        
 * - support to update homekit ie in beacon frame. 
 *        
 * - support to enable/disable apple airplay service. 
 * 
 * 4) Unit Test Result: PASS
**
** 10 07 2015 raju.singh
** [BORA00005224] [MT7637E2][IOT][FW] incorrect channel in DS(Direct Sequence Parameter Set) IE
** 1) Purpose: Incorrect channel in DS parameter
** 	
** 	2) Changed function name:
** 	   - patch__RTMPSetPhyMode
** 	3) Code change description brief: wdev->Channel updated with default channel
** 	
** 	4) Unit Test Result: PASS
 *
 * 10 07 2015 navaneeth.krishnan
 * [BORA00005223] [MT7637E2][IOT][FW] LDPC Related Changes
 * . 1) Purpose: Enable LDPC
 *   2) Changed function name:
 *       - patch__RTMPCheckHt
 *       - patch__RTMPSetProfileParameters
 *       - patch__mt7636_chipCap_init
 *    3) Code change description: LDPC enable and set LDPC flag based on peer capability.
 *    4) Unit test result: PASS
**
** 10 07 2015 raju.singh
** [BORA00005151] [MT7637E2][IOT][FW] BSS Co-exist2040 support in AP mode
** 1) Purpose: BSS Coex2040 support in AP mode.
** 	
** 	2) Changed function name:
** 	   - setOpMode
** 	   - patch__RTMPFreeAdapter
** 	   - IoT_APOverlappingBSSScan
** 	   - OBSSScanTimeout
** 	   - IoT_APStartUp
** 	   - patch__ap_func_init
** 	   - patch__rtmp_ap_init
** 	   - patch__PeerPublicAction
** 	   - patch__SendNotifyBWActionFrame
** 
** 	3) Code change description brief: BSS Coex2040 support added.
** 	
** 	4) Unit Test Result: PASS
**
** 10 07 2015 raju.singh
** [BORA00005166] [MT7637E2][IOT][FW] TGn TC-4.2.9 Re-association/Bridging Tests Fail for APUT
** 1) Purpose: Re-assoc issue fix.
** 	
** 	2) Changed function name:
** 	   - patch__CFG80211OS_NewSta
** 	
** 	3) Code change description brief: Re-assoc IE not handled 
** in CM4, so new Re-assoc event(0x8C0A) defined and sent to CM4 with
**  Re-assoc IE.
** 	
** 	4) Unit Test Result: PASS
**
** 10 07 2015 raju.singh
** [BORA00005186] [MT7637E2][IOT][FW] TGn TC-4.2.31, APUT not using Non-member Protection
** 1) Purpose: TGn TC-4.2.31, APUT not using Non-member Protection.
** 	
** 	2) Changed function name:
** 	    - patch__NICReadEEPROMParameters
** 	
** 	3) Code change description brief: Set Rx antenna path = 1 
** 	
** 	4) Unit Test Result: PASS
**
** 10 06 2015 raju.singh
** [BORA00005188] [MT7637E2][IOT][FW] Packet allocation fails for Probe response in mix-mode security.
** 1) Purpose: PSE pkt allocation failure due to single probe response Tx 3 times in AP mode. 
** 	
** 	2) Changed function name: 
** 		- patch__netlib_packet_buffer_init
** 		- patch__MiniportMMRequest
** 		- RTMPAllocateNdisPacket
** 		- APPeerProbeReqAction				
** 	
** 	3) Code change description brief: Probe response Tx once only.
** 	
** 	4) Unit Test Result: PASS
**
** 10 06 2015 raghav.monga
** [BORA00005209] [MT7637E2][IOT][FW] TGn 4.2.47 failing. APUT sends more than 1 packet for PS POLL
** 1) Purpose: TGn Test case 4.2.47 failing due to AP sending more than one 
** packet for ps poll
** 	
** 	2) Changed function name: patch__MtPsIndicate
** 	
** 	3) Code change description brief:
** 	Flush all the packets in LMAC queue when STA moves to powersave
** 	4) Unit Test Result:
** 	TGn Testcase 4.2.47 pass
**
** 10 05 2015 hasanali.stationwala
** [BORA00005202] [MT7637E2][IoT][FW]Low UDP throughput incase os PSMODE 2
** 1) Purpose:Low Throughput in case of Power save
** 	
** 	2) Changed function name:
** 	
** 	3) Code change description brief:
** 	
** 	4) Unit Test Result:
**
** 10 02 2015 hasanali.stationwala
** [BORA00005191] [MT7637E2][IoT][FW]SQC Fast PSP Issue
** 1) Purpose:
** 	STA Sending PS-Poll frames in FAST PSP
** 	STA send corrupt PS Poll Frames	
** 2) Changed function name:
** 	
** 	3) Code change description brief:
** 	
** 	4) Unit Test Result:
**
** 09 30 2015 hasanali.stationwala
** [BORA00004905] [MT7687]Implement CM4-N9 inband commands interface
** 1) Purpose: Changed handling of get channel query
**
** 09 30 2015 hasanali.stationwala
** [BORA00004905] [MT7687]Implement CM4-N9 inband commands interface
** 1) Purpose: Re-Assigned values to Patch Macros which were previously marked as TEMP
**
** 09 28 2015 raju.singh
** [BORA00005149] [MT7637E2][IOT][FW] Mix mode security support in AP mode
** .
**
** 09 25 2015 avishad.verma
** [BORA00004984] [MT7637E2][IOT][FW]Power Save Implementation
** 	SMCTL Update.
** 	RIFS Support in STA.
** 	Disable CM4 Wake up on Data Pkt.
**
** 09 25 2015 raghav.monga
** [BORA00004891] [MT7637E2][IOT][FW] ERP and 11n protection not functional
** Periodically check for Non-Member protection in AP mode.
**
** 09 25 2015 raghav.monga
** [BORA00005140] [MT7637E2][IOT][FW] TCP Rx very low with D-link 825
** Treat Legacy frames of peer as AMPDU if BA sesssion is created so that
** reordering window is updated properly.
**
** 09 24 2015 raghav.monga
** [BORA00005133] [MT7637E2][IOT][FW] HT Rate Tx stops working after 70dB attenuation
** Enable Preamble Detection for primary channel CCA.
**
** 09 21 2015 raju.singh
** [BORA00005085] [MT7637E2][IOT][FW] BSS Co-exist2040 issue in STA mode
** BSS co-exist2040 support added.
**
** 09 19 2015 hasanali.stationwala
** [BORA00005078] [MT7637E2][IoT][FW]Add null packet handling in ncase of TX data
** Removed compilation error and added size.
**
** 09 19 2015 hasanali.stationwala
** [BORA00005078] [MT7637E2][IoT][FW]Add null packet handling in ncase of TX data
** Fix for:
** 1. PSE Dual freeing in AMSDU.
** 2. Delayed beacon(wrong header length in BAR TXD).
** 3. WEP Key index other than 0.
** 4. memory leak in case of OpMode switch.
**
** 09 18 2015 hasanali.stationwala
** [BORA00005078] [MT7637E2][IoT][FW]Add null packet handling in ncase of TX data
** Temporary macro for Drop duplicate not getting set issue and issue with AP PS packet buffering /
** / All net_pkt_blks gets enqueued in PS queue, this leads to Zero Rx pkt thereby a 100% drop in Rx  /.
**
** 09 16 2015 hasanali.stationwala
** [BORA00005078] [MT7637E2][IoT][FW]Add null packet handling in ncase of TX data
** 	Added Size in patch config file.
 *
 * 09 16 2015 raghav.monga
 * [BORA00005075] [MT7637E2][IOT][FW] 4-way handshake failing when STAUT connect to non-QoS AP
 * Following issues fixed:
 * 1)BORA00005075: 4-way handshake failing when STAUT connect to non-QoS AP.
 * 2) BORA00005080: Same PSE FID gets freed up twice in case of broadcast packets*
 * 3) BORA00005078: Add null packet handling in ncase of TX data.
 *
 * 09 11 2015 hasanali.stationwala
 * [BORA00005060] [MT7637E2][IoT][FW]Implement Zero copy architecture in TX Path
 * Tx Zero Copy.
 * fix for Plain text Broadcast data in case of AP(Raghav)
**
** 09 10 2015 avishad.verma
** [BORA00004984] [MT7637E2][IOT][FW]Power Save Implementation
** 	resolve powersave issue due to Frag/Defrag
 *
 * 09 10 2015 raghav.monga
 * [BORA00005053] [MT7637E2][IOT][FW] Add PS POLL handling in AP mode
 * Add PsPoll frame handling support in AP mode.
 *
 * 09 09 2015 raghav.monga
 * [BORA00005048] [MT7637E2][IOT][FW] Add WMM support in AP mode.
 * Fix issue of QueIDx zero going to PSE in all conditions
 * Fix merge issue of Auth and Assoc retires..
 *
 * 09 08 2015 raghav.monga
 * [BORA00005044] [MT7637E2][IOT][FW] Add 5 retries for Auth and Assoc frames.
 * Add 5 retries for Auth and Assoc frames to reduce the connection times in case of retries.
**
** 09 07 2015 avishad.verma
** [BORA00004984] [MT7637E2][IOT][FW]Power Save Implementation
** 	.Device PowerSave Implementation
**
** 09 07 2015 raju.singh
** [BORA00005028] [MT7637E2][IOT][FW] De-fragmentation support
** . De-fragmentation fails when TKIP encryption enabled.
**
** 09 04 2015 raju.singh
** [BORA00005038] [MT7637E2][IOT][FW] Basic rate not indicated in Beacon
** .
 *
 * 09 04 2015 hasanali.stationwala
 * [BORA00005036] [MT7637E2][IoT][FW]Data packets are transmitted in 20Mhz in 40Mhz network in case of Auto rate
 * Enabled patch.
**
** 09 04 2015 raju.singh
** [BORA00005028] [MT7637E2][IOT][FW] De-fragmentation support
** .
 *
 * 09 03 2015 raghav.monga
 * [BORA00005022] [MT7637E2][IOT][FW] STA disconnect when run high bw udp
 * 1) Do WCID mapping of BAR frame using addr2.
 * 2) Reduce the g_MaxReorderMpduNum to 15 to sync with buffer availability.
**
** 08 26 2015 avishad.verma
** [BORA00004984] [MT7637E2][IOT][FW]Power Save Implementation
** 	N9 Powersave Implemenation
**
** 08 25 2015 avishad.verma
** [BORA00004930] [MT7637E2][IOT][FW]Efuse read on bootup
** 	Efuse binary added.
 *
 * 08 25 2015 hasanali.stationwala
 * [BORA00004891] [MT7637E2][IOT][FW] ERP and 11n protection not functional
 * Protection enabled.
 *
 * 08 25 2015 raghav.monga
 * [BORA00004970] [MT7637E2][IOT][FW] Add rate adaptation code in N9
 * .
**
** 08 24 2015 raju.singh
** [BORA00004972] [MT7637E2][IOT][FW] Garbage value of Partial bitmap in Beacon.
** .
**
** 08 24 2015 raju.singh
** [BORA00004891] [MT7637E2][IOT][FW] ERP and 11n protection not functional
** .Protection code disabled.
 *
 * 08 23 2015 hasanali.stationwala
 * [BORA00004905] [MT7687]Implement CM4-N9 inband commands interface
 * Fix for channel value to host.
**
** 08 23 2015 raju.singh
** [BORA00004965] [MT7637E2][IOT][FW] IoT_STA assert while connecting to AP operating in 20MHz channel 4
** 	.
**
** 08 22 2015 raju.singh
** [BORA00004961] [MT7637E2][IOT][FW] 20MHz STA not connecting to 40MHz AP.
** .
 *
 * 08 21 2015 raghav.monga
 * [BORA00004927] [MT7637E2][IOT][FW] Zero Mac Address sent to Supplicant when connected in STA mode
 * Use address in MlmeAux instead of MAC TABLE  while sending assoc event as address in MAC TABLE is not filled yet.
**
** 08 20 2015 raju.singh
** [BORA00004953] [MT7637E2][IOT][FW] STA fails to connect in high traffic network
** .
**
** 08 20 2015 avishad.verma
** [BORA00004905] [MT7687]Implement CM4-N9 inband commands interface
** 	.
**
** 08 19 2015 raju.singh
** [BORA00004891] [MT7637E2][IOT][FW] ERP and 11n protection not functional
** .
**
** 08 19 2015 raju.singh
** [BORA00004843] [MT7637E2][IOT][FW] Implementation of partial scan in AP mode
** .
**
** 08 19 2015 avishad.verma
** [BORA00004913] [MT7637E2][IOT][FW]AP Mode Only support
** 	.
**
** 08 19 2015 avishad.verma
** [BORA00004905] [MT7687]Implement CM4-N9 inband commands interface
** 	In Band commands support added
 *
 * 08 18 2015 hasanali.stationwala
 * [BORA00004905] [MT7687]Implement CM4-N9 inband commands interface
 * In band commands implemented.
**
** 08 18 2015 raju.singh
** [BORA00004903] [MT7637E2][IOT][FW] Read profile parameters from NVRAM
** .
**
** 08 17 2015 raju.singh
** [BORA00004924] [MT7637E2][IOT][FW] Wow feature: ARP offloading, Bitmap pattern and Rekey offloading.
** 	. ARP and rekey offload code merge back after E1 sync.
 *
 * 08 13 2015 raghav.monga
 * [BORA00004889] [MT7637E2][IOT][FW] Open Mode Wep security not working in STA mode.
 * Enabled open mode WEP security in AP mode. AP was not installing pairwise key when STA connected to AP.
**
** 08 11 2015 avishad.verma
** [BORA00004898] [MT7637E2][IOT][FW] extra 12 bytes in Tx packets
** 	.
 *
 * 08 11 2015 raghav.monga
 * [BORA00004889] [MT7637E2][IOT][FW] Wep security not working in STA mode.
 * Save the WEP key when set by wpa_supplicant and install it when connection established.
**
** 08 11 2015 avishad.verma
** [BORA00004894] [MT7637E2][IOT][FW]Disable events send through hemExtCmdGenResultAndSend2Hif
** [MT7637E2][IOT][FW]Disable events send through hemExtCmdGenResultAndSend2Hif
** 	.
**
** 08 11 2015 raju.singh
** [BORA00004770] [MT7637E2][IOT][FW]AP sending ADDBA Req in 11b mode also
** . Patch function optimization.
**
** 08 11 2015 raju.singh
** [BORA00004844] [MT7637E2][IOT][FW] AP cannot connects STA when configured in 11bg mode.
** . Bug in wmode_2_cfgmode() resolved.
 *
 * 08 10 2015 hasanali.stationwala
 * [BORA00004872] [MT7687]AP does not announce Secondary channel offset in HT info
 * Changed BORA ID in macro.
**
** 08 10 2015 avishad.verma
** [BORA00004883] [MT7637E2][IOT][FW]set op mode is not working for Promiscuous mode
** 	Fix the promiscuous mode issue.
**
** 08 10 2015 avishad.verma
** [BORA00004781] [MT7637E2][IOT][FW]Invalid in-band command sent by CM4
** 	Hash entry delete after disconnect.
** 	Handling disconnect instead of queueing
**
** 08 10 2015 avishad.verma
** [BORA00004881] Send Broadcast packets on TBTT instead of Pre TBTT
** 	Enabled TBTT interrupt in AP mode and Sending the Broadcast packets on TBTT.
**
** 08 10 2015 raju.singh
** [BORA00004844] [MT7637E2][IOT][FW] AP cannot connects STA when configured in 11bg mode.
** .
 *
 * 08 10 2015 hasanali.stationwala
 * [BORA00004841] [MT7687]Implement Zero copy Architecture on Rx Path
 * Enabled beacon secondary channel annoucement fix.
 * Disabbled Zero copy
**
** 08 10 2015 raju.singh
** [BORA00004770] [MT7637E2][IOT][FW]AP sending ADDBA Req in 11b mode also
** .
**
** 08 10 2015 raju.singh
** [BORA00004765] [MT7637E2][IOT][FW] AP malfunctions after OpMode change
** .
**
** 08 10 2015 raju.singh
** [BORA00004764] [MT7637E2][IOT][FW] STA disconnects when scan initiated in 40MHz Connection
** . STA does not connect in 40MHz with SCA.
**
** 08 05 2015 raghav.monga
** [BORA00004863] [MT7637E2][IOT][FW] Add MAC address field in LINKUP event sent to CM4
** 	add target AP��s BSSID to IW_STA_LINKUP_EVENT_FLAG & IW_STA_LINKUP_EVENT_FLAG event
** 	RAM Size increased by 60b : Total= 0x2b4e4
**
** 08 05 2015 raghav.monga
** [BORA00004859] [MT7637E2][IOT][FW] Profile download fail due to wrong address in FW
** 1)__iot_api_begin to be used instead of __iot_data_begin for data_end in iot_init()..
** 2) Disable CFG_SUPPORT_RDM only for IoT build.
** RAM Size increased by 88b : Total= 0x2b4a8
 *
 */

#ifndef _PATCH_CONFIG_H
#define _PATCH_CONFIG_H

/* Added support for inband commands - Update homekit IE and enable/disable Airplay */
/* Patch Size: 288 bytes */
#ifdef CONFIG_AP_SUPPORT
#define MT7687_BORA00005211			1
#endif


/*Beacon with same Sequence Number sent on air*/
/*Patch Size: 146080 - 146096 = -16 bytes (reduced)*/
/*Fix: */
/* NOT NEEDED NOW*/
#define MT7687_BORA00004769         0 



/* BUG-ON coming while forwarding packets in AP mode*/	
/*Patch Size: */
/*Fix: */
#define MT7687_BORA00004763			1 


/* Profile download fail due to wrong address in FW*/	
/*Patch Size: 88 bytes*/
/*Fix: change iot_data_begin with iot_api_begin*/
/* NOT NEEDED NOW*/
#define MT7687_BORA00004859			0


/*RX zero copy*/
/*Patch Size: 0x98 - 0x8c = -12 bytes(reduced)*/
/*Fix: Do not allocate new packet and perform memcopy, update offset value and return same packet instead*/
	/*ENABLE LATER*/
#define MT7687_BORA00004841			0



/* Add MAC address field in LINKUP event sent to CM4*/	
/*Patch Size: 60Bytes Total = 0x2b4e4*/
/*Fix: */
/*	MACRO REMOVED */
//#define MT7687_BORA00004863 			1


/*secondary channel offset is not announced in Beacon*/
/*Patch Size: 0x24948 - 0x24930= 24 bytes(increased)*/
/*Fix: update pAd->CommonCfg.AddHTInfo.AddHtInfo.ExtChanOffset in make channel data*/
#define MT7687_BORA00004872				1

/* ISSUE:  STA disconnects when scan initiated in 40MHz connection. */
/*     	   STA does not connect in 40MHz with ext channel above. */
/* Patch Size:  20 bytes */
#define MT7687_BORA00004764 			1


/* ISSUE:  AP malfunction after opmode switch (AP-->STA-->AP)	*/
/* Patch Size:  372 bytes 					*/
	/*ENABLE LATER*/
#define MT7687_BORA00004765				1//0


/* ISSUE: AddBA request Tx in 11b mode 	*/
/* Patch Size:  124 bytes 				*/
#define MT7687_BORA00004770				1


/* ISSUE:  Partial scan  */
/* Patch Size: 	4kB		*/
	/*FEATURE ENABLE LATER*/
#define MT7687_BORA00004843				0


/* ISSUE:  Profile testing fails due to inconsistent usage of PhyMode 	*/
/* Patch Size:  2196 bytes 												*/
	/*NOT Needed Now PHYMODE Replaced by WDEV MODE*/
#define MT7687_BORA00004844				0


/* Enabled Tbtt interrupt in AP mode */
/* Sending Broadcast Packets on Tbtt instead of Pre TBtt in AP mode*/	
/*Patch Size: 177584 - 177508 = 76 Bytes */
/*Fix: */
	/*AP Mode */
#define MT7687_BORA00004881 			1//0//Sonal


/* Deletion of Hash entry on Disconnect */
/* Handle the Disconnect event when discoonect is invoked instead of queuing it*/	
/*Patch Size: 177688 - 177584 = 104 Bytes */
/*Patch Size: 177612 - 177508 = 104 Bytes */
/*Fix: */
#define MT7687_BORA00004781 			1


/* Setop mode fix for promiscuous mode*/	
/*Patch Size: 17508  - 17508 = 0Bytes*/
/*Fix: */
#define MT7687_BORA00004883 			1

/* Disable events send by hemExtCmdGenResultAndSend2Hif*/	
/*Patch Size: 180160  - 179968 = -192Bytes (reduced)*/
/*Fix: */
/*NOt Present in Our Code*/
//#define MT7687_BORA00004894 			0

/* Wep security not working in STA mode.*/	
/*Patch Size: 436 Bytes*/
/*Fix: */
#define MT7687_BORA00004889				1


/* ERP and 11n protection 	*/	
/* Patch Size: 560 bytes 	*/
#define MT7687_BORA00004891				1


/* extra 12 bytes in Tx packets.*/	
/*Patch Size: 8Bytes*/
/*Fix: */
	/*NOT NEEDED NOW */
#define MT7687_BORA00004898				0



/* WiFi command event implementation: STA scan */	
/* Patch Size: */
#define MT7687_BORA00004901				1


/* Read profile parameters from NVRAM */	
/* Patch Size: 1100bytes */

#define MT7687_BORA00004903				1



/* WiFi command event implementation */	
/* Patch Size: 976 bytes */
	/*ENABLE LATER*/
#define MT7687_BORA00004905				1//0//Sonal


/* ReKey offloading */	
/* Patch Size: 100 bytes */

#define MT7687_BORA00004924			1


/*AP Start/Stop Extended commands */	
/* Patch Size: */
#define MT7687_BORA00004935				1


/* AP only mode support.*/	
/*Patch Size: 108Bytes*/
/*Fix: */
#define MT7687_BORA00004913             1


/* STA fails to connect in high traffic network.*/	
/*Patch Size: 220 bytes  */
#define MT7687_BORA00004953             1

/*  Zero Mac Address sent to Supplicant when connected in STA mode.*/	
/*Patch Size:   12byte*/
/*MACRO REMOVED*/
//#define  MT7687_BORA00004927 					1


/* ISSUE:  STA configured in 20MHz cannot connect to AP operating in 40MHz channel. */
/* Patch Size:  116 bytes */
#define MT7687_BORA00004961 			1


/* ISSUE:  IoT_STA assert while connecting to AP operating in 20MHz channel 4. */
/* Patch Size:  28 bytes */
#define MT7687_BORA00004965 			1


/*  Secing correct value for channel and RSSI for beacon and probeResponse to host*/	
/*Patch Size:   12byte*/
#define  MT7687_BORA00004959 					1

/*Fix for AP fall back from 20 to 40M*/
/* Patch Size:  16 bytes */
#define MT7687_BORA00004963				1


/* Garbage value in Partial bitmap of Beacon frame  */	
/* Patch Size:   64 byte */
	/*AP Mode */
#define  MT7687_BORA00004972 					1

/*  Rate Adaptation code*/	
/*Patch Size:1892b*/
/*Enable Later*/
#define MT7687_BORA00004970 					0


/* Efuse bin content */	
/* Patch Size: 828bytes */
/*Enable Later*/
#define MT7687_BORA00004930			0

/*  PowerSave N9*/	
/*Patch Size:2836 Bytes*/
/*Enable Later*/
#define MT7687_BORA00004984 					1//0//Sonal


/*  STA disconnect when run high bw udp*/	
/*Patch Size:208 bytes*/
#define MT7687_BORA00005022						1


/* Defragmentation */	
/* Patch Size: 1112 + 1140 = 2252 bytes */
#define MT7687_BORA00005028						1




/*Sta enters into protection by default after BA session is established*/
/*Patch size -60bytes*/
#define MT7687_BORA00005029						1

/*Data packets are transmitted in 20Mhz if auto rate is enabled.*/
/*Patch Size 180 Bytes*/
#define MT7687_BORA00005036						1


/* Basic rate not indicated in Beacon frame in Sniffer capture */
/* Patch Size 12 Bytes */
	/*AP Mode */
#define MT7687_BORA00005038						1



/*  Add 5 retries for Auth and Assoc frames. */
/* Patch Size : 248 bytes */
	/*Feature enable Later UNPATHING Done*/
#define MT7687_BORA00005044                     1//0//Sonal




/*Add WMM support in AP mode.*/
/*Patch Size: 80 bytes*/

#define MT7687_BORA00005048 					1


/* Add PS POLL handling in AP mode  */
/* Patch Size :1200 bytes*/
	/*AP Mode Enable Later Need TXS handling*/
#define MT7687_BORA00005053						1//0//Sonal


/*TxZero copy implementation*/
/*Patch Size : 628b*/
/*Not Needed Now*/
#define MT7687_BORA00005060						0


/*4-way handshake failing when STAUT connect to non-QoS AP*/
/*Patch Size :192Bytes*/
/*MACRO not required already unpatched*/
//#define MT7687_BORA00005075          0

/*Same PSE FID gets freed up twice in case of broadcast packets*/
/*Patch Size :32bytes*/
	/*AP Mode */
#define MT7687_BORA00005080		1

/*Add null packet handling in ncase of TX data*/
/*Patch Size : 252b*/
	/*Enable later realted to interface change*/
#define MT7687_BORA00005078	    0 


/* BSS CoExist2040 issue fix in STA mode */
/* Patch Size : 104 bytes */
#define MT7687_BORA00005085								1


/*
1. Double freeing in case of AMSDU
Patch Size : 116b
*/
#define MT7687_BORA00005114 		1


/*
1. Buffer leak in case of OpMOde switch
Patch Size : 56b
*/
	/*MACRO Not Needed*/
#define MT7687_BORA00005117 		0


/*
1. Incorrect Header length in TxD
Patch Size : 24b
*/
#define MT7687_BORA00005119 		1


/*
1. Iissue with AP PS packet buffering
All net_pkt_blks gets enqueued in PS queue, this leads to Zero Rx pkt thereby a 100% drop in Rx 
Patch Size : 8b
*/
	/*AP Mode*/
#define MT7687_BORA00005120 		1

/*
1. Drop duplicate not getting set issue 
Patch Size : 52b
*/
#define MT7687_BORA00005121 		1

/*HT Rate Tx stops working after 70dB attenuation*/
/*Patch Size : 28 bytes*/
/*Need to change in N9 txminit*/
#define MT7687_BORA00005133		0


/*TCP Rx very low with D-link 825*/
/*Patch Size: 0B*/
#define MT7687_BORA00005140		1



/*RIFS Support in Sta Mode*/
/*PATCH SIZE : 44Bytes*/
#define MT7687_BORA00005144     1


/* Mix-mode security support in AP mode */
/* PATCH SIZE : 28 bytes */
#define MT7687_BORA00005149     		1

/* Get channel query returns incorrect value */
/*  Patch Size -4Bytes*/
#define MT7687_WCNCR00047006			1


/* STA Send PS POll in Fast PSP Mode */
/*  Patch Size 40Bytes*/
#define MT7687_BORA00005191			1

/* low Rx Throughput in power save */
/*  Patch Size 40Bytes*/
	/*Enable Later related to Low power throughput*/
#define MT7687_BORA00005202			1//0//Sonal


/*TGn 4.2.47 failing. APUT sends more than 1 packet for PS POLL*/
/*Patch Size: 1048 Bytes*/
	/*AP Mode*/
#define MT7687_BORA00005209			0


/* Packet allocation fail for Probe response packet  in AP with mix-mode security */
/* PATCH SIZE : 420 bytes */
	/*AP Mode*/
#define MT7687_BORA00005188     		1



/* BSS Co-exist2040 support in AP mode */
/* PATCH SIZE : 2100 bytes */
	/*AP Mode Enable Later*/
#define MT7687_BORA00005151     1//0//Sonal

/* TGn TC-4.2.31, APUT not using Non-member Protection */
/* PATCH SIZE : 24 bytes */
/*AP Mode*/
#define MT7687_BORA00005186     		1

/* TGn TC-4.2.9, Re-association/bridging test fails for APUT */
/* PATCH SIZE : 136 bytes */
/*AP Mode*/
#define MT7687_BORA00005166     		1


/*LDPC Related changes */
/* PATCH SIZE : 160 bytes */
#define MT7687_BORA00005223				1


/* Incorrect DS parameter in Beacon */
/* PATCH SIZE : 12 bytes */
	/*AP Mode*/
#define MT7687_BORA00005224     		1


/* 11bg STA sending HT IE in Assoc request  */
/* PATCH SIZE : 208 bytes */
#define MT7687_BORA00005234     		1


/* Stop sending Beacon to Host during Coex2040 scan  */
/* PATCH SIZE : 24 bytes */
#define MT7687_BORA00005235     		1


/* Specific channel scan in STA mode  */
/* PATCH SIZE : 8 bytes */

#define MT7687_BORA00005251     		1


/* Low throughput in forwarding  */
/* PATCH SIZE : 52 bytes */
/*Enable Later After Profiling*/
#define MT7687_WCNCR00047582			0



/*TCP Rx not working for APUT with Intel STA Driver: 13.5.0.6 */
/* PATCH SIZE : 168 bytes */
/*Txm Init Change TBD in N9*/
#define MT7687_BORA00005253			0

/*Station list for 9 stations not going to CM4*/
/* PATCH SIZE : -80 bytes */
/*Already taken care of this patch*/
#define MT7687_WCNCR00047038			0


/* AP(20MHz, channel 6) asserts when receiving coex-2040 mgmt frame from associated STA */
/* PATCH SIZE :  184 bytes */
/*AP Mode Enable :Later*/
#define MT7687_BORA00005270			1//0//Sonal


/* Tx stops while running TGn-4.2.8 testcase */
/* PATCH SIZE :  4bytes */
/* Heap Size increased by 1792Bytes */
/*AP Mode*/
#define MT7687_BORA00005292			1

/*Ping not working with RT2870 station in AP mode*/
/* PATCH SIZE : 0 bytes */
#define MT7687_BORA00005300 			1

/* RIFS mode enabled in AP mode*/
/* PATCH SIZE :  84bytes */
#define MT7687_BORA00005306			1


/* changed Periodic Exec to 1 sec*/
/* PATCH SIZE :  8bytes */
	/*Not Needed Now*/
#define MT7687_BORA00005308			0


/* Increase Retry limit of MGMT Frames*/
/* PATCH SIZE : 0 bytes */
#define MT7687_BORA00005319		1

/*TGn 4.2.10 APUT sending continuous MC data even if TIB =0*/
/*Patch Size :306B*/
/*AP Mode Will be enabled once AP Mode is done*/	
#define MT7687_BORA00005322                     1//0//Sonal 

/*Handling of get RSSI command from CM4*/
#define MT7687_BORA00005355			1
/*Patch Size : 80B*/


/* AP booting in 11bg mode with 40MHz BW setting */
/* Patch Size : 36 bytes*/
/*AP Mode*/	
#define MT7687_BORA00005370			1

/* Remove Extended capability info IE from Probe and Assoc req/resp frame when STA operating in 11bg mode */
/* Patch Size : 96 bytes*/
	/*Removed Macro already Handled*/
#define MT7687_BORA00005376			1


/*Handling of get PS Mode command from CM4*/
/*Patch Size : 36B*/
#define MT7687_BORA00005377			1


/* Support eFuse access commands over inband channel */
/* Patch Size : 1340 bytes */
	/*Enable Later May be need to send to N9*/
#define MT7687_BORA00005743			0
/*Added WPS support*/
/*Patch Size :1416bytes*/
	
#define MT7687_BORA00005109			1
/*With Sony Xperia Z2 as STA, MT7687 set wpa2psk aes, N9 crashed when ping size 1000bytes */
/* Patch Size : 0 bytes */
#define MT7687_BORA00005406			1

/*WCNCR00052264*/
/*[MT7687][STA][IOT]AP:Trendnet TEW-634GRU, MT7687 sta connect, ping size 10000, loss >20%, (PSMode set to 0, time loss improve)*/
/* Patch Size : 168 bytes */

#define MT7687_BORA00005408			1


/*AP set wpa2psk aes, phone to connect, tera term ping phone 10000,ping loss >20%*/
/*Patch Size : 0B*/
#define MT7687_BORA00005420			1


/* MIC failure event for Counter measure at supplicant  */
/* Patch size:  552bytes - 488bytes = 64bytes*/
#define MT7687_BORA00005413			1


/* Set EDCA parameter for AP in 11b only mode */
/* Patch size: 60bytes */
	/*AP Mode*/
#define MT7687_BORA00005476			1

/* TGn-5.2.30 and TGn-5.2.33: Shortslot time used in CCK mode instead of Longslot time. */
/* Patch size: 200bytes */
#define MT7687_BORA00005515			1

/*Coonection to an AP fails in 40Mhz if previously connected AP was on some other channel*/
/* Patch size: bytes */
	/*AP Mode*/
#define MT7687_BORA00005521			1

/* Rx-Zero copy in intra-bss forwarding to improve throughput */
/* Patch size: 336bytes */
	/*Enable Later  after interface confirmed*/
#define MT7687_BORA00005537			0

/* APCLI Related fixes for SQC*/
/* Patch size: 4.9KB */
/*APCLI  Mode Disable*/
#define MT7687_BORA00005562			0

/*Efuse check added,if Efuse is present then pnly extract contents */
/* Patch Size : 160 bytes */
	/*Enable Later*/
#define MT7687_BORA00005417 			0

/*STA sending Qos frames to non-WMM AP */
/* Patch Size :  68bytes */ 
#define  MT7687_BORA00005601 			1


/*Dual Interface related changes */
/* Patch Size : 452KB */ 
/*Enable Later might not be required*/
#define MT7687_BORA00005606 			0


/*WMM Queuing support*/
/*Patch Size: 76Bytes*/
	/*WMM Support Enable Later*/
#define MT7687_BORA00005701          0



/* Issue :country region update dynamically support */
/*Patch Size: 28 bytes*/

#define MT7687_BORA00005728          1


/* On a particluar RFBs throughput got stuck in AP mode (afterwards p4 changelist-CL: 34660 + WMM changes) */
/* Patch Size : 120-76=44 bytes */
	/*Enable Later WMM change*/
#define MT7687_BORA00005791				0



/* 5G support in Station Mode */
/*Patch Size:  1460bytes*/
	/*Enable Later if require as 5G not present in Mt7686*/
#define MT7687_BORA00005738          0


/* Issue :[Homekit][N9] Lost response from N9 with switch opmode repeatedly */
/* Patch Size : 56 + 48 = 104 bytes */
	/*Enable Later after opmode switch conclusion*/
#define MT7687_BORA00005852				1//0//Sonal 


/* New feature: Tx Raw 802.11 packet */
/* Patch Size : 536 bytes */
	/*enable Later on need*/
#define MT7687_BORA00005865				0


/*APCLI security related changes*/
/* Patch Size : 600bytes*/
	/*APCLI Not Present*/
#define MT7687_BORA00005874				0


/* Security is mixed mode, DUT APCLI some case connect will FAIL */
/* Patch Size : 436bytes*/
/*APCLI Not Present*/
#define MT7687_BORA00005933				0


/* MBSS Feature */
/* Patch Size : 4516bytes*/
	/*Enable Later */
#define MT7687_BORA00005950             0

/* SmartConnect support in SoftAP mode */
/* Patch Size : 612bytes*/
	/*AP Mode Feature*/
#define MT7687_BORA00005974             0


/* AP Send De-Auth to all associated client when switch to other mode */
/* Patch Size : 140 bytes*/
/*AP Mode*/
#define MT7687_BORA00006024             1


/* Set/Get Tx power API support */
/* Patch Size : 336 bytes*/
/*Enable Later Dependency on EFUSE*/
#define MT7687_BORA00006036             1//0//Sonal 

/* APCLI fail to connect after OpMode switch from STA to APCLI */
/* Patch Size : 0 bytes*/
/*APCLI Not Present*/
#define MT7687_BORA00006054             0

/* AP Send disconnect when Data packets are received from NON-connected clients */
/* Patch Size : 52 bytes*/
/*AP Mode*/
#define MT7687_BORA00006069            1

/*TC#4.2.25-> Not able to ping from STA1 to STA2 and vice versa*/
/* Patch Size : 40Bytes*/
/*AP Mode Enable Later Dependency on AP mode Path*/
#define MT7687_WCNCR00082711  			0


/*WMM TC5.2.27 fails at step 5,6,7,8 */
/* Patch Size :  36bytes*/
/*AP mode WMM enable Later*/
#define MT7687_BORA00006058				0


/*WMM TC4.2.21 TC4.2.22 */
/* Patch Size :  16bytes*/
/*Enable Later WMM*/
#define MT7687_BORA00006065				0


/* Configure Free - MediaTek proprietary WiFi zero config (invented by Michael Rong) */
/* Patch Size : 1560 bytes (1428 text + 132 data) */
/*Enable Later Feature*/
#define MT7687_BORA00006139             0


/*Bootup Time optimization for AMAZON*/
/* Patch Size : -408 bytes */
/*Enable Later*/
#define MT7687_BORA00006165				1

/*P2P Support*/
/* Patch Size */
/*Enable Later feature*/
#define MT7687_BORA00006164				0


/*TGN 5.2.17 Mic attackFailure*/
/*pacth Size : -28 bytes*/
/*MACRO Removed*/
#define MT7687_BORA00006320				1

/*country region dynamic update fail*/
/*pacth Size :  180 bytes*/
#define MT7687_BORA00006349				1


/*Dbg Log to host*/
/*pacth Size :  176 bytes*/
/*Not Needed*/
#define MT7687_BORA00006330				0

/*Bootup Memory Optimization */
/*pacth Size :  -2.5K*/
/*Not Needed*/
#define MT7687_BORA00006413				0



/*Bandwidth adjust with channel above/below*/
/* Patch Size 132bytes*/
#define MT7687_BORA00006431				1


/*APLCI LINK STATUS*/
/* Patch Size 28bytes*/
/*APCLI Not Present */
#define MT7687_BORA00006432				0

/*20/40 Scan fix for AP and STA*/
/*patch Size :  28bytes*/
/*Enable Later*/
#define MT7687_BORA00006480				1


/*Enable RSSI threshold*/
/*Patch Size - 496bytes*/
#define MT7687_BORA00006495				1//0 Sonal


/*Enable Rx vector Group 3*/
/*Patch Size - 72Bytes*/
#define MT7687_BORA00006494				1

/*Fix for STA not connecting to AP that has better Rssi*/
/*Patch Size - 24bytes*/
#define MT7687_BORA00006496             1


/*Optimizations*/
/*pacth Size :  -9168bytes*/
/*Enable Later*/
#define MT7687_BORA00006178				0

/*APCLI LINK DOWN EVENT to CM4 enable*/
/*pacth Size :  52Bytes*/
	/*APCLI Not needed*/
#define MT7687_BORA00006502				0


/*AP Only and STA only Interface*/
/*pacth Size : 292Bytes */
	/*Not Needed Now*/
#define MT7687_BORA00006503				0


/*TX Power inconsistent between QA-Tool and Normal mode*/
/*Patch Size -8 bytes*/
	/*Not needed*/
#define MT7687_BORA00006506				0


/*Enter PSM need 5.8 second DON'T Enable the Workaround, will cause issues in stability*/
/*Patch Size 0 bytes*/
/*Power save dependency*/
#define MT7687_BORA00006507				1


/*5G Support in AP Mode*/
/*Patch Size 4 bytes*/
	/*5G Support Not needed*/
#define MT7687_BORA00006508				0


/*Reason Code Enable*/
/*patch Size :  756bytes*/
#define MT7687_BORA00006617				1


/*Send correct Disassoc Event*/
/*patch Size :  0bytes*/
/*APCLI Fix not needed*/
#define MT7687_BORA00006606				0


/* management pkt filter */
/* patch size: 556bytes */
/*Feature enable Later*/
#define MT7687_BORA00006676				1//0//Sonal 

/*Memory Leak*/
/*patch size: 12Bytes*/
/*Enable Later Pse Kick out Dependency*/
#define MT7687_BORA00006736				0


/*Ownetrship transfer to Cm4 if with N9 before sending any packet to CM4*/
/*patch size: -36 Bytes*/
/*Not Needed Now */
#define MT7687_BORA00006764             0

/*APCLI RX throughput on CLI tab is <20Mbps*/
/*patch size:  116 Bytes*/
/*APCLI Not Needed*/
#define MT7687_BORA00006799             0


/*With ASUS RT-N66U AP 5G has IOT connection issue*/
/*patch size:  0 Bytes*/
#define MT7687_WSAP00005625				1


/*Retry Count increased to 0x1E from and CCA->ED*/
/*patch size:  -8 Bytes*/
/*Not Needed Now*/
#define MT7687_BORA00006835				0


/*Roaming from 11g to 11an*/
/*patch size:   60Bytes*/
#define MT7687_BORA00006848				1



/*N9 Crash FIx due to OBSS timeout*/
/*patch size:   68Bytes*/
/*Enable Later*/
#define MT7687_BORA00006890				0

/*Channel added in SSID and BSSID Commands*/
/*pacth Size :  332Bytes*/
#define MT7687_BORA00007143				1



/* STA fail to connect AP in hidden-ssid mode */
/*Patch Size:  68 Bytes */
#undef MT7687_BORA00007280
#define MT7687_BORA00007280            1

//Sonal
/*Mixed mode fix for STA after BSSID Channel Cmd*/
/*patch Size :  480 Bytes*/
#define MT7687_BORA00007195				1

//Sonal
/*Country Code Configuration*/
/* patch size: 840 Bytes */
#define MT7687_BORA00007497 						1


/*MT7682 Fix for 40 Mhz*/
/* patch size: 16 Bytes */
#define MT7682_BORA00008292					        1//Sonal 

//Sonal
/*Set bandwidth inband command fix*/
/* patch size: 136 Bytes */
#define MT7682_BORA00008220                         1

//Sonal
/* Smart connection Regression Failure Fix */
/*Patch Size: 260 Bytes */
//#undef MT7687_BORA00007317
#define MT7687_BORA00007317			1


/*Beacon Loss Time configuration and when beacon loss occurs
the connection will be maintained if data traffic exists*/
/*pacth Size :  152Bytes*/
#define MT7687_BORA00007132				1

//Sonal
/*[MT7682 STA mode]some wifi event handler can't work*/
/*patch size 208 bytes*/
#define WSAP00020512 			1


/*[mt7682 STA mode]boot up in station mode, the system has more than 50% possibility  stopped after inband BSSID command with log "MlmeAux.CentralCh = 11"*/
/*patch Size 48 bytes*/
#define WSAP00020506			1

//Sonal 
/*Low Power Patch Size  2856 bytes*/
#define MT7687_BORA00008228				1

//Sonal
/*Fix _config_disconnect_sta: STA disconnection Fail */
/* PATCH SIZE : 16 bytes */
#define MT7687_BORA00008293     1

//Patch Optimizations
/*patch size : reduced Code size by 10276 bytes*/
#define MT7687_BORA00007544             1


//DUT connect AP successfully, when AP change security, DUT happen crash.
//SQC issue  WSAP00016818
//Patch Size=32 bytes
#define MT7687_BORA00007992							1


/*40Mhz Link-up fail after BSSID Channel Cmd*/
/*pacth Size : 72  Bytes*/
//#undef MT7687_BORA00007212
#define MT7687_BORA00007212			 1


/*Country Code Configuration - Channellist to TxPower mapping*/
/*Patch Size 232 bytes*/
#define MT7687_BORA00007879 						0


//Sonal 
/*EAPOL Retry issue with NetGear AP AMAZON DEMO Issue*/
/*Patch Size 2256 bytes*/
#define MT7687_BORA00007020						1


//Sonal 
/*Patch Size 1700  Bytes*/
#define MT7687_BORA00008227					1

//Sonal 
/*Add FW Reload Support*/
/*size: 656b*/
#define MT7682_WSAP00020527			1


//Sonal 
/*[MT7682 STA mode]some wifi event handler can't work*/
/*patch size 208 bytes*/
//#define WSAP00020512 			1
/*Enable Interrupt based support*/
/*Size 750b*/
#define INTERRUPT_BASED_SUPPORT	1

/*AP Functionality not proper*/
/*Size = 64b*/
#define MT7682_WSAP00020539			1

/*[WSAP00020009][[Wifi Task][MT7682][MT7686]TX AC Queue mapping correction*/
/*Patch Size: 24b*/
#define MT7682_WSAP00020009			1

/*fix for only 20Mhz rates after 40Mhz connection*/
/*	Patch Size 60Bytes */
#define MT7682_WSAP00020564		1

/*Tx power API Support*/
/*	Patch Size 288Bytes */
#define MT7687_WSAP00020513			    1

/*[MT7682] Low power LP Ownership transfer and Inband command*/
#define MT7687_BORA00008519				1

/*Tx power values are taken from Flash Addr*/
/*Patch Size: 72b*/
#define MT7682_WSAP00020532			1

/*send wireless event to Upper layer if auth timeout or assoc timeout*/
/*Patch size  128 bytes*/
#define MT7687_BORA00008636				1


/*TGN test case TC#5.2.35 and 5.2.48  20/40 MHz Coexistence bit is set to "0" in Extended capabilities IE(127)*/
/*Patch size 24bytes*/
#define MT7682_WSAP00040462			1

/*[Coverity scan] CID: 17308, ucTargetAddrIdx >= 256 is always false regardless of the values of its operands*/
#define MT7682_WSAP00040295					1


//Coverity Scan issues CID : 176065, 173100 , 173089 and 100362 
//Size 24bytes
#define MT7682_WSAP00041075						1

#endif
