#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <my_sec_store_ta.h>

TEE_Result test (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);

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

TEE_Result test (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    char file_name[] = "aaaa.txt";
    char contents[] = "This is a test!!!";
    char read_buf[255] = {0};
    uint32_t count = 0;
    TEE_ObjectHandle po = TEE_HANDLE_NULL;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");
    
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
                                     contents, sizeof(contents),
                                     &po);
    if (ret) {
        EMSG("TEE_CreatePersistentObject failed, code=0x%x", ret);
        goto L1;
    }

    IMSG("Create PO Success:)");
    TEE_CloseObject(po);
    po = TEE_HANDLE_NULL;

    //////////////////////////////////////////////////////////////////////////////////

    ret = TEE_OpenPersistentObject(TEE_STORAGE_PRIVATE, file_name, sizeof(file_name),
                                    TEE_DATA_FLAG_ACCESS_READ | TEE_DATA_FLAG_ACCESS_WRITE_META,
                                    &po);
    if (ret) {
        EMSG("TEE_OpenPersistentObject failed, code=0x%x", ret);
        goto L1;
    }
    IMSG("Open PO Success:)");

    ret = TEE_ReadObjectData(po, read_buf, sizeof(read_buf), &count);
    if (ret) {
        EMSG("TEE_ReadObjectData failed, code=0x%x", ret);
        goto L1;
    }
    IMSG("Read PO Success:)");
    IMSG("init contents=%s", contents);
    IMSG("read content=%s", read_buf);
    IMSG("count=%u", count);

//  TEE_CloseObject(po);
    TEE_CloseAndDeletePersistentObject1(po);
    po = TEE_HANDLE_NULL;

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
    default:
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    DMSG("---");
    return ret;
}
