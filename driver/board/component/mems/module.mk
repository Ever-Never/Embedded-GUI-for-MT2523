
###################################################

MEMS_DRIVER_SRC = driver/board/component/mems
MEMS_DRIVER_FILES = $(MEMS_DRIVER_SRC)/src/mems_bus.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/sensor_manager_driver.c

ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),BMI160)
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/bmi160.c
endif

ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),BMA255)
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/bma2x2.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/bma255_sensor_adaptor.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/bma255_i2c_operation.c

ifeq ($(BMA255_ORIENTATION),OPT1)
CFLAGS += -DBAND_ORIENTATION_OPT1
else
ifeq ($(BMA255_ORIENTATION),OPT2)
CFLAGS += -DBAND_ORIENTATION_OPT2
endif
endif

ifeq ($(BMA255_USE_POLLING),y)
CFLAGS += -DBMA255_USE_POLLING
endif

endif

ifeq ($(MTK_SENSOR_BAROMETER_USE),BMP280)
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/bmp280.c
endif

ifeq ($(MTK_SENSOR_PROXIMITY_USE),CM36672)
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/cm36672.c
endif

ifeq ($(MTK_SENSOR_MAGNETIC_USE),YAS533)
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/yas_main.c
# include path
CFLAGS += -I$(SOURCE_DIR)/$(MEMS_DRIVER_SRC)/inc/libyas533
# driver libraries
LIBS += $(SOURCE_DIR)/driver/board/component/mems/lib/libyas533_protected_CM4_GCC.a
endif

ifeq ($(MTK_SENSOR_ACCELEROMETER_USE),ICM30630)
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/DeviceIcm30xxx.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxCmd.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxCtrl.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxFifo.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxLoad.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxLogger.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxSelfTest.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxSetup.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices/Drivers/Icm30xxx/Icm30xxxTransport.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/EmbUtils/DataConverter.c
MEMS_DRIVER_FILES += $(MEMS_DRIVER_SRC)/src/icm30630/Invn/FifoProtocol/FifoProtocolCore.c
CFLAGS += -I$(SOURCE_DIR)/$(MEMS_DRIVER_SRC)/src/icm30630
CFLAGS += -I$(SOURCE_DIR)/$(MEMS_DRIVER_SRC)/src/icm30630/image
CFLAGS += -I$(SOURCE_DIR)/$(MEMS_DRIVER_SRC)/src/icm30630/Invn/Devices
endif

C_FILES += $(MEMS_DRIVER_FILES)

###################################################
# include path
CFLAGS += -I$(SOURCE_DIR)/$(MEMS_DRIVER_SRC)/inc
CFLAGS += -I$(SOURCE_DIR)/middleware/MTK/sensor_subsys/inc
