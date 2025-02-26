#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <mbedtls/rsa.h>
#include <mbedtls/bignum.h>
#include <string.h>
#include "ta_rsa.h"

#define STORAGE_ID TEE_STORAGE_PRIVATE
#define RSA_KEY_OBJECT_ID "rsa_public_key"

TEE_Result generate_rsa_keypair(void) {
    TEE_Result res;
    TEE_ObjectHandle keypair;
    uint8_t public_key[256];
    size_t pub_key_size = sizeof(public_key);

    // RSA 키 생성
    res = TEE_AllocateTransientObject(TEE_TYPE_RSA_KEYPAIR, 2048, &keypair);
    if (res != TEE_SUCCESS) {
        return res;
    }

    res = TEE_GenerateKey(keypair, 2048, NULL, 0);
    if (res != TEE_SUCCESS) {
        TEE_FreeTransientObject(keypair);
        return res;
    }

    // 공개키 모듈러스(N) 추출
    res = TEE_GetObjectBufferAttribute(keypair, TEE_ATTR_RSA_MODULUS, public_key, &pub_key_size);
    if (res != TEE_SUCCESS) {
        TEE_FreeTransientObject(keypair);
        return res;
    }

    // 공개키를 Secure Storage에 저장
    TEE_ObjectHandle storage;
    res = TEE_CreatePersistentObject(STORAGE_ID, RSA_KEY_OBJECT_ID, strlen(RSA_KEY_OBJECT_ID),
                                     TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_READ,
                                     NULL, public_key, pub_key_size, &storage);

    TEE_FreeTransientObject(keypair);
    TEE_CloseObject(storage);
    return res;
}

TEE_Result get_rsa_public_key(uint8_t *buffer, size_t *size) {
    TEE_Result res;
    TEE_ObjectHandle storage;

    // Secure Storage에서 공개키 읽기
    res = TEE_OpenPersistentObject(STORAGE_ID, RSA_KEY_OBJECT_ID, strlen(RSA_KEY_OBJECT_ID),
                                   TEE_DATA_FLAG_ACCESS_READ, &storage);
    if (res != TEE_SUCCESS) {
        return res;
    }

    res = TEE_ReadObjectData(storage, buffer, *size, size);
    TEE_CloseObject(storage);
    return res;
}

TEE_Result decrypt_with_public_key(uint8_t *encrypted, size_t enc_size, uint8_t *decrypted, size_t *dec_size) {
    TEE_Result res;
    mbedtls_rsa_context rsa;
    uint8_t public_key[256];
    size_t pub_key_size = sizeof(public_key);

    // Secure Storage에서 공개키 가져오기
    res = get_rsa_public_key(public_key, &pub_key_size);
    if (res != TEE_SUCCESS) {
        return res;
    }

    mbedtls_rsa_init(&rsa, MBEDTLS_RSA_PKCS_V15, 0);

    // 공개 지수(E) 설정 (RSA 65537 = 0x10001)
    unsigned char exponent[3] = {0x01, 0x00, 0x01};

    // 공개키 로드
    if (mbedtls_rsa_import_raw(&rsa,
                                public_key, pub_key_size,  // N (모듈러스)
                                NULL, 0, NULL, 0, NULL, 0,  // P, Q, D 없음 (공개키만 사용)
                                exponent, sizeof(exponent)) != 0) {  // E (공개 지수)
        return TEE_ERROR_SECURITY;
    }

    // 복호화 수행
    if (mbedtls_rsa_pkcs1_decrypt(&rsa, NULL, NULL, MBEDTLS_RSA_PUBLIC, dec_size,
                                  encrypted, decrypted, 256) != 0) {
        return TEE_ERROR_SECURITY;
    }

    mbedtls_rsa_free(&rsa);
    return TEE_SUCCESS;
}

// TA Entry Point Functions
TEE_Result TA_CreateEntryPoint(void) {
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    if (param_types != TEE_PARAM_TYPES(TEE_MEM_INPUT, TEE_MEM_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE))
        return TEE_ERROR_BAD_PARAMETERS;

    uint8_t *original = params[0].memref.buffer;
    size_t original_size = params[0].memref.size;

    uint8_t *encrypted = params[1].memref.buffer;
    size_t encrypted_size = params[1].memref.size;

    uint8_t decrypted[256];
    size_t decrypted_size = sizeof(decrypted);

    TEE_Result res = decrypt_with_public_key(encrypted, encrypted_size, decrypted, &decrypted_size);
    if (res != TEE_SUCCESS) {
        return TEE_ERROR_SECURITY;
    }

    if (decrypted_size != original_size || TEE_MemCompare(original, decrypted, original_size) != 0) {
        return TEE_ERROR_SECURITY;
    }

    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id,
    uint32_t param_types, TEE_Param params[4]) {
    switch (cmd_id) {
    case CMD_GET_PRIVATE_KEY:
        return generate_rsa_keypair();
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
