#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mbedtls/ecdsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/sha256.h"

#define MSGLEN   5
#define SIGLEN   64

void hex_print(const char *label, const unsigned char *buf, size_t len) {
    printf("%s", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02X ", buf[i]);
    }
    printf("\n");
}

int main(void) {
    unsigned char message[MSGLEN] = {0x11, 0x22, 0x33, 0x44, 0x55};
    unsigned char hash[32];
    unsigned char sig[SIGLEN];
    size_t sig_len;

    unsigned char decrypted_hash[32];

    // ECDSA context
    mbedtls_ecdsa_context ecdsa;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    const char *pers = "ecdsa_sign";

    mbedtls_ecdsa_init(&ecdsa);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);

    // RNG 초기화
    if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func,
                               &entropy, (const unsigned char *)pers,
                               strlen(pers)) != 0) {
        printf("Failed to initialize RNG\n");
        return 1;
    }

    // 키 생성 (secp256r1)
    if (mbedtls_ecdsa_genkey(&ecdsa, MBEDTLS_ECP_DP_SECP256R1,
                             mbedtls_ctr_drbg_random, &ctr_drbg) != 0) {
        printf("Failed to generate ECDSA key\n");
        return 1;
    }

    // 개인키 출력 (16진수 문자열로 변환)
    char priv_buf[100];
    size_t priv_len;
    if (mbedtls_mpi_write_string(&ecdsa.d, 16, priv_buf, sizeof(priv_buf), &priv_len) == 0)
        hex_print("Private Key:  ", priv_buf, priv_len);
    else
        printf("Private Key:  [Error printing]\n\n");
    printf("\n");

    // 공개키 출력
    unsigned char pubkey[65];
    size_t olen;
    mbedtls_ecp_point_write_binary(&ecdsa.grp, &ecdsa.Q,
                                   MBEDTLS_ECP_PF_UNCOMPRESSED,
                                   &olen, pubkey, sizeof(pubkey));
    printf("[generating public_key with private_key..]\n");
    hex_print("Public Key:   ", pubkey, olen);
    printf("\n");

    // 메시지 출력
    printf("Message:      ");
    for (int i = 0; i < MSGLEN; i++)
        printf("%02X ", message[i]);
    printf("\n");
    printf("\n");

    // 메시지 해시
    mbedtls_sha256(message, MSGLEN, hash, 0);

    // 서명
    printf("[signing with private_key..]\n");
    if (mbedtls_ecdsa_write_signature(&ecdsa, MBEDTLS_MD_SHA256,
                                      hash, 32, sig, &sig_len,
                                      mbedtls_ctr_drbg_random, &ctr_drbg) != 0) {
        printf("Failed to sign\n");
        return 1;
    }
    hex_print("Signature:    ", sig, sig_len);
    printf("\n");

    // 검증
    printf("[verifying signature with public_key..]\n");
    int ret = mbedtls_ecdsa_read_signature(&ecdsa,
                                           hash, 32,
                                           sig, sig_len);

    if (ret == 0)
        printf("✅ Verified\n");
    else
        printf("❌ Invalid\n");


    // 공격자 테스트
    printf("\n-- Attack Test --\n");

    // 공격자: 별도 키 쌍 생성
    mbedtls_ecdsa_context attacker;
    mbedtls_ecdsa_init(&attacker);
    mbedtls_ecdsa_genkey(&attacker, MBEDTLS_ECP_DP_SECP256R1,
                         mbedtls_ctr_drbg_random, &ctr_drbg);
    unsigned char fake_sig[SIGLEN];
    size_t fake_sig_len;

    // 공격자 개인키 출력
    char fake_priv_buf[100];
    size_t fake_priv_len;
    mbedtls_mpi_write_string(&attacker.d, 16, fake_priv_buf, sizeof(fake_priv_buf), &fake_priv_len);
    // printf("Fake Priv:      %s\n\n", fake_priv_buf);
    hex_print("Private Key:  ", fake_priv_buf, fake_priv_len);
    printf("\n");

    printf("[signing with fake_key..]\n");
    mbedtls_ecdsa_write_signature(&attacker, MBEDTLS_MD_SHA256,
                                  hash, 32, fake_sig, &fake_sig_len,
                                  mbedtls_ctr_drbg_random, &ctr_drbg);
    hex_print("Fake Signature: ", fake_sig, fake_sig_len);
    printf("\n");

    printf("[verifying signature with public_key..]\n");
    ret = mbedtls_ecdsa_read_signature(&ecdsa,
                                       hash, 32,
                                       fake_sig, fake_sig_len);
    if (ret == 0)
        printf("❌ Attack Passed\n");
    else
        printf("✅ Attack Detected\n");

    // Clean up
    mbedtls_ecdsa_free(&ecdsa);
    mbedtls_ecdsa_free(&attacker);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);

    return 0;
}
