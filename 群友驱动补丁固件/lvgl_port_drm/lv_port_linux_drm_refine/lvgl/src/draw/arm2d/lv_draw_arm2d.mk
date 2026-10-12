CSRCS += lv_gpu_arm2d.c

DEPPATH += --dep-path $(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/arm2d
VPATH += :$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/arm2d

CFLAGS += "-I$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/draw/arm2d"
