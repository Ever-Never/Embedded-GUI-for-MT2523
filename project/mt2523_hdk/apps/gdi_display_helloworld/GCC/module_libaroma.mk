AROMA_SRC = project/mt2523_hdk/apps/gdi_display_helloworld/src/libaroma
ZLIB_SRC = project/mt2523_hdk/apps/gdi_display_helloworld/src/libaroma/libs/zlib

PNG_SRC = project/mt2523_hdk/apps/gdi_display_helloworld/src/libaroma/libs/png

FREE_TYPE_SRC = project/mt2523_hdk/apps/gdi_display_helloworld/src/libaroma/libs/freetype

LIBAROMA_MK_WITH_ZLIB := false
LIBAROMA_MK_WITH_JPEG := false
LIBAROMA_MK_WITH_PNG := true
LIBAROMA_MK_WITH_FREE_TYPE := true
LIBAROMA_MK_DEBUG_LEVEL := 7
LIBAROMA_MK_DEBUG_WITH_FILE := 2
LIBAROMA_MK_DEBUG_MEMORY_LEAK := 0
LIBAROMA_MK_DEBUG_COMPILER := 0
LIBAROMA_MK_HARFBUZZNG_PATH := $(AROMA_SRC)/libs/harfbuzz-ng

LIBAROMA_MK_DEVICE_FILES := \
  $(AROMA_SRC)/src/contrib/platform/rtos/platform.c 

LIBAROMA_MK_ZIP_FILES := \
    $(ZLIB_SRC)/src/adler32.c \
	  $(ZLIB_SRC)/src/compress.c \
	  $(ZLIB_SRC)/src/crc32.c \
	  $(ZLIB_SRC)/src/deflate.c \
	  $(ZLIB_SRC)/src/infback.c \
	  $(ZLIB_SRC)/src/inflate.c \
	  $(ZLIB_SRC)/src/inftrees.c \
	  $(ZLIB_SRC)/src/inffast.c \
	  $(ZLIB_SRC)/src/trees.c \
	  $(ZLIB_SRC)/src/uncompr.c \
	  $(ZLIB_SRC)/src/zutil.c 

LIBAROMA_MK_GZ_FILES := \
	  $(ZLIB_SRC)/src/gzclose.c \
	  $(ZLIB_SRC)/src/gzlib.c \
	  $(ZLIB_SRC)/src/gzread.c \
	  $(ZLIB_SRC)/src/gzwrite.c 

LIBAROMA_GRAPH_FILES :=  \
  	$(AROMA_SRC)/src/aroma/aroma.c \
    $(AROMA_SRC)/src/aroma/version.c \
    $(AROMA_SRC)/src/aroma/utils/array.c \
    $(AROMA_SRC)/src/aroma/utils/minutf8.c\
    $(AROMA_SRC)/src/aroma/utils/runtime.c \
    $(AROMA_SRC)/src/aroma/utils/time.c \
    $(AROMA_SRC)/src/aroma/utils/motions.c \
    $(AROMA_SRC)/src/aroma/utils/stream.c \
    $(AROMA_SRC)/src/aroma/utils/strings.c \
    $(AROMA_SRC)/src/aroma/utils/system.c \
    $(AROMA_SRC)/src/aroma/graph/artworker.c \
    $(AROMA_SRC)/src/aroma/graph/jpeg.c \
    $(AROMA_SRC)/src/aroma/graph/png.c \
    $(AROMA_SRC)/src/aroma/graph/canvas.c \
    $(AROMA_SRC)/src/aroma/graph/image.c \
    $(AROMA_SRC)/src/aroma/graph/svg.c \
    $(AROMA_SRC)/src/aroma/graph/draw/blur.c \
    $(AROMA_SRC)/src/aroma/graph/draw/commondraw.c \
    $(AROMA_SRC)/src/aroma/graph/draw/filters.c \
    $(AROMA_SRC)/src/aroma/graph/draw/gradient.c \
    $(AROMA_SRC)/src/aroma/graph/draw/path.c \
    $(AROMA_SRC)/src/aroma/graph/draw/scale.c \
    $(AROMA_SRC)/src/aroma/graph/engine/alignblt.c \
    $(AROMA_SRC)/src/aroma/graph/engine/alpha.c \
    $(AROMA_SRC)/src/aroma/graph/engine/blt.c \
    $(AROMA_SRC)/src/aroma/graph/engine/color.c \
    $(AROMA_SRC)/src/aroma/graph/engine/dither.c \
    $(AROMA_SRC)/src/contrib/platform/rtos/platform.c \
    $(AROMA_SRC)/src/aroma/debug/compiler_message.c \
    $(AROMA_SRC)/src/aroma/debug/memory_tracking.c    
    


 #$(AROMA_SRC)/src/aroma/debug/memory_tracking.c    

LIBAROMA_FB_FILES := \
    $(AROMA_SRC)/src/contrib/platform/rtos/fb_driver.c  \
    $(AROMA_SRC)/src/aroma/graph/fb.c 
    
C_FILES +=  $(LIBAROMA_MK_DEVICE_FILES)
C_FILES +=  $(LIBAROMA_GRAPH_FILES)

C_FILES +=  $(LIBAROMA_FB_FILES)


 




LIBAROMA_UI_FILES := \
    $(AROMA_SRC)/src/aroma/ui/color_manager.c \
    $(AROMA_SRC)/src/aroma/ui/window.c \
    $(AROMA_SRC)/src/aroma/ui/window_manager.c \
    $(AROMA_SRC)/src/aroma/ui/control.c \
    $(AROMA_SRC)/src/aroma/ui/window_layer.c 

LIBAROMA_HID_FILES := \
    $(AROMA_SRC)/src/aroma/hid/hid.c \
    $(AROMA_SRC)/src/aroma/hid/messages.c \
    $(AROMA_SRC)/src/contrib/platform/rtos/hid_driver.c 


LIBAROMA_MK_PNG_FILES := \
    $(PNG_SRC)/png.c \
	  $(PNG_SRC)/pngerror.c \
	  $(PNG_SRC)/pngget.c \
	  $(PNG_SRC)/pngmem.c \
	  $(PNG_SRC)/pngpread.c \
	  $(PNG_SRC)/pngread.c \
	  $(PNG_SRC)/pngrio.c \
	  $(PNG_SRC)/pngrtran.c \
	  $(PNG_SRC)/pngrutil.c \
	  $(PNG_SRC)/pngset.c \
	  $(PNG_SRC)/pngtrans.c \
	  $(PNG_SRC)/pngwio.c \
	  $(PNG_SRC)/pngwrite.c \
	  $(PNG_SRC)/pngwtran.c \
	  $(PNG_SRC)/pngwutil.c 

    
LIBAROMA_CONTRL_FILES := \
    $(AROMA_SRC)/src/aroma/controls/ctl_bar.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_image.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_pager.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_tabs.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_button.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_label.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_progress.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_fragment.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_list.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_scroll.c \
    $(AROMA_SRC)/src/aroma/controls/listitem/listitem_caption.c \
    $(AROMA_SRC)/src/aroma/controls/listitem/listitem_divider.c \
    $(AROMA_SRC)/src/aroma/controls/listitem/listitem_menu.c \
    $(AROMA_SRC)/src/aroma/controls/ctl_clock.c \
    $(AROMA_SRC)/src/aroma/controls/listitem/listitem_check.c \
    $(AROMA_SRC)/src/aroma/controls/listitem/listitem_image.c \
    $(AROMA_SRC)/src/aroma/controls/listitem/listitem_option.c \
    $(AROMA_SRC)/src/aroma/ui/dialog.c \
    $(AROMA_SRC)/src/aroma/controls/pay/ctl_pay.c \
    $(AROMA_SRC)/src/aroma/controls/pay/code128.c \
    $(AROMA_SRC)/src/aroma/controls/pay/bitstream.c \
    $(AROMA_SRC)/src/aroma/controls/pay/mask.c \
    $(AROMA_SRC)/src/aroma/controls/pay/mmask.c \
    $(AROMA_SRC)/src/aroma/controls/pay/mqrspec.c \
    $(AROMA_SRC)/src/aroma/controls/pay/qrencode.c \
    $(AROMA_SRC)/src/aroma/controls/pay/QR_Encode.c \
    $(AROMA_SRC)/src/aroma/controls/pay/qrinput.c \
    $(AROMA_SRC)/src/aroma/controls/pay/qrspec.c \
    $(AROMA_SRC)/src/aroma/controls/pay/qrspec.c \
    $(AROMA_SRC)/src/aroma/controls/pay/rscode.c \
    $(AROMA_SRC)/src/aroma/controls/pay/split.c 
    




    

LIBAROMA_MK_FREE_TYPE_FILES := \
	  $(FREE_TYPE_SRC)/src/base/ftbbox.c \
    $(FREE_TYPE_SRC)/src/base/ftbitmap.c \
    $(FREE_TYPE_SRC)/src/base/ftfstype.c \
    $(FREE_TYPE_SRC)/src/base/ftglyph.c \
    $(FREE_TYPE_SRC)/src/base/ftlcdfil.c \
    $(FREE_TYPE_SRC)/src/base/ftstroke.c \
    $(FREE_TYPE_SRC)/src/base/fttype1.c \
    $(FREE_TYPE_SRC)/src/base/ftbase.c \
    $(FREE_TYPE_SRC)/src/base/ftsystem.c \
    $(FREE_TYPE_SRC)/src/base/ftinit.c \
    $(FREE_TYPE_SRC)/src/base/ftgasp.c \
    $(FREE_TYPE_SRC)/src/raster/raster.c \
    $(FREE_TYPE_SRC)/src/sfnt/sfnt.c \
    $(FREE_TYPE_SRC)/src/smooth/smooth.c \
    $(FREE_TYPE_SRC)/src/autofit/autofit.c \
    $(FREE_TYPE_SRC)/src/truetype/truetype.c \
    $(FREE_TYPE_SRC)/src/cff/cff.c \
    $(FREE_TYPE_SRC)/src/cid/type1cid.c \
    $(FREE_TYPE_SRC)/src/bdf/bdf.c \
    $(FREE_TYPE_SRC)/src/type1/type1.c \
    $(FREE_TYPE_SRC)/src/type42/type42.c \
    $(FREE_TYPE_SRC)/src/winfonts/winfnt.c \
    $(FREE_TYPE_SRC)/src/pcf/pcf.c \
    $(FREE_TYPE_SRC)/src/pfr/pfr.c \
    $(FREE_TYPE_SRC)/src/psaux/psaux.c \
    $(FREE_TYPE_SRC)/src/psnames/psnames.c \
    $(FREE_TYPE_SRC)/src/pshinter/pshinter.c \
    $(FREE_TYPE_SRC)/src/gzip/ftgzip.c \
    $(FREE_TYPE_SRC)/src/lzw/ftlzw.c 



#    $(AROMA_SRC)/src/aroma/utils/json.c \


C_FILES +=  $(LIBAROMA_UI_FILES)
C_FILES +=  $(LIBAROMA_CONTRL_FILES)
C_FILES +=  $(LIBAROMA_UI_FILES)
C_FILES +=  $(LIBAROMA_HID_FILES)


ifeq ($(LIBAROMA_MK_WITH_ZLIB),true)
  C_FILES += $(LIBAROMA_MK_ZIP_FILES)
  CFLAGS  += -I$(SOURCE_DIR)/$(AROMA_SRC)/libs/zlib
else

endif

ifeq ($(LIBAROMA_MK_WITH_FREE_TYPE),true)
  C_FILES += $(LIBAROMA_MK_FREE_TYPE_FILES)
  C_FILES += $(AROMA_SRC)/src/aroma/graph/text.c
  CFLAGS += -I$(SOURCE_DIR)/$(AROMA_SRC)/libs/harfbuzz-ng/src/hb-ucdn 
  C_FILES += $(AROMA_SRC)/libs/harfbuzz-ng/src/hb-ucdn/ucdn.c 
  CFLAGS += -I$(SOURCE_DIR)/$(AROMA_SRC)/libs/freetype/include
  CFLAGS += -DFT2_NO_HARFBUZZ -DFT2_BUILD_LIBRARY -DDARWIN_NO_CARBON  -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG 
else
  C_FILES += $(AROMA_SRC)/src/aroma/graph/text.c
  CFLAGS += -I$(SOURCE_DIR)/$(AROMA_SRC)/libs/harfbuzz-ng/src/hb-ucdn 
  C_FILES += $(AROMA_SRC)/libs/harfbuzz-ng/src/hb-ucdn/ucdn.c
  include $(SOURCE_DIR)/middleware/third_party/freetype2/module.mk
endif

ifeq ($(LIBAROMA_MK_WITH_PNG),true)
  C_FILES += $(LIBAROMA_MK_ZIP_FILES)
  CFLAGS  += -I$(SOURCE_DIR)/$(AROMA_SRC)/libs/zlib
  C_FILES += $(LIBAROMA_MK_PNG_FILES)
  CFLAGS  += -I$(SOURCE_DIR)/$(AROMA_SRC)/libs/png
else
   include $(SOURCE_DIR)/middleware/third_party/zlib/module.mk
# LIBPNG source files
   include $(SOURCE_DIR)/middleware/third_party/libpng/module.mk
   #CFLAGS  += -DLIBAROMA_CONFIG_NOPNG
endif


#################################################################################
#include path
CFLAGS  += -I$(SOURCE_DIR)/$(AROMA_SRC)/src
CFLAGS  += -I$(SOURCE_DIR)/$(AROMA_SRC)/include
CFLAGS  += -I$(SOURCE_DIR)/$(AROMA_SRC)/src/contrib/platform/rtos/include



#################################################################################
#Enable the feature by configuring
CFLAGS += -DLIBAROMA_CONFIG_NOMINZIP=1 -DRTOS_FILE  -DRTOS_FILE -DLIBAROMA_CONFIG_TEXT_NOHARFBUZZ

CFLAGS += -DLIBAROMA_CONFIG_NOJPEG=1 -DLIBAROMA_CONFIG_NO_RUNTIME -DUI_LIBAROMA

CFLAGS +=  -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG 


CFLAGS += \
   -DLIBAROMA_CONFIG_DEBUG=$(LIBAROMA_MK_DEBUG_LEVEL) \
   -DLIBAROMA_CONFIG_DEBUG_FILE=$(LIBAROMA_MK_DEBUG_WITH_FILE) \
   -DLIBAROMA_CONFIG_DEBUG_MEMORY=$(LIBAROMA_MK_DEBUG_MEMORY_LEAK) \
   -DLIBAROMA_CONFIG_COMPILER_MESSAGE=$(LIBAROMA_MK_DEBUG_COMPILER) \
   -DLIBAROMA_CONFIG_NO_ANDROID_MEM  -DLIBAROMA_CONFIG_NOPNGWRITE


