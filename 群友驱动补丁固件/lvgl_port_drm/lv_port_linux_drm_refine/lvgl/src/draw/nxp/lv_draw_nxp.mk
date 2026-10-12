CSRCS += lv_gpu_nxp.c

DEPPATH += --dep-path $(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp
VPATH += :$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp

CFLAGS += "-I$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp"

include $(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/pxp/lv_draw_nxp_pxp.mk
include $(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/nxp/vglite/lv_draw_nxp_vglite.mk
