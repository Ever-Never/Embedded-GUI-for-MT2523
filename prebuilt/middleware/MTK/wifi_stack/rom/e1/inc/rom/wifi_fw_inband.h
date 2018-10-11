
#include "rom/wifi_fw.h"

//#include "wifi_fw_int.h"
//#include "rt_config.h"
#include "cm4_type.h"
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WIFI_FW_INBAND_H__
#define __WIFI_FW_INBAND_H__


/**
 * The following fields can be kept in a response buffer:
 * 1. length
 * 2. extended id
 * 3. extended options
 * 4. sequence number
 * 5. payload
 */
typedef struct wifi_inband_response_header_s
{
    uint16_t    len;
    uint8_t     ext_id;
    uint8_t     ext_id_opt;
    uint8_t     seq_no;
} wifi_inband_response_header_t;
typedef struct wifi_inband_response_s
{
	wifi_inband_response_header_t hdr;
	uint8_t data[0];
} wifi_inband_response_t;

#define FIXED_PARM_ASSOC_REQ		4
#define FIXED_PARM_ASSOC_RESP		6
#define FIXED_PARM_REASSOC_REQ		10
#define FIXED_PARM_REASSOC_RESP		6
#define FIXED_PARM_PROBE_REQ		0
#define FIXED_PARM_PROBE_RESP		12
#define FIXED_PARM_BEACON			12
#define FIXED_PARM_ATIM				0
#define FIXED_PARM_DISASSOC			2
#define FIXED_PARM_AUTH				6
#define FIXED_PARM_DEAUTH			2
typedef struct _EXTRA_IE_MGMT_PKT_T 
{
       UINT8   element_id;    /*221:  vendor ie,     others: reserved*/
       UINT8   length;        /*reserved*/
       UINT8   Out[3];            
} EXTRA_IE_MGMT_PKT_T;
extern UINT_16 g_fwd_mgmt_FType;
extern UINT_16 g_fwd_mgmt_ie_check;
extern EXTRA_IE_MGMT_PKT_T g_eIE;
typedef struct _airplay_ie {
	UINT8 *beacon_ie;
	UINT8 beacon_ie_len;
	UINT8 EN;
} airplay_ie_t;

#if (MT7687_BORA00005211 == 1)
extern airplay_ie_t* (*airplay_ie_get)(void);
#define AIRPLAY_ON(x) ((x->EN == 1) && (x->beacon_ie_len))
#endif

#define WIFI_INBAND_RESP_PAYLOAD_MAX    (296)

#define WIFI_INBAND_RESP_MAX            (WIFI_INBAND_RESP_PAYLOAD_MAX + \
                                         sizeof(wifi_inband_response_header_t))

#if 1
/****************************************************************************
 *
 * eFuse API
 *
 ****************************************************************************/

#define EFUSE_CMD_GET_FREE             (0)
#define EFUSE_CMD_GET_EFUSE            (1)
#define EFUSE_CMD_SET_EFUSE            (2)
#define EFUSE_CMD_EVAL_EFUSE           (3)

#define _EFUEE_BLOCK_LENGTH             (16)

/**
 * The request data structure for EFUSE_CMD_GET_FREE.
 */
__packed struct efuse_getfree_req_s {
    uint16_t    command:  3;
    uint16_t    rsvd:    13;
};
typedef struct efuse_getfree_req_s efuse_getfree_req_t;


/**
 * The reply data structure for EFUSE_CMD_GET_FREE.
 */
__packed struct efuse_getfree_rep_s {
    uint8_t     len;
    uint8_t     free_blocks;
};
typedef struct efuse_getfree_rep_s efuse_getfree_rep_t;


/**
 * The request data structure for EFUSE_CMD_GET_EFUSE.
 */
__packed struct efuse_get_req_s {
    uint16_t    command:  3;
    uint16_t    space:    1; // 0: physical, 1: logical
    uint16_t    address: 12;
};
typedef struct efuse_get_req_s efuse_get_req_t;


/**
 * The reply data structure for EFUSE_CMD_GET_EFUSE.
 */
__packed struct efuse_get_rep_s {
    uint8_t     len;
    uint8_t     data[_EFUEE_BLOCK_LENGTH];
};
typedef struct efuse_get_rep_s efuse_get_rep_t;


/**
 * The request data structure for EFUSE_CMD_SET_EFUSE.
 *
 * @note There is no reply for EFUSE_CMD_SET_EFUSE request.
 */
__packed struct efuse_set_req_s {
    uint16_t    command:  3;
    uint16_t    space:    1; // 0: physical, 1: logical
    uint16_t    address: 12;
    uint8_t     data[_EFUEE_BLOCK_LENGTH];
    uint8_t     len;
};
typedef struct efuse_set_req_s efuse_set_req_t;


/**
 * The request data structure for EFUSE_CMD_EVAL_EFUSE.
 */
typedef struct efuse_set_req_s efuse_eval_req_t;


/**
 * The reply data structure for EFUSE_CMD_EVAL_EFUSE.
 */
__packed struct efuse_eval_rep_s {
    uint8_t     len;
    uint8_t     ok;
    uint8_t     used;
    uint8_t     noop;
    uint8_t     reuse;
};
typedef struct efuse_eval_rep_s efuse_eval_rep_t;

#endif
#define _8_BITS(__i__, __n__) (((__i__) >> (__n__)) & 0xFF)

#define PUT_BYTES_LITTLE_ENDIAN(_ptr_, _i_) \
    do {                                    \
        (_ptr_)[0] = _8_BITS(_i_,  0);      \
        (_ptr_)[1] = _8_BITS(_i_,  8);      \
        (_ptr_)[2] = _8_BITS(_i_, 16);      \
        (_ptr_)[3] = _8_BITS(_i_, 24);      \
    } while (0)
                                         
typedef struct PACKED wifi_power_state_ctrl{
	UINT_8 power_manage_number;
	UINT_8 power_manage_state;
} wifi_power_state_ctrl_t;

typedef struct _WIFI_CMD_T
{
    /* DW#0 */
    // uint16_t     u2Length;
    uint16_t     u2TxByteCount;
    uint16_t     u2PQ_ID;            /* Must be 0x8000 (Port1, Queue 0) */

    /* DW#1 */
    uint8_t      ucCID;
    uint8_t      ucPktTypeID;        /* Must be 0xA0 (CMD Packet by long format) */
   // uint8_t      ucSetQuery;
    uint8_t	 ucReserved;
    uint8_t      ucSeqNum;

    /* DW#2 */
    uint8_t      ucD2B0Rev;          /* padding fields, hw may auto modify this field */
    uint8_t      ucExtenCID;         /* Extend CID for MT7603 */
    uint8_t      ucD2B2Rev;          /* padding fields, hw may auto modify this field */
    uint8_t      ucExtCmdOption;     /* Extend CID option */

    /* DW#3-7 */
    // uint32_t     au4D3toD7Rev[5];    /* padding fields */

    uint8_t      aucBuffer[0];
} wifi_cmd_t, WIFI_CMD_T, *P_WIFI_CMD_T;

typedef wlan_status_t (*iot_cmd_func_t)(wifi_fw_t *fw, wifi_cmd_t *request, wifi_inband_response_t *response);
extern iot_cmd_func_t g_hif_cmd_table[];
#define IS_HIF_CMD(__cmd__)   (((__cmd__) & 0xF0) == 0xD0)
#define GET_HIF_FUNC(__cmd__) (g_hif_cmd_table[ (__cmd__ & 0x0F) ])

extern int8_t (*wifi_fw_inband_early_eject)(uint8_t cmd_id);
#endif /* __WIFI_FW_INBAND_H__ */

#ifdef __cplusplus
}
#endif
