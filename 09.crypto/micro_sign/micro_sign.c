#include <stdio.h>
#include <stdint.h>
#include <string.h>

static void mini_hash24(const char *data, size_t len, char out[3]) {
    uint32_t h = 0xABCDEF;
    for (size_t i = 0; i < len; i++) {
        h ^= (uint32_t)data[i] + i * 13;
        h = (h << 5) | (h >> 27);
    }
    h &= 0xFFFFFF;
    out[0] = (h >> 16) & 0xFF;
    out[1] = (h >>  8) & 0xFF;
    out[2] =  h        & 0xFF;
}

static int is_prime(uint32_t x) {
    if (x < 2) return 0;
    for (uint32_t i = 2; i*i <= x; i++)
        if (x % i == 0) return 0;
    return 1;
}

static int32_t modinv32(int32_t e, int32_t phi) {
    int32_t t=0, newt=1, r=phi, newr=e;
    while (newr) {
        int32_t q = r/newr;
        int32_t tmp=newt; newt=t-q*newt; t=tmp;
        tmp=newr; newr=r-q*newr; r=tmp;
    }
    if (r>1) return -1;
    if (t<0) t+=phi;
    return t;
}

static uint32_t modexp(uint32_t b, uint32_t e, uint32_t m) {
    uint64_t r=1, base=b%m;
    while (e) {
        if (e&1) r=(r*base)%m;
        base=(base*base)%m;
        e>>=1;
    }
    return (uint32_t)r;
}

static void derive_rsa(const char priv[2], uint32_t *n, uint32_t *d) {
    uint32_t seed = (priv[0]<<8)|priv[1];
    uint32_t p = ((seed>>4)&0x0FFF)|0x0800;
    while (!is_prime(p)) p++;
    uint32_t q = (seed&0x0FFF)|0x0800;
    while (!is_prime(q)) q++;
    *n = p*q;
    uint32_t phi = (p-1)*(q-1);
    *d = (uint32_t)modinv32(17, phi);
}

void generate_public_key(const char priv[2], char pub[3]) {
    uint32_t n,d;
    derive_rsa(priv, &n, &d);
    pub[0] = (n>>16)&0xFF;
    pub[1] = (n>>8 )&0xFF;
    pub[2] =  n      &0xFF;
}

void micro_sign(const char message[5], const char priv[2], char sig[3]) {
    char h24[3];
    mini_hash24(message, 5, h24);
    uint32_t n, d;
    derive_rsa(priv, &n, &d);
    uint32_t hval  = (h24[0]<<16)|(h24[1]<<8)|h24[2];
    uint32_t h_mod = hval % n;
    uint32_t s     = modexp(h_mod, d, n);
    sig[0] = (s>>16)&0xFF;
    sig[1] = (s>>8 )&0xFF;
    sig[2] =  s      &0xFF;
}

int micro_verify(const char message[5], const char pub[3], const char sig[3], char hash[3], char decrypted_sig[3]) {
    char h24[3];
    mini_hash24(message, 5, h24);
    uint32_t n     = (pub[0]<<16)|(pub[1]<<8)|pub[2];
    uint32_t hval  = (h24[0]<<16)|(h24[1]<<8)|h24[2];
    uint32_t h_mod = hval % n;
    uint32_t s     = (sig[0]<<16)|(sig[1]<<8)|sig[2];
    uint32_t rec   = modexp(s, 17, n);
    hash[0] = (h_mod >> 16) & 0xFF;
    hash[1] = (h_mod >>  8) & 0xFF;
    hash[2] =  h_mod        & 0xFF;

    decrypted_sig[0] = (rec >> 16) & 0xFF;
    decrypted_sig[1] = (rec >>  8) & 0xFF;
    decrypted_sig[2] =  rec        & 0xFF;
    return rec == h_mod;
}

int main(void) {
    char message[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
    char priv[2]   = {0x12, 0x34};
    char pub[3];
    char sig[3];
    char hash[3];
    char decrypted_sig[3];

    printf("Private Key:  %02X %02X\n", priv[0], priv[1]);
    printf("\n");

    printf("[generating public_key with private_key..]\n");
    generate_public_key(priv, pub);
    printf("Public Key:   %02X %02X %02X\n", pub[0], pub[1], pub[2]);
    printf("\n");

    printf("Message:      ");
    for (int i = 0; i < 5; i++)
        printf("%02X ", message[i]);
    printf("\n");
    printf("\n");

    printf("[signing with private_key..]\n");
    micro_sign(message, priv, sig);
    printf("Signature:    %02X %02X %02X\n", sig[0], sig[1], sig[2]);
    printf("\n");

    printf("[verifying signature with public_key..]\n");
    if (micro_verify(message, pub, sig, hash, decrypted_sig))
        printf("✅ Verified\n");
    else
        printf("❌ Invalid\n");
    printf("Hash         :    %02X %02X %02X\n", hash[0], hash[1], hash[2]);
    printf("decrypted_sig:    %02X %02X %02X\n", decrypted_sig[0], decrypted_sig[1], decrypted_sig[2]);


    printf("\n-- Attack Test --\n");
    char fake_priv[2] = {0xFF, 0xEE};
    char fake_sig[3];
    printf("Fake Priv:      %02X %02X\n", fake_priv[0], fake_priv[1]);
    printf("\n");
    
    printf("[signing with fake_key..]\n");
        micro_sign(message, fake_priv, fake_sig);
    printf("Fake Signature: %02X %02X %02X\n",
           fake_sig[0], fake_sig[1], fake_sig[2]);
    printf("\n");
    
    printf("[verifying signature with public_key..]\n");
    if (micro_verify(message, pub, fake_sig, hash, decrypted_sig))
        printf("❌ Attack Passed\n");
    else
        printf("✅ Attack Detected\n");
    printf("Hash         :    %02X %02X %02X\n", hash[0], hash[1], hash[2]);
    printf("decrypted_sig:    %02X %02X %02X\n", decrypted_sig[0], decrypted_sig[1], decrypted_sig[2]);
    return 0;
}
