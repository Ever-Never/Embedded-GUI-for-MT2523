#ifndef __HR_APP_H__
#define __HR_APP_H__

#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"

#include "queue.h"
#include "task.h"
#include "bt_gattc_connect.h"

#define SRV_HR_SERVICE_MAX_NUM  2

#define SRV_HRM_SER_UUID    0x180D

/*heartrate service's characteristics*/
typedef enum {
    HEART_RATE_MEASURE,
    BODY_SENSOR_LOCATION,
    HRART_RATE_CNT_POINT,
    HRART_RATE_TYPE_TOTAL
} srv_hrsp_char_type_enum;


/*device inforamtion saved in app*/
typedef struct {
    void *reg_ctx;
} app_hrp_context_struct;


/*characteristic */
typedef enum {
    HRM_CHAR_UUID = 0x2A37,/*only need change this char*/
    HBL_CHAR_UUID,
    HCP_CHAR_UUID,
    HR_CHAR_TOTAL
} char_uuid_enum;

typedef enum {
    CLIENT_CONFI_DESCRI = 0x2902,
    CLIENT_CONFI_TOTAL
} desc_uuid_enum;

/*heat rate's register*/
void heart_rate_init(void);

/*heat rate's deregister*/
void heart_rate_deinit(void);


typedef struct {
    bt_msg_type_t event_id;
    bt_status_t status;
    int8_t param[512];
} hear_rate_message_struct;
extern hear_rate_message_struct app_queue_data;
extern QueueHandle_t app_queue;


#endif /*__HR_APP_H__*/
