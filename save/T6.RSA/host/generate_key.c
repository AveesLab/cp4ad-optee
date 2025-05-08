#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
// 1. Modify ta header file path
#include "ta_rsa.h"
#include "user_ta_header_defines.h"

#define PRIVATE_KEY_FILE "private.pem"

void save_private_key(uint8_t *key, size_t key_size) {
    FILE *fp = fopen(PRIVATE_KEY_FILE, "wb");
    if (!fp) {
        perror("Failed to open file for private key");
        return;
    }
    fwrite(key, 1, key_size, fp);
    fclose(fp);
    printf("Private key saved to %s\n", PRIVATE_KEY_FILE);
}

int main(void) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_RSA_UUID;
    uint8_t private_key[2048];
    size_t key_size = sizeof(private_key);

    // OP-TEE 세션 생성
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed\n");
        return -1;
    }

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, NULL);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_OpenSession failed\n");
        TEEC_FinalizeContext(&ctx);
        return -1;
    }

    // 개인키 요청
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = private_key;
    op.params[0].tmpref.size = key_size;

    res = TEEC_InvokeCommand(&sess, CMD_GET_PRIVATE_KEY, &op, NULL);
    if (res == TEEC_SUCCESS) {
        save_private_key(private_key, op.params[0].tmpref.size);
    } else {
        printf("Failed to retrieve private key\n");
    }

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}

