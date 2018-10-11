###################################################
# Sources
###################################################

GDI_SOURCE = middleware/MTK/gdi/src
GDI_FILES =  $(GDI_SOURCE)/gdi.c \
             $(GDI_SOURCE)/gdi_font_show_char.c \
             $(GDI_SOURCE)/gdi_layer.c \
             $(GDI_SOURCE)/gdi_lcd.c \
             $(GDI_SOURCE)/gdi_primitive.c \
             $(GDI_SOURCE)/gd_primitive.c \
             $(GDI_SOURCE)/gd_primitive8.c \
             $(GDI_SOURCE)/gd_primitive16.c \
             $(GDI_SOURCE)/gd_primitive24.c \
             $(GDI_SOURCE)/gd_primitive32.c \
             $(GDI_SOURCE)/gdi_image.c \
             $(GDI_SOURCE)/gdi_image_bmp.c \
             $(GDI_SOURCE)/gdi_imgdec_bmp.c \
             $(GDI_SOURCE)/gdi_imgdec_bytestream.c \
             $(GDI_SOURCE)/gdi_imgdec_resizer.c \
             $(GDI_SOURCE)/gdi_resource.c \
             $(GDI_SOURCE)/gdi_resource_weak_symbol.c \
             $(GDI_SOURCE)/gdi_common_language.c \
             $(GDI_SOURCE)/gdi_font_engine.c \
             $(GDI_SOURCE)/gdi_language_module.c

C_FILES += $(GDI_FILES)
###################################################
# include path
###################################################

CFLAGS  += -I$(SOURCE_DIR)/middleware/MTK/gdi/inc
