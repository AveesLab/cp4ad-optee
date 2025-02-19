#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

// 1. Modify ta header file path.
#include "ta_secret_text.h"

// 2. Define secure data here.
static char secure_data[100];

TEE_Result TA_CreateEntryPoint(void) {
    DMSG("TA created.");
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void) {
    DMSG("TA destroyed.");
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types, TEE_Param params[4], void **sess_ctx) {
    (void)param_types;
    (void)params;
    (void)sess_ctx;
    IMSG("Session opened.");
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *sess_ctx) {
    (void)sess_ctx;
    IMSG("Session closed.");
}

// 2. Implement function here. If needed, you can create new functions.
TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx, uint32_t cmd_id, uint32_t param_types, TEE_Param params[4]) {
    (void)sess_ctx;
    (void)param_types;
    (void)params;

    switch (cmd_id) {
    case CMD_SECRET_TEXT:
        TEE_MemMove(params[0].memref.buffer, params[0].memref.buffer, params[0].memref.size);
        *secure_data = "This is a secret text.";
        TEE_MemMove(params[0].memref.buffer, secure_data, sizeof(secure_data));
        return TEE_SUCCESS;
    case CMD_SECRET_TEXT_ADDR:
        *secure_data = "This is a secret text.";
        params[0].value.a = (uintptr_t)secure_data;
        return TEE_SUCCESS;
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
