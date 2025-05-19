#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 5
#define KEY_SIZE 5

// 5바이트 (40비트) 키
uint64_t key = 0xABCDE12345;  // 상위 3바이트 무시됨

void print_hex(const char *label, const unsigned char *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// 간단한 Feistel 함수 (XOR + 3비트 회전)
uint32_t feistel(uint32_t half, uint64_t key40) {
    uint32_t key_part = (uint32_t)(key40 & 0xFFFFFFFF);  // 하위 32비트만 사용
    half ^= key_part;
    return (half << 3) | (half >> (32 - 3));
}

// 간단한 DES-like 암복호화 (2라운드, 40비트 키)
uint64_t simple_des(uint64_t input, uint64_t key40, int encrypt) {
    uint32_t left  = (input >> 20) & 0xFFFFF;  // 20비트
    uint32_t right = input & 0xFFFFF;          // 20비트

    if (encrypt) {
        for (int i = 0; i < 2; i++) {
            uint32_t temp = right;
            right = left ^ (feistel(right, key40) & 0xFFFFF);  // 20비트 마스킹
            left = temp;
        }
    } else {
        for (int i = 0; i < 2; i++) {
            uint32_t temp = left;
            left = right ^ (feistel(left, key40) & 0xFFFFF);
            right = temp;
        }
    }

    return (((uint64_t)left << 20) | right) & 0xFFFFFFFFFFULL;  // 40비트 출력
}

// 5바이트 → uint64_t (하위 40비트 사용)
uint64_t bytes_to_uint40(const unsigned char *block) {
    uint64_t result = 0;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        result = (result << 8) | block[i];
    }
    return result & 0xFFFFFFFFFFULL;  // 하위 40비트만 유지
}

// uint64_t → 5바이트
void uint40_to_bytes(uint64_t value, unsigned char *block) {
    for (int i = BLOCK_SIZE - 1; i >= 0; i--) {
        block[i] = value & 0xFF;
        value >>= 8;
    }
}

void encrypt(unsigned char *plaintext, unsigned char *encrypted) {
    uint64_t plain40 = bytes_to_uint40(plaintext);
    uint64_t cipher40 = simple_des(plain40, key, 1);
    uint40_to_bytes(cipher40, encrypted);
}

void decrypt(unsigned char *encrypted, unsigned char *decrypted) {
    uint64_t cipher40 = bytes_to_uint40(encrypted);
    uint64_t plain40 = simple_des(cipher40, key, 0);
    uint40_to_bytes(plain40, decrypted);
}

int main() {
    char plaintext[] = "ABCDE";
    char encrypted[KEY_SIZE+1] = {0};
    char decrypted[KEY_SIZE+1] = {0};
    printf("Input    : %s\n", plaintext);
    
    encrypt(plaintext, encrypted);
    printf("Encrypted: %s\n", encrypted);
    print_hex("Encrypted", encrypted, 5);

    decrypt(encrypted, decrypted);
    printf("Decrypted: %s\n", decrypted);

    return 0;
}
