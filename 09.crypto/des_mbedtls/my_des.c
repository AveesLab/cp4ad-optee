#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define DES_KEY_SIZE 8


uint64_t key = 0x0123456789ABCDEF;

// Feistel 함수 (간단한 XOR + 회전)
uint32_t feistel(uint32_t half, uint64_t key) {
    half ^= (uint32_t)(key & 0xFFFFFFFF);  // key 하위 32비트와 XOR
    return (half << 3) | (half >> (32 - 3));  // 3비트 왼쪽 회전
}

// DES-like 암복호화 함수 (1라운드)
uint64_t simple_des(uint64_t input, uint64_t key, int encrypt) {
    uint32_t left  = (input >> 32) & 0xFFFFFFFF;
    uint32_t right = input & 0xFFFFFFFF;

    if (encrypt) {
        uint32_t temp = right;
        right = left ^ feistel(right, key);
        left = temp;
    } else {
        uint32_t temp = left;
        left = right ^ feistel(left, key);
        right = temp;
    }

    return ((uint64_t)left << 32) | right;
}

// 유틸: 8바이트 블록 → 64비트 정수로
uint64_t bytes_to_uint64(unsigned char *block) {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = (result << 8) | block[i];
    }
    return result;
}

// 유틸: 64비트 정수 → 8바이트 블록
void uint64_to_bytes(uint64_t value, unsigned char *block) {
    for (int i = 7; i >= 0; i--) {
        block[i] = value & 0xFF;
        value >>= 8;
    }
}
void encrypt(unsigned char *plaintext, unsigned char *encrypted) {
    uint64_t plain64 = bytes_to_uint64(plaintext);
    uint64_t cipher64 = simple_des(plain64, key, 1);
    uint64_to_bytes(cipher64, encrypted);
}

void decrypt(unsigned char *encrypted, unsigned char *decrypted) {
    uint64_t cipher64 = bytes_to_uint64(encrypted);
    uint64_t plain64 = simple_des(cipher64, key, 0);
    uint64_to_bytes(plain64, decrypted);
}

// 테스트
int main() {
    unsigned char plaintext[] = "ABCDEFGH";   // 64비트 입력
    unsigned char encrypted[DES_KEY_SIZE+1], decrypted[DES_KEY_SIZE+1];
    printf("Input     : %s\n", plaintext);
    printf("Encrypted     : %s\n", encrypted);
    encrypt(plaintext, encrypted);
    encrypted[8] = '\0';
    printf("Input     : %s\n", plaintext);
    printf("Encrypted     : %s\n", encrypted);
    decrypt(encrypted, decrypted);
    decrypted[8] = '\0';

    
    for (int i = 0; i < 8; i++) printf("%02X ", encrypted[i]);
    printf("\nDecrypted : %s\n", plaintext);

    return 0;
}
