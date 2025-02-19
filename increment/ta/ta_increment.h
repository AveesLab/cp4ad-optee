// 1. Modify macro variable name(header guard), if needed.
#ifndef TA_INCREMENT_H
#define TA_INCREMENT_H

// 2. Modify UUID name and UUID.
#define TA_INCREMENT_UUID \
	{ 0x343594e3, 0xd3a3, 0x43e8, \
		{ 0x93, 0x0a, 0x70, 0x60, 0x57, 0xd3, 0x89, 0x57} }

// 3. Modify command ID.
#define CMD_INCREMENT		0
#define CMD_DECREMENT		1

#endif
