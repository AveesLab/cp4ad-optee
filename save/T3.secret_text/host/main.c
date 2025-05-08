#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
// 1. Modify ta header file path
#include "ta_secret_text.h"

#include "user_ta_header_defines.h"
#include <string.h>
extern char secret_text[]; // Access secure data directly

void ta_access() {
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
}

void direct_access() {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_Result res;
    uint32_t err_origin;
// 5. define secret_addr
    uintptr_t secret_addr;

    /* Initialize OP-TEE context */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /* Open OP-TEE session */
    TEEC_UUID uuid = TA_SECRET_TEXT_UUID;

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_OpenSession failed with code 0x%x origin 0x%x", res, err_origin);

    printf("OP-TEE session opened successfully!\n");
    printf("\n[Direct Access] Getting secret_text address and accessing it directly...\n");

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    res = TEEC_InvokeCommand(&sess, CMD_SECRET_TEXT_ADDR, &op, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);

    secret_addr = op.params[0].value.a;
    printf("Received secret text address from TA: 0x%lx\n", (unsigned long)secret_addr);

    /* 직접 접근 (Segmentation Fault 예상) */
    char *secret_ptr = (char *)secret_addr;
    printf("Attempting to access secret_text directly at 0x%lx...\n", (unsigned long)secret_ptr);
    printf("Secret text: %s\n", secret_ptr);  // Segmentation Fault 예상

    /* Terminate session and context */
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("\nUsage: %s <mode>\n", argv[0]);
        printf("  0 - TA Access (InvokeCommand)\n");
        printf("  1 - Direct Access (Memory address access, may cause Segmentation Fault)\n");
    } else {
        int mode = atoi(argv[1]);
        if (mode == 0) {
            ta_access();
        } else if (mode == 1) {
            direct_access();
        } else {
            printf("Invalid mode! Use 1 for TA access, 2 for direct access.\n");
        }
    }

    return 0;
}