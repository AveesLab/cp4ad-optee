#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
// 1. Modify ta header file path
#include "ta_optee_crypto.h"

#include "user_ta_header_defines.h"
#define AES_BLOCK_SIZE 16

unsigned char *read_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("cannot open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    unsigned char *data = (unsigned char *)malloc(*size);
    if (!data) {
        perror("failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(data, 1, *size, file);
    fclose(file);
    return data;
}

// 파일 저장 함수
int write_file(const char *filename, unsigned char *data, size_t size) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("cannot open file");
        return -1;
    }
    fwrite(data, 1, size, file);
    fclose(file);
    return 0;
}

// int request_tee_decryption(const char *src, const char *dst) {
//     TEEC_Context ctx;
//     TEEC_Session sess;
//     TEEC_Operation op;
//     TEEC_UUID uuid = TA_AES_UUID;
//     uint32_t origin;
//     size_t size;

//     // 암호화된 파일 읽기
//     unsigned char *ciphertext = read_file(src, &size);
//     if (!ciphertext) return -1;

//     unsigned char *plaintext = (unsigned char *)malloc(size);
//     if (!plaintext) {
//         perror("failed to allocate memory");
//         free(ciphertext);
//         return -1;
//     }

//     // TEE 연결 초기화
//     TEEC_InitializeContext(NULL, &ctx);
//     TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

//     // TEE 내부 복호화 요청
//     op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
//     op.params[0].tmpref.buffer = ciphertext;
//     op.params[0].tmpref.size = size;
//     op.params[1].tmpref.buffer = plaintext;
//     op.params[1].tmpref.size = size;

//     TEEC_InvokeCommand(&sess, 0, &op, &origin);

//     // 복호화된 데이터 저장
//     write_file(dst, plaintext, size);

//     // 리소스 해제
//     free(ciphertext);
//     free(plaintext);
//     TEEC_CloseSession(&sess);
//     TEEC_FinalizeContext(&ctx);
//     return 0;
// }

int request_tee_decryption(const char *src, const char *dst) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_OPTEE_CRYPTO_UUID;
    uint32_t origin;
    size_t size;

    unsigned char *ciphertext = read_file(src, &size);
    if (!ciphertext) return -1;

    unsigned char *plaintext = (unsigned char *)malloc(size);
    if (!plaintext) {
        perror("failed to allocate memory");
        free(ciphertext);
        return -1;
    }

    // TEE 연결
    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    // TEE 내부 복호화 요청
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = ciphertext;
    op.params[0].tmpref.size = size;
    op.params[1].tmpref.buffer = plaintext;
    op.params[1].tmpref.size = size;

    TEEC_Result res = TEEC_InvokeCommand(&sess, CMD_OPTEE_CRYPTO, &op, &origin);

    if (res == TEEC_ERROR_ITEM_NOT_FOUND) {
        printf("No AES key found in Secure Storage! Please run ./save_key first.\n");
        return -1;
    }

    write_file(dst, plaintext, size);

    free(ciphertext);
    free(plaintext);
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <encrypted_file> <output_file>\n", argv[0]);
        return 1;
    }

    printf("Requesting TEE to decrypt file...\n");
    if (request_tee_decryption(argv[1], argv[2]) == 0) {
        printf("Decryption successful: %s -> %s\n", argv[1], argv[2]);
    } else {
        printf("Decryption failed\n");
    }

    return 0;
}