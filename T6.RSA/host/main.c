#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
// 1. Modify ta header file path
#include <mbedtls/rsa.h>
#include <mbedtls/bignum.h>
#include <mbedtls/pk.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <string.h>
#include "ta_rsa.h"
#include "user_ta_header_defines.h"

#define PRIVATE_KEY_FILE "private.pem"
#define ORIGINAL_FILE "original.txt"
#define ENCRYPTED_FILE "encrypted.dat"

size_t read_file(const char *filename, uint8_t **buffer) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open file");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    *buffer = malloc(file_size);
    fread(*buffer, 1, file_size, fp);
    fclose(fp);

    return file_size;
}

size_t encrypt_with_private_key(uint8_t *input, size_t input_size, uint8_t **encrypted_output) {
    mbedtls_pk_context pk;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    int ret;

    // 컨텍스트 초기화
    mbedtls_pk_init(&pk);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    // 난수 생성기 초기화
    ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);
    if (ret != 0) {
        printf("Failed to initialize random generator.\n");
        goto cleanup;
    }

    // 개인키 로드 (PEM 파일에서 가져옴)
    ret = mbedtls_pk_parse_keyfile(&pk, PRIVATE_KEY_FILE, NULL);
    if (ret != 0) {
        printf("Failed to load private key from file.\n");
        goto cleanup;
    }

    // RSA 키인지 확인
    if (!mbedtls_pk_can_do(&pk, MBEDTLS_PK_RSA)) {
        printf("The key is not an RSA key!\n");
        goto cleanup;
    }

    // 메모리 할당
    *encrypted_output = malloc(256);
    if (*encrypted_output == NULL) {
        printf("Memory allocation failed.\n");
        goto cleanup;
    }
    memset(*encrypted_output, 0, 256);

    // 암호화 수행 (개인키 사용)
    ret = mbedtls_rsa_pkcs1_encrypt(mbedtls_pk_rsa(pk), mbedtls_ctr_drbg_random, &ctr_drbg,
                                    MBEDTLS_RSA_PRIVATE, input_size, input, *encrypted_output);
    if (ret != 0) {
        printf("Encryption failed.\n");
        free(*encrypted_output);
        *encrypted_output = NULL;
        goto cleanup;
    }

    // 성공적으로 암호화됨
    ret = 256;

cleanup:
    mbedtls_pk_free(&pk);
    mbedtls_entropy_free(&entropy);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    return ret;
}


int main(void) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    uint8_t *original_data;
    size_t original_size = read_file(ORIGINAL_FILE, &original_data);
    if (original_size == 0) {
        printf("Failed to read original file.\n");
        return -1;
    }

    uint8_t *encrypted_data;
    size_t encrypted_size = encrypt_with_private_key(original_data, original_size, &encrypted_data);
    if (encrypted_size == 0) {
        printf("Encryption failed.\n");
        free(original_data);
        return -1;
    }

    /* Initialize OP-TEE context */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InitializeContext failed\n");
        free(original_data);
        free(encrypted_data);
        return -1;
    }

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = original_data;
    op.params[0].tmpref.size = original_size;
    op.params[1].tmpref.buffer = encrypted_data;
    op.params[1].tmpref.size = encrypted_size;

    /* Open OP-TEE session */
// 2. Modify ta UUID
    TEEC_UUID uuid = TA_RSA_UUID;

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, &op, NULL, NULL);
    if (res == TEEC_SUCCESS) {
        printf("Session opened successfully.\n");
    } else {
        printf("Session failed to open.\n");
    }

    /* Execute TA command */

// 3. Modify operation parameters (if needed) //
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

// 4. Modify command ID //
    // res = TEEC_InvokeCommand(&sess, CMD_RSA, &op, &err_origin);
    // if (res != TEEC_SUCCESS)
    //     errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);

    // printf("TA command executed successfully!\n");

    /* Terminate session and context */
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}
