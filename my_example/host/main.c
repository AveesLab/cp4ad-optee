#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

int main(void) {
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* OP-TEE 컨텍스트 초기화 */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /* OP-TEE 세션 열기 */
    TEEC_UUID uuid = TA_UUID;

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_OpenSession failed with code 0x%x origin 0x%x", res, err_origin);

    printf("OP-TEE 세션 성공적으로 열림!\n");

    /* TA 명령 실행 */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_NONE, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    res = TEEC_InvokeCommand(&sess, CMD_HELLO, &op, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);

    printf("TA 명령 실행 성공!\n");

    /* 세션 종료 */
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}
