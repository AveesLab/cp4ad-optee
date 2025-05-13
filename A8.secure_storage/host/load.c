#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

#define MAX_VALUE_SIZE 0

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = {0xe4a86432, 0x409d, 0x4542, {0xa5, 0x3e, 0xe2, 0xce, 0x9d, 0x65, 0x77, 0xd3}};
    uint32_t origin;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <obj_name>\n", argv[0]);
        return -1;
    }

    char *key = argv[1];
    size_t key_len = strlen(key);

    char *data = malloc(0);
    size_t data_len = 0;

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = key;
    op.params[0].tmpref.size   = key_len;
    op.params[1].tmpref.buffer = data;
    op.params[1].tmpref.size   = data_len;
    TEEC_InvokeCommand(&sess, CMD_LOAD, &op, &origin);
    data_len = op.params[1].tmpref.size;
    
    if (data_len !=0) {
        printf("Object(%s) loaded successfully!\n", key);
        printf("Object name           : %s\n", key);
        printf("Object name length    : %zu\n", key_len);
        printf("Object value          : %s\n", data);
        printf("Object value length   : %zu\n", data_len);
    }
    else {
        printf("Object(%s) not found!\n", key);
    }
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    free(data);
    return 0;
}
