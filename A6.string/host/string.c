#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"

extern char secret_text[]; // Access secure data directly

int main(int argc, char *argv[]) {

    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;
// 2. define buffer;
    char buffer[100];

    /* Initialize OP-TEE context */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /* Open OP-TEE session */
// 2. Modify ta UUID
    TEEC_UUID uuid = TA_SECRET_TEXT_UUID;

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_OpenSession failed with code 0x%x origin 0x%x", res, err_origin);

    printf("OP-TEE session opened successfully!\n");
    printf("\n[TA Access] Getting secret_text using InvokeCommand...\n");

    /* Execute TA command */

// 3. Modify operation parameters (if needed) //
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = buffer;
    op.params[0].tmpref.size = sizeof(buffer);

// 4. Modify command ID //
    // Access secure data through TA
    res = TEEC_InvokeCommand(&sess, CMD_SECRET_TEXT, &op, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    printf("TA command executed successfully!\n");

    printf("Secret text: %s\n", buffer);

    /* Terminate session and context */
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}