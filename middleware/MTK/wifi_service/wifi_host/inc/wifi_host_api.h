/* Copyright Statement:
 *
 * (C) 2005-2016 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

/**
 * @file wifi_host_api.h
 *
 * Wi-Fi processor configuration / settings for the STA operations.
 *
 */

/**@addtogroup WIFI
* @{
* This section introduces the Wi-Fi driver APIs including terms and acronyms, supported features,
* software architecture, details on how to use this driver, Wi-Fi function groups, enumerations,
* structures and functions. The Wi-Fi APIs are divided into three categories:
* Profile, Configuration and Connection. And there are four types of operation modes supported in Wi-Fi module:
* #WIFI_MODE_STA_ONLY, #WIFI_MODE_AP_ONLY, #WIFI_MODE_REPEATER and #WIFI_MODE_MONITOR.
*
* Profile APIs get or set the profile in the Flash memory. The profile settings can be read by
* Wi-Fi profile get APIs. The profile settings can be used to initialize the Wi-Fi
* driver configuration at system boot up. There are many profile sections in the Flash memory, such as:
* STA, AP, common. Use the same profile section for #WIFI_PORT_STA/#WIFI_PORT_APCLI.
*
* Configuration APIs get and set the Wi-Fi driver configuration. In most cases they take effect
* immediately, but there are some configuration APIs that only take
* effect by calling the #wifi_host_config_reload_setting() API, such as: \n
* #wifi_host_config_set_security_mode() \n
* #wifi_host_config_set_ssid() \n
*
* The Configuration APIs include wifi_host_config_xxx APIs. They use in-band mechanism (in-band is a mechanism used for Wi-Fi driver and Wi-Fi firmware communication),
* thus these APIs must be called after OS task scheduler is started to make sure an in-band task is running.
*
* The Connection APIs include wifi_host_connection_xxx APIs. They manage the link status, such as disconnect AP, disconnect the station, get the link status, get the station list, start or stop the scan.
* Register an event handler for scan, connect or disconnect event. The Connection APIs use an in-band mechanism, thus these APIs must be called after the OS Task
* scheduler is started, to make sure an in-band task is running.
*
*
* Terms                         |Details                                                                 |
* ------------------------------|------------------------------------------------------------------------|
* \b STA                        | In station (STA) mode the device operates as a client that connects to a Wi-Fi access point.|
* \b AP                         | In Access Point (AP) mode other devices can connect to the Wi-Fi access point.|
* \b APCLI                      | In AP Client (APCLI) mode the Access Point becomes a wireless client for another AP.|
* \b DTIM                       | Delivery Traffic Indication Message (DTIM) is how the AP (wireless router) warns its clients that it is about to transmit the multicast (and broadcast*) frames it queued up since the previous DTIM. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Delivery_traffic_indication_message">introduction to DTIM in Wikipedia</a>. |
* \b MCS                        | Modulation and Coding Set (MCS) implies the packet data rate. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to MCS in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard to create a secure wireless home network. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b PIN                        | Personal identification number (PIN) is a method of WPS, has to be read from either a sticker or display on the new wireless device. This PIN must then be entered at the "representant" of the network, usually the network's AP. Alternately, a PIN provided by the AP may be entered into the new device. This method is the mandatory baseline mode and everything must support it. The Wi-Fi direct specification supersedes this requirement by stating that all devices with a keypad or display must support the PIN method.|
* \b PBC                        | Push button (PBC) is a method of WPS. For more information, please refer to <a herf="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to PBC in Wikipedia</a>.|
* \b STBC                       | Space-time Block Code (STBC) used in wireless telecommunications. Space-time block coding is a technique used in wireless communications to transmit multiple copies of a data stream across a number of antennas and to exploit the various received versions of the data to improve the reliability of data-transfer. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Space%E2%80%93time_block_code">introduction to STBC in Wikipedia</a>.|
* \b FCS                        | Frame Check Sequence (FCS) refers to the extra error-detecting code added to a frame in a communications protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Frame_check_sequence">introduction to FCS in Wikipedia</a>.|
* \b Broadcast                  | A communication where a piece of information is sent from one point to all other points.|
* \b Multicast                  | A communication where a piece of information is sent from one or more points to a set of other points.|
* \b RTS                        | Request to Send (RTS) is IEEE 802.11 control signal is an optional mechanism used by the 802.11 wireless networking protocol to reduce frame collisions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/RTS/CTS">introduction to RTS in Wikipedia</a>.|
* \b CTS                        | Clear to Send (CTS) is IEEE 802.11 control signal is an optional mechanism used by the 802.11 wireless networking protocols to reduce frame collisions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/RTS/CTS">introduction to CTS in Wikipedia</a>.|
* \b RSSI                       | Received Signal Strength Indication (RSSI). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Received_signal_strength_indication">introduction to RSSI in Wikipedia</a>. |
* \b Beacon                     | Beacon frame is one of the management frames in IEEE 802.11 based WLANs.|
* \b WPA                        | Wi-Fi Protected Access (WPA) is a wireless encryption standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Access">introduction to WPA in Wikipedia</a>.|
* \b WPS                        | Wi-Fi Protected Setup (WPS) is a network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wi-Fi_Protected_Setup">introduction to WPS in Wikipedia</a>.|
* \b OPMODE                     | Operate Mode (OPMODE) is STA/AP/APCLI.|
* \b PSK                        | Pre-shared key (PSK) is a method to set encryption keys. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Pre-shared_key">introduction to PSK in Wikipedia</a>.|
* \b PMK                        | Pair-wise Master Key (PMK) is calculated from PSK.|
* \b WEP                        | Wired Equivalent Privacy (WEP) is a wireless network security standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Wired_Equivalent_Privacy">introduction to WEP in Wikipedia</a>.|
* \b WOW                        | Wake on Wireless (WOW) is a technology that allows remote wake-up of workstations from a standby power state to facilitate device management. |
* \b TKIP                       | Temporal Key Integrity Protocol (TKIP) is an algorithm used to secure wireless computer networks. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Temporal_Key_Integrity_Protocol">introduction to TKIP in Wikipedia</a>.|
* \b AES                        | Advanced Encryption Standard process (AES) is a 'symmetric block cipher' for encrypting texts which can be decrypted with the original encryption key. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard">introduction to AES in Wikipedia</a>.|
* \b PHY                        | PHY of the Open system interconnection (OSI) model and refers to the circuitry required to implement physical layer functions. For more information, please refer to <a href="https://en.wikipedia.org/wiki/PHY_(chip)">introduction to PHY in Wikipedia</a>.|
* \b BSS                        | The basic service set (BSS) provides the basic building-block of an 802.11 wireless LAN. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b IBSS                       | With 802.11, one can set up an ad hoc network of client devices without a controlling access point; the result is called an IBSS (independent BSS). For more information, please refer to <a href="https://en.wikipedia.org/wiki/Service_set_%28802.11_network%29">introduction to 802.11 network in Wikipedia</a>.|
* \b MCS                        | Modulation and Coding Scheme (MCS) is related to the maximum data rate and robustness of data transmission. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Link_adaptation">introduction to Link adaptation in Wikipedia</a>.|
* \b LDPC                       | In information theory, a low-density parity-check (LDPC) code is a linear error correcting code, a method of transmitting a message over a noisy transmission channel. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Low-density_parity-check_code">introduction to Low-density parity-check code in Wikipedia</a>.|
* \b GI                         | In telecommunications, guard intervals (GI) are used to ensure that distinct transmissions do not interfere with one another. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Guard_interval">introduction to Guard interval in Wikipedia</a>.|
* \b MPDU                       | MPDU stands for MAC protocol data unit.|
* \b TSC                        | TKIP uses a per-MPDU TKIP sequence counter (TSC) to sequence the MPDUs it sends. The receiver drops MPDUs received out of order, i.e., not received with increasing sequence numbers.|
*
*/

#ifndef __WIFI_HOST_API_H__
#define __WIFI_HOST_API_H__

#include "stdint.h"
#include "stdbool.h"
//#include "wifi_api.h"


#ifdef __cplusplus
extern "C" {
#endif

/**@defgroup WIFI_DEFINE Define
* @{
*/

/**@brief The maximum length of SSID.
*/
#define WIFI_MAX_LENGTH_OF_SSID             (32)

/**@brief MAC address length.
*/
#define WIFI_MAC_ADDRESS_LENGTH             (6)

/**
* @brief Length of 802.11 MAC header.
*/
#define WIFI_LENGTH_802_11                  (24)

/**@brief Timestamp length in a frame header.
*/
#define WIFI_TIMESTAMP_LENGTH               (8)

/**@brief Beacon interval length in a frame header.
*/
#define WIFI_BEACON_INTERVAL_LENGTH         (2)

/**@brief Length of capability information in a frame header.
*/
#define WIFI_CAPABILITY_INFO_LENGTH         (2)

/**@brief The Maximum number of stations that can
* connect to LinkIt 7687 HDK in SoftAP mode.
*/
#define WIFI_MAX_NUMBER_OF_STA              (16)

/**@brief The maximum length of passphrase used in WPA-PSK and WPA2-PSK encryption types.
*/
#define WIFI_LENGTH_PASSPHRASE              (64)

/**@brief The maximum length of PMK used in WPA-PSK and WPA2-PSK encryption types.
*/
#define WIFI_LENGTH_PMK                     (32)

/**
* @brief Station operation mode. In this mode the device works as a Wi-Fi client.
*/
#define WIFI_MODE_STA_ONLY      (1)

/**
* @brief SoftAP operation mode. In Access Point (AP) mode, other client devices can connect to the Wi-Fi AP.
*/
#define WIFI_MODE_AP_ONLY       (2)

/**
* @brief Specifies the STA operation.
*/
#define WIFI_PORT_STA           (0)

/**
* @brief Specifies the AP operation.
*/
#define WIFI_PORT_AP            (1)

#define WIFI_NUMBER_WEP_KEYS     (4)    /**<  The group number of WEP keys. */
#define WIFI_MAX_WEP_KEY_LENGTH  (26)   /**<  The maximum length of each WEP key. */

/**
* @}
*/


/**@defgroup WIFI_ENUM Enumeration
* @{
*/
/**@brief This enumeration defines the supported events generated by the Wi-Fi driver. The event will be sent to the upper layer handler registered in #wifi_connection_register_event_handler().
*/
typedef enum {
    WIFI_EVENT_IOT_CONNECTED = 0,               /**< Connected event. */
    WIFI_EVENT_IOT_SCAN_COMPLETE,               /**< Scan completed event.*/
    WIFI_EVENT_IOT_DISCONNECTED,                /**< Disconnected event.*/
    WIFI_EVENT_IOT_PORT_SECURE,                 /**< Secure event, mainly processed in supplicant. It can be used at the DHCP start.*/
    WIFI_EVENT_IOT_REPORT_BEACON_PROBE_RESPONSE,/**< Sends notification, if beacon or probe request is received.*/
    WIFI_EVENT_IOT_WPS_COMPLETE,                /**< Credential event, sends notification when WPS process is complete.*/
    WIFI_EVENT_IOT_INIT_COMPLETE,               /**< Initialization complete event. */
    WIFI_EVENT_IOT_REPORT_FILTERED_FRAME,       /**< Report the expected packet frame. */
    WIFI_EVENT_IOT_CONNECTION_FAILED,           /**< Connection has failed. This option is not available yet. */
    WIFI_EVENT_MAX_NUMBER
} wifi_event_t;

/** @brief This enumeration defines the RX filter register's bitmap. Each bit indicates a specific drop frame.
*/
typedef enum {
    WIFI_RX_FILTER_DROP_STBC_BCN_BC_MC,       /**< bit 0   Drops the STBC beacon/BC/MC frames. */
    WIFI_RX_FILTER_DROP_FCS_ERR,              /**< bit 1   Drops the FCS error frames. */
    WIFI_RX_FILTER_RESERVED,                  /**< bit 2   A reserved bit, not used. */
    WIFI_RX_FILTER_DROP_VER_NOT_0,            /**< bit 3   Drops the version field of Frame Control field. It cannot be 0. */
    WIFI_RX_FILTER_DROP_PROBE_REQ,            /**< bit 4   Drops the probe request frame. */
    WIFI_RX_FILTER_DROP_MC_FRAME,             /**< bit 5   Drops multicast frame. */
    WIFI_RX_FILTER_DROP_BC_FRAME,             /**< bit 6   Drops broadcast frame. */
    WIFI_RX_FILTER_DROP_BSSID_BCN = 11,       /**< bit 11  Drops beacon frame. */
    WIFI_RX_FILTER_RM_FRAME_REPORT_EN = 12,   /**< bit 12  Enables report frames. If this bit is set to 1, all the Wi-Fi packets in the air will be reported, even though the other bits are set to drop their respective frames. */
    WIFI_RX_FILTER_DROP_CTRL_RSV,             /**< bit 13  Drops reserved definition control frames. */
    WIFI_RX_FILTER_DROP_CTS,                  /**< bit 14  Drops CTS frames. */
    WIFI_RX_FILTER_DROP_RTS,                  /**< bit 15  Drops RTS frames. */
    WIFI_RX_FILTER_DROP_DUPLICATE,            /**< bit 16  Drops duplicate frames. */
    WIFI_RX_FILTER_DROP_NOT_MY_BSSID,         /**< bit 17  Drops not my BSSID frames. */
    WIFI_RX_FILTER_DROP_NOT_UC2ME,            /**< bit 18  Drops not unicast to me frames. */
    WIFI_RX_FILTER_DROP_DIFF_BSSID_BTIM,      /**< bit 19  Drops different BSSID TIM (Traffic Indication Map) Broadcast frame. */
    WIFI_RX_FILTER_DROP_NDPA                  /**< bit 20  Drops the NDPA or not. */
} wifi_rx_filter_t;

/** @brief This enumeration defines the wireless authentication mode to indicate the Wi-Fi device’s authentication attribute.
*/
typedef enum {
    WIFI_AUTH_MODE_OPEN = 0,                        /**< Open mode.     */
    WIFI_AUTH_MODE_SHARED,                          /**< Not supported. */
    WIFI_AUTH_MODE_AUTO_WEP,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA,                             /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK,                         /**< WPA_PSK.       */
    WIFI_AUTH_MODE_WPA_None,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA2,                            /**< Not supported. */
    WIFI_AUTH_MODE_WPA2_PSK,                        /**< WPA2_PSK.      */
    WIFI_AUTH_MODE_WPA_WPA2,                        /**< Not supported. */
    WIFI_AUTH_MODE_WPA_PSK_WPA2_PSK,                /**< Mixture mode.  */
} wifi_auth_mode_t;

/** @brief This enumeration defines the wireless encryption type to indicate the Wi-Fi device’s encryption attribute.
*/
typedef enum {
    WIFI_ENCRYPT_TYPE_WEP_ENABLED = 0,                                              /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_ENCRYPT1_ENABLED = WIFI_ENCRYPT_TYPE_WEP_ENABLED,             /**< WEP encryption type.  */
    WIFI_ENCRYPT_TYPE_WEP_DISABLED = 1,                                             /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_DISABLED = WIFI_ENCRYPT_TYPE_WEP_DISABLED,            /**< No encryption.     */
    WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT = 2,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_KEY_ABSENT = WIFI_ENCRYPT_TYPE_WEP_KEY_ABSENT,        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED = 3,                                        /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_ENCRYPT_NOT_SUPPORTED = WIFI_ENCRYPT_TYPE_WEP_NOT_SUPPORTED,  /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_ENABLED = 4,                                             /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT2_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_ENABLED,            /**< TKIP encryption.   */
    WIFI_ENCRYPT_TYPE_AES_ENABLED = 6,                                              /**< AES encryption.    */
    WIFI_ENCRYPT_TYPE_ENCRYPT3_ENABLED = WIFI_ENCRYPT_TYPE_AES_ENABLED,             /**< AES encryption.     */
    WIFI_ENCRYPT_TYPE_AES_KEY_ABSENT = 7,                                           /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_TKIP_AES_MIX = 8,                                             /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_ENCRYPT4_ENABLED = WIFI_ENCRYPT_TYPE_TKIP_AES_MIX,            /**< TKIP or AES mix.   */
    WIFI_ENCRYPT_TYPE_TKIP_AES_KEY_ABSENT = 9,                                      /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP40_ENABLED = 10,                                     /**< Not supported.     */
    WIFI_ENCRYPT_TYPE_GROUP_WEP104_ENABLED = 11,                                    /**< Not supported.     */
#ifdef WAPI_SUPPORT
    WIFI_ENCRYPT_TYPE_ENCRYPT_SMS4_ENABLED,                                         /**< WPI SMS4 support.  */
#endif /* WAPI_SUPPORT */
} wifi_encrypt_type_t;


/** @brief This enumeration defines the wireless physical mode.
*/
typedef enum {
    WIFI_PHY_11BG_MIXED = 0,     /**<  0, 2.4GHz band. */
    WIFI_PHY_11B,                /**<  1, 2.4GHz band. */
    WIFI_PHY_11A,                /**<  2, 5GHz band. */
    WIFI_PHY_11ABG_MIXED,        /**<  3, both 2.4G and 5G band. */
    WIFI_PHY_11G,                /**<  4, 2.4GHz band. */
    WIFI_PHY_11ABGN_MIXED,       /**<  5, both 2.4G and 5G band. */
    WIFI_PHY_11N_2_4G,           /**<  6, 11n-only with 2.4GHz band. */
    WIFI_PHY_11GN_MIXED,         /**<  7, 2.4GHz band. */
    WIFI_PHY_11AN_MIXED,         /**<  8, 5GHz band. */
    WIFI_PHY_11BGN_MIXED,        /**<  9, 2.4GHz band. */
    WIFI_PHY_11AGN_MIXED,        /**< 10, both 2.4G and 5G band. */
    WIFI_PHY_11N_5G,             /**< 11, 11n-only with 5GHz band. */
} wifi_phy_mode_t;

/** @brief This enumeration defines wireless security cipher suits.
*/
typedef enum {
        WIFI_CIPHER_NONE = 0,     /**<  0, No cipher. */
        WIFI_CIPHER_WEP64,        /**<  1, WEP64 cipher.  */
        WIFI_CIPHER_WEP128,       /**<  2, WEP128 cipher. */
        WIFI_CIPHER_TKIP,         /**<  3, TKIP cipher. */
        WIFI_CIPHER_AES,          /**<  4, AES cipher. */
        WIFI_CIPHER_CKIP64,       /**<  5, CKIP64 cipher. */
        WIFI_CIPHER_CKIP128,      /**<  6, CKIP128 cipher. */
        WIFI_CIPHER_CKIP152,      /**<  7, CKIP152 cipher. */
        WIFI_CIPHER_SMS4,         /**<  8, SMS4 cipher. */
        WIFI_CIPHER_WEP152,       /**<  9, WEP152 cipher. */
        WIFI_CIPHER_BIP,          /**< 10, BIP cipher. */
        WIFI_CIPHER_WAPI,         /**< 11, WAPI cipher. */
        WIFI_CIPHER_TKIP_NO_MIC   /**< 12, TKIP_NO_MIC cipher. */
} wifi_security_cipher_suits_t;


/** @brief This enumeration defines 40MHz bandwidth extension.
*/
typedef enum {
    WIFI_BANDWIDTH_EXT_40MHZ_UP,    /**< 40MHz up. */
    WIFI_BANDWIDTH_EXT_40MHZ_BELOW  /**< 40MHz below. */
} wifi_bandwidth_ext_t;


/**
* @brief This enumeration defines three power saving modes.
*/
typedef enum {
    WIFI_POWER_SAVING_MODE_OFF = 0,          /**<  WIFI_POWER_SAVING_MODE_OFF is a power saving mode that keeps the radio powered up continuously to ensure there is a minimal lag in response time. This power saving setting consumes the most power but offers the highest throughput. */
    WIFI_POWER_SAVING_MODE_LEGACY,           /**<  WIFI_POWER_SAVING_MODE_LEGACY: The access point buffers incoming messages for the radio. The radio occasionally 'wakes up' to determine if any buffered messages are waiting and then returns to sleep mode after it has responded to each message. This setting conserves the most power but also provides the lowest throughput. It is recommended for radios in which power consumption is the most important (such as small battery-operated devices). */
    WIFI_POWER_SAVING_MODE_FAST              /**<  WIFI_POWER_SAVING_MODE_FAST is a power saving mode that switches between power saving and WIFI_POWER_SAVING_MODE_OFF modes, depending on the network traffic. For example, it switches to WIFI_POWER_SAVING_MODE_OFF mode after receiving a large number of packets and switches back to power saving mode after the packets have been retrieved. Fast is recommended when power consumption and throughput are a concern.  */
} wifi_power_saving_mode_t;


/** @brief This enumeration of device password ID is used for WPS.
* This enumeration is only applicable when the boolean variable selected_registrar inside #wifi_wps_element_t is true.
* For more information, please refer to Table 37, "Device Password ID" in chapter 12, "Data Element Definitions"
* of the Wi-Fi Simple Configuration Technical Specification V2.0.5.
*/
typedef enum {
    WIFI_WPS_DPID_DEFAULT             = 0x0000,     /**<  Default value or use PIN if selected_registrar is true. */
    WIFI_WPS_DPID_USER_SPECIFIED      = 0x0001,     /**<  User specified. */
    WIFI_WPS_DPID_MACHINE_SPECIFIED   = 0x0002,     /**<  Machine specified. */
    WIFI_WPS_DPID_REKEY               = 0x0003,     /**<  Rekey. */
    WIFI_WPS_DPID_PUSHBUTTON          = 0x0004,     /**<  Pushbutton. */
    WIFI_WPS_DPID_REGISTRAR_SPECIFIED = 0x0005,     /**<  Registrar specified. */
    WIFI_WPS_DPID_MAX_NUMBER          = 0xFFFF
} wps_device_password_id_t;


/**
* @brief This enumeration lists the configuration methods supported by the Enrollee or Registrar.
* For more information, please refer to Table 33, "Configuration Methods" in chapter 12, "Data Element Definitions"
* of the Wi-Fi Simple Configuration Technical Specification V2.0.5.
*/
typedef enum {
    WIFI_WPS_CONFIG_USBA                    = 0x0001,  /**<  @deprecated Ignored in WPS version 2.0. */
    WIFI_WPS_CONFIG_ETHERNET                = 0x0002,  /**<  @deprecated Ignored in WPS version 2.0. */
    WIFI_WPS_CONFIG_LABEL                   = 0x0004,  /**<  8-digit static PIN.  */
    WIFI_WPS_CONFIG_DISPLAY                 = 0x0008,  /**<  A dynamic PIN from a display. */
    WIFI_WPS_CONFIG_EXTERNAL_NFC_TOKEN      = 0x0010,  /**<  The NFC tag is used for WPS.  */
    WIFI_WPS_CONFIG_INTEGRATED_NFC_TOKEN    = 0x0020,  /**<  The NFC tag is integrated in the device.  */
    WIFI_WPS_CONFIG_NFC_INTERFACE           = 0x0040,  /**<  The device contains an NFC interface. */
    WIFI_WPS_CONFIG_PUSHBUTTON              = 0x0080,  /**<  WPS PBC supports either a physical or a virtual pushbutton. */
    WIFI_WPS_CONFIG_KEYPAD                  = 0x0100,  /**<  Device is capable of entering a PIN. */
    WIFI_WPS_CONFIG_VIRTUAL_PUSHBUTTON      = 0x0280,  /**<  WPS PBC supports software user interface. */
    WIFI_WPS_CONFIG_PHY_PUSHBUTTON          = 0x0480,  /**<  WPS PBC available on a physical push button of the device. */
    WIFI_WPS_CONFIG_VIRTUAL_DISPLAY         = 0x2008,  /**<  The dynamic PIN is displayed through a remote user interface, such as an HTML page. */
    WIFI_WPS_CONFIG_PHY_DISPLAY             = 0x4008   /**<  The dynamic PIN is shown on a display that is part of the device. */
} wps_config_method_t;

/**
* @}
*/

/**@defgroup WIFI_TYPEDEF Typedef
* @{
*/
/**
* @brief This defines the Wi-Fi event handler. Call #wifi_connection_register_event_handler() to register a handler, then the Wi-Fi driver generates an event and sends it to the handler.
* @param[in] event  is an optional event to register. For more details, please refer to #wifi_event_t.
*
* @param[in] payload  is the payload for the event.
*                     When the event is WIFI_EVENT_IOT_CONNECTED in AP mode, payload is the connected STA's MAC address.
*                     When the event is WIFI_EVENT_IOT_CONNECTED in STA mode, payload is the connected AP's BSSID.
* @param[in] length is the length of a packet.
*
* @return The return value is reserved and it is ignored.
*/
typedef int32_t (* wifi_event_handler_t)(wifi_event_t event, uint8_t *payload, uint32_t length);


/**
* @brief This is only for compatibility purpose, so that applications implemented with an earlier version of the API can work with the updated API.
*/
typedef wifi_event_handler_t wifi_event_handler;


/**
* @brief This defines the 802.11 RX raw packet handler. Call #wifi_config_register_rx_handler() to register a handler, then the network processor will deliver the packets to the handler function instead of sending them to the IP stack.
* @param[in] payload  is the packet payload.
* @param[in] length is the packet length.
*
* @return  1: handled, and the packet won't be processed by other tasks 0: not handled, it will be processed by other tasks.
*/
typedef int32_t (* wifi_rx_handler_t)(uint8_t *payload, uint32_t length);

/**
* @}
*/


/**@defgroup WIFI_STRUCT Structure
* @{
*/

/** @brief This structure is the Wi-Fi configuration for initialization in STA mode.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< The SSID of the target AP. */
    uint8_t ssid_length;                      /**< The length of the SSID. */
    uint8_t bssid_present;                    /**< The BSSID is present if it is set to 1. Otherwise, it is set to 0. */
    uint8_t bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< The MAC address of the target AP. */
    uint8_t password[WIFI_LENGTH_PASSPHRASE]; /**< The password of the target AP. */
    uint8_t password_length;                  /**< The length of the password. If the length is 64, the password is regarded as PMK. */
} wifi_sta_config_t;


/** @brief This structure is the Wi-Fi configuration for initialization in AP mode.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< The SSID of the AP. */
    uint8_t ssid_length;                      /**< The length of the SSID. */
    uint8_t password[WIFI_LENGTH_PASSPHRASE]; /**< The password of the AP. */
    uint8_t password_length;                  /**< The length of the password. */
    wifi_auth_mode_t auth_mode;               /**< The authentication mode. */
    wifi_encrypt_type_t encrypt_type;         /**< The encryption mode. */
    uint8_t channel;                          /**< The channel. */
    uint8_t bandwidth;                        /**< The bandwidth that is either #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ or #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ. */
    wifi_bandwidth_ext_t bandwidth_ext;       /**< The bandwidth extension. It is only applicable when the bandwidth is set to #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ. */
} wifi_ap_config_t;


/** @brief Wi-Fi configuration for initialization.
*/
typedef struct {
    uint8_t opmode;                          /**< The operation mode. The value should be #WIFI_MODE_STA_ONLY, #WIFI_MODE_AP_ONLY, #WIFI_MODE_REPEATER or #WIFI_MODE_MONITOR*/
    wifi_sta_config_t sta_config;            /**< The configurations for the STA. It should be set when the OPMODE is #WIFI_MODE_STA_ONLY or #WIFI_MODE_REPEATER. */
    wifi_ap_config_t ap_config;              /**< The configurations for the AP. It should be set when the OPMODE is #WIFI_MODE_AP_ONLY or #WIFI_MODE_REPEATER. */
} wifi_config_t;


/** @brief Wi-Fi extention configuration for initialization.
*/
typedef struct {
    uint32_t sta_wep_key_index_present: 1;        /**< Set to 1 to mark the presence of the sta_wep_key_index, set to 0, otherwise. */
    uint32_t sta_auto_connect_present: 1;         /**< Set to 1 to mark the presence of the sta_auto_connect, set to 0, otherwise. */
    uint32_t ap_wep_key_index_present: 1;         /**< Set to 1 to mark the presence of the ap_wep_key_index, set to 0, otherwise. */
    uint32_t ap_hidden_ssid_enable_present: 1;    /**< Set to 1 to mark the presence of the ap_hidden_ssid_enable, set to 0, otherwise. */
    uint32_t country_code_present: 1;             /**< Set to 1 to mark the presence of the country_code[4], set to 0, otherwise. */
    uint32_t sta_bandwidth_present: 1;            /**< Set to 1 to mark the presence of the sta_bandwidth, set to 0, otherwise. */
    uint32_t sta_wireless_mode_present: 1;        /**< Set to 1 to mark the presence of the sta_wireless_mode, set to 0, otherwise. */
    uint32_t sta_listen_interval_present: 1;      /**< Set to 1 to mark the presence of the sta_listen_interval, set to 0, otherwise. */
    uint32_t sta_power_save_mode_present: 1;      /**< Set to 1 to mark the presence of the sta_power_save_mode, set to 0, otherwise. */
    uint32_t ap_wireless_mode_present: 1;         /**< Set to 1 to mark the presence of the ap_wireless_mode, set to 0, otherwise. */
    uint32_t ap_dtim_interval_present: 1;         /**< Set to 1 to mark the presence of the ap_dtim_interval, set to 0, otherwise. */
    uint32_t reserved_bit: 21;                    /**< Reserved. */
    uint32_t reserved_word[3];                    /**< Reserved. */

    uint8_t sta_wep_key_index;                    /**< The WEP key index for STA. It should be set when the STA uses the WEP encryption. */
    uint8_t sta_auto_connect;                     /**< Set to 1 to enable the STA to automatically connect to the target AP after the initialization. Set to 0 to force the STA to stay idle after the initialization and to call #wifi_config_reload_setting() to trigger connection. The default is set to 1. */
    uint8_t ap_wep_key_index;                     /**< The WEP key index for AP. It should be set when the AP uses WEP encryption. */
    uint8_t ap_hidden_ssid_enable;                /**< Set to 1 to enable the hidden SSID in the beacon and probe response packets. The default is set to 0. */
    uint8_t country_code[4];                      /**< The country code setting. */
    uint8_t sta_bandwidth;                        /**< The bandwidth setting for STA. The value is either #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_20MHZ or #WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_40MHZ, or WIFI_IOT_COMMAND_CONFIG_BANDWIDTH_2040MHZ.*/
    wifi_phy_mode_t sta_wireless_mode;            /**< The wireless mode setting for STA. Please refer to the definition of #wifi_phy_mode_t.*/
    uint8_t sta_listen_interval;                  /**< The listening interval setting for STA. The interval range is from 1 to 255 beacon intervals.*/
    wifi_power_saving_mode_t sta_power_save_mode; /**< The power saving mode setting for STA. Please refer to the definition of #wifi_power_saving_mode_t.*/
    wifi_phy_mode_t ap_wireless_mode;             /**< The wireless mode setting for AP. Please refer to the definition of #wifi_phy_mode_t.*/
    uint8_t ap_dtim_interval;                     /**< The DTIM interval setting for AP. The interval range is from 1 to 255 beacon intervals. */

} wifi_config_ext_t;

#ifndef WIFI_HOST_ADV_SETTING_STRUCT
typedef struct  {
    uint8_t sta_mac[WIFI_MAC_ADDRESS_LENGTH];
    uint8_t ap_mac[WIFI_MAC_ADDRESS_LENGTH];
}wifi_host_adv_setting_t;
#define WIFI_HOST_ADV_SETTING_STRUCT
#endif

typedef struct {
    wifi_config_t wifi_config;
    wifi_config_ext_t wifi_config_ext;
    wifi_host_adv_setting_t wifi_host_adv_config;
} wifi_host_system_config_t;


/** @brief Wi-Fi WEP keys.
*/
typedef struct {
    uint8_t wep_key[WIFI_NUMBER_WEP_KEYS][WIFI_MAX_WEP_KEY_LENGTH];     /**< There are four WEP keys. */
    uint8_t wep_key_length[WIFI_NUMBER_WEP_KEYS];                       /**< WEP key length. */
    uint8_t wep_tx_key_index;                                           /**< Default key index for TX frames using WEP. */
} wifi_wep_key_t;


/** @brief Wi-Fi profile entity.
*/
typedef struct {
    uint8_t ssid[WIFI_MAX_LENGTH_OF_SSID];          /**< Stores the predefined SSID.  */
    uint8_t ssid_length;                            /**< The length of SSID.         */
    uint8_t mac_address[WIFI_MAC_ADDRESS_LENGTH];   /**< MAC address of the STA.     */
    uint8_t wireless_mode;                          /**< Please refer to the definition of #wifi_phy_mode_t. */
    uint8_t channel;                                /**< The channel used.                    */
    uint8_t bandwidth;                              /**< The bandwidth is 20MHz or 40MHz.     */
    uint8_t mcs;                                    /**< Defines the packet data rate.        */
    uint8_t auth_mode;                              /**< Wi-Fi authentication mode, please refer to the definition of #wifi_auth_mode_t.   */
    uint8_t encrypt_mode;                           /**< Wi-Fi encryption mode, please refer to the definition of #wifi_encrypt_type_t.*/
} wifi_profile_t;


/** @brief This defines the WPS element.
*/
typedef struct {
    bool selected_registrar;                        /**< Indicates whether AP WPS is triggered and in progress. 1 - triggered, 0 - not triggered. */
    wps_device_password_id_t device_password_id;    /**< Indicates AP WPS status, only when selected_registrar is 1, this parameter will take effect. Please refer to the definition of #wps_device_password_id_t. */
    wps_config_method_t configuration_methods;      /**< Indicates AP's capability to support WPS methods. Please refer to the definition of #wps_config_method_t.  */
} wifi_wps_element_t;


/** @brief This structure defines the list of scanned APs with their corresponding information.
*/
typedef struct {
    uint8_t  is_valid;                         /**< Indicates whether the scanned item is valid. */
    int8_t   rssi;                             /**< Records the RSSI value when probe response is received. */
    uint8_t  ssid[WIFI_MAX_LENGTH_OF_SSID];    /**< Stores the predefined SSID. */
    uint8_t  ssid_length;                      /**< Length of the SSID. */
    uint8_t  bssid[WIFI_MAC_ADDRESS_LENGTH];   /**< AP's MAC address. */
    uint8_t  channel;                          /**< The channel used. */
    uint8_t  central_channel;                  /**< The center channel in 40MHz bandwidth. */
    bool     is_wps_supported;                 /**< Indicates whether WPS is supported or not. 1: WPS is supported, 0: WPS is not supported. */
    wifi_auth_mode_t     auth_mode;            /**< Please refer to the definition of #wifi_auth_mode_t. */
    wifi_encrypt_type_t  encrypt_type;         /**< Please refer to the definition of #wifi_encrypt_type_t. */
    bool     is_hidden;                        /**< Indicates the SSID is hidden or not. 1: hidden SSID, 0: not hidden SSID. */
    uint8_t  bss_type;                         /**< Indicates the type of BSS. 0: IBSS 1: Infrastructure. */
    uint16_t beacon_interval;                  /**< Indicates the beacon interval. */
    uint16_t capability_info;                  /**< The Capability Information field contains a number of subfields that are used to indicate requested or advertised optional capabilities. */
    wifi_wps_element_t wps_element;            /**< Please refer to the definition of #wifi_wps_element_t. */
} wifi_scan_list_item_t;

/** @brief This structure defines a link to the scanned AP list.
*/
typedef struct {
    wifi_scan_list_item_t ap_data;  /**< The information about an AP obtained through the scan result is stored in this data member. The information for all scanned AP(s) is stored in the linked list. Please refer to the definition of #wifi_scan_list_item_t.*/
    struct _wifi_scan_list_t *next; /**< Next item in the list.*/
} wifi_scan_list_t;

/** @brief The RSSI sample is a measurement of the power in a received radio signal.
*/
typedef struct {
    int8_t          last_rssi;             /**< The latest received RSSI.     */
    uint8_t         reserved;              /**< Reserved.               */
    int16_t         average_rssi;          /**< 8 times of the average RSSI.*/
} wifi_rssi_sample_t;


/** @brief Specifies the Wi-Fi transmitter settings, including MCS, LDPC, bandwidth, short-GI algorithms, STBC and others.
*/
typedef union {
    struct {
        uint16_t mcs: 6;                      /**< Implies the data rate. */
        uint16_t ldpc: 1;                     /**< Checks if a low-density parity-check code is adopted. */
        uint16_t bandwidth: 2;                /**< Decides the bandwidth. */
        uint16_t short_gi: 1;                 /**< Checks if short-GI algorithms are applied. */
        uint16_t stbc: 1;                     /**< Checks if a space-time code is adopted.    */
        uint16_t external_tx_beam_forming: 1; /**< Checks if an external buffer is adopted.   */
        uint16_t internal_tx_beam_forming: 1; /**< Checks if an internal buffer is adopted.   */
        uint16_t mode: 3;
    } field;                                 /**< Structure name for the Wi-Fi transmitter settings. */
    uint16_t word;                           /**< Defines the length of the union data. */
} wifi_transmit_setting_t;

/** @brief Specifies the Wi-Fi station items, including the Wi-Fi transmitter settings, the RSSI sample, MAC address, power saving mode, bandwidth and keep alive status.
*/
typedef struct {
    wifi_transmit_setting_t last_tx_rate;            /**< The latest TX rate.  */
    uint16_t reserved_one;                           /**< For padding.   */
    wifi_transmit_setting_t last_rx_rate;            /**< The latest RX rate.  */
    uint16_t reserved_two;                           /**< For padding.   */
    wifi_rssi_sample_t rssi_sample;                  /**< The RSSI sample.   */
    uint8_t mac_address[WIFI_MAC_ADDRESS_LENGTH];    /**< Station MAC address.     */
    uint8_t power_save_mode;                         /**< Station power save mode. */
    uint8_t bandwidth;                               /**< Negotiation bandwidth.   */
    uint8_t keep_alive;                              /**< Keep alive status.       */
    uint8_t reserved[3];                             /**< For padding.             */
} wifi_sta_list_t;

/** @brief This structure provides the detailed cipher information.
*/
typedef struct {
    uint8_t   key[16];            /**< Four keys are implemented, with a maximum length of 128bits.*/
    uint8_t   rx_mic[8];          /**< 8 bytes of RX MIC key.  */
    uint8_t   tx_mic[8];          /**< 8 bytes of TX MIC key. */
    uint8_t   tx_tsc[6];          /**< 48bit TSC value. */
    uint8_t   rx_tsc[6];          /**< 48bit TSC value. */
    uint8_t   cipher_type;        /**< 0:none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128.*/
    uint8_t   key_length;         /**< Key length for each key, 0: entry is invalid.*/
    uint8_t   bssid[6];           /**< AP's MAC address. */
    uint8_t   type;               /**< Indicates the type (pairwise or group) when reporting a MIC error.*/
} wifi_cipher_key_t;

/** @brief This structures provide the country code information.
*/
typedef struct {
    uint8_t first_channel;        /**< The first channel in the single channel table.*/
    uint8_t num_of_channel;       /**< The number of channels in the single channel table.*/
    uint8_t max_tx_power;         /**< The maximum transmit power in the single channel table, the unit is dBm.*/
    uint8_t geography;            /**< Not supported.*/
    bool    dfs_req;              /**< 0: No DFS request, 1: DFS request.*/
} wifi_channel_table_t;

typedef struct {
    uint8_t country_code[4];                /**< Country code string.*/
    uint8_t dfs_type;                       /**< Not supported.*/
    uint8_t num_of_channel_table;           /**< Number of channel table.*/
    wifi_channel_table_t channel_table[];   /**< The details of each channel table, please refer to #wifi_channel_table_t.*/
} wifi_country_code_t;

/**
*@}
*/



/**
* @brief This function initializes the Wi-Fi module.
*
* @param[in] wifi_host_cfg is the Wi-Fi configuration to be set, it should not be null.
*
* @note Call this function only once at the initialization stage.
*/
int32_t wifi_host_config_init(wifi_host_system_config_t *pwifi_host_cfg);

/**
* @brief This function receives the wireless operation mode of the Wi-Fi driver.
*
* @param[out]  mode indicates the operation mode.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_get_opmode(uint8_t *mode);

/**
* @brief This function sets the Wi-Fi operation mode and it takes effect immediately.
*
* @param[in] mode the operation mode to set.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0x01                       | #WIFI_MODE_STA_ONLY|
* \b 0x02                       | #WIFI_MODE_AP_ONLY|
* \b 0x03                       | #WIFI_MODE_REPEATER|
* \b 0x04                       | #WIFI_MODE_MONITOR|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note This function resets the Wi-Fi driver configuration to default values of the target's operation mode (including supplicant and firmware) previously set by Wi-Fi Configuration APIs.
* @note Once the Wi-Fi operation mode is set to station mode, the device will switch to idle state and will try to connect to the AP
* @note only after calling #wifi_host_config_reload_setting().
*/
int32_t wifi_host_config_set_opmode(uint8_t mode);

/**
* @brief This function gets the MAC address of a specific wireless port used by the Wi-Fi driver.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  address is the MAC address. The length of the MAC address is #WIFI_MAC_ADDRESS_LENGTH in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note To get the MAC address, follow these steps:
*       1. Read eFuse from address 0x00, take bytes 4 to 9 as the STA MAC address and generate AP address from it by increasing the last byte by 1.
*       2. If NVDM is enabled, read NVDM from group "STA" and key "MacAddr" for STA MAC address and from group "AP" and key "MacAddr" for AP MAC address.
*          If the MAC address is retrieved successfully in Step 1, skip Step 2.
*          If no information is set in eFuse and NVDM, this function will return an error and consequently the Wi-Fi driver initialization will possibly fail, too.
*
*/
int32_t wifi_host_config_get_mac_address(uint8_t port, uint8_t *address);

/**
* @brief This function gets the SSID and SSID length of a specific wireless port used by the Wi-Fi driver.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[out]  ssid is the SSID content. In STA mode, the SSID is the target AP's SSID;\n
*                       In SoftAP mode, the SSID indicates the device's own SSID.
* @param[out]  ssid_length is the length of the SSID. The maximum length is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_get_ssid(uint8_t port, uint8_t *ssid, uint8_t *ssid_length);

/**
* @brief This function sets the SSID and SSID length that the Wi-Fi driver uses for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called.
*
* @param[in] port indicates the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] ssid is the SSID content. If #WIFI_PORT_STA or #WIFI_PORT_APCLI, the SSID indicates the target AP's SSID.
*                       If WIFI_PORT_AP, the SSID indicates the device's own SSID.
* @param[in] ssid_length is the length of the SSID, the maximum length is #WIFI_MAX_LENGTH_OF_SSID in bytes.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_ssid(uint8_t port, uint8_t *ssid, uint8_t ssid_length);

/**
* @brief This function sets the authentication and encryption modes used in the Wi-Fi driver for a specific wireless port.
* This operation only takes effect after #wifi_host_config_reload_setting() is called.
*
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] auth_mode is the authentication mode. For details about this parameter, please refer to
* #wifi_auth_mode_t.
* @param[in] encrypt_type is the encryption mode. For details about this parameter, please refer to
* #wifi_encrypt_type_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note In station mode, the device can connect to AP without setting any security mode.\n
*       If #wifi_host_config_set_security_mode() is not called, the device will change security mode dynamically according to the target AP.\n
*       If #wifi_host_config_set_security_mode() is called, the device will set a security mode to connect to the target AP.\n
*       Calling #wifi_host_config_set_security_mode() is optional in station mode.\n
* To skip the security mode setting, simply do not call #wifi_host_config_set_security_mode() before #wifi_host_config_reload_setting().
*/
int32_t wifi_host_config_set_security_mode(uint8_t port, wifi_auth_mode_t auth_mode, wifi_encrypt_type_t encrypt_type);

/**
* @brief This function sets the password used in the Wi-Fi driver for a specific wireless port.
* @param[in] port is the Wi-Fi port that the function will operate on.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b #WIFI_PORT_STA             | STA|
* \b #WIFI_PORT_APCLI           | AP Client|
* \b #WIFI_PORT_AP              | AP|
*
* @param[in] password is the password of AP.
*
* @param[in] password_length is the password length of AP.
*
* @param[in] wep_key_index is the wep key index of WEP AP. For other kinds of AP, set it to -1.
*
* @return >=0 the operation completed successfully, <0 the operation failed.
*/

int32_t wifi_host_config_set_password(uint8_t port, uint8_t *password, uint8_t password_length, uint8_t wep_key_index);


/**
* @brief This function informs the wpa_supplicant to reload the configuration and applies the
* configuration settings of the Wi-Fi Configuration APIs. This function is applied to
* the following APIs: #wifi_host_config_set_ssid(), #wifi_host_config_set_security_mode(),
* #wifi_host_config_set_wpa_psk_key(), #wifi_host_config_set_wep_key() and  #wifi_host_config_set_pmk().
* In wireless station mode, the device will use the new configuration and start to scan
* and connect to the target AP router. In wireless AP mode, device will load the new
* SSID and encryption information.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_reload_setting(void);


/**
* @brief This function disconnects the current connection from AP. It takes effect immediately and the device
* switches to idle state. After calling #wifi_host_config_reload_setting(), the device switches to scan state and tries to connect to an AP router with an existing Wi-Fi
* configuration. This API should be used only in the station mode.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*/
int32_t wifi_host_connection_disconnect_ap(void);



/**
* @brief This function gets the current STA port's link up or link down connection status.
*
* @param[out]  link_status indicates the current STA port's link up or link down connection status.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | #WIFI_STATUS_LINK_DISCONNECTED|
* \b 1                          | #WIFI_STATUS_LINK_CONNECTED|
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note #WIFI_STATUS_LINK_DISCONNECTED indicates station may be in
*       IDLE/ SCAN/ CONNECTING state.
*/
int32_t wifi_host_connection_get_link_status(uint8_t *link_status);

/**
* @brief This function initializes the scan table to the driver.
*
* @param[in] ap_list is a pointer of the user buffer where the further scanned AP list will be stored.
* @param[in] max_count is the maximum number of ap_list can be stored.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note #wifi_host_connection_scan_init() should be called before calling #wifi_host_connection_start_scan(), and it should be called only once to initialize one scan.\n
*       When the scan is done, the scanned AP list is already stored in parameter ap_list with descending order of the RSSI values.
*/
int32_t wifi_host_connection_scan_init(wifi_scan_list_item_t *ap_list, uint32_t max_count);

/**
* @brief This function deinitializes the scan table.
*
* @return   >=0 the operation completed successfully, <0 the operation failed.
*
* @note When the scan is finished, #wifi_host_connection_scan_deinit() should be called to unload the buffer from the driver. After that, the data in the parameter ap_list can be safely processed by user applications, and then another scan can be initialized by calling #wifi_host_connection_scan_init().
*/
int32_t wifi_host_connection_scan_deinit(void);

/**
* @brief This function starts Wi-Fi Scanning.
*
* @param[in] ssid specifies the SSID to be included in the probe request packet for scanning the hidden AP. If the SSID is NULL, the SSID field in probe request packet will be NULL.
* @param[in] ssid_length specifies the length of the SSID.
* @param[in] bssid specifies the BSSID of the AP to be scanned. If the BSSID is specified, the unicast probe request is sent. If the BSSID is NULL, then the broadcast probe request is sent.
* @param[in] scan_mode is the scan mode that can be either a full scan or a partial scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Full|
* \b 1                          | Partial, it's expected to be used in the SoftAP mode and keep stations online.|
*
* In the partial scan mode, the scanning is performed as follows.
* If the AP is currently on channel 5, it jumps to channels (1, 2),
* and then back to channel 5. After that it scans the channels (3, 4).
* The number of channels it scans each time is specified in the implementation.
* @param[in] scan_option selects scan options based on one of the following: active scan, passive scan or force active scan.
*
* Value                         |Definition                                                              |
* ------------------------------|------------------------------------------------------------------------|
* \b 0                          | Active scan, (Passive in regulatory channels). The channels that will be scanned are decided by the product's country and region. A probe request will be sent to each of these channels.|
* \b 1                          | Passive in all channels. Receives and processes the beacon. No probe request is sent.|
* \b 2                          | Force active (active in all channels). Forced to active scan in all channels. A probe request will be sent to each of these channels.|
*
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note 1. No matter what the SSID and BSSID parameters are, the device will receive all beacon and probe response packets over the air, \n
* and record the result into ap_list buffer registered by #wifi_host_connection_scan_init().
* @note 2. The station mode supports only the full scan mode.
* @note 3. The AP/repeater/p2p GO mode supports only the partial scan mode.
*/
int32_t wifi_host_connection_start_scan(uint8_t *ssid, uint8_t ssid_length, uint8_t *bssid, uint8_t scan_mode, uint8_t scan_option);

/**
* @brief This function stops the Wi-Fi Scanning triggered by #wifi_host_connection_start_scan(). If the device in station mode cannot connect to an AP, it keeps scanning till it connects to the target AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_stop_scan(void);

/**
* @brief This function gets the RSSI of the connected AP. It's only used for the STA mode and while the station is connected to the AP.
*
* @param[out]  rssi returns the RSSI of the connected AP.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
*
*/
int32_t wifi_host_connection_get_rssi(int8_t *rssi);

/**
* @brief This function registers the Wi-Fi event handler. Each event can register multiple event handlers with maximum number of 16.
* In the AP/STA mode, the scan stops automatically when the scan cycle is finished
* and SCAN COMPLETE event is triggered;\n
* In the AP mode, once the device is disconnected from the station,  a DISCONNECT event with station's
* MAC address is triggered;\n
* In the AP mode, once the station connects to the device, CONNECT event with station's
* MAC address is triggered;\n
* In the STA mode, once the device disconnects from the AP, DISCONNECT event with BSSID is triggered;\n
* In the STA mode, once the device connect to the AP, CONNECT event with BSSID is triggered.\n
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*
* @note  It is not recommended to call any other Wi-Fi APIs or sleep in any form in the event handler.
*/
int32_t wifi_host_connection_register_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
* @deprecated This function is deprecated! This is only for compatibility purposes, so that applications implemented with an earlier version of the API can work with the updated API. Please use #wifi_host_connection_register_event_handler() to replace this function.
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] notifier is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_register_event_notifier(uint8_t event, wifi_event_handler_t notifier);

/**
* @brief This function unregisters Wi-Fi event handler. The function #wifi_host_connection_register_event_handler() registers an event and matches it with the corresponding event handler. For the event behavior, please refer to #wifi_host_connection_register_event_handler().
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t for more details.
*
* @param[in] handler is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_unregister_event_handler(wifi_event_t event, wifi_event_handler_t handler);

/**
* @deprecated This function is deprecated! This is only for compatibility purposes, so that applications implemented with an earlier version of the API can work with the updated API. Please use #wifi_host_connection_unregister_event_handler() to replace this function.
*
* @param[in] event is the event ID. For more details, please refer to #wifi_event_t.
*
* @param[in] notifier is the event handler. For more details, please refer to #wifi_event_handler_t.
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_connection_unregister_event_notifier(uint8_t event, wifi_event_handler_t notifier);


/**
* @brief This function starts wifi module. \n
*
* @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_start(void);


/**
* @brief This function stops wifi module. \n
*
** @return  >=0 the operation completed successfully, <0 the operation failed.
*/
int32_t wifi_host_config_set_wifi_stop(void);



#ifdef __cplusplus
}
#endif

/**
*@}
*/
#endif /* __WIFI_HOST_API_H__ */

