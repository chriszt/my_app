#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <my_random_ta.h>

TEE_Result TA_CreateEntryPoint (void)
{
    printf("*** b6c53aba-9669-4668-a7f2-205629d00f87.ta\n");
    printf("*** %s()\n", __func__);
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint (void)
{
    printf("*** %s()\n", __func__);
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t paramTypes,
                                    TEE_Param params[TEE_NUM_PARAMS],
                                    void **sess_ctx)
{
    (void) paramTypes;
    (void) params;
    (void) sess_ctx;

    printf("*** %s()\n", __func__);

    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint (void *sess_ctx)
{
    (void) sess_ctx;
    printf("*** %s()\n", __func__);
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmdID,
                                      uint32_t paramTypes,
                                      TEE_Param params[TEE_NUM_PARAMS])
{
    uint8_t *uuid_buf = NULL;
    uint32_t uuid_size = 0;
    
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    (void) sess_ctx;

    printf("*** %s()\n", __func__);
    printf("cmdID=%u\n", cmdID);
    
    printf("exp_paramTypes=%u\n", exp_paramTypes);
    if (exp_paramTypes != paramTypes) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    uuid_buf = params[0].memref.buffer;
    uuid_size = params[0].memref.size;
    
    printf("uuid_buf=%p, size=%u\n", uuid_buf, uuid_size);

    TEE_GenerateRandom(uuid_buf, uuid_size);
    
    return TEE_SUCCESS;
}