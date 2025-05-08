#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
// 1. Modify ta header file path
#include "ta_optee_crypto.h"
#include "user_ta_header_defines.h"
#define AES_KEY_SIZE 16

int store_aes_key_in_tee() {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_OPTEE_CRYPTO_UUID;
    uint32_t origin;

    uint8_t aes_key[AES_KEY_SIZE];

    printf("Enter a 16-character AES key: ");
    fgets((char *)aes_key, AES_KEY_SIZE + 1, stdin);
    
    // 개행 문자 제거
    aes_key[strcspn((char *)aes_key, "\n")] = 0;

    // TEE 연결
    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    // AES 키 저장 요청
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = aes_key;
    op.params[0].tmpref.size = AES_KEY_SIZE;

    TEEC_InvokeCommand(&sess, CMD_SAVE_KEY, &op, &origin);

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}

int main() {
    printf("Storing AES key in Secure Storage...\n");
    store_aes_key_in_tee();
    printf("AES key stored successfully!\n");
    return 0;
}
