#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"
#include <string.h>

int main(int argc, char *argv[]) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    uint32_t err_origin;

    TEEC_InitializeContext(NULL, &ctx);
    TEEC_UUID uuid = {0xe9b09cc1, 0x355f, 0x4aeb, {0xac, 0xa1, 0xbb, 0x61, 0xd9, 0x1d, 0xf7, 0x58}};
    TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);

    int a = 3;

    // (1) string initialization //
    // input: "Hi TA !"

    //


    // (2) print string //

    //
    char output[100];

    memset(&op, 0, sizeof(op));
    // (3) fill argments//
    op.paramTypes = TEEC_PARAM_TYPES(, , , TEEC_NONE);
    
    op.params[0].value.a = a;
    // (4) //


    //

    // (5) //


    //

    TEEC_InvokeCommand(&sess, CMD_STRING, &op, &err_origin);
    // TEEC_InvokeCommand(&sess, CMD_ECHO, &op, &err_origin);

    printf("\n[TA -> Host] %s\n", output);

    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}