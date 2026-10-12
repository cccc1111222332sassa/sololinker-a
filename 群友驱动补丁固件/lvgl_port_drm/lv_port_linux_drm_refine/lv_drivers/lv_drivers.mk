LV_DRIVERS_DIR_NAME ?= lv_drivers

override CFLAGS := -I$(PROJECT_DIR) $(CFLAGS)

CSRCS += $(wildcard $(PROJECT_DIR)/$(LV_DRIVERS_DIR_NAME)/*.c)
CSRCS += $(wildcard $(PROJECT_DIR)/$(LV_DRIVERS_DIR_NAME)/indev/*.c)
CSRCS += $(wildcard $(PROJECT_DIR)/$(LV_DRIVERS_DIR_NAME)/display/*.c)

