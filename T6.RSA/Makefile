## 1. No need to modify the this Makefile.

CROSS_COMPILE="${CROSS_COMPILE_AARCH64}"
TARGET_DIR := $(notdir $(shell pwd))
build_dir=$(shell realpath $(shell pwd)/../..)/build/t234
install_dir=$(shell realpath $(shell pwd)/../..)/install/t234
TA_DEV_KIT_DIR=${build_dir}/export-ta_arm64/
# O=${build_dir}
O=../build
O=$(shell realpath $(shell pwd)/build)
OPTEE_CLIENT_EXPORT=$(install_dir)/usr
MAKEFLAGS += --no-print-directory
TA_INSTALL_DIR=/lib/optee_armtz/
CA_EXE_DIR=/usr/sbin/

OPTEE_MBEDTLS_DIR = $(shell realpath $(shell pwd)/../..)/optee_os/lib/libmbedtls
CFLAGS += -I$(OPTEE_MBEDTLS_DIR)/include
LDFLAGS += -L$(OPTEE_MBEDTLS_DIR) -lmbedtls -lmbedcrypto -lmbedx509

.PHONY: all
all:
	$(MAKE) -C ta \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR) \
		O=$(O)/ta/$(TARGET_DIR)
	$(MAKE) -C host \
		CROSS_COMPILE=$(CROSS_COMPILE) \
		OPTEE_CLIENT_EXPORT=$(OPTEE_CLIENT_EXPORT) \
		O=$(O)/ca/$(TARGET_DIR) \
		CFLAGS="$(CFLAGS)" \
		LDFLAGS="$(LDFLAGS)" \
		--no-builtin-variables
	@echo "\n[$(notdir $(shell pwd))] Make successfully done."

.PHONY: install
install:
	sudo cp $(O)/ta/$(TARGET_DIR)/*.ta $(TA_INSTALL_DIR)
	sudo cp $(O)/ca/$(TARGET_DIR)/* $(CA_EXE_DIR)
	@echo "Install complete."
	
.PHONY: clean
clean:
	$(MAKE) -C ta \
		TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR) \
		O=$(O)/ta/$(TARGET_DIR) \
		clean
	$(MAKE) -C host \
		OPTEE_CLIENT_EXPORT=$(OPTEE_CLIENT_EXPORT) \
		O=$(O)/ca/$(TARGET_DIR) \
		clean
	rm -rf $(O)/ca/$(TARGET_DIR)
