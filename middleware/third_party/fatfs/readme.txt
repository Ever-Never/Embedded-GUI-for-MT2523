FatFS module usage guide

Brief:          This module is the file system implementation to manage file operation.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/third_party/fatfs/module.mk" in your GCC project Makefile, add "MTK_FATFS_ON_SPI_SD = y" in your GCC project feature.mk if SD is implemented based on SPI.
                KEIL/IAR: Drag the ff.c and diskio.c and syscall.c and unicode.c according to current configuration to your project and add middleware/third_party/fatfs/src to include paths, add "MTK_FATFS_ON_SPI_SD" if SD is implemented based on SPI.
Dependency:     Please define HAL_SD_MODULE_ENABLED in hal_feature_config.h under the inc folder of your project.
Notice:         Middleware/third_party/fatfs/src/diskio.c does not have RTC implementation.
                Please configure the RTC for FatFS under middleware/third_party/fatfs/src/diskio.c if time information is needed.
Relative doc:   Please refer to the open source user guide under the doc folder for more detail.
Example project:None.
                You can do the porting of the examples to our platform in FatFS official website as below.
                    Step1. Download the FatFS example project such as ffsample/avr_foolproof by clicking "FatFs sample projects for various platforms" on the FatFS official website.
                    Step2. Delete the line of "#inlcude <avr/io.h>" in main.c under ffsample/avr_foolproof/.
                    Step3. Copy project/mt2523_hdk/apps/freertos_thread_creation/ and replace the main.c with the one got in step2.
                    Step4. Rename the copied "freertos_thread_creation" project.
                    Step5. Enable FatFS module according to the above method.
