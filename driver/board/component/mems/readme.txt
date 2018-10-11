mems (sensor driver) module usage guide

Brief:          This module enables/disables, configures, and gets data from Microelectromechanical Systems (MEMS) physical sensors.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/driver/board/component/mems/module.mk" in your GCC project Makefile.
                KEIL/IAR: Drag the driver\board\component\mems folder to your project, 
                and define the macros for the required sensors, e.g. MTK_SENSOR_ACCELEROMETER_USE, MTK_SENSOR_ACCELEROMETER_USE_BMA255, MTK_SENSOR_BIO_USE, and MTK_SENSOR_BIO_USE_MT2511.
                Add driver\board\component\mems\inc and middleware\MTK\sensor_subsys\inc to include paths.
Dependency:     HAL_SPI_MASTER_MODULE_ENABLED, HAL_I2C_MASTER_MODULE_ENABLED (dependent on the sensor hardware connection), and HAL_EINT_MODULE_ENABLED in the hal_feature_config.h under the project inc folder.
Notice:         None.
Relative doc:   Please refer to the sensor subsystem API Reference for more detail.
Example project:Please find the project under project folder with sensor_subsys_ prefix.
