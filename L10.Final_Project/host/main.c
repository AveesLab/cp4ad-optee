#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

#include "secure_storage.h"
#include "crypto.h"

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = {0x4014bf39, 0x668c, 0x4253, {0x90, 0xb9, 0x70, 0x4b, 0x82, 0x57, 0xd1, 0x29}};
    uint32_t origin;

    printf("Secure Storage Example\n\n");
    TEEC_InitializeContext(NULL, &ctx);
    printf("TEEC_InitializeContext\n\n");
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    printf("TEEC_OpenSession\n\n");
    
    /////////////////////////
    // secure_storage TEST //
    /////////////////////////
    // save_object(&sess, "object_test");

    // load_object(&sess, "object_test");

    // delete_object(&sess, "object_test");

    //////////////////////
    // print_bytes TEST //
    //////////////////////
    // unsigned char id[3] = {0x11, 0x22, 0x33};
    // print_bytes(id, 3);

    //////////////////////////
    // string_to_bytes TEST //
    // bytes_to_string TEST //
    //////////////////////////
    // unsigned char *public_key = "0x12AB";
    // unsigned char public_key_byte[2];
    // string_to_bytes(public_key, public_key_byte);
    // print_bytes(public_key_byte, 2);

    // unsigned char public_key_str[7];
    // bytes_to_string(public_key_byte, public_key_str);
    // printf("Public key string: %s\n", public_key_str);

    
    set_whitelist(&sess);
    set_priv_key(&sess);
    printf("\n");

    unsigned char public_key[3];
    generate_public_key(&sess, public_key);
    printf("\n");
    unsigned char message[5] = {0x11, 0x22, 0x33, 0x44, 0x55};
    unsigned char signature[3];

    micro_sign(&sess, message, signature);
    printf("Message     : ");
    for (int i = 0; i < 5; i++) printf("%02X ", message[i]);
    printf("\nSignature   : %02X %02X %02X\n\n", signature[0], signature[1], signature[2]);

    micro_verify(&sess, message, signature, public_key);

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}