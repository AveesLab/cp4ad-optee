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
    (void)cmd_id;
    (void)param_types;

    char input_data[100];
    char reply_data[] = "Hello Host !";
    TEE_MemMove(input_data, params[1].memref.buffer, params[1].memref.size);
    
    switch (cmd_id){
        case CMD_STRING:
            TEE_MemMove(params[2].memref.buffer, reply_data, strlen(reply_data));
            break;
        case CMD_ECHO:
            TEE_MemMove(params[2].memref.buffer, input_data, strlen(input_data));
            break;
        default:
            return TEE_ERROR_BAD_PARAMETERS;
    }

    return TEE_SUCCESS;
}
