#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tee_client_api.h>
#include "ta.h"
#include "user_ta_header_defines.h"

#define MAX_VALUE_SIZE 1024

int main(int argc, char *argv[]) {
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = {0xe4a86432, 0x409d, 0x4542,
                      {0xa5, 0x3e, 0xe2, 0xce, 0x9d, 0x65, 0x77, 0xd3}};
    uint32_t origin;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <obj_name>\n", argv[0]);
        return -1;
    }

    const char *obj_name = argv[1];
    size_t obj_name_len = strlen(obj_name);

    /* 로드할 값을 받을 버퍼 할당 */
    char *obj_value = malloc(MAX_VALUE_SIZE);
    memset(obj_value, 0, MAX_VALUE_SIZE);
    size_t obj_value_buf_len = MAX_VALUE_SIZE;

    /* TEE 컨텍스트 및 세션 초기화 */
    TEEC_InitializeContext(NULL, &ctx);
    TEEC_OpenSession(&ctx, &sess, &uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);

    /* Invoke CMD_LOAD: 식별자 전달, 값은 output 버퍼로 받음 */
    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(
        TEEC_MEMREF_TEMP_INPUT,
        TEEC_MEMREF_TEMP_OUTPUT,
        TEEC_NONE,
        TEEC_NONE
    );
    op.params[0].tmpref.buffer = (void *)obj_name;
    op.params[0].tmpref.size   = obj_name_len;
    op.params[1].tmpref.buffer = obj_value;
    op.params[1].tmpref.size   = obj_value_buf_len;

    res = TEEC_InvokeCommand(&sess, CMD_LOAD, &op, &origin);
    if (res != TEEC_SUCCESS) {
        fprintf(stderr,
            "CMD_LOAD failed: TEEC_Result=0x%x, origin=0x%x\n",
            res, origin);
    } else {
        size_t read_len = op.params[1].tmpref.size;
        printf("Object value          : %.*s\n",
               (int)read_len, obj_value);
        printf("Object value length   : %zu\n", read_len);
    }

    /* 실제 읽어들인 바이트 수 */
    size_t read_len = op.params[1].tmpref.size;

    printf("Object name           : %s\n", obj_name);
    printf("Object name length    : %zu\n", obj_name_len);
    printf("Object value          : %s\n", obj_value);
    printf("Object value length   : %zu\n", read_len);

    /* 후처리 */
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    free(obj_value);
    return EXIT_SUCCESS;
}
