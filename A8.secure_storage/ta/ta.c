#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include "ta.h"
#include <string.h>

/* TA 진입점들 그대로 유지 */
TEE_Result TA_CreateEntryPoint(void) { return TEE_SUCCESS; }
void TA_DestroyEntryPoint(void) { }
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param params[4],
                                    void **sess_ctx) {
    (void)param_types; (void)params; (void)sess_ctx;
    return TEE_SUCCESS;
}
void TA_CloseSessionEntryPoint(void *sess_ctx) { (void)sess_ctx; }

/*
 * save_object: key/key_len → 식별자
 *              data/data_len → 저장할 값
 */
static TEE_Result save_object(uint32_t param_types, TEE_Param params[4])
{
    TEE_Result       res;
    TEE_ObjectHandle obj = TEE_HANDLE_NULL;

    size_t key_len = params[0].memref.size;
    char *key = TEE_Malloc(key_len, 0);
    TEE_MemMove(key, params[0].memref.buffer, key_len);

    size_t data_len = params[1].memref.size;
    char *data = TEE_Malloc(data_len, 0);
    TEE_MemMove(data, params[1].memref.buffer, data_len);


    /* 1) 객체 생성 시도 */
    res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                     key, key_len,
                                     TEE_DATA_FLAG_ACCESS_READ |
                                     TEE_DATA_FLAG_ACCESS_WRITE,
                                     TEE_HANDLE_NULL,
                                     data, data_len,
                                     &obj);
    if (res == TEE_ERROR_ACCESS_CONFLICT) {
        /* 이미 있으면, 열고 삭제한 뒤 재생성 */
        TEE_Result res2 = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                                   key, key_len,
                                                   TEE_DATA_FLAG_ACCESS_WRITE,
                                                   &obj);
        if (res2 != TEE_SUCCESS)
            return res2;
        TEE_CloseAndDeletePersistentObject(obj);
        res = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                         key, key_len,
                                         TEE_DATA_FLAG_ACCESS_READ |
                                         TEE_DATA_FLAG_ACCESS_WRITE |
                                         TEE_DATA_FLAG_ACCESS_WRITE_META |
                                         TEE_DATA_FLAG_OVERWRITE,
                                         TEE_HANDLE_NULL,
                                         data, data_len,
                                         &obj);
    }
    if (res != TEE_SUCCESS) {
        return res;
    }
    TEE_CloseObject(obj);
    TEE_Free(key);
    TEE_Free(data);
    return TEE_SUCCESS;
}

static TEE_Result load_object(uint32_t param_types, TEE_Param params[4])
{
    const uint32_t exp_param_types =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
				TEE_PARAM_TYPE_MEMREF_OUTPUT,
				TEE_PARAM_TYPE_NONE,
				TEE_PARAM_TYPE_NONE);
    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;
                
    TEE_Result       res;
    TEE_ObjectHandle obj;
    TEE_ObjectInfo object_info;
    
    uint32_t read_bytes;
    char *key;
    size_t key_len;
    char *data;
    size_t data_len;

    key_len = params[0].memref.size;
    key = TEE_Malloc(key_len, 0);
    if (!key)
        return TEE_ERROR_OUT_OF_MEMORY;
    TEE_MemMove(key, params[0].memref.buffer, key_len);

    data_len = params[1].memref.size;
    data = TEE_Malloc(data_len, 0);
    if (!data) {
        return TEE_ERROR_OUT_OF_MEMORY;
    }

    /* 1) 객체 열기 */
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                   key, key_len,
                                   TEE_DATA_FLAG_ACCESS_READ |	TEE_DATA_FLAG_SHARE_READ,
                                   &obj);
    if (res != TEE_SUCCESS) {
        TEE_Free(key);
        TEE_Free(data);
        return res;
    }
    res = TEE_GetObjectInfo1(obj, &object_info);
    if (res != TEE_SUCCESS) {
		EMSG("Failed to create persistent object, res=0x%08x", res);
		goto exit;
	}

    if (object_info.dataSize > data_len) {
		/*
		 * Provided buffer is too short.
		 * Return the expected size together with status "short buffer"
		 */
		params[1].memref.size = object_info.dataSize;
		res = TEE_ERROR_SHORT_BUFFER;
		goto exit;
	}

    res = TEE_ReadObjectData(obj, data, object_info.dataSize, &read_bytes);
    if (res == TEE_SUCCESS)
		TEE_MemMove(params[1].memref.buffer, data, read_bytes);
	if (res != TEE_SUCCESS || read_bytes != object_info.dataSize) {
		goto exit;
	}

    params[1].memref.size = read_bytes;
exit:
    TEE_CloseObject(obj);
    TEE_Free(key);
    TEE_Free(data);
    return res;
}

/*
 * delete_object: key/key_len → 식별자
 */
static TEE_Result delete_object(uint32_t param_types, TEE_Param params[4])
{
	const uint32_t exp_param_types =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
				TEE_PARAM_TYPE_NONE,
				TEE_PARAM_TYPE_NONE,
				TEE_PARAM_TYPE_NONE);
	TEE_ObjectHandle object;
	TEE_Result res;
	char *obj_id;
	size_t obj_id_sz;

	/*
	 * Safely get the invocation parameters
	 */
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	obj_id_sz = params[0].memref.size;
	obj_id = TEE_Malloc(obj_id_sz, 0);
	if (!obj_id)
		return TEE_ERROR_OUT_OF_MEMORY;

	TEE_MemMove(obj_id, params[0].memref.buffer, obj_id_sz);

	/*
	 * Check object exists and delete it
	 */
	res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
					obj_id, obj_id_sz,
					TEE_DATA_FLAG_ACCESS_READ |
					TEE_DATA_FLAG_ACCESS_WRITE_META, /* we must be allowed to delete it */
					&object);
	if (res != TEE_SUCCESS) {
		EMSG("Failed to open persistent object, res=0x%08x", res);
		TEE_Free(obj_id);
		return res;
	}

	TEE_CloseAndDeletePersistentObject1(object);
	TEE_Free(obj_id);

	return res;
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types,
                                      TEE_Param params[4])
{
    (void)sess_ctx;

    switch (cmd_id) {
    case CMD_SAVE:
        return save_object(param_types, params);

    case CMD_LOAD:
        return load_object(param_types, params);

    case CMD_DELETE:
        return delete_object(param_types, params);

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
