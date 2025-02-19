// 1. Modify macro variable name(header guard), if needed.
#ifndef TA_H
#define TA_H

// 2. Modify UUID name and UUID.
#define MY_EXAMPLE_UUID \
	{ 0xa17cfba6, 0xfb31, 0x401d, \
		{ 0xb6, 0x7d, 0xcf, 0x78, 0x3c, 0x06, 0x46, 0xde} }
#define TA_TEST_CMD_INC_VALUE		0
#define TA_TEST_CMD_DEC_VALUE		1

// 3. Modify command ID.
#define CMD_HELLO 0

#endif
