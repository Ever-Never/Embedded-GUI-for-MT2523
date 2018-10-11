Sensor subsystem module usage guide

Brief:          This module manages the buffer, the registration of physical sensor driver and fusion algorithm and the subscription from applications.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/sensor_subsys/module.mk" in your GCC project Makefile.
                KEIL/IAR: Drag the middleware\MTK\sensor_subsys folder and required sensor driver (e.g. driver\board\component\mems and driver\board\component\biosensor) to your project,
                and define the macros for the required sensors, e.g. MTK_SENSOR_ACCELEROMETER_USE, MTK_SENSOR_ACCELEROMETER_USE_BMA255, MTK_SENSOR_BIO_USE, MTK_SENSOR_BIO_USE_MT2511, 
                FUSION_HEART_RATE_MONITOR_USE=M_INHOUSE_HEART_RATE_MONITOR, FUSION_HEART_RATE_VARIABILITY_USE=M_INHOUSE_HEART_RATE_VARIABILITY. 
                Add middleware\MTK\sensor_subsys\inc and required sensor inclusion path (e.g.driver\board\component\mems\inc, driver\board\component\biosensor\mt2511\inc, 
                driver\board\component\biosensor\mt2511\module\default, driver\board\component\biosensor\mt2511\module\everlight, 
                middleware\MTK\sensor_subsys\lib\fusion_algo\agc\inc, middleware\MTK\sensor_subsys\lib\fusion_algo\common\inc, middleware\MTK\sensor_subsys\lib\fusion_algo\heart_rate\inc\,
                middleware\MTK\sensor_subsys\lib\fusion_algo\heart_rate_variability\inc\, and middleware\MTK\sensor_subsys\lib\fusion_algo\ppg_control\inc)
                to include paths.
Dependency:     HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
Notice:         None.
Relative doc:   Please refer to the sensor subsystem API Reference for more detail.
Example project:Please find the project under project folder with sensor_subsys_ prefix.
