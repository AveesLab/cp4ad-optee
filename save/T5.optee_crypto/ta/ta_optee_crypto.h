// 1. Modify macro variable name(header guard), if needed.
#ifndef TA_OPTEE_CRYPTO_H
#define TA_OPTEE_CRYPTO_H

// 2. Modify UUID name and UUID.
#define TA_OPTEE_CRYPTO_UUID \
	{ 0xe4a86432, 0x409d, 0x4542, \
		{ 0xa5, 0x3e, 0xe2, 0xce, 0x9d, 0x65, 0x77, 0xd3} }

// 3. Modify command ID.
#define CMD_OPTEE_CRYPTO 0
#define CMD_SAVE_KEY 1

// TEE_Result save_aes_key(uint8_t *key, size_t key_size);
// TEE_Result load_aes_key(uint8_t *key, size_t key_size);
// TEE_Result decrypt_data(uint8_t *ciphertext, size_t size, uint8_t *plaintext);

#endif
