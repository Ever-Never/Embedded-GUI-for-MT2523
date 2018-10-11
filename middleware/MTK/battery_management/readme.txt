Battery Management module usage guide

Brief:          This module is the battery management implementation.
                The battery management support battery charging and provide the battery information such as capacity, temperature, etc.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/battery_management/module.mk" in your GCC project Makefile.
                KEIL: Drag the middleware/MTK/battery_management folder to your project.
                      Include the prebuilt/middleware/MTK/fuelgauge/lib/libfgauge.lib in your project.
                      Add middleware/MTK/battery_management/inc and prebuilt/middleware/MTK//fuelgauge/inc/ to INCLUDE_PATH.
                IAR: Drag the middleware/MTK/battery_management folder to your project.
                     Include the prebuilt/middleware/MTK/fuelgauge/lib/libfgauge_IAR.a in your project.
                     Add middleware/MTK/battery_management/inc and prebuilt/middleware/MTK/fuelgauge/inc/ to INCLUDE_PATH.
Dependency:     HAL_CHARGER_MODULE_ENABLED,HAL_SLEEP_MANAGER_ENABLED,HAL_RTC_MODULE_ENABLED,HAL_GPT_MODULE_ENABLED,HAL_PMU_MODULE_ENABLED,HAL_PMU_AUXADC_MODULE_ENABLED
                must also defined in the hal_feature_config.h under the project inc folder.
                Please also refer each module dependency.
Notice:         Please ensure the following files are in the project inc folder.
                Such as:cust_charging.h,cust_battery_meter.h,cust_battery_meter_table.h,cust_battery_temperature_table.h
                Please ensure the following MACROs and corresponding values are defined in the project task_def.h file.
                #define BMT_TASK_NAME "BMT"
                #define BMT_TASK_STACKSIZE 4096
                #define BMT_TASK_PRIO TASK_PRIORITY_ABOVE_NORMAL
                #define BMT_QUEUE_LENGTH 1

Relative doc:   None.
Example project:Please find the project under project folder with battery_management_ prefix.
