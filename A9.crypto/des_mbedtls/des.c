#include "mbedtls/des.h"
#include <string.h>
#define DES_KEY_SIZE 8  // DES key size (64 bits = 8 bytes)

void print_hex(const char *label, const unsigned char *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

void des_example() {
    unsigned char key[] = "mykey123";       // 64bit key
    unsigned char input[] = "ABCDEFGH";     // 64bit plaintext
    unsigned char output[DES_KEY_SIZE+1] = {0};
    unsigned char decrypted[DES_KEY_SIZE+1] = {0};
    printf("Input: %s\n", input);
    mbedtls_des_context ctx;

    mbedtls_des_init(&ctx);

    // Encrypt
    mbedtls_des_setkey_enc(&ctx, key);
    mbedtls_des_crypt_ecb(&ctx, input, output);
    



    // Decrypt
    mbedtls_des_setkey_dec(&ctx, key);
    mbedtls_des_crypt_ecb(&ctx, output, decrypted);
    
    printf("Input (string): %s\n", input);
    printf("Encrypted: %s\n", output);
    print_hex("Encrypted", output, 8);
    printf("Key (string): %s\n", key);
    printf("Decrypted (string): %s\n", decrypted);
    
    mbedtls_des_free(&ctx);
}

int main() {
    des_example();
    return 0;
}