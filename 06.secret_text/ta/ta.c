#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta.h"
#include <string.h>



TEE_Result TA_CreateEntryPoint(void) {
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[], void **sess_ctx) {
    (void)param_types;
    (void)params;
    (void)sess_ctx;
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void)sess_ctx;
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[]) {
    (void)sess_ctx;
    (void)param_types;
    char secure_data[100] = "[This is a secret text.]";

    switch (cmd_id) {
    case CMD_SESSION_ACCESS:
        TEE_MemMove(params[0].memref.buffer, secure_data, strlen(secure_data)+1);
        return TEE_SUCCESS;

    case CMD_DIRECT_ACCESS:
        params[0].value.a = (uintptr_t)secure_data;
        return TEE_SUCCESS;
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
