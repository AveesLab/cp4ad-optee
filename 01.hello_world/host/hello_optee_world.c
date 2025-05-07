#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"

int main(int  argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    uint32_t err_origin;

    TEEC_InitializeContext(NULL, &ctx);
    printf("[Initialized Context!]\n");

    TEEC_UUID uuid = TA_UUID;

    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    printf("[Session Opened!]\n");

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    TEEC_InvokeCommand(&sess, CMD_HELLO, &op, &err_origin); // (CMD_HELLO, CMD_GOODBYE)
    int res = op.params[0].value.a;
    if (res == 0) {
        printf("Hello OP-TEE World!\n");
    }
    else {
        printf("Goodbye OP-TEE World!\n");
    }

    TEEC_CloseSession(&sess);
    printf("[Closed Session!]\n");
    
    TEEC_FinalizeContext(&ctx);
    printf("[Finalized Context!]\n");
    return 0;
}