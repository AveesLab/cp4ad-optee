#include <stdio.h>
#include <string.h>

#define MICRO_KEYLEN 2
#define MICRO_SIGLEN 3

// 해시 함수: XOR + 회전 → 24비트 (3바이트) 출력
void mini_hash(const unsigned char *data, size_t len, unsigned char hash[3]) {
    unsigned int h = (hash[0] << 16) | (hash[1] << 8) | hash[2];

    for (size_t i = 0; i < len; i++) {
        h ^= (data[i] + i * 13);
        h = (h << 5) | (h >> (27));
    }

    h &= 0xFFFFFF;
    hash[0] = (h >> 16) & 0xFF;
    hash[1] = (h >> 8) & 0xFF;
    hash[2] = h & 0xFF;
}

// 개인키 → 공개키 (XOR with 0xA5)
void generate_public_key(unsigned char *pub, const unsigned char *priv) {
    for (int i = 0; i < MICRO_KEYLEN; i++) {
        pub[i] = priv[i] ^ 0xA5;
    }
}

// 서명: signature = hash(message || private_key)
void micro_sign(const unsigned char *message, size_t msg_len,
                const unsigned char *private_key, unsigned char *signature,
                unsigned char hash[3]) {
    unsigned char buffer[64];
    memcpy(buffer, message, msg_len);
    memcpy(buffer + msg_len, private_key, MICRO_KEYLEN);

    mini_hash(buffer, msg_len + MICRO_KEYLEN, hash);
    memcpy(signature, hash, MICRO_SIGLEN);
}

// 검증: 서명 == hash(message || derived_private_key)
int micro_verify(const unsigned char *message, size_t msg_len,
                 const unsigned char *signature,
                 const unsigned char *public_key,
                 unsigned char hash[3]) {
    unsigned char derived_priv[MICRO_KEYLEN];
    for (int i = 0; i < MICRO_KEYLEN; i++) {
        derived_priv[i] = public_key[i] ^ 0xA5;
    }

    unsigned char buffer[64];
    memcpy(buffer, message, msg_len);
    memcpy(buffer + msg_len, derived_priv, MICRO_KEYLEN);

    mini_hash(buffer, msg_len + MICRO_KEYLEN, hash);
    return (memcmp(signature, hash, 3) == 0);
}

int main() {
    unsigned char message[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
    unsigned char hash[3] = {0xAB, 0xCD, 0xE1};  // 초기 해시값

    unsigned char private_key[MICRO_KEYLEN] = {0x12, 0x34};
    unsigned char public_key[MICRO_KEYLEN];

    generate_public_key(public_key, private_key);

    unsigned char signature[MICRO_SIGLEN];
    micro_sign(message, sizeof(message), private_key, signature, hash);

    printf("Message     : ");
    for (int i = 0; i < 5; i++) printf("%02X ", message[i]);

    printf("\nSignature   : %02X %02X %02X\n", signature[0], signature[1], signature[2]);
    printf("Public Key  : %02X %02X\n", public_key[0], public_key[1]);

    // 검증을 위해 해시 초기화 필요
    hash[0] = 0xAB; hash[1] = 0xCD; hash[2] = 0xE1;
    if (micro_verify(message, sizeof(message), signature, public_key, hash)) {
        printf("✅ Signature Verified: Sender is authentic\n");
    } else {
        printf("❌ Signature Invalid\n");
    }

    // 공격자 테스트
    unsigned char fake_priv[MICRO_KEYLEN] = {0x99, 0x88};
    unsigned char fake_sig[MICRO_SIGLEN];

    // 해시 다시 초기화
    hash[0] = 0xAB; hash[1] = 0xCD; hash[2] = 0xE1;
    micro_sign(message, sizeof(message), fake_priv, fake_sig, hash);

    // 해시 다시 초기화
    hash[0] = 0xAB; hash[1] = 0xCD; hash[2] = 0xE1;
    if (micro_verify(message, sizeof(message), fake_sig, public_key, hash)) {
        printf("❌ Attack Passed (should not happen!)\n");
    } else {
        printf("✅ Attack Detected (verification failed as expected)\n");
    }

    return 0;
}
