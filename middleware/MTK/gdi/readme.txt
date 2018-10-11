gdi module usage guide

Brief:           This module is the graphic, font engine and resource implementation.
Usage:           
                 1) Generate the font, image and string resource by using the tool "\tools\mtk\resgen".
                    (1) Copy the folder "\tools\mtk\resgen" to the windows environment.
                    (2) Read the help document by using the windows command "make ?".
                    (3) Modify the files in the folder \tools\mtk\resgen\custom_resource according to the help document. 
                    (4) Generate the resource files by using the windows command "make resgen".
                    (5) Copy the generated files under the folder \tools\mtk\resgen\output to the project folder such as project/[chip]/apps/[project]/src
                        and project/[chip]/apps/[project]/inc.

                 2) Build the source files.
                 GCC:  (1) Include the module with "-include $(SOURCE_DIR)/middleware/MTK/gdi/module.mk" in your GCC project Makefile.
                       (2) Add the generated files to the GCC project Makefile.                     
                 KEIL: (1) Add all source files under the path "middleware/MTK/gdi/src" and the generated files to your project.
                       (2) Add the path "..\..\..\..\..\middleware\MTK\gdi\inc" to the "include paths" of the C/C++ options.
                 IAR:  (1) Add all source files under the path "middleware/MTK/gdi/src" and the generated files to your project 
                       (2) Add the path "$PROJ_DIR$\..\..\..\..\..\middleware\MTK\gdi\inc" to the "Additional include directories" of the preprocessor.
Dependency:      None.
Notice:          None.
Relative doc:    Find the GDI section in the 2523 API reference under the [sdk_root]/doc folder.
Example project: Please find the project under project folder with bioband_demo and iot_sdk_demo.
