CFLAGS += "-I$(PROJECT_DIR)/drm/include"
LDFLAGS +=-L$(PROJECT_DIR)/drm/lib/ -ldrm #-ldrm_rockchip -lkms
