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

    size_t key_len = params[0].memref.size;
    char *key = TEE_Malloc(key_len, 0);
    TEE_MemMove(key, params[0].memref.buffer, key_len);

    size_t data_len = params[1].memref.size;
    char *data = TEE_Malloc(data_len, 0);
    TEE_MemMove(data, params[1].memref.buffer, data_len);

    TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                     key, key_len,
                                     TEE_DATA_FLAG_ACCESS_WRITE |
                                     TEE_DATA_FLAG_OVERWRITE,
                                     TEE_HANDLE_NULL,
                                     data, data_len,
                                     &obj);
    TEE_CloseObject(obj);
    TEE_Free(key);
    TEE_Free(data);
    return TEE_SUCCESS;
}

static TEE_Result load_object(TEE_Param params[4])
{
    TEE_ObjectHandle obj;
    TEE_ObjectInfo object_info;
    
    char *key;
    size_t key_len;
    char *data;
    size_t data_len;

    key_len = params[0].memref.size;
    key = TEE_Malloc(key_len, 0);
    TEE_MemMove(key, params[0].memref.buffer, key_len);

    TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                   key, key_len,
                                   TEE_DATA_FLAG_ACCESS_READ,
                                   &obj);
    TEE_GetObjectInfo1(obj, &object_info);

    data = TEE_Malloc(object_info.dataSize, 0);

    TEE_ReadObjectData(obj, data, object_info.dataSize, &data_len);
    TEE_MemMove(params[1].memref.buffer, data, object_info.dataSize);
    params[1].memref.size = object_info.dataSize;

    TEE_CloseObject(obj);
    TEE_Free(key);
    TEE_Free(data);
    return TEE_SUCCESS;
}

static TEE_Result delete_object(TEE_Param params[4])
{
	TEE_ObjectHandle object;

	char *key;
	size_t key_len;

	key_len = params[0].memref.size;
	key = TEE_Malloc(key_len, 0);

	TEE_MemMove(key, params[0].memref.buffer, key_len);

	TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
					key, key_len,
					TEE_DATA_FLAG_ACCESS_WRITE_META,
					&object);
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
