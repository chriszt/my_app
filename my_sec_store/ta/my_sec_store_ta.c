#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <my_sec_store_ta.h>

#define FILE_NAME_BUF_SIZE    20

static TEE_Result open (char *file_name_tmp, uint32_t file_name_size,
        uint32_t flags, TEE_ObjectHandle *po);
TEE_Result test (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);
TEE_Result create (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);
TEE_Result write (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);
TEE_Result read (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);

TEE_Result TA_CreateEntryPoint (void)
{
    DMSG("+++");

    // TODO

    DMSG("---");
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint (void)
{
    DMSG("+++");

    // TODO

    DMSG("---");
}

TEE_Result TA_OpenSessionEntryPoint (uint32_t paramTypes,
    TEE_Param params[TEE_NUM_PARAMS], void **sessCtx)
{
    (void) paramTypes;
    (void) params;
    (void) sessCtx;
    DMSG("+++");

    // TODO

    DMSG("---");
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint (void *sessCtx)
{
    (void) sessCtx;
    DMSG("+++");
    
    // TODO

    DMSG("---");
}

static TEE_Result open (char *file_name_tmp, uint32_t file_name_size,
        uint32_t flags, TEE_ObjectHandle *po)
{
    TEE_Result ret = TEE_SUCCESS;
    char file_name[FILE_NAME_BUF_SIZE] = {0};
    
    DMSG("+++");

    /*
     * [GP 5.7.1] TEE_OpenPersistentObject(...)
     * objectID, objectIDLen : The object identifier. Note that this cannot reside in shared memory.
     */
    TEE_MemMove(file_name, file_name_tmp, file_name_size);
    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE,
                                   file_name, file_name_size,
                                   flags,
                                   po);

    DMSG("---");
    return ret;
}

TEE_Result test (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    char file_name[] = "test.txt";
    char write_buf[] = "This is a test!!!";
    char read_buf[255] = {0};
    uint32_t read_count = 0;
    TEE_ObjectHandle po = TEE_HANDLE_NULL;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");
    (void) params;
    
    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    //////////////////////////////////////////////////////////////////////////////////

    ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE,
                                     file_name, sizeof(file_name),
                                     TEE_DATA_FLAG_ACCESS_WRITE,
                                     TEE_HANDLE_NULL,
                                     write_buf, sizeof(write_buf),
                                     &po);
    if (ret) {
        EMSG("TEE_CreatePersistentObject failed, code=0x%x", ret);
        goto L1;
    }

    IMSG("Create PO Success:)");
    TEE_CloseObject(po);
    po = TEE_HANDLE_NULL;

    //////////////////////////////////////////////////////////////////////////////////

    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, 
                                   file_name, sizeof(file_name),
                                   TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE_META,
                                   &po);
    if (ret) {
        EMSG("TEE_OpenPersistentObject failed, code=0x%x", ret);
        goto L1;
    }
    IMSG("Open PO Success:)");

    ret = TEE_ReadObjectData(po, read_buf, sizeof(read_buf), &read_count);
    if (ret) {
        EMSG("TEE_ReadObjectData failed, code=0x%x", ret);
        goto L1;
    }
    IMSG("Read PO Success:)");
    IMSG("write_buf=%s", write_buf);
    IMSG("read_buf=%s", read_buf);
    IMSG("read_count=%u", read_count);

//  TEE_CloseObject(po);
    TEE_CloseAndDeletePersistentObject1(po);
    po = TEE_HANDLE_NULL;

L1:
    DMSG("---");
    return ret;
}

TEE_Result create (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    TEE_ObjectHandle po = TEE_HANDLE_NULL;
    char *file_name_tmp = NULL;
    char file_name[FILE_NAME_BUF_SIZE] = {0};
    uint32_t file_name_size = 0;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");

    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    /*
     * [GP 5.7.2] TEE_CreatePersistentObject(...)
     * objectID, objectIDLen : The object identifier. Note that this cannot reside in shared memory.
     */
    file_name_tmp = params[0].memref.buffer;
    file_name_size = params[0].memref.size;
    TEE_MemMove(file_name, file_name_tmp, file_name_size);
    IMSG("file_name=%s", file_name);
    IMSG("file_name_size=%u", file_name_size);

    ret = TEE_CreatePersistentObject(TEE_STORAGE_PRIVATE, 
                                     file_name, file_name_size,
                                     TEE_DATA_FLAG_ACCESS_WRITE | TEE_DATA_FLAG_ACCESS_WRITE_META,
                                     TEE_HANDLE_NULL,
                                     NULL, 0,
                                     &po);
    if (ret) {
        EMSG("Create %s failed, code=0x%x", file_name, ret);
        goto L1;
    }
    IMSG("Create %s Success:)", file_name);
    TEE_CloseObject(po);
L1:
    DMSG("---");
    return ret;
}

TEE_Result write (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    TEE_ObjectHandle po = TEE_HANDLE_NULL;
    char *file_name_tmp = NULL;
    uint32_t file_name_size = 0;
    char *write_buf = NULL;
    uint32_t write_buf_size = 0;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                              TEE_PARAM_TYPE_MEMREF_INPUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");

    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    file_name_tmp = params[0].memref.buffer;
    file_name_size = params[0].memref.size;
    write_buf = params[1].memref.buffer;
    write_buf_size = params[1].memref.size;

    IMSG("file_name_tmp=%s", file_name_tmp);
    IMSG("file_name_size=%u", file_name_size);
    
    ret = open(file_name_tmp, file_name_size, TEE_DATA_FLAG_ACCESS_WRITE, &po);
    if (ret) {
        EMSG("Open %s failed, code=0x%x", file_name_tmp, ret);
        goto L1;
    }
    IMSG("Open %s Success:)", file_name_tmp);

    ret = TEE_WriteObjectData(po, write_buf, write_buf_size);
    if (ret) {
        EMSG("Write failed, code=0x%x", ret);
        goto L2;
    }
    IMSG("Write Success:)");

L2:
    TEE_CloseObject(po);
L1:
    DMSG("---");
    return ret;
}

TEE_Result read (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    TEE_ObjectHandle po = TEE_HANDLE_NULL;
    char *file_name_tmp = NULL;
    uint32_t file_name_size = 0;
    char *read_buf = NULL;
    uint32_t read_buf_size = 0;
    uint32_t read_length = 0;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                              TEE_PARAM_TYPE_MEMREF_OUTPUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");

    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }
    
    file_name_tmp = params[0].memref.buffer;
    file_name_size = params[0].memref.size;
    read_buf = params[1].memref.buffer;
    read_buf_size = params[1].memref.size;

    IMSG("file_name_tmp=%s", file_name_tmp);
    IMSG("file_name_size=%u", file_name_size);
    
    ret = open(file_name_tmp, file_name_size, TEE_DATA_FLAG_ACCESS_READ, &po);
    if (ret) {
        EMSG("Open %s failed, code=0x%x", file_name_tmp, ret);
        goto L1;
    }
    IMSG("Open %s Success:)", file_name_tmp);

    ret = TEE_ReadObjectData(po, read_buf, read_buf_size, &read_length);
    if (ret) {
        EMSG("Read failed, code=0x%x", ret);
        goto L2;
    }
    IMSG("Read Success:)");

L2:
    TEE_CloseObject(po);
L1:
    DMSG("---");
    return ret;
}

TEE_Result TA_InvokeCommandEntryPoint (void *sessCtx, uint32_t cmdID,
    uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;

    (void) &sessCtx;
    DMSG("+++");

    switch (cmdID) {
    case TA_CMD_TEST:
        ret = test(paramTypes, params);
        break;
    case TA_CMD_CREATE:
        ret = create(paramTypes, params);
        break;
    case TA_CMD_WRITE:
        ret = write(paramTypes, params);
        break;
    case TA_CMD_READ:
        ret = read(paramTypes, params);
        break;
    default:
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    DMSG("---");
    return ret;
}
