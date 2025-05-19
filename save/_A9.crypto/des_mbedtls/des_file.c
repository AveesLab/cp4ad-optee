#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mbedtls/des.h>

#define DES_KEY_SIZE 8      // 64비트 키
#define DES_BLOCK_SIZE 8    // 64비트 블록

unsigned char des_key[DES_KEY_SIZE] = "mykey123"; // DES 키 (8바이트)

// 파일 읽기 함수
unsigned char *read_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("cannot open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    rewind(file);

    // 8바이트 패딩 처리
    size_t padded_size = (*size + DES_BLOCK_SIZE - 1) / DES_BLOCK_SIZE * DES_BLOCK_SIZE;
    unsigned char *data = (unsigned char *)calloc(1, padded_size);  // zero-padding
    if (!data) {
        perror("failed to allocate memory");
        fclose(file);
        return NULL;
    }

    fread(data, 1, *size, file);
    *size = padded_size;

    fclose(file);
    return data;
}

// 파일 쓰기 함수
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

// DES ECB 암호화/복호화 함수
int des_ecb_crypt(const char *src, const char *dst, int encrypt) {
    size_t size;
    unsigned char *input = read_file(src, &size);
    if (!input) return -1;

    unsigned char *output = (unsigned char *)malloc(size);
    if (!output) {
        perror("failed to allocate memory");
        free(input);
        return -1;
    }

    mbedtls_des_context ctx;
    mbedtls_des_init(&ctx);

    if (encrypt)
        mbedtls_des_setkey_enc(&ctx, des_key);
    else
        mbedtls_des_setkey_dec(&ctx, des_key);

    for (size_t i = 0; i < size; i += DES_BLOCK_SIZE) {
        mbedtls_des_crypt_ecb(&ctx, input + i, output + i);
    }

    mbedtls_des_free(&ctx);
    int result = write_file(dst, output, size);

    free(input);
    free(output);
    return result;
}

// main 함수
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: %s <0:encrypt|1:decrypt> <src> <dst>\n", argv[0]);
        return 1;
    }

    int mode = atoi(argv[1]);
    const char *src = argv[2];
    const char *dst = argv[3];

    if (mode == 0) {
        printf("file encrypting...\n");
    } else if (mode == 1) {
        printf("file decrypting...\n");
    } else {
        printf("invalid argument: 0 (encrypt), 1 (decrypt)\n");
        return 1;
    }

    if (des_ecb_crypt(src, dst, mode == 0) == 0) {
        printf("%s Success!: %s -> %s\n", (mode == 0) ? "Encrypt" : "Decrypt", src, dst);
    } else {
        printf("%s Fail..\n", (mode == 0) ? "Encrypt" : "Decrypt");
    }

    return 0;
}
