
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __WIFI_HAL_INT_H__
#define __WIFI_HAL_INT_H__

typedef void (*wifi_hal_int_callback_t)(void *user_data);

typedef enum {
    WIFI_INT_ID_PSE,
    WIFI_INT_ID_MAX
} wifi_int_id_t;

void wifi_hal_int_register_callback(wifi_int_id_t           int_id,
                                    wifi_hal_int_callback_t callback,
                                    void                    *user_data);

void wifi_hal_int_enable(wifi_int_id_t int_id);

/** disable the interrupt */
void wifi_hal_int_disable(wifi_int_id_t int_id);

/** simulate the interrupt has occurred */
void wifi_hal_int_fire(wifi_int_id_t int_id);

#endif

#ifdef __cplusplus
}
#endif
