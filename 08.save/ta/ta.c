#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta.h"
#include <string.h>

TEE_Result TA_CreateEntryPoint(void) { return TEE_SUCCESS; }
void TA_DestroyEntryPoint(void) { }
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param params[4],
                                    void **sess_ctx) {
    (void)param_types; (void)params; (void)sess_ctx;
    return TEE_SUCCESS;
}
void TA_CloseSessionEntryPoint(void *sess_ctx) { (void)sess_ctx; }

static TEE_Result save_object(const void *key, size_t key_len,
                              const void *data, size_t data_len)
{
    TEE_Result       res;
    TEE_ObjectHandle obj = TEE_HANDLE_NULL;

    /* Try to create */
    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                     key, key_len,
                                     TEE_DATA_FLAG_ACCESS_READ |
                                     TEE_DATA_FLAG_ACCESS_WRITE,
                                     TEE_HANDLE_NULL,
                                     data, data_len,
                                     &obj);
    if (res == TEE_ERROR_ACCESS_CONFLICT) {
        /* Already exists: open and delete first */
        TEE_Result res2 = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                                   key, key_len,
                                                   TEE_DATA_FLAG_ACCESS_WRITE,
                                                   &obj);
        if (res2 != TEE_SUCCESS)
            return res2;
        TEE_CloseAndDeletePersistentObject(obj);
        /* Recreate */
        res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                         key, key_len,
                                         TEE_DATA_FLAG_ACCESS_READ |
                                         TEE_DATA_FLAG_ACCESS_WRITE,
                                         TEE_HANDLE_NULL,
                                         data, data_len,
                                         &obj);
    }
    if (res != TEE_SUCCESS) {
        return res;
    }
    TEE_CloseObject(obj);
    return TEE_SUCCESS;
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types,
                                      TEE_Param params[4])
{
    (void)sess_ctx;
    if (cmd_id == CMD_SAVE &&
        param_types == TEE_PARAM_TYPES(
            TEE_PARAM_TYPE_MEMREF_INPUT,
            TEE_PARAM_TYPE_MEMREF_INPUT,
            TEE_PARAM_TYPE_NONE,
            TEE_PARAM_TYPE_NONE)) {
        return save_object(params[0].memref.buffer,
                           params[0].memref.size,
                           params[1].memref.buffer,
                           params[1].memref.size);
    }
    return TEE_ERROR_BAD_PARAMETERS;
}