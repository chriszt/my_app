#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <my_test_ta.h>

TEE_Result inc (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);
TEE_Result dec (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);

TEE_Result TA_CreateEntryPoint (void)
{
    IMSG("+++++ aaaa9a0b-f50f-484b-a899-932aa6a879a1.ta +++++");
    DMSG("+++");

    // TODO

    DMSG("---");
    return TEE_SUCCESS;
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

TEE_Result inc (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");
    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    params[0].value.a += params[0].value.b;
    params[0].value.b = 0;

L1:
    DMSG("---");
    return ret;
}

TEE_Result dec (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");
    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    params[0].value.a -= params[0].value.b;
    params[0].value.b = 0;
    
L1:
    DMSG("---");
    return ret;
}

TEE_Result TA_InvokeCommandEntryPoint (void *sessCtx, uint32_t cmdID,
    uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_ERROR_GENERIC;

    (void) &sessCtx;
    DMSG("+++");

    switch (cmdID) {
    case TA_MY_TEST_CMD_INC:
        ret = inc(paramTypes, params);
        break;
    case TA_MY_TEST_CMD_DEC:
        ret = dec(paramTypes, params);
        break;
    default:
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    DMSG("---");
    return ret;
}

void TA_CloseSessionEntryPoint (void *sessCtx)
{
    (void) sessCtx;
    DMSG("+++");
    
    // TODO

    DMSG("---");
}

void TA_DestroyEntryPoint (void)
{
    DMSG("+++");

    // TODO

    DMSG("---");
    IMSG("---------------------------------------------------\n");
}