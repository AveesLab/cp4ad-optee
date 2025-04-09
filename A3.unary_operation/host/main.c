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

    int operation;
    int initial_value;
    
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <operation (0: increment, 1: decrement)> <initial_value>\n", argv[0]);
        return 1;
    }

    operation = atoi(argv[1]);
    initial_value = atoi(argv[2]);

    /*
    [Initialize the context]
    */
    TEEC_InitializeContext(NULL, &ctx);
    printf("[Initialized Context!]\n");

    /*
    [Set the UUID]
    */
    TEEC_UUID uuid = TA_UUID;

    /*
    [Open a session]
    */
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    printf("[Session Opened!]\n");

    /*
    [Set Parameters]
    */
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
                    TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = initial_value;

    /*
    [Invoke the command]
    */
    if (operation == 0) {
        TEEC_InvokeCommand(&sess, CMD_INCREMENT, &op, &err_origin);
        initial_value = op.params[0].value.a;
        printf("[TA command (increment) executed successfully!]\n");
        printf("number after increment = %d\n", initial_value);
    } else if (operation == 1) {
        TEEC_InvokeCommand(&sess, CMD_DECREMENT, &op, &err_origin);
        initial_value = op.params[0].value.a;
        printf("[TA command (decrement) executed successfully!]\n");
        printf("number after decrement = %d\n", initial_value);
    } else {
        printf("Invalid operation type. Use 0 for increment, 1 for decrement.\n");
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