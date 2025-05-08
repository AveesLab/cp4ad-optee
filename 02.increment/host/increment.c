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
    
    int initial_value = 42;

    /*
    [Initialize the context]
    */
    __FILL_THIS_FUNCTION__(NULL, &ctx); // (1) //
    printf("[Initialized Context!]\n");

    /*
    [Set the UUID]
    */
    TEEC_UUID uuid = ; // (2) //

    /*
    [Open a session]
    */
    __FILL_THIS_FUNCTION__(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin); // (3) //
    printf("[Session Opened!]\n");

    /*
    [Set Parameters]
    */

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = ; // (4) //

    /*
    [Invoke the command]
    */
    TEEC_InvokeCommand(& , , &op, &err_origin); // (5) //
    initial_value = op.params[0].value.a;
    printf("[TA command (increment) executed successfully!]\n");
    printf("number after increment = %d\n", initial_value);

    /*
    [Close the session]
    */
    TEEC_CloseSession(& ); // (6) //
    printf("[Closed Session!]\n");
    
    /*
    [Finalize the context]
    */
    TEEC_FinalizeContext(& ); // (7) //
    printf("[Finalized Context!]\n");

    return 0;
}