#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"

int main() {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    uint32_t err_origin;
    
    int initial_value = ; // (1) //

    /*
    [Initialize the context]
    */
    __FILL_THIS_FUNCTION__(NULL, &ctx); // (2) //
    printf("[Initialized Context!]\n");

    /*
    [Set the UUID]
    */
    TEEC_UUID uuid = ; // (3) //

    /*
    [Open a session]
    */
    __FILL_THIS_FUNCTION__(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin); // (4) //
    printf("[Session Opened!]\n");

    /*
    [Set Parameters]
    */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
                    TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = ; // (5) //

    /*
    [Invoke the command]
    */
    TEEC_InvokeCommand(& , , &op, &err_origin); // (6) //
    initial_value = op.params[0].value.a;
    printf("[TA command (increment) executed successfully!]\n");
    printf("number after increment = %d\n", initial_value);

    /*
    [Close the session]
    */
    TEEC_CloseSession(& ); // (7) //
    printf("[Closed Session!]\n");
    
    /*
    [Finalize the context]
    */
    TEEC_FinalizeContext(& ); // (8) //
    printf("[Finalized Context!]\n");

    return 0;
}