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
    unsigned char encrypted[DES_KEY_SIZE+1] = {0};
    unsigned char decrypted[DES_KEY_SIZE+1] = {0};

    mbedtls_des_context ctx;
    mbedtls_des_init(&ctx);

    // Encrypt
    mbedtls_des_setkey_enc(&ctx, key);
    mbedtls_des_crypt_ecb(&ctx, input, encrypted);
    
    // Decrypt
    mbedtls_des_setkey_dec(&ctx, key);
    mbedtls_des_crypt_ecb(&ctx, encrypted, decrypted);
    
    printf("Input     : %s\n", input);
    printf("Encrypted : %s\n", encrypted);
    print_hex("Encrypted", encrypted, 8);
    printf("Decrypted : %s\n", decrypted);
    
    mbedtls_des_free(&ctx);
}

int main() {
    des_example();
    return 0;
}