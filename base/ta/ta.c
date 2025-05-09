#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta.h"

TEE_Result TA_CreateEntryPoint(void) {
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, 
                                TEE_Param params[], void **sess_ctx) {
    (void)param_types;
    (void)params;
    (void)sess_ctx;
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void)sess_ctx;
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id, 
                                uint32_t param_types, TEE_Param params[]) {
    (void)sess_ctx;
    (void)param_types;
    (void)params;

    switch (cmd_id) {
    case CMD_HELLO:
        params[0].value.a = 0;
        return TEE_SUCCESS;
    case CMD_GOODBYE:
        params[0].value.a = 1;
        return TEE_SUCCESS;
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
