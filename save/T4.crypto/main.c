#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mbedtls/aes.h>

#define AES_KEY_SIZE 16  // AES-128 (128-bit = 16 bytes)
#define AES_BLOCK_SIZE 16

// AES key and Nonce settings (Nonce is used as the initial counter value)
unsigned char key[AES_KEY_SIZE] = "1234567890abcdef";  // Example key (should be securely managed in a real environment)
unsigned char nonce[AES_BLOCK_SIZE] = "abcdef1234567890"; // Nonce (should be randomly generated in a real environment)

// Function to read a file
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

// Function to write data to a file
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

// AES-128 CTR encryption & decryption (using mbedTLS)
int aes_ctr_crypt(const char *src, const char *dst) {
    size_t size;
    unsigned char *input = read_file(src, &size);
    if (!input) return -1;

    unsigned char *output = (unsigned char *)malloc(size);
    if (!output) {
        perror("failed to allocate memory");
        free(input);
        return -1;
    }

    // Initialize mbedTLS AES-CTR
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, key, 128);

    unsigned char stream_block[AES_BLOCK_SIZE] = {0}; // Stream block buffer
    size_t nc_off = 0;  // Nonce counter offset
    unsigned char counter[AES_BLOCK_SIZE]; 
    memcpy(counter, nonce, AES_BLOCK_SIZE); // Set the initial counter value

    // Perform encryption or decryption (same function is used for both)
    mbedtls_aes_crypt_ctr(&aes, size, &nc_off, counter, stream_block, input, output);

    // Free memory and save the result
    mbedtls_aes_free(&aes);
    int result = write_file(dst, output, size);
    
    free(input);
    free(output);
    return result;
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("usage: %s <0:encrypt|1:decrypt> <src> <dst>\n", argv[0]);
        return 1;
    }

    int mode = atoi(argv[1]);
    const char *src = argv[2];
    const char *dst = argv[3];

    if (mode == 0) {
        printf("file encoding...\n");
    } else if (mode == 1) {
        printf("file decoding...\n");
    } else {
        printf("invalid argument: 0 (encrypt), 1 (decrypt)\n");
        return 1;
    }

    if (aes_ctr_crypt(src, dst) == 0) {
        printf("%s Success!: %s -> %s\n", (mode == 0) ? "encrypt" : "decrypt", src, dst);
    } else {
        printf("%s Fail..\n", (mode == 0) ? "encrypt" : "decrypt");
    }

    return 0;
}
