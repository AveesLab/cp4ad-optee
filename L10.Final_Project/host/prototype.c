#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"
#define DIGITAL_SIGNATURE_KEY_SIZE 2

// object handle function
int save_object(TEEC_Session *sess, const char *key) {
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t origin;

    unsigned char *data = NULL;
    size_t data_len = 0;
    size_t buffer_len = 0;
    printf("Enter the object value: \n");
    data_len = getline(&data, &buffer_len, stdin);
    if (data_len > 0 && data[data_len-1] == '\n') {
        data[data_len-1] = '\0';
        --data_len;
    }

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = (void *)key;
    op.params[0].tmpref.size = strlen(key) + 1; // +1 for null terminator
    op.params[1].tmpref.buffer = (void *)data;
    op.params[1].tmpref.size = strlen(data) + 1; // +1 for null terminator

    res = TEEC_InvokeCommand(sess, CMD_SAVE, &op, &origin);
    if (res != TEEC_SUCCESS) {
        printf("Error saving object: %x\n", res);
        return -1;
    }
    return 0;
}

int load_object(TEEC_Session *sess, const char *key) {
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t origin;

    unsigned char *data = NULL;
    size_t data_len = 0;

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = (void *)key;
    op.params[0].tmpref.size = strlen(key) + 1; // +1 for null terminator
    op.params[1].tmpref.buffer = (void *)data;
    op.params[1].tmpref.size = data_len;

    res = TEEC_InvokeCommand(sess, CMD_LOAD, &op, &origin);
    if (res != TEEC_SUCCESS) {
        printf("Error loading object: %x\n", res);
        return -1;
    }
    return 0;
}

int delete_object(TEEC_Session *sess, const char *key) {
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t origin;

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = (void *)key;
    op.params[0].tmpref.size = strlen(key) + 1; // +1 for null terminator

    res = TEEC_InvokeCommand(sess, CMD_DELETE, &op, &origin);
    if (res != TEEC_SUCCESS) {
        printf("Error deleting object: %x\n", res);
        return -1;
    }
    return 0;
}

int set_whitelist(TEEC_Session *sess) {
    save_object(sess, "whitelist");
    return 0;
}

int save_private_key(TEEC_Session *sess, int key_size) {
    save_hex_object(sess, "private_key");
    return 0;
}

int generate_public_key(TEEC_Session *sess, unsigned char *public_key, int key_size, unsigned char *hash) {
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t origin;

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = (void *)hash;
    op.params[0].tmpref.size = 3;
    op.params[1].tmpref.buffer = (void *)public_key;
    op.params[1].tmpref.size = key_size;

    TEEC_InvokeCommand(sess, CMD_GENERATE_PUBLIC_KEY, &op, &origin);
    return 0;
}

int setup(TEEC_Session *sess) {
    // Whitelist setup
    if (!set_whitelist(&sess)) {
        printf("Whitelist setup failed\n");
        return -1;
    }
    printf("Whitelist setup succeeded\n");

    // Digital signature setup
    unsigned char *hash = {0x11, 0x22, 0x33};
    save_private_key(&sess, DIGITAL_SIGNATURE_KEY_SIZE);
    unsigned char public_key[2];
    generate_public_key(&sess, public_key, DIGITAL_SIGNATURE_KEY_SIZE, hash);
    printf("Public key generated: ");
    for (int i = 0; i < DIGITAL_SIGNATURE_KEY_SIZE; i++) {
        printf("%02X ", public_key[i]);
    }
    printf("\n");
    return 0;
}

unsigned char *micro_sign(const unsigned char *message, size_t msg_len,
                            const unsigned char *private_key, unsigned char *signature,
                            unsigned char hash[3]) {
    unsigned char buffer[64];
    memcpy(buffer, message, msg_len);
    memcpy(buffer + msg_len, private_key, DIGITAL_SIGNATURE_KEY_SIZE);

    mini_hash(buffer, msg_len + DIGITAL_SIGNATURE_KEY_SIZE, hash);
    memcpy(signature, hash, 3);
    return signature;
}

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = {0x4014bf39, 0x668c, 0x4253, {0x90, 0xb9, 0x70, 0x4b, 0x82, 0x57, 0xd1, 0x29}};
    uint32_t origin;

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    if (!setup(&sess)) {
        printf("Setup succeeded\n");
    }
    else {
        printf("Setup failed\n");
        return -1;
    }

    while(true){
        if (can_recieve(&can_msg)){
            printf("Received CAN message: ID = %X, Data = ", can_msg.id);
            for (int i = 0; i < 8; i++) {
                printf("%02X ", can_msg.data[i]);
            }
            printf("\n");

            if (verify_whitelist(can_msg.id, &sess)) {
                printf("CAN message ID %X is in the whitelist.\n", can_msg.id);

                unsigned char *singed_data = micro_sign(can_msg.data, can_msg.dlc, private_key, signature, hash);
                can_send(signed_data);
            }
            else {
                printf("CAN message ID %X is not in the whitelist.\n", can_msg.id);
                continue;
            }
        } else {
            printf("No new CAN messages.\n");
            continue;
        }
    }


    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}