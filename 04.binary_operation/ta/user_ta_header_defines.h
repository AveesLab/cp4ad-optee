// No Need to modify this file.

#ifndef __USER_TA_HEADER_DEFINES_H__
#define __USER_TA_HEADER_DEFINES_H__

#include <ta.h>
#define TA_UUID         {0x7eafaf96, 0x2de7, 0x4d68, {0xa8, 0xa6, 0x84, 0x51, 0x2e, 0x00, 0x4a, 0x1b}}
#define TA_FLAGS			TA_FLAG_EXEC_DDR
#define TA_STACK_SIZE			(2 * 1024)
#define TA_DATA_SIZE			(32 * 1024)
#define TA_VERSION	"1.0"
#define TA_DESCRIPTION	"Example of OP-TEE Hello World Trusted Application"
#define TA_CURRENT_TA_EXT_PROPERTIES \
    { "org.linaro.optee.examples.hello_world.property1", \
    USER_TA_PROP_TYPE_STRING, \
        "Some string" }, \
    { "org.linaro.optee.examples.hello_world.property2", \
    USER_TA_PROP_TYPE_U32, &(const uint32_t){ 0x0010 } }

#endif
