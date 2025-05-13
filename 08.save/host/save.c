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
    TEEC_UUID uuid = {0xe4a86432, 0x409d, 0x4542,
                      {0xa5,0x3e,0xe2,0xce,0x9d,0x65,0x77,0xd3}};
    uint32_t origin;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <obj_name>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *obj_name = argv[1];
    size_t obj_name_len = strlen(obj_name);

    /* Read value */
    char *obj_value = NULL;
    size_t buf_cap = 0;
    ssize_t val_len = getline(&obj_value, &buf_cap, stdin);
    if (val_len <= 0) {
        fprintf(stderr, "No input\n");
        free(obj_value);
        return EXIT_FAILURE;
    }
    if (obj_value[val_len-1] == '\n') {
        obj_value[--val_len] = '\0';
    }

    /* Invoke TA */
    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid,
                     TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    op.paramTypes = TEEC_PARAM_TYPES(
        TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_INPUT,
        TEEC_NONE,
        TEEC_NONE
    );
    op.params[0].tmpref.buffer = (void*)obj_name;
    op.params[0].tmpref.size   = obj_name_len;
    op.params[1].tmpref.buffer = obj_value;
    op.params[1].tmpref.size   = (size_t)val_len;

    res = TEEC_InvokeCommand(&sess, CMD_SAVE, &op, &origin);
    if (res != TEEC_SUCCESS) {
        fprintf(stderr,
            "CMD_SAVE failed: res=0x%x, origin=0x%x\n",
            res, origin);
    } else {
        printf("Saved '%s'='%s' successfully\n",
               obj_name, obj_value);
    }

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    free(obj_value);
    return (res == TEEC_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}