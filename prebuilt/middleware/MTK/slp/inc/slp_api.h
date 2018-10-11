#ifndef __SLP_API_H__
#define __SLP_API_H__

/*******************************************
*         DO NOT CHANGE THIS FILE          *
*  this is interface and callback function *
*  to/from SLPD for FLPD, AGPSD, and SLPD  *
*******************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define SCLP_CMD_BUFF_SIZE 1024
#define SCLP_CALLBACK_BUFF_SIZE 1024
#define SCLP_CMD_SOCKET_PATH "mtk_slp_cmd_socket"
#define SCLP_CALLBACK_SOCKET_PATH "mtk_slp_callback_socket"
#define SCLP_MAX_CELL_SIZE 10

typedef enum {
    SCLP_CMD_START = 0,
    SCLP_CMD_START_2,
    SCLP_CMD_START_3,
    SCLP_CMD_DNS_RESULT,
    SCLP_CMD_TIMEOUT,
    SCLP_CMD_CONFIG,
    SCLP_CMD_RESET,
} sclp_cmd_enum;

typedef enum {
    SCLP_CALLBACK_REBOOT = 0,
    SCLP_CALLBACK_LOCATION_RESULT,
} sclp_callback_enum;

typedef enum {
    SCLP_CALLER_FLPD = 0,
    SCLP_CALLER_AGPSD,
    SCLP_CALLER_SLPD,
    SCLP_CALLER_COUNT,
} sclp_caller;

typedef enum {
    SCLP_RESULT_SUCCESS = 0,
    SCLP_RESULT_ERROR = -1,
    SCLP_RESULT_ERROR_WAIT_UPDATE = -2,
    SCLP_RESULT_ERROR_INVALID_PARAM = -3,
    SCLP_RESULT_ERROR_QUEUE_IS_FULL = -4,
    SCLP_RESULT_ERROR_DNS_FAIL = -5,
    SCLP_RESULT_ERROR_CREATE_SOCKET_FAIL = -6,
    SCLP_RESULT_ERROR_IN_SOCKET_CONNECTED = -7,
    SCLP_RESULT_ERROR_IN_SUPL = -8,
    SCLP_RESULT_ERROR_IN_TCP = -9,
    SCLP_RESULT_ERROR_IN_TLS = -10,
    SCLP_RESULT_ERROR_SOCKET_CONNECT_TIMEOUT = -11,
    SCLP_RESULT_ERROR_TLS_HANDSHAKING_TIMEOUT = -12,
    SCLP_RESULT_ERROR_SUPL_UT1_TIMEOUT = -13,
    SCLP_RESULT_ERROR_SUPL_UT2_TIMEOUT = -14,
    SCLP_RESULT_ERROR_SUPL_UT3_TIMEOUT = -15,
    SCLP_RESULT_ERROR_NO_POSITION = -16,
} sclp_result_enum;

typedef enum {
    SCLP_TLS_VER_TLS_1_0 = 0,
    SCLP_TLS_VER_TLS_1_1,
    SCLP_TLS_VER_TLS_1_2,
} sclp_tls_ver_enum;

typedef enum {
    SCLP_CELL_GSM = 0,
    SCLP_CELL_WCDMA,
    SCLP_CELL_LTE,
    SCLP_CELL_C2K,
} sclp_cell_enum;

typedef struct {
    short mcc; //999
    short mnc; //999
    unsigned short lac; //65535
    unsigned short cid; //65535
} sclp_gsm_cell;

typedef struct {
    short mcc; //999
    short mnc; //999
    unsigned short lac; //65535
    int uc; //268435455
} sclp_wcdma_cell;

typedef struct {
    short mcc; //999
    short mnc; //999
    unsigned short tac; //65535
    int ci; //268435455
    short pci; //503
} sclp_lte_cell;

typedef struct {
    unsigned short sid; //32767
    unsigned short nid; //65535
    unsigned short bid; //65535
} sclp_cdma_cell;

typedef struct {
    sclp_cell_enum cell_type;
    union {
        sclp_gsm_cell gsm_cell;
        sclp_wcdma_cell wcdma_cell;
        sclp_lte_cell lte_cell;
        sclp_cdma_cell cdma_cell;
    } cell;
} sclp_cell_info;

typedef struct {
    int index; //0..(SCLP_MAX_CELL_SIZE-1)
    sclp_result_enum result;
    float lat; //in degree
    float lng; //in degree
    int alt; //meter
    int acc; //meter
} sclp_cell_location;

typedef struct {
    sclp_caller caller;
    int session_id;
    int num;
    sclp_cell_info cells[SCLP_MAX_CELL_SIZE];
} sclp_cell_info_list;

typedef struct {
    int session_id;
    int num;
    sclp_cell_location locations[SCLP_MAX_CELL_SIZE];
} sclp_cell_location_list;

typedef struct {
    char addr[128];
    int port;
    char tls_enable;
} sclp_supl_profile;

typedef struct {
    char has_profile;
    sclp_supl_profile profile;

    char has_supl_version;
    int supl_version;   // 1=SUPL1.0 2=SUPL2.0

    char has_tls_version;
    sclp_tls_ver_enum tls_version;
    const char *cert_file;
    const char *peer_name;
} sclp_config;


typedef struct {
    void (*slp_reboot)();
    void (*slp_location_result)(sclp_cell_location_list* result);
} sclp_callbacks;

void dump_sclp_cell_info_list(sclp_cell_info_list* list);
void dump_sclp_cell_location_list(sclp_cell_location_list* list);

//-1 means failure
int slp_bind_callback(sclp_caller caller);

void slp_callback_hdlr(int fd, sclp_callbacks* callbacks);

//-1 means failure
int slp_start(sclp_cell_info_list* list);

//-1 means failure
int slp_start2(const char* addr, int port, int tls_enabled, sclp_cell_info_list* list);

//-1 means failure
int slp_start3(sclp_config* config, sclp_cell_info_list* list);

//-1 means failure
int slp_config_profile(const char* addr, int port, char tls_enable);

//-1 means failure
int slp_config_supl_version(int version);

//-1 means failure
int slp_config_tls_version(int version);

//-1 means failure
int slp_config(sclp_config* config);

//-1 means failure
int slp_reset();

void slp_location_result_callback(sclp_cell_location_list* result);

#ifdef __cplusplus
}
#endif

#endif
