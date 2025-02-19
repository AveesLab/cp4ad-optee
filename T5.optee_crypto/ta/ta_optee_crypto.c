#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta_optee_crypto.h"
#include <string.h>

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

// Secure Storage에 AES 키 저장
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

// Secure Storage에서 AES 키 불러오기
TEE_Result load_aes_key(uint8_t *key, size_t key_size) {
    TEE_ObjectHandle object;
    TEE_Result res;
    uint32_t read_size;

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

// 암호화된 데이터를 복호화
TEE_Result decrypt_data(uint8_t *ciphertext, size_t size, uint8_t *plaintext) {
    uint8_t aes_key[AES_KEY_SIZE];
    TEE_OperationHandle op;
    TEE_ObjectHandle key_handle;
    TEE_Attribute attr;

    if (load_aes_key(aes_key, AES_KEY_SIZE) != TEE_SUCCESS) {
        return TEE_ERROR_ITEM_NOT_FOUND;
    }

    TEE_AllocateTransientObject(TEE_TYPE_AES, AES_KEY_SIZE * 8, &key_handle);
    TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE, aes_key, AES_KEY_SIZE);
    TEE_PopulateTransientObject(key_handle, &attr, 1);
    TEE_AllocateOperation(&op, TEE_ALG_AES_CTR, TEE_MODE_DECRYPT, AES_KEY_SIZE * 8);
    TEE_SetOperationKey(op, key_handle);
    
    uint8_t nonce[AES_BLOCK_SIZE] = {0};
    TEE_CipherInit(op, nonce, AES_BLOCK_SIZE);

    // 복호화 수행
    TEE_CipherDoFinal(op, ciphertext, size, plaintext, &size);

    // 리소스 해제
    TEE_FreeOperation(op);
    TEE_FreeTransientObject(key_handle);

    return TEE_SUCCESS;
}

// TEE 내부 명령어 처리
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id, uint32_t param_types,
                                      TEE_Param params[4]) {
    if (cmd_id == CMD_SAVE_KEY) {
        return save_aes_key(params[0].memref.buffer, AES_KEY_SIZE);
    } else if (cmd_id == CMD_OPTEE_CRYPTO) {
        return decrypt_data(params[0].memref.buffer, params[0].memref.size,
                            params[1].memref.buffer);
    }
    return TEE_ERROR_BAD_PARAMETERS;
}
