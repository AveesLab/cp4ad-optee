#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

#define MAX_VALUE_SIZE 1024

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = {0x4014bf39, 0x668c, 0x4253, {0x90, 0xb9, 0x70, 0x4b, 0x82, 0x57, 0xd1, 0x29}};
    uint32_t origin;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <obj_name>\n", argv[0]);
        return -1;
    }

    char *key = argv[1];
    size_t key_len = strlen(key);

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = (void *)key;
    op.params[0].tmpref.size   = key_len;
    TEEC_InvokeCommand(&sess, CMD_DELETE, &op, &origin);

    printf("Object(%s) deleted successfully!\n", key);
    printf("Object name           : %s\n", key);
    printf("Object name length    : %zu\n", key_len);

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}
