slp module usage guide

Brief:          This module is simple protocol for location provider. This module communicate with the SUPL server based on TCP/IP and TLS protocol.
                This module takes the cell information, SUPL server domain name and the SUPL server digital certificate as input. 
                The cell information, SUPL server domain name must be provided, the SUPL server digital certificate is optional.
                After takes the input, this module will response reference location (latitude, longitude and height) as response.
                There are several available SUPL server: (supl.qxwz.com is available in china), (supl.nokia.com supl.google.com is available oversea).
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/MTK/slp/module.mk" in your GCC project Makefile.
                Step 1: call sclp_init function to init this module and to register the application callback function.
								Step 2: call slp_start3 to input the cell information and SUPL server domain name, and then this module will communicate with SUPL server to get location information.
								Step 3: This module response location to the application callback function.
Dependency:     Please include middleware/third_party/lwip/module.mk and middleware/third_party/mbedtls/module.mk in the project.
Notice:         This module is not available on the project without modem.
Example project:Please find the iot_modem_wifi5931_ref_design project under project folder.
