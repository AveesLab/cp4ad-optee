#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid;
    uint32_t err_origin;
    int operation, value_0 = 0, value_1 = 0, result;
    TEEC_Result res;

    if ((argc == 3 && (atoi(argv[1]) == 0 || atoi(argv[1]) == 1)) ||
        (argc == 4 && (atoi(argv[1]) == 2 || atoi(argv[1]) == 3))) {
        operation = atoi(argv[1]);
        value_0 = atoi(argv[2]);
        if (argc == 4)
            value_1 = atoi(argv[3]);
    } else {
        fprintf(stderr, 
            "Usage:\n"
            "  %s <operation (0: increment, 1: decrement)> <initial_value>\n"
            "  %s <operation (2: addition, 3: subtraction)> <value_0> <value_1>\n", 
            argv[0], argv[0]);
        return 1;
    }

    /*
    [Initialize the context]
    */
    TEEC_InitializeContext(NULL, &ctx);
    printf("[Initialized Context]\n");

    /*
    [Set the UUID]
    */
    uuid = (operation < 2) ? (TEEC_UUID)TA_UUID_UNARY : (TEEC_UUID)TA_UUID_BINARY;

    /*
    [Open a session]
    */
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    printf("[Session Opened]\n");

    /*
    [Set Parameters]
    */
    memset(&op, 0, sizeof(op));
    if (operation < 2) { // Unary
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
        op.params[0].value.a = value_0;
    } else { // Binary
        op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);
        op.params[0].value.a = value_0;
        op.params[1].value.a = value_1;
    }

    /*
    [Invoke the command]
    */
    TEEC_InvokeCommand(&sess, operation, &op, &err_origin);

    result = op.params[0].value.a;
    printf("[TA command executed successfully]\n");

    switch (operation) {
        case 0: printf("Result after increment = %d\n", result); break;
        case 1: printf("Result after decrement = %d\n", result); break;
        case 2: printf("Result after addition = %d\n", result); break;
        case 3: printf("Result after subtraction = %d\n", result); break;
    }

    /*
    [Close the session]
    */
    TEEC_CloseSession(&sess);
    printf("[Session Closed]\n");

    /*
    [Finalize the context]
    */
    TEEC_FinalizeContext(&ctx);
    printf("[Context Finalized]\n");

    return 0;
}
