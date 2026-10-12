CSRCS += lv_gpu_swm341_dma2d.c

DEPPATH += --dep-path $(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/swm341_dma2d
VPATH += :$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/swm341_dma2d

CFLAGS += "-I$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/swm341_dma2d"
