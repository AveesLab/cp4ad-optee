#include "crypto.h"

void print_bytes(const unsigned char *bytes, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("0x%02X", bytes[i]);
        if (i < len - 1)
            printf(", ");
    }
    printf("\n");
}

void string_to_bytes(const char in[7], char out[2]) {
    if (strlen(in) != 6 || in[0] != '0' || in[1] != 'x') {
        return -1;
    }

    unsigned int value;
    if (sscanf(in + 2, "%4x", &value) != 1) {
        return -1;
    }

    out[0] = (value >> 8) & 0xFF;
    out[1] = value & 0xFF;
    return 0;
}

void bytes_to_string(const char in[2], char out[7]) {
    sprintf(out, "0x%02X%02X", in[0], in[1]);
}


int set_whitelist(TEEC_Session *sess) {
    save_object(sess, "whitelist");
    return 0;
}

int set_priv_key(TEEC_Session *sess) {
    save_hex_object(sess, "private_key");
    return 0;
}

// write here - fill the right key(private_key / public_key)
int generate_public_key(TEEC_Session *sess, unsigned char *) {
//
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t origin;

    memset(&op, 0, sizeof(op));
    // write here - fill the right PARAM_TYPES //
    op.paramTypes = TEEC_PARAM_TYPES(, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    //

    // write here - fill the right key(private_key / public_key)
    op.params[0].tmpref.buffer = ;
    //
    op.params[0].tmpref.size = 3;
    res = TEEC_InvokeCommand(sess, CMD_GENERATE_PUBLIC_KEY, &op, &origin);
    if (res != TEEC_SUCCESS) {
        printf("Error generating public key: %x\n", res);
        return -1;
    }
    printf("Public key generated successfully!\n");
    printf("Public key: ");
    for (int i = 0; i < 3; i++) {
        printf("%02X ", public_key[i]);
    }
    printf("\n");
    return 0;
}

// write here - fill the right value
int micro_sign(TEEC_Session *sess, const unsigned char *, unsigned char *) {
//
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t origin;

    memset(&op, 0, sizeof(op));
    // write here - fill the right PARAM_TYPES //
    op.paramTypes = TEEC_PARAM_TYPES(, , TEEC_NONE, TEEC_NONE);
    //

    // write here - fill the right value
    op.params[0].tmpref.buffer = ;
    //
    op.params[0].tmpref.size = 5;
    // write here - fill the right value
    op.params[1].tmpref.buffer = ;
    //
    op.params[1].tmpref.size = 3;
    res = TEEC_InvokeCommand(sess, CMD_SIGN, &op, &origin);
    if (res != TEEC_SUCCESS) {
        printf("Error sign: %x\n", res);
        return -1;
    }
    printf("Signature generated successfully!\n");
    printf("Signature: ");
    for (int i = 0; i < 3; i++) {
        printf("%02X ", signature[i]);
    }
    printf("\n");
    return 0;
}

// write here - fill the right values //
int micro_verify(TEEC_Session *sess, const unsigned char *, const unsigned char *, const unsigned char *) {
//
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t origin;
    
    memset(&op, 0, sizeof(op));
    // write here - fill the right PARAM_TYPES //
    op.paramTypes = TEEC_PARAM_TYPES(, , , );
    // write here - fill the right value
    op.params[0].tmpref.buffer = ;
    //
    op.params[0].tmpref.size = 5;
    // write here - fill the right value
    op.params[1].tmpref.buffer = ;
    //
    op.params[1].tmpref.size = 3;
    // write here - fill the right value
    op.params[2].tmpref.buffer = ;
    //
    op.params[2].tmpref.size = 3;
    op.params[3].value.a = 0;
    res = TEEC_InvokeCommand(sess, CMD_VERIFY, &op, &origin);

    if (res != TEEC_SUCCESS) {
        printf("Error verify: %x, %d n", res, origin);
        return -1;
    }
    if (op.params[3].value.a == 1) {
        printf("Signature is valid\n");
        return 1;
    } else {
        printf("Signature is invalid\n");
        return 0;
    }
}