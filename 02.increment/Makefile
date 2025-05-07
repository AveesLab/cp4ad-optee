# No Need to modify this file.

TARGET_DIR := $(notdir $(shell pwd))

build_dir := /home/avees/Linux_for_Tegra/sources/tegra/optee-src/nv-optee/optee/build/t234
install_dir := /home/avees/Linux_for_Tegra/sources/tegra/optee-src/nv-optee/optee/install/t234

TA_DEV_KIT_DIR := $(build_dir)/export-ta_arm64/
OPTEE_CLIENT_EXPORT := $(install_dir)/usr

MAKEFLAGS += --no-print-directory

TA_INSTALL_DIR := /lib/optee_armtz/
CA_EXE_DIR := /usr/sbin/

.PHONY: all install clean ta host

all: ta host
	@echo "\n[$(notdir $(shell pwd))] Build successfully done."

ta:
	@echo "[building TA...]"
	$(MAKE) -C ta TA_DEV_KIT_DIR=$(TA_DEV_KIT_DIR) O=../build/ta

host:
	@echo "\n[building HOST...]"
	$(MAKE) -C host OPTEE_CLIENT_EXPORT=$(OPTEE_CLIENT_EXPORT) O=../build/host --no-builtin-variables

install:
	@echo "[installing TA...]"
	sudo cp build/ta/*.ta $(TA_INSTALL_DIR)
	@echo "[installing HOST...]"
	$(MAKE) -C host CA_EXE_DIR=$(CA_EXE_DIR) O=../build/host install
	@echo "\n[$(notdir $(shell pwd))] Install complete."

clean:
	rm -rf build
