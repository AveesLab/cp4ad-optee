## TA Makefile

O ?= ../build/ta
CFG_TEE_TA_LOG_LEVEL ?= 4

CPPFLAGS += -DCFG_TEE_TA_LOG_LEVEL=$(CFG_TEE_TA_LOG_LEVEL)

## UUID (BINARY 이름)
BINARY := b6486f4a-81bd-4bab-8674-71ea9bf8e51e

## TA 빌드 규칙
include $(TA_DEV_KIT_DIR)/mk/ta_dev_kit.mk
