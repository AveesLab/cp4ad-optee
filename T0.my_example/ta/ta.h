// 1. Modify macro variable name(header guard), if needed.
#ifndef TA_HELLO_WORLD_H
#define TA_HELLO_WORLD_H

// 2. Modify UUID name.
#define TA_UUID				{ 0xb6486f4a, 0x81bd, 0x4bab, \
	{ 0x86, 0x74, 0x71, 0xea, 0x9b, 0xf8, 0xe5, 0x1e} }

// 3. Modify command ID.
#define CMD_HELLO 0
#define CMD_GOODBYE 1
#define CMD_INCREMENT		0
#define CMD_DECREMENT		1

#endif

