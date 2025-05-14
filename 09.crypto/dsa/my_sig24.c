//Micro EdDSA Lite Signature
// 24비트 서명 알고리즘
// 2바이트 개인키, 3바이트 서명
// 2바이트 공개키 (개인키 XOR 0xA5)
// 5바이트 메시지
// 24비트 해시 (XOR + 회전)
// 3바이트 서명
// 2바이트 개인키 → 공개키 (개인키 XOR 0xA5)
// 3바이트 서명 = 해시(메시지 || 개인키)
// 검증: 서명 == 해시(메시지 || 공개키 XOR 0xA5)

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MICRO_KEYLEN 2     // 2바이트 개인키/공개키
#define MICRO_SIGLEN 3     // 3바이트 서명

// 간단한 해시 함수 (XOR + 회전) → 24비트 출력
uint32_t mini_hash(const uint8_t *data, size_t len) {
    uint32_t hash = 0xABCDE1;
    for (size_t i = 0; i < len; i++) {
        hash ^= (data[i] + i * 13);
        hash = (hash << 5) | (hash >> (27)); // 5비트 좌회전
    }
    return hash & 0xFFFFFF;
}

// 개인키 → 공개키 (공개키는 priv ⊕ 0xA5)
void generate_public_key(uint8_t *pub, const uint8_t *priv) {
    for (int i = 0; i < MICRO_KEYLEN; i++) {
        pub[i] = priv[i] ^ 0xA5;
    }
}

// 서명 함수: signature = hash(message || priv_key)
void micro_sign(const uint8_t *message, size_t msg_len, const uint8_t *private_key, uint8_t *signature) {
    uint8_t buffer[64];
    memcpy(buffer, message, msg_len);
    memcpy(buffer + msg_len, private_key, MICRO_KEYLEN);

    uint32_t sig = mini_hash(buffer, msg_len + MICRO_KEYLEN);

    signature[0] = (sig >> 16) & 0xFF;
    signature[1] = (sig >> 8) & 0xFF;
    signature[2] = sig & 0xFF;
}

// 검증 함수: signature == hash(message || derived_private_key)
int micro_verify(const uint8_t *message, size_t msg_len, const uint8_t *signature, const uint8_t *public_key) {
    uint8_t derived_priv[MICRO_KEYLEN];
    for (int i = 0; i < MICRO_KEYLEN; i++) {
        derived_priv[i] = public_key[i] ^ 0xA5;
    }

    uint8_t buffer[64];
    memcpy(buffer, message, msg_len);
    memcpy(buffer + msg_len, derived_priv, MICRO_KEYLEN);

    uint32_t expected = mini_hash(buffer, msg_len + MICRO_KEYLEN);
    uint32_t recv = (signature[0] << 16) | (signature[1] << 8) | signature[2];

    return (expected == recv);
}

// MAIN 함수
int main() {
    // 5바이트 메시지
    uint8_t message[5] = {0x11, 0x22, 0x33, 0x44, 0x55};

    // 개인키 (2바이트)
    uint8_t private_key[MICRO_KEYLEN] = {0x12, 0x34};

    // 공개키 계산
    uint8_t public_key[MICRO_KEYLEN];
    generate_public_key(public_key, private_key);

    // 서명
    uint8_t signature[MICRO_SIGLEN];
    micro_sign(message, sizeof(message), private_key, signature);

    // 출력
    printf("Message     : ");
    for (int i = 0; i < 5; i++) printf("%02X ", message[i]);
    printf("\nSignature   : %02X %02X %02X\n", signature[0], signature[1], signature[2]);
    printf("Public Key  : %02X %02X\n", public_key[0], public_key[1]);

    // 검증
    if (micro_verify(message, sizeof(message), signature, public_key)) {
        printf("✅ Signature Verified: Sender is authentic\n");
    } else {
        printf("❌ Signature Invalid\n");
    }

    // 공격자 테스트: 다른 개인키로 서명한 메시지를 public_key로 검증
    uint8_t fake_priv[MICRO_KEYLEN] = {0x99, 0x88};
    uint8_t fake_sig[MICRO_SIGLEN];
    micro_sign(message, sizeof(message), fake_priv, fake_sig);

    if (micro_verify(message, sizeof(message), fake_sig, public_key)) {
        printf("❌ Attack Passed (should not happen!)\n");
    } else {
        printf("✅ Attack Detected (verification failed as expected)\n");
    }

    return 0;
}
