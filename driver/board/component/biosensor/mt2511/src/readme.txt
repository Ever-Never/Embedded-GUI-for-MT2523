Bio sensor(mt2511) module usage guide

Brief:          This module is used to enable/disable bio sensor.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/driver/board/component/biosensor/mt2511/src/module.mk" in your GCC project Makefile.
                KEIL/IAR: Drag the driver/board/component/biosensor/mt2511 folder to your project and define the MTK_SENSOR_BIO_USE and MTK_SENSOR_BIO_USE_MT2511 macro. 
				Add driver/board/component/biosensor/mt2511/inc to INCLUDE_PATH.
Dependency:     HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
Notice:         None.
Relative doc:   Please refer to API_Reference under the driver/board/component/biosensor/mt2511 folder for more detail.
Example project:Please find the iot_clover project under project folder. 