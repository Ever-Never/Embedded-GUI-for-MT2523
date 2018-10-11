#ifndef _IN_BAND_MSG_H_
#define _IN_BAND_MSG_H_
													  
struct racfghdr {
	u8     command_id;
	u8     sequence;
	u16   length;
	s8     status;
	u8     cmd_seq;
	u16	 signal; // gpio_hw_radio for HEART_BEAT to host, WOW signal for HEART_BEAT to iNIC, is4wayframe
	u8     data[0];
}
#ifdef IAR_COMPILE
__packed;
#else
__attribute__((packed));
#endif

#define RACFG_CMD_TYPE_PASSIVE_MASK  0x7FFF
#define RACFG_CMD_TYPE_RSP_FLAG      0x8000


#if 0
/* command type */
/* protocol = 2880 */
#define RACFG_CMD_TYPE_ATE          0x0005
/* protocol = ffff */
#define RACFG_CMD_TYPE_BOOTSTRAP    0x0001
#define RACFG_CMD_TYPE_ASYNC        0x0005
#define RACFG_CMD_TYPE_COPY_TO_USER     0x0006
#define RACFG_CMD_TYPE_IWREQ_STRUC      0x0007
#define RACFG_CMD_TYPE_IOCTL_STATUS     0x0008
#define RACFG_CMD_TYPE_SYNC         0x0009
#define RACFG_CMD_TYPE_IW_HANDLER	    0x000a
/* ASYNC FEEDBACK Command ID */
#define RACFG_CMD_CONSOLE           0x0000
#define RACFG_CMD_WSC_UPDATE_CFG      0x0001
#define RACFG_CMD_WIRELESS_SEND_EVENT 0x0002
#define RACFG_CMD_SEND_DAEMON_SIGNAL    0x0003
#define RACFG_CMD_HEART_BEAT    0x0004
#define RACFG_CMD_BEACON				0x0007
#define RACFG_CMS_WPS_PBC_AP_SORT		0x0008
/* SYNC FEEDBACK Command ID */
#define RACFG_CMD_GET_MAC             0x0000
#define RACFG_CMD_MBSS_OPEN           0x0001
#define RACFG_CMD_MBSS_CLOSE          0x0002
#define RACFG_CMD_WDS_OPEN            0x0003
#define RACFG_CMD_WDS_CLOSE           0x0004
#define RACFG_CMD_APCLI_OPEN          0x0005
#define RACFG_CMD_APCLI_CLOSE         0x0006
#define RACFG_CMD_WPS_PBC_SSID		  0x0007
/* IW HANDLER Command ID */
/* Must enumerate the Command ID from 0x0001 */
#define RACFG_CMD_GET_STATS           0x0001
#endif
// ====== begin of ATE cmd (00 ~ 0x1F reserved 32) ======
#if 0
#define COMPACT_ATE_CMD_BEGIN            0x00     
#define COMPACT_ATE_Set             0x00
#define COMPACT_ATE_SetDA         0x01
#define COMPACT_ATE_SetSA         0x02
#define COMPACT_ATE_SetBSSID    0x03
#define COMPACT_ATE_SetChannel    0x04
#define COMPACT_ATE_SetTxPow0    0x05
#define COMPACT_ATE_SetTxPow1    0x06
#define COMPACT_ATE_SetTxAnt    0x07
#define COMPACT_ATE_SetRxAnt    0x08
#define COMPACT_ATE_SetTxFreqOff    0x09
#define COMPACT_ATE_SetTxBW    0x0A
#define COMPACT_ATE_SetTxLen    0x0B
#define COMPACT_ATE_SetTxCnt    0x0C
#define COMPACT_ATE_SetTxMCS    0x0D
#define COMPACT_ATE_SetTxMode    0x0E
#define COMPACT_ATE_SetTxGI    0x0F
#define COMPACT_ATE_SetRxFER    0x10
#define COMPACT_ATE_SetRRF   0x11
#define COMPACT_ATE_SetWRF1    0x12
#define COMPACT_ATE_SetWRF2    0x13
#define COMPACT_ATE_SetWRF3    0x14
#define COMPACT_ATE_SetWRF4    0x15
#define COMPACT_ATE_SetLDE2P    0x16
#define COMPACT_ATE_SetRE2P   0x17
#define COMPACT_ATE_Show    0x18
#define COMPACT_ATE_Help    0x19
#define COMPACT_ATE_TxStop    0x1A
#define COMPACT_ATE_RxStop    0x1B
#define COMPACT_ATE_ANTENNA 0x1C
#define COMPACT_ATE_TXALC	0x1D
#define COMPACT_ATE_CMD_END            0x1F
#endif

// ====== end of ATE cmd  ======

// ====== begin of SYNC cmd (20~5F reserved 64) =======
#if 0
#define COMPACT_SYNC_CMD_BEGIN            0x20     
#define COMPACT_STATUS          0x20
#define COMPACT_SetSSID         0x21
#define COMPACT_SetWirelessMode 0x22
#define COMPACT_SetChannel      0x23           
#define COMPACT_ToggleHeartBeat 0x24
#define COMPACT_SetLED          0x25
#define COMPACT_SetDebug        0x26
#define COMPACT_SetConsole      0x27
#define COMPACT_SetNetworkType  0x28
#define COMPACT_SetAuthMode     0x29
#define COMPACT_SetEncrypType   0x2A
#define COMPACT_SetKey          0x2B
#define COMPACT_SetWpaPsk       0x2C
#define COMPACT_SetDefaultKeyID 0x2D
#define COMPACT_SetPSMode          0x2E
#define COMPACT_ATE_QA_ENABLE      0x2F
#endif
#define COMPACT_RACFG_CMD_GET_MAC           0x30
#if 0
#define COMPACT_RACFG_CMD_CLOSE             0x31
#define COMPACT_RACFG_CMD_WPS_PBC_SSID		0x32
#define COMPACT_RACFG_CMD_WPS_PIN_SSID		0x33
#define COMPACT_RACFG_CMD_GET_STATS         0x34
#define COMPACT_GET_QUALITY                    0x35
#define COMPACT_GET_PORT_SECURED         0x36
#define COMPACT_UPDATE_PROFILE              0x37
#define COMPACT_RACFG_RESET_COUNTER	0x38
#define COMPACT_RACFG_PBC_SESSION_OVERLAP	0x39
#define COMPACT_FREQ_SWITCH_OFF	            0x3A
#define COMPACT_EHANCE_PS_SWITCH	        0x3B 
#define COMPACT_EHANCE_PS_TRIG_ALIVE	        0x3C 
#define COMPACT_EHANCE_PS_TRIG_ASLEEP	        0x3D  
#define COMPACT_SYNC_CMD_END                0x5F 
// ====== end of SYNC cmd =======================

// ====== begin of ASYNC cmd (60~7F reserved 32) =======
#define COMPACT_ASYNC_CMD_BEGIN            0x60     
#define COMPACT_RACFG_CMD_HEART_BEAT             0x60
#define COMPACT_RACFG_CMD_CONSOLE                0x61
#define COMPACT_RACFG_CMD_WSC_UPDATE_CFG         0x62
#endif
#define COMPACT_RACFG_CMD_WIRELESS_SEND_EVENT    0x63
#define COMPACT_RACFG_CMD_WIRELESS_SEND_EVENT2   0x64
#define COMPACT_RACFG_CMD_BEACON                 0x65
#define COMPACT_RACFG_CMS_WPS_PBC_AP_SORT		 0x66
#define COMPACT_RACFG_CMS_WPS_PIN_AP_SORT		 0x67
#if 0
#define COMPACT_RACFG_CMD_BOOT_UPLOAD            0x68
#define COMPACT_RACFG_CMD_WIFI_FREEDESC         0x69

#define COMPACT_RACFG_CMD_MLME			              0x6a
#define COMPACT_UPDATE_BEACON_BUF				0x6b
#define COMPACT_UPDATE_BEACON_TXWI				0x6c
#define COMPACT_MM_REQUEST						0x6d
#define COMPACT_SYNC_ACTIVE_CFG_CMD				0x6e
#define COMPACT_SYNC_BA_CMD						0x6f
#define COMPACT_PERIODIC_SYNC_CMD				0x70
#define COMPACT_SEND_EAPOL_CMD					0x71
#define COMPACT_SYNC_SETHT_CMD					0x73
#define COMPACT_SYNC_SEC_CMD						0x74
#define COMPACT_SYNC_PROFILE              				0x75
#endif
#define COMPACT_RACFG_CMD_HT2040					0x76

#define COMPACT_SCRIPT_CMD 			0x7F
#define COMPACT_ASYNC_CMD_END             0x7F     
// ====== end of ASYNC cmd ======================

//========== begin of IOCTL (80~DF reserved 64) =====
#define COMPACT_IOCTL_STD_BEGIN     0x80  
#define COMPACT_IOCTL_STD_END       0xDF  
//========== end of IOCTL ====================

//========== begin of IOCTL (E0~FE reserved 31) =====
#define COMPACT_IOCTL_PRIV_BEGIN     0xE0  
#define COMPACT_IOCTL_PRIV_END       0xFE  
//========== end of IOCTL ====================

#endif /* _IN_BAND_MSG_H_ */
