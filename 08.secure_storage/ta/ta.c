#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta.h"
#include <string.h>

static TEE_Result save_object(TEE_Param params[4]);
static TEE_Result load_object(TEE_Param params[4]);
static TEE_Result delete_object(TEE_Param params[4]);

TEE_Result TA_CreateEntryPoint(void) { return TEE_SUCCESS; }
void TA_DestroyEntryPoint(void) { }
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param params[4],
                                    void **sess_ctx) {
    (void)param_types; (void)params; (void)sess_ctx;
    return TEE_SUCCESS;
}
void TA_CloseSessionEntryPoint(void *sess_ctx) { (void)sess_ctx; }

static TEE_Result save_object(TEE_Param params[4])
{
    TEE_ObjectHandle obj = TEE_HANDLE_NULL;

    // write here //
    size_t key_len = ;
    char *key = ;
    TEE_MemMove();

    // write here //
    size_t data_len = ;
    char *data = ;
    TEE_MemMove();

    // write here //
    TEE_CreatePersistentObject();

    TEE_CloseObject(obj);
    TEE_Free(key);
    TEE_Free(data);
    return TEE_SUCCESS;
}

static TEE_Result load_object(TEE_Param params[4])
{
    TEE_ObjectHandle obj;
    TEE_ObjectInfo object_info;
    
    // write here //
    size_t key_len = ;
    char *key key = ;
    TEE_MemMove();
    TEE_OpenPersistentObject();

    TEE_GetObjectInfo1(obj, &object_info);
    char *data = TEE_Malloc(object_info.dataSize, 0);
    size_t data_len = 0;
    TEE_ReadObjectData(obj, data, object_info.dataSize, &data_len);
    TEE_MemMove(params[1].memref.buffer, data, object_info.dataSize);

    // write here //
    params[1].memref.size = ;

    TEE_CloseObject(obj);
    TEE_Free(key);
    TEE_Free(data);
    return TEE_SUCCESS;
}

static TEE_Result delete_object(TEE_Param params[4])
{
	TEE_ObjectHandle object;

    // write here //
	sizr_t key_len = ;
	char *key = TEE_Malloc();
	TEE_MemMove();
	TEE_OpenPersistentObject();

	TEE_CloseAndDeletePersistentObject1(object);
	TEE_Free(key);

	return TEE_SUCCESS;
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types,
                                      TEE_Param params[4])
{
    (void)sess_ctx;
    (void)param_types;

    switch (cmd_id) {
    case CMD_SAVE:
        return save_object(params);

    case CMD_LOAD:
        return load_object(params);

    case CMD_DELETE:
        return delete_object(params);

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
