#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta_optee_crypto.h"
#include <string.h>
#include <mbedtls/aes.h>
#include <mbedtls/cipher.h>

#define AES_KEY_SIZE 16
#define AES_BLOCK_SIZE 16

static const char *key_id = "aes_key";

TEE_Result TA_CreateEntryPoint(void) {
    DMSG("TA created.");
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
    DMSG("TA destroyed.");
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    (void)param_types;
    (void)params;
    (void)sess_ctx;
    IMSG("Session opened.");
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void)sess_ctx;
    IMSG("Session closed.");
}

// ✅ Secure Storage에 AES 키 저장
TEE_Result save_aes_key(uint8_t *key, size_t key_size) {
    TEE_ObjectHandle object;
    TEE_Result res;

    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, key_id, strlen(key_id),
                                     TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ,
                                     TEE_HANDLE_NULL, key, key_size, &object);
    if (res != TEE_SUCCESS) {
        EMSG("Failed to store AES key.");
        return res;
    }

    TEE_CloseObject(object);
    return TEE_SUCCESS;
}

// ✅ Secure Storage에서 AES 키 불러오기
TEE_Result load_aes_key(uint8_t *key, size_t key_size) {
    TEE_ObjectHandle object;
    TEE_Result res;
    size_t read_size;

    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, key_id, strlen(key_id),
                                   TEE_DATA_FLAG_ACCESS_READ, &object);
    if (res != TEE_SUCCESS) {
        EMSG("No AES key found in Secure Storage.");
        return res;
    }

    res = TEE_ReadObjectData(object, key, key_size, &read_size);
    TEE_CloseObject(object);

    if (res != TEE_SUCCESS || read_size != key_size) {
        return TEE_ERROR_GENERIC;
    }
    return TEE_SUCCESS;
}

// ✅ 저장된 AES 키를 불러와서 복호화 수행
TEE_Result crypt_data(uint8_t *ciphertext, size_t size, uint8_t *plaintext) {
    uint8_t aes_key[AES_KEY_SIZE];
    uint8_t nonce[AES_BLOCK_SIZE] = "abcdef1234567890";  // Nonce 값 (랜덤하게 설정해야 함)

    // 🔹 Secure Storage에서 AES 키 불러오기
    if (load_aes_key(aes_key, AES_KEY_SIZE) != TEE_SUCCESS) {
        return TEE_ERROR_ITEM_NOT_FOUND;
    }

    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);

    // AES 키 설정 (복호화와 암호화 동일)
    if (mbedtls_aes_setkey_enc(&aes, aes_key, 128) != 0) {
        mbedtls_aes_free(&aes);
        return TEE_ERROR_GENERIC;
    }

    unsigned char stream_block[AES_BLOCK_SIZE] = {0};  // 스트림 블록 버퍼
    size_t nc_off = 0;  // Nonce counter offset
    unsigned char counter[AES_BLOCK_SIZE];

    memcpy(counter, nonce, AES_BLOCK_SIZE);  // Nonce를 카운터로 설정

    
    // AES CTR 복호화 수행 (암호화와 동일한 함수 사용)
    if (mbedtls_aes_crypt_ctr(&aes, size, &nc_off, counter, stream_block, ciphertext, plaintext) != 0) {
        mbedtls_aes_free(&aes);
        return TEE_ERROR_GENERIC;
    }

    mbedtls_aes_free(&aes);
    return TEE_SUCCESS;
}

// ✅ TEE 내부 명령어 처리
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id, uint32_t param_types,
                                      TEE_Param params[4]) {
    if (cmd_id == CMD_SAVE_KEY) {
        return save_aes_key(params[0].memref.buffer, AES_KEY_SIZE);
    } else if (cmd_id == CMD_OPTEE_CRYPTO) {
        return crypt_data(params[0].memref.buffer, params[0].memref.size,
                            params[1].memref.buffer);
    }
    return TEE_ERROR_BAD_PARAMETERS;
}
