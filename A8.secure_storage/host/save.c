#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

int main(int argc, char *argv[]) {
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = {0xe4a86432, 0x409d, 0x4542, {0xa5, 0x3e, 0xe2, 0xce, 0x9d, 0x65, 0x77, 0xd3}};
    uint32_t origin;

    // uint8_t obj_value[128];

    if (argc != 2) {
        printf("Usage: %s <obj_name>\n", argv[0]);
        return -1;
    }

    const char *key = argv[1];
    size_t key_len = strlen(key);

    char *data = NULL;
    size_t data_len = 0;
    size_t buffer_len = 0;

    printf("Enter the object value: \n");
    data_len = getline(&data, &buffer_len, stdin);
    if (data_len > 0 && data[data_len-1] == '\n') {
        data[data_len-1] = '\0';
        --data_len;
    }

    printf("Object name           : %s\n", key);
    printf("Object name length    : %zu\n", key_len);
    printf("Object value          : %s\n", data);
    printf("Object value length   : %zu\n", data_len);
    

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = key;
    op.params[0].tmpref.size = key_len;
    op.params[1].tmpref.buffer = data;
    op.params[1].tmpref.size = data_len;
    res = TEEC_InvokeCommand(&sess, CMD_SAVE, &op, &origin);
    if (res != TEEC_SUCCESS) {
        printf("Command WRITE_RAW failed: 0x%x / %u\n", res, origin);
    } else {
        size_t read_len = op.params[1].tmpref.size;
        printf("Object value          : %.*s\n",
               (int)read_len, data);
        printf("Object value length   : %zu\n", read_len);
    }

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    // printf("Object(%s) stored successfully!\n", obj_name);
    return 0;
}
