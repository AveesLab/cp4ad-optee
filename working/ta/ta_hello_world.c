#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta_hello_world.h"
#include <trace.h>

TEE_Result TA_CreateEntryPoint(void) {
    IMSG("ii");
    DMSG("dd");
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    (void)param_types;
    (void)params;
    (void)sess_ctx;
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void)sess_ctx;
}

// 2. Implement function here. If needed, you can create new functions.
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {
    (void)sess_ctx;
    (void)param_types;
    (void)params;
    switch (cmd_id) {
    case CMD_HELLO:
        IMSG("Hello OP-TEE!");
        return TEE_SUCCESS;
    case CMD_GOODBYE:
        IMSG("Goodbye OP-TEE!");
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
