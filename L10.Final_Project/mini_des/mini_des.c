#include <stdio.h>
#include <stdint.h>
#include <string.h>
#define DES_KEY_SIZE 8

uint64_t key = 0x89ABCDEF01234567;

void print_hex(const char *label, const unsigned char *data, size_t len) {
    printf("%s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
}

// Feistel 함수 (간단한 XOR + 회전)
uint32_t feistel(uint32_t half, uint64_t key) {
    half ^= (uint32_t)(key & 0xFFFFFFFF);  // key 하위 32비트와 XOR
    return (half << 3) | (half >> (32 - 3));  // 3비트 왼쪽 회전
}

// DES-like 암복호화 함수 (2라운드)
uint64_t simple_des(uint64_t input, uint64_t key, int encrypt) {
    uint32_t left  = (input >> 32) & 0xFFFFFFFF;
    uint32_t right = input & 0xFFFFFFFF;

    if (encrypt) {
        for (int i = 0; i < 2; i++) {
            uint32_t temp = right;
            right = left ^ feistel(right, key);
            left = temp;
        }
    } else {
        for (int i = 0; i < 2; i++) {
            uint32_t temp = left;
            left = right ^ feistel(left, key);
            right = temp;
        }
    }

    return ((uint64_t)left << 32) | right;
}

// 8바이트 블록 → 64비트 정수로
uint64_t bytes_to_uint64(unsigned char *block) {
    uint64_t result = 0;
    for (int i = 0; i < 5; i++) {
        result = (result << 8) | block[i];
    }
    return result;
}

// 64비트 정수 → 8바이트 블록
void uint64_to_bytes(uint64_t value, unsigned char *block) {
    for (int i = 4; i >= 0; i--) {
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

int main() {
    // char plaintext[] = "ABCDE";
    // char encrypted[6] = {0};
    // char decrypted[6] = {0};
    // printf("Input    : %s\n", plaintext);
    
    // encrypt(plaintext, encrypted);
    // printf("Encrypted: %s\n", encrypted);
    // print_hex("Encrypted", encrypted, 5);

    // decrypt(encrypted, decrypted);
    // printf("Decrypted: %s\n", decrypted);


    unsigned char buf_send[8] = {'A', 'B', 'C', 'D', 'E', 0, 0, 0};
    printf("Input    : %s\n", buf_send);
    print_hex("Input", buf_send, 8);

    encrypt(buf_send, buf_send);
    printf("Encrypted: %s\n", buf_send);
    print_hex("Encrypted", buf_send, 8);

    decrypt(buf_send, buf_send);
    printf("Decrypted: %s\n", buf_send);
    print_hex("Decrypted", buf_send, 8);

    /*
    struct can_msg send_msg = {0};
    send_msg.id = 0x123;
    unsigned char len = 5;
    send_msg.len = len;
    unsigned char buf_send[8];
    buf_send[0] = ( camera_adc >> 0 ) & 0xff;
    buf_send[1] = ( lidar_adc >> 8 ) & 0xff;
    buf_send[2] = ( lidar_adc >> 0) & 0xff;
    buf_send[3] = (throttle_adc >> 8) & 0xff;
    buf_send[4] = ( throttle_adc >> 0) & 0xff;
    send_msg.buf = buf_send;
    CAN_sendMsg(send_msg);
    */
    return 0;
}
