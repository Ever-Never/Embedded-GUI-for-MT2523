#ifndef __IOCTL_H__
#define __IOCTL_H__

#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE                              0x8BE0
#endif
#ifndef SIOCIWFIRSTPRIV
#define SIOCIWFIRSTPRIV								SIOCDEVPRIVATE
#endif

#ifdef CONFIG_AP_SUPPORT
#define RT_PRIV_IOCTL								(SIOCIWFIRSTPRIV + 0x01)
#else
#define RT_PRIV_IOCTL_SYNC_AP_FOR_WSC_UPNP			(SIOCIWFIRSTPRIV + 0x01) // Sync. with AP for wsc upnp daemon
#endif

#define RTPRIV_IOCTL_SET							(SIOCIWFIRSTPRIV + 0x02)
				

#define RTPRIV_IOCTL_BBP                            (SIOCIWFIRSTPRIV + 0x03)
#define RTPRIV_IOCTL_BBP_IDX	0
#define RTPRIV_IOCTL_RF_IDX		1
#define RTPRIV_IOCTL_MAC                            (SIOCIWFIRSTPRIV + 0x05)
#define RTPRIV_IOCTL_E2P                            (SIOCIWFIRSTPRIV + 0x07)


#ifdef RALINK_ATE
#ifdef RALINK_28xx_QA
#define RTPRIV_IOCTL_ATE							(SIOCIWFIRSTPRIV + 0x08)
#endif // RALINK_28xx_QA //
#endif // RALINK_ATE //

#define RTPRIV_IOCTL_STATISTICS                     (SIOCIWFIRSTPRIV + 0x09)
#define RTPRIV_IOCTL_ADD_PMKID_CACHE                (SIOCIWFIRSTPRIV + 0x0A)
#define RTPRIV_IOCTL_RADIUS_DATA                    (SIOCIWFIRSTPRIV + 0x0C)
#define RTPRIV_IOCTL_GSITESURVEY					(SIOCIWFIRSTPRIV + 0x0D)

#ifdef CONFIG_AP_SUPPORT
#define RTPRIV_IOCTL_ADD_WPA_KEY                    (SIOCIWFIRSTPRIV + 0x0E)
#else
#define RT_PRIV_IOCTL								(SIOCIWFIRSTPRIV + 0x0E) // Sync. with RT61 (for wpa_supplicant)
#endif

#define RTPRIV_IOCTL_GET_MAC_TABLE					(SIOCIWFIRSTPRIV + 0x0F)
#define RTPRIV_IOCTL_STATIC_WEP_COPY                (SIOCIWFIRSTPRIV + 0x10)


#define RTPRIV_IOCTL_SHOW							(SIOCIWFIRSTPRIV + 0x11)
#define RTPRIV_IOCTL_WSC_PROFILE                    (SIOCIWFIRSTPRIV + 0x12)
#define RTPRIV_IOCTL_QUERY_BATABLE                  (SIOCIWFIRSTPRIV + 0x16)

#define RTPRIV_IOCTL_MEM                            (SIOCIWFIRSTPRIV + 0x17)
#define RTPRIV_IOCTL_E2P_READ_W						(SIOCIWFIRSTPRIV + 0x19)
#if 0  // integrate into RTPRIV_IOCTL_BBP
//wayne_note for RTPRIV_IOCTL_RF requested from Jeff
#define RTPRIV_IOCTL_RF                             (SIOCIWFIRSTPRIV + 0x1b)
#endif
#endif /* __IOCTL_H__*/
