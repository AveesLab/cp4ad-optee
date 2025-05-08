#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
// 1. Modify ta header file path
#include "ta_increment.h"

#include "user_ta_header_defines.h"

int main(void) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Initialize OP-TEE context */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /* Open OP-TEE session */
// 2. Modify ta UUID
    TEEC_UUID uuid = TA_INCREMENT_UUID;

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_OpenSession failed with code 0x%x origin 0x%x", res, err_origin);

    printf("OP-TEE session opened successfully!\n");

    /* Execute TA command */

// 3. Modify operation parameters (if needed) //
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);

// 4. Modify command ID //
    int number = 42;
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE,
                    TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = number;

    res = TEEC_InvokeCommand(&sess, CMD_INCREMENT, &op, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    number = op.params[0].value.a;
    printf("TA command(increment) executed successfully!\n");
    printf("number after increment = %d\n", number);

    res = TEEC_InvokeCommand(&sess, CMD_DECREMENT, &op, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    number = op.params[0].value.a;
    printf("TA command(decrement) executed successfully!\n");
    printf("number after decrement = %d\n", number);

    /* Terminate session and context */
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}
