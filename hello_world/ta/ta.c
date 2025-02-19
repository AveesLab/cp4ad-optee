#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

// 1. Modify ta header file path.
#include "ta.h"

TEE_Result TA_CreateEntryPoint(void) {
    DMSG("TA가 생성되었습니다.");
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
    DMSG("TA가 제거되었습니다.");
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    (void)param_types;
    (void)params;
    (void)sess_ctx;
    IMSG("세션이 열렸습니다.");
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void)sess_ctx;
    IMSG("세션이 닫혔습니다.");
}

// 2. Implement function here. If needed, you can create new functions.
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {
    (void)sess_ctx;
    (void)param_types;
    (void)params;

    switch (cmd_id) {
    case CMD_HELLO:
        IMSG("Hello from Trusted Application!");
        return TEE_SUCCESS;
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
