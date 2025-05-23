 #ifndef USER_TA_HEADER_DEFINES_H
 #define USER_TA_HEADER_DEFINES_H
 
 #include <ta.h>
 
 #define TA_UUID				{0xe4a86432, 0x409d, 0x4542, {0xa5, 0x3e, 0xe2, 0xce, 0x9d, 0x65, 0x77, 0xd3}}
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
 
 #endif /* USER_TA_HEADER_DEFINES_H */
 