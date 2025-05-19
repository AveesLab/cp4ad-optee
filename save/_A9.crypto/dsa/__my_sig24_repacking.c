#include <stdint.h>
#include <string.h>

static void mini_hash24(const unsigned char *data, size_t len,
                        unsigned char hash[3])
{
    uint32_t h = ((uint32_t)hash[0] << 16) |
                 ((uint32_t)hash[1] <<  8) |
                  (uint32_t)hash[2];

    for (size_t i = 0; i < len; i++) {
        h ^= (uint32_t)data[i] + i * 13;
        h = (h << 5) | (h >> 27);
    }
    h &= 0xFFFFFF;
    hash[0] = (h >> 16) & 0xFF;
    hash[1] = (h >>  8) & 0xFF;
    hash[2] =  h        & 0xFF;
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
        int32_t tmp = newt; newt = t - q*newt; t = tmp;
        tmp = newr; newr = r - q*newr; r = tmp;
    }
    if (r>1) return -1;
    if (t<0) t += phi;
    return t;
}
static uint32_t modexp(uint32_t b, uint32_t e, uint32_t m) {
    uint64_t r=1, base=b%m;
    while (e) {
        if (e&1) r = (r*base)%m;
        base = (base*base)%m;
        e >>= 1;
    }
    return (uint32_t)r;
}
static void derive_rsa(const unsigned char priv[2],
                       uint32_t *n, uint32_t *d)
{
    uint32_t seed = (priv[0]<<8)|priv[1];
    uint32_t p = ((seed>>4)&0x0FFF)|0x0800;
    while (!is_prime(p)) p++;
    uint32_t q = (seed&0x0FFF)|0x0800;
    while (!is_prime(q)) q++;
    *n = p*q;
    uint32_t phi=(p-1)*(q-1);
    *d = (uint32_t)modinv32(17, phi);
}

void generate_public_key(const unsigned char *private_key /*2B*/,
                         unsigned char *public_key /*3B*/)
{
    uint32_t n, d;
    derive_rsa(private_key, &n, &d);
    public_key[0] = (n >> 16) & 0xFF;
    public_key[1] = (n >>  8) & 0xFF;
    public_key[2] =  n        & 0xFF;
}

void micro_sign(const unsigned char *message       /*5B*/,
                const unsigned char *private_key  /*2B*/,
                unsigned char       hash[3],     /*init→out*/
                unsigned char       signature[3]) /*out*/
{
    unsigned char buf[5 + 2];
    memcpy(buf, message, 5);
    memcpy(buf + 5, private_key, 2);
    mini_hash24(buf, 5 + 2, hash);

    uint32_t n, d;
    derive_rsa(private_key, &n, &d);

    uint32_t hval = ((uint32_t)hash[0]<<16)|
                    ((uint32_t)hash[1]<<8) |
                     (uint32_t)hash[2];
    uint32_t hmod = hval % n;
    uint32_t s    = modexp(hmod, d, n);

    signature[0] = (s >> 16) & 0xFF;
    signature[1] = (s >>  8) & 0xFF;
    signature[2] =  s        & 0xFF;
}

int micro_verify(const unsigned char *message       /*5B*/,
                 const unsigned char *public_key    /*3B*/,
                 const unsigned char *signature     /*3B*/,
                 unsigned char       hash[3]) /*init→out*/
{
    unsigned char buf[5 + 3];
    memcpy(buf, message, 5);
    memcpy(buf + 5, public_key, 3);
    mini_hash24(buf, 5 + 3, hash);

    uint32_t n = ((uint32_t)public_key[0]<<16) |
                 ((uint32_t)public_key[1]<< 8) |
                  (uint32_t)public_key[2];
    uint32_t s = ((uint32_t)signature[0]<<16)|
                 ((uint32_t)signature[1]<< 8)|
                  (uint32_t)signature[2];
    uint32_t rec = modexp(s, 17, n);

    uint32_t hval = ((uint32_t)hash[0]<<16)|
                    ((uint32_t)hash[1]<< 8)|
                     (uint32_t)hash[2];
    uint32_t hmod = hval % n;
    hash[0] = (hmod >> 16) & 0xFF;
    hash[1] = (hmod >>  8) & 0xFF;
    hash[2] =  hmod        & 0xFF;

    return rec == hmod;
}

int main(void) {
    unsigned char message[5]   = {0x11,0x22,0x33,0x44,0x55};
    unsigned char hash[3]     = {0xAB,0xCD,0xE1};  // 초기 해시
    unsigned char private_key[2] = {0x12,0x34};
    unsigned char public_key[3];
    unsigned char signature[3];

    generate_public_key(private_key, public_key);
    micro_sign(message, private_key, hash, signature);

    printf("Message          : %02X %02X %02X %02X %02X\n", message[0],message[1],message[2],message[3],message[4]);
    printf("PrivateKey (d)   : %02X %02X\n", private_key[0],private_key[1]);

    printf("PublicKey (n): %02X %02X %02X\n",
           public_key[0],public_key[1],public_key[2]);
    printf("Hash after sign: %02X %02X %02X\n",
           hash[0],hash[1],hash[2]);
    printf("Signature        : %02X %02X %02X\n",
           signature[0],signature[1],signature[2]);

    unsigned char verify_hash[3] = {0xAB,0xCD,0xE1};
    if (micro_verify(message, public_key, signature, verify_hash))
        printf("✅ Verified (hash now %02X %02X %02X)\n",
               verify_hash[0],verify_hash[1],verify_hash[2]);
    else
        printf("❌ Invalid (hash now %02X %02X %02X)\n",
               verify_hash[0],verify_hash[1],verify_hash[2]);

    return 0;
}
