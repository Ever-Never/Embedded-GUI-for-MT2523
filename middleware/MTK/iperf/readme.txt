Iperf module usage guide

Brief:          This module is the implementation of iperf 2.x protocol.
Usage:          GCC: Include the module with "include $(SOURCE_DIR)/middleware/mtk/iperf/module.mk" in your GCC project Makefile and set MTK_IPERF_ENABLE to "y".
                KEIL:Drag the middleware/MTK/iperf folder to your project and define the MTK_IPERF_ENABLE macro. Add middleware/MTK/iperf/inc to include paths.
                IAR: Drag the middleware/MTK/iperf folder to your project and define the MTK_IPERF_ENABLE macro. Add middleware/MTK/iperf/inc to include paths.
Dependency:     MTK_MINICLI_ENABLE must be defined in feature.mk and include lwip module in your project.
Notice:         Note that iPerf2 is not compatible with iPerf3.
Relative doc:   N/A.
Example project:Please find the iot_sdk_demo project under project/mt7687_hdk/apps folder. To use Iperf, please submit "iperf -h" in cli command line to get the iperf usage.
                The relative sources of iperf cli implementation are in middleware/MTK/iperf/src/iperf_cli.c.