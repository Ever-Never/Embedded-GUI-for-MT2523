include $(CLEAR_VARS)

LOCAL_MODULE := libaroma_testx
LOCAL_MODULE_TAGS             := eng
LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_SRC_FILES := \
    examples/libaroma_test.c \

  LOCAL_SRC_FILES += \
    src/aroma/aroma.c \
    src/aroma/version.c \
    src/aroma/utils/array.c \
    src/aroma/utils/minutf8.c\
    src/aroma/utils/runtime.c \
    src/aroma/utils/strings.c \
    src/aroma/utils/time.c \
    src/aroma/utils/json.c \
    src/aroma/utils/motions.c \
    src/aroma/utils/stream.c \
    src/aroma/utils/system.c \
    src/aroma/utils/zip.c \
    src/aroma/debug/compiler_message.c \
    src/aroma/debug/memory_tracking.c \
    src/aroma/graph/artworker.c \
    src/aroma/graph/fb.c \
    src/aroma/graph/jpeg.c \
    src/aroma/graph/canvas.c \
    src/aroma/graph/image.c \
    src/aroma/graph/png.c \
    src/aroma/graph/svg.c \
    src/aroma/graph/text.c \
    src/aroma/graph/draw/blur.c \
    src/aroma/graph/draw/commondraw.c \
    src/aroma/graph/draw/filters.c \
    src/aroma/graph/draw/gradient.c \
    src/aroma/graph/draw/path.c \
    src/aroma/graph/draw/scale.c \
    src/aroma/graph/engine/alignblt.c \
    src/aroma/graph/engine/alpha.c \
    src/aroma/graph/engine/blt.c \
    src/aroma/graph/engine/color.c \
    src/aroma/graph/engine/dither.c \
    src/aroma/hid/hid.c \
    src/aroma/hid/messages.c \
    src/aroma/ui/color_manager.c \
    src/aroma/ui/dialog.c \
    src/aroma/ui/window.c \
    src/aroma/ui/window_manager.c \
    src/aroma/ui/control.c \
    src/aroma/ui/window_layer.c \
    src/aroma/controls/ctl_bar.c \
    src/aroma/controls/ctl_image.c \
    src/aroma/controls/ctl_pager.c \
    src/aroma/controls/ctl_tabs.c \
    src/aroma/controls/ctl_button.c \
    src/aroma/controls/ctl_label.c \
    src/aroma/controls/ctl_progress.c \
    src/aroma/controls/ctl_fragment.c \
    src/aroma/controls/ctl_list.c \
    src/aroma/controls/ctl_scroll.c \
    src/aroma/controls/listitem/listitem_caption.c \
    src/aroma/controls/listitem/listitem_divider.c \
    src/aroma/controls/listitem/listitem_menu.c \
    src/aroma/controls/listitem/listitem_check.c \
    src/aroma/controls/listitem/listitem_image.c \
    src/aroma/controls/listitem/listitem_option.c \
    \
    $(LIBAROMA_MK_DEVICE_FILES)
    $(LIBAROMA_MK_ENGINE_FILES)

LOCAL_CFLAGS                  := -O2 
LOCAL_CFLAGS                  += -fdata-sections -ffunction-sections -Wl,--gc-sections
LOCAL_CFLAGS                  += -D_GLIBCXX_DEBUG_PEDANTIC -D_GLIBCXX_DEBUG
LOCAL_CFLAGS                  += -Wl,--gc-sections -fPIC -DPIC
LOCAL_CFLAGS                  += -fPIC -DPIC -Wl,-s 
LOCAL_CFLAGS                  += -O3 -ftree-vectorize -mvectorize-with-neon-quad -fprefetch-loop-arrays -funsafe-math-optimizations -fopenmp  -mfpu=neon 
LOCAL_CFLAGS                  += -D__ARM_HAVE_NEON -DLIBAROMA_CONFIG_OPENMP=1 -DLIBAROMA_CONFIG_DEBUG_TRACE=0
LOCAL_CFLAGS                  += -DANDROID=1 -D__ANDROID__ 



LOCAL_CFLAGS += \
    -DLIBAROMA_CONFIG_DEBUG=$(LIBAROMA_MK_DEBUG_LEVEL) \
    -DLIBAROMA_CONFIG_DEBUG_FILE=$(LIBAROMA_MK_DEBUG_WITH_FILE) \
    -DLIBAROMA_CONFIG_DEBUG_MEMORY=$(LIBAROMA_MK_DEBUG_MEMORY_LEAK) \
    -DLIBAROMA_CONFIG_COMPILER_MESSAGE=$(LIBAROMA_MK_DEBUG_COMPILER)  \

LOCAL_C_INCLUDES := \
    $(LIBAROMA_PATH)/include \
    $(LIBAROMA_PATH)/src \
    $(LIBAROMA_PATH)/src/contrib/platform/linux/include \
    $(LIBAROMA_MK_ZLIB_PATH) \
    $(LIBAROMA_MK_PNG_PATH) \
    $(LIBAROMA_MK_FREETYPE_PATH)/builds \
    $(LIBAROMA_MK_FREETYPE_PATH)/include \
    $(LIBAROMA_MK_HARFBUZZNG_PATH)/src \
    $(LIBAROMA_MK_HARFBUZZNG_PATH)/src/hb-ucdn \
    $(LIBAROMA_MK_MINZIP_PATH) \



LOCAL_STATIC_LIBRARIES        :=   libm libc \
    $(LIBAROMA_MK_ZLIB_LIB) \
    $(LIBAROMA_MK_PNG_LIB) \
    $(LIBAROMA_MK_FREETYPE_LIB) \
    $(LIBAROMA_MK_HARFBUZZNG_LIB) \
    $(LIBAROMA_MK_MINZIP_LIB) \
    libstdc++ \

  ##
  ## with jpeg support
  ##
ifeq ($(LIBAROMA_MK_WITH_JPEG),true)
  LOCAL_STATIC_LIBRARIES += $(LIBAROMA_MK_JPEG_LIB)
  LOCAL_C_INCLUDES += $(LIBAROMA_MK_JPEG_PATH)
else
  LOCAL_CFLAGS += -DLIBAROMA_CONFIG_NOJPEG=1
endif

include $(BUILD_EXECUTABLE)


