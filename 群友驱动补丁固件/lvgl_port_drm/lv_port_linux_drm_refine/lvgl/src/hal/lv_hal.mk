CSRCS += lv_hal_disp.c
CSRCS += lv_hal_indev.c
CSRCS += lv_hal_tick.c

DEPPATH += --dep-path $(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/hal
VPATH += :$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/hal

CFLAGS += "-I$(PROJECT_DIR)/$(LVGL_DIR_NAME)/src/hal"
