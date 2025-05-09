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

    /*
    [Initialize the context]
    */
    TEEC_InitializeContext(NULL, &ctx);
    printf("[Initialized Context!]\n");

    /*
    [Set the UUID]
    */
    TEEC_UUID uuid = {0xb6486f4a, 0x81bd, 0x4bab, {0x86, 0x74, 0x71, 0xea, 0x9b, 0xf8, 0xe5, 0x1e}};

    /*
    [Open a session]
    */
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    printf("[Session Opened!]\n");

    /*
    [Set Parameters]
    */
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    /*
    [Invoke the command]
    */
    TEEC_InvokeCommand(&sess, CMD_HELLO, &op, &err_origin); // (CMD_HELLO, CMD_GOODBYE)
    int res = op.params[0].value.a;
    if (res == 0) {
        printf("Hello OP-TEE World!\n");
    }
    else {
        printf("Goodbye OP-TEE World!\n");
    }

    /*
    [Close the session]
    */
    TEEC_CloseSession(&sess);
    printf("[Closed Session!]\n");
    
    /*
    [Finalize the context]
    */
    TEEC_FinalizeContext(&ctx);
    printf("[Finalized Context!]\n");
    return 0;
}