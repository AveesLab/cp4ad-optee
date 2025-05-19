#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"
#include <string.h>

void ta_access();
void direct_access();

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

void ta_access() {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    uint32_t err_origin;

    char buffer[100];

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_UUID uuid = {0x54eac490, 0x4f7a, 0x4db5, {0x8f, 0xd7, 0x22, 0x1b, 0x7a, 0x22, 0x39, 0xea}};
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);

    printf("\n[TA Access] Getting secret_text using InvokeCommand...\n");

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = buffer;
    op.params[0].tmpref.size = sizeof(buffer);
    TEEC_InvokeCommand(&sess, CMD_SESSION_ACCESS, &op, &err_origin);
    printf("TA command executed successfully!\n");
    printf("Secret text: %s\n", buffer);

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
}

void direct_access() {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    uint32_t err_origin;

    uintptr_t secret_addr;

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_UUID uuid = {0x54eac490, 0x4f7a, 0x4db5, {0x8f, 0xd7, 0x22, 0x1b, 0x7a, 0x22, 0x39, 0xea}};
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);

    printf("\n[Direct Access] Getting secret_text address...\n");

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);

    TEEC_InvokeCommand(&sess, CMD_DIRECT_ACCESS, &op, &err_origin);
    secret_addr = op.params[0].value.a;
    printf("Received secret text address from TA: 0x%lx\n", (unsigned long)secret_addr);

    /* 직접 접근 (Segmentation Fault 예상) */
    char *secret_ptr = (char *)secret_addr;
    printf("Attempting to access secret_text directly at 0x%lx...\n", (unsigned long)secret_ptr);
    printf("Secret text: %s\n", secret_ptr);  // Segmentation Fault 예상 지점점

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
}