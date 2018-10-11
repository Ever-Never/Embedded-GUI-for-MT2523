###############################################################################
#
# @brief     This file is part of the TouchGFX 4.5.0 evaluation distribution.
#
# @author    Draupner Graphics A/S <http://www.touchgfx.com>
#
###############################################################################
#
# @section Copyright
#
# Copyright (C) 2014-2016 Draupner Graphics A/S <http://www.touchgfx.com>.
# All rights reserved.
#
# TouchGFX is protected by international copyright laws and the knowledge of
# this source code may not be used to write a similar product. This file may
# only be used in accordance with a license and should not be re-
# distributed in any way without the prior permission of Draupner Graphics.
#
# This is licensed software for evaluation use, any use must strictly comply
# with the evaluation license agreement provided with delivery of the
# TouchGFX software.
#
# The evaluation license agreement can be seen on www.touchgfx.com
#
# @section Disclaimer
#
# DISCLAIMER OF WARRANTY/LIMITATION OF REMEDIES: Draupner Graphics A/S has
# no obligation to support this software. Draupner Graphics A/S is providing
# the software "AS IS", with no express or implied warranties of any kind,
# including, but not limited to, any implied warranties of merchantability
# or fitness for any particular purpose or warranties against infringement
# of any proprietary rights of a third party.
#
# Draupner Graphics A/S can not be held liable for any consequential,
# incidental, or special damages, or any other relief, or for any claim by
# any third party, arising from your use of this software.
#
###############################################################################

# Only set alpha_dither if not already set by project
# Get identification of this system
UNAME := $(shell uname -s)

SOURCE_DIR ?= ../../..
OUTPATH ?= Build

$(info ~~~~~~~~~~~~~~~~$(SCREEN_CONFIG))

ifeq ($(SCREEN_CONFIG),mt2523_watch_v2)
UI_PATH ?= project/mt2523_watch/apps/watch_ref_design/src/ui
endif

ifeq ($(SCREEN_CONFIG),mt2523_hdk)
UI_PATH ?= project/mt2523_hdk/apps/watch_ref_design/src/ui
endif

alpha_dither ?= no
build_root_path := $(realpath $(OUTPATH))
object_output_path := $(build_root_path)
application_path := $(SOURCE_DIR)/$(UI_PATH)
touchgfx_tools_path := $(SOURCE_DIR)/tools/touchgfx

makefile_name := $(lastword $(MAKEFILE_LIST))
makefile_path := $(shell dirname "$(realpath $(makefile_name))")
# Get path where the Application is
root_path := $(realpath $(makefile_path)/$(SOURCE_DIR))

# Location of folder containing bmp/png files.
asset_images_input := $(SOURCE_DIR)/$(UI_PATH)/assets/images
# Location of folder where the texts.xlsx is placed
asset_texts_input  := $(SOURCE_DIR)/$(UI_PATH)/assets/texts
# Location of folder to search for ttf font files
asset_fonts_input  := $(SOURCE_DIR)/$(UI_PATH)/assets/fonts

# Location of folder containing dynamic bmp files.
dynamic_images_input := $(SOURCE_DIR)/$(UI_PATH)/assets/dynamic_images

text_database := $(asset_texts_input)/texts.xlsx

asset_root_path := $(build_root_path)/generated
asset_images_output := $(asset_root_path)/images
asset_fonts_output  := $(asset_root_path)/fonts
asset_texts_output  := $(asset_root_path)/texts

dynamic_images_output := $(asset_root_path)/dynamic_images

#include application specific configuration
include $(application_path)/config/gcc/app.mk

# Param $(1): List of directories to search
# Param $(2): The file pattern to search for
define find
	$(foreach dir,$(1),$(foreach d,$(wildcard $(dir)/*),\
		$(call find,$(d),$(2))) $(wildcard $(dir)/$(strip $(2))))
endef
unexport find

# image files
imageconvert_bmp_lower_files := $(call find, $(asset_images_input), *.bmp)
imageconvert_bmp_upper_files := $(call find, $(asset_images_input), *.BMP)
imageconvert_png_lower_files := $(call find, $(asset_images_input), *.png)
imageconvert_png_upper_files := $(call find, $(asset_images_input), *.PNG)
imageconvert_image_files := $(imageconvert_bmp_lower_files) \
			    $(imageconvert_bmp_upper_files) \
			    $(imageconvert_png_lower_files) \
			    $(imageconvert_png_upper_files)
			    
# dynamic image files
imageconvert_dynamic_bmp_lower_files := $(call find, $(dynamic_images_input), *.bmp)
imageconvert_dynamic_bmp_upper_files := $(call find, $(dynamic_images_input), *.BMP)
imageconvert_dynamic_png_lower_files := $(call find, $(dynamic_images_input), *.png)
imageconvert_dynamic_png_upper_files := $(call find, $(dynamic_images_input), *.PNG)
imageconvert_dynamic_image_files := $(imageconvert_dynamic_bmp_lower_files) \
			    $(imageconvert_dynamic_bmp_upper_files) \
			    $(imageconvert_dynamic_png_lower_files) \
			    $(imageconvert_dynamic_png_upper_files)

# image cpp
imageconvert_cpp_files := $(imageconvert_bmp_lower_files:$(asset_images_input)/%.bmp=$(asset_images_output)/src/%.cpp) \
			  $(imageconvert_bmp_upper_files:$(asset_images_input)/%.BMP=$(asset_images_output)/src/%.cpp) \
			  $(imageconvert_png_lower_files:$(asset_images_input)/%.png=$(asset_images_output)/src/%.cpp) \
			  $(imageconvert_png_upper_files:$(asset_images_input)/%.PNG=$(asset_images_output)/src/%.cpp)

# dynamic image cpp
dynamic_image_convert_cpp_files := $(imageconvert_dynamic_bmp_lower_files:$(dynamic_images_input)/%.bmp=$(dynamic_images_output)/src/%.cpp) \
			  $(imageconvert_dynamic_bmp_upper_files:$(dynamic_images_input)/%.BMP=$(dynamic_images_output)/src/%.cpp) \
			  $(imageconvert_dynamic_png_lower_files:$(dynamic_images_input)/%.png=$(dynamic_images_output)/src/%.cpp) \
			  $(imageconvert_dynamic_png_upper_files:$(dynamic_images_input)/%.PNG=$(dynamic_images_output)/src/%.cpp)

# font files
fontconvert_ttf_lower_files := $(call find, $(asset_fonts_input), *.ttf)
fontconvert_ttf_upper_files := $(call find, $(asset_fonts_input), *.TTF)
fontconvert_otf_lower_files := $(call find, $(asset_fonts_input), *.otf)
fontconvert_otf_upper_files := $(call find, $(asset_fonts_input), *.OTF)
fontconvert_bdf_lower_files := $(call find, $(asset_fonts_input), *.bdf)
fontconvert_bdf_upper_files := $(call find, $(asset_fonts_input), *.BDF)
fontconvert_font_files := $(fontconvert_ttf_lower_files) \
			  $(fontconvert_ttf_upper_files) \
			  $(fontconvert_otf_lower_files) \
			  $(fontconvert_otf_upper_files) \
			  $(fontconvert_bdf_lower_files) \
			  $(fontconvert_bdf_upper_files)

ifeq ($(UNAME), Linux)
imageconvert_executable := $(touchgfx_tools_path)/imageconvert/build/linux/imageconvert.out
fontconvert_executable := $(touchgfx_tools_path)/fontconvert/build/linux/fontconvert.out
dynamic_imageconvert_executable := $(touchgfx_tools_path)/imageconvert/build/linux/dynamic_imageconvert_lzma.out
else
ifeq ($(UNAME), Darwin)
imageconvert_executable := $(touchgfx_tools_path)/imageconvert/build/osx/imageconvert.out
fontconvert_executable := $(touchgfx_tools_path)/fontconvert/build/osx/fontconvert.out
else
imageconvert_executable := $(touchgfx_tools_path)/imageconvert/build/win/imageconvert.out
fontconvert_executable := $(touchgfx_tools_path)/fontconvert/build/win/fontconvert.out
dynamic_imageconvert_executable := $(touchgfx_tools_path)/imageconvert/build/win/dynamic_imageconvert_lzma.out
st_link_executable := "$(PROGRAMFILES)\\STMicroelectronics\\STM32 ST-LINK Utility\\ST-LINK Utility\\ST-LINK_CLI.exe"
endif
endif


textconvert_script_path := $(touchgfx_tools_path)/textconvert
textconvert_executable := $(call find, $(textconvert_script_path), *.rb)

# source files & include path settings
include_paths := $(foreach comp, $(asset_images_output) $(asset_fonts_output) $(asset_texts_output), $(comp)/include)
###################################################
# write image filenames to a temp file used by the image converter. This is necessary to avoid exceeding 32Kb limit in a mingw command line for large touchgfx projects
$(shell mkdir -p $(object_output_path))
$(shell echo "$(imageconvert_image_files)">$(object_output_path)/image_files.txt)
$(shell echo "$(dynamic_image_convert_cpp_files)">$(object_output_path)/dynamic_image_files.txt)

.PHONY: assets all

module_mk_name := touchgfx_module.mk

all: assets
	@echo C_FILES += $(patsubst $(root_path)/%,%,$(call find, $(asset_fonts_output) $(asset_texts_output),*.c)) >$(asset_root_path)/$(module_mk_name)
	@echo CXX_FILES += $(patsubst $(root_path)/%,%,$(call find, $(asset_fonts_output) $(asset_texts_output),*.cpp)) >>$(asset_root_path)/$(module_mk_name)
	@echo CXX_FILES += $(patsubst $(root_path)/%,%,$(imageconvert_cpp_files)) >>$(asset_root_path)/$(module_mk_name)
	@echo CXX_FILES += $(patsubst $(root_path)/%,%,$(dynamic_image_convert_cpp_files)) >>$(asset_root_path)/$(module_mk_name)
	@echo CXX_FILES += $(patsubst $(root_path)/%,%,$(asset_images_output)/src/BitmapDatabase.cpp) >>$(asset_root_path)/$(module_mk_name)
	@echo CFLAGS += $(patsubst %,-I%,$(include_paths)) >>$(asset_root_path)/$(module_mk_name)
	@echo $(asset_root_path)/$(module_mk_name) is genereated

assets: $(asset_images_output)/include/BitmapDatabase.hpp $(asset_texts_output)/include/texts/TextKeysAndLanguages.hpp

$(asset_images_output)/include/BitmapDatabase.hpp $(asset_images_output)/src/BitmapDatabase.cpp: $(imageconvert_cpp_files) $(imageconvert_image_files) $(dynamic_image_convert_cpp_files) $(imageconvert_dynamic_image_files) $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $(asset_images_output)/include/BitmapDatabase.hpp
	@echo Generating $(asset_images_output)/src/BitmapDatabase.cpp
	@$(imageconvert_executable) -header -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $(object_output_path)/image_files.txt
	@rm -f $(object_output_path)/image_files.txt
	@mkdir -p $(asset_images_output)/include
	@mkdir -p $(asset_images_output)/src
	@mkdir -p $(dynamic_images_output)/include
	@mkdir -p $(dynamic_images_output)/src
	@mv BitmapDatabase.hpp $(asset_images_output)/include/BitmapDatabase.hpp
	@mv BitmapDatabase.cpp $(asset_images_output)/src/BitmapDatabase.cpp


$(asset_images_output)/src/%.0.cpp: $(asset_images_input)/%.0.bmp $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)

$(asset_images_output)/src/%.90.cpp: $(asset_images_input)/%.90.bmp $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate90

$(asset_images_output)/src/%.180.cpp: $(asset_images_input)/%.180.bmp $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate180

$(asset_images_output)/src/%.270.cpp: $(asset_images_input)/%.270.bmp $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate270


$(asset_images_output)/src/%.cpp: $(asset_images_input)/%.bmp $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)


$(asset_images_output)/src/%.0.cpp: $(asset_images_input)/%.0.BMP $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)

$(asset_images_output)/src/%.90.cpp: $(asset_images_input)/%.90.BMP $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate90

$(asset_images_output)/src/%.180.cpp: $(asset_images_input)/%.180.BMP $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate180

$(asset_images_output)/src/%.270.cpp: $(asset_images_input)/%.270.BMP $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate270

$(asset_images_output)/src/%.cpp: $(asset_images_input)/%.BMP $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)


$(asset_images_output)/src/%.0.cpp: $(asset_images_input)/%.0.png $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)

$(asset_images_output)/src/%.90.cpp: $(asset_images_input)/%.90.png $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate90

$(asset_images_output)/src/%.180.cpp: $(asset_images_input)/%.180.png $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate180

$(asset_images_output)/src/%.270.cpp: $(asset_images_input)/%.270.png $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate270

$(asset_images_output)/src/%.cpp: $(asset_images_input)/%.png $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)


$(asset_images_output)/src/%.0.cpp: $(asset_images_input)/%.0.PNG $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)

$(asset_images_output)/src/%.90.cpp: $(asset_images_input)/%.90.PNG $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate90

$(asset_images_output)/src/%.180.cpp: $(asset_images_input)/%.180.PNG $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate180

$(asset_images_output)/src/%.270.cpp: $(asset_images_input)/%.270.PNG $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation) -rotate270

$(asset_images_output)/src/%.cpp: $(asset_images_input)/%.PNG $(application_path)/config/gcc/app.mk $(imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)

# dynamic image convert with LZMA compression
$(dynamic_images_output)/src/%.cpp: $(dynamic_images_input)/%.bmp $(application_path)/config/gcc/app.mk $(dynamic_imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)
	@cp $@ $@_origin
	@rm -f $@
	@$(dynamic_imageconvert_executable) $@_origin $@

$(dynamic_images_output)/src/%.cpp: $(dynamic_images_input)/%.BMP $(application_path)/config/gcc/app.mk $(dynamic_imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)
	@cp $@ $@_origin
	@rm -f $@
	@$(dynamic_imageconvert_executable) $< $@

$(dynamic_images_output)/src/%.cpp: $(dynamic_images_input)/%.png $(application_path)/config/gcc/app.mk $(dynamic_imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)
	@cp $@ $@_origin
	@rm -f $@
	@$(dynamic_imageconvert_executable) $@_origin $@

$(dynamic_images_output)/src/%.cpp: $(dynamic_images_input)/%.PNG $(application_path)/config/gcc/app.mk $(dynamic_imageconvert_executable)
	@echo Generating $@ from $<
	@mkdir -p $(@D)
	@$(imageconvert_executable) -alpha_dither $(alpha_dither) -opaque_image_format $(opaque_image_format) -non_opaque_image_format $(non_opaque_image_format) -f $< -o $@ $(screen_orientation)
	@cp $@ $@_origin
	@rm -f $@
	@$(dynamic_imageconvert_executable) $< $@

# text_data_format in app.mk
$(asset_texts_output)/include/texts/TextKeysAndLanguages.hpp: $(text_database) $(textconvert_executable) $(fontconvert_executable) $(fontconvert_font_files)
	@rm -f $(asset_fonts_output)/src/*
	@rm -f $(asset_fonts_output)/include/fonts/*
	@rm -f $(asset_fonts_output)/UnicodeList*.txt
	@rm -f $(asset_fonts_output)/CharSizes*.csv
	@mkdir -p $(asset_texts_output)/include/texts
	@ruby -C$(textconvert_script_path) main.rb $(abspath $(text_database)) $(abspath $(fontconvert_executable)) $(abspath $(asset_fonts_output)) $(abspath $(asset_texts_output)) $(abspath $(asset_fonts_input)) $(abspath ./) $(text_data_format)

clean:
	@echo Cleaning
	@rm -rf $(build_root_path)
