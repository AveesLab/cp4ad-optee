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
static TEE_Result save_object(const void *key, size_t key_len,
                              const void *data, size_t data_len)
{
    TEE_Result       res;
    TEE_ObjectHandle obj = TEE_HANDLE_NULL;

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

/*
 * load_object: key/key_len     → 식별자
 *              data             → 읽은 데이터를 저장할 버퍼
 *              data_len_ptr     → [in]버퍼 최대 크기, [out]실제 읽은 크기
 */
static TEE_Result load_object(const void *key, size_t key_len,
                              void *data, size_t *data_len_ptr)
{
    TEE_Result       res;
    TEE_ObjectHandle obj = TEE_HANDLE_NULL;
    size_t           read_size = *data_len_ptr;

    /* 1) 객체 열기 */
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                   key, key_len,
                                   TEE_DATA_FLAG_ACCESS_READ,
                                   &obj);
    if (res != TEE_SUCCESS)
        return res;

    /* 2) 데이터 읽기 */
    res = TEE_ReadObjectData(obj, data, read_size, &read_size);
    TEE_CloseObject(obj);
    if (res != TEE_SUCCESS)
        return res;

    /* 3) 실제 읽은 크기를 호출자에게 전달 */
    *data_len_ptr = read_size;
    return TEE_SUCCESS;
}

/*
 * delete_object: key/key_len → 식별자
 */
static TEE_Result delete_object(const void *key, size_t key_len)
{
    TEE_Result       res;
    TEE_ObjectHandle obj = TEE_HANDLE_NULL;

    /* 1) 객체 열기 */
    res = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                   key, key_len,
                                   TEE_DATA_FLAG_ACCESS_WRITE,
                                   &obj);
    if (res != TEE_SUCCESS)
        return res;

    /* 2) 삭제 */
    TEE_CloseAndDeletePersistentObject(obj);
    return TEE_SUCCESS;
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types,
                                      TEE_Param params[4])
{
    (void)sess_ctx;

    switch (cmd_id) {
    case CMD_SAVE:
        if (param_types != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_MEMREF_INPUT,
                TEE_PARAM_TYPE_MEMREF_INPUT,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE))
            return TEE_ERROR_BAD_PARAMETERS;
        return save_object(params[0].memref.buffer,
                           params[0].memref.size,
                           params[1].memref.buffer,
                           params[1].memref.size);

    case CMD_LOAD:
        if (param_types != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_MEMREF_INPUT,
                TEE_PARAM_TYPE_MEMREF_OUTPUT,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE))
            return TEE_ERROR_BAD_PARAMETERS;
        return load_object(params[0].memref.buffer,
                           params[0].memref.size,
                           params[1].memref.buffer,
                           &params[1].memref.size);

    case CMD_DELETE:
        if (param_types != TEE_PARAM_TYPES(
                TEE_PARAM_TYPE_MEMREF_INPUT,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE,
                TEE_PARAM_TYPE_NONE))
            return TEE_ERROR_BAD_PARAMETERS;
        return delete_object(params[0].memref.buffer,
                             params[0].memref.size);

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
