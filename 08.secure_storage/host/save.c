#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = {0xe4a86432, 0x409d, 0x4542, {0xa5, 0x3e, 0xe2, 0xce, 0x9d, 0x65, 0x77, 0xd3}};
    uint32_t origin;

    if (argc != 2) {
        printf("Usage: %s <obj_name>\n", argv[0]);
        return -1;
    }

    char *key = argv[1];
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

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    // write here //
    op.paramTypes = TEEC_PARAM_TYPES(, , TEEC_NONE, TEEC_NONE);
    op.params
    op.params
    op.params
    op.params
    TEEC_InvokeCommand(&sess, CMD_SAVE, &op, &origin);

    printf("Object(%s) saved successfully!\n", key);
    printf("Object name           : %s\n", key);
    printf("Object name length    : %zu\n", key_len);
    printf("Object value          : %s\n", data);
    printf("Object value length   : %zu\n", data_len);

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}
