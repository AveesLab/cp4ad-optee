// 1. Modify macro variable name(header guard), if needed.
#ifndef TA_SECRET_TEXT_H
#define TA_SECRET_TEXT_H

// 2. Modify UUID name and UUID.
#define TA_SECRET_TEXT_UUID \
	{ 0x54eac490, 0x4f7a, 0x4db5, \
		{ 0x8f, 0xd7, 0x22, 0x1b, 0x7a, 0x22, 0x39, 0xea} }

// 3. Modify command ID.
#define CMD_SECRET_TEXT 0
#define CMD_SECRET_TEXT_ADDR 1

#endif
