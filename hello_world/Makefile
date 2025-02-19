# Modify TA root Makefile
CROSS_COMPILE="${CROSS_COMPILE_AARCH64}"
TARGET_DIR := $(notdir $(shell pwd))
build_dir=/home/seungwoo/optee/Linux_for_Tegra/sources/tegra/optee-src/nv-optee/optee/build/t234
install_dir=/home/seungwoo/optee/Linux_for_Tegra/sources/tegra/optee-src/nv-optee/optee/install/t234
TA_DEV_KIT_DIR=${build_dir}/export-ta_arm64/
# O=${build_dir}
O=../build
OPTEE_CLIENT_EXPORT=${install_dir}/usr

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
		--no-builtin-variables

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
