#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <my_add_lib_ta.h>

#include <algo.h>

TEE_Result cmd_add (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);
TEE_Result cmd_sub (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS]);

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

TEE_Result cmd_add (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                                              TEE_PARAM_TYPE_VALUE_OUTPUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");
    
    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    params[1].value.a = add(params[0].value.a, params[0].value.b);

L1:
    DMSG("---");
    return ret;
}

TEE_Result cmd_sub (uint32_t paramTypes, TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result ret = TEE_SUCCESS;
    uint32_t exp_paramTypes = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                                              TEE_PARAM_TYPE_VALUE_OUTPUT,
                                              TEE_PARAM_TYPE_NONE,
                                              TEE_PARAM_TYPE_NONE);
    DMSG("+++");
    
    if (paramTypes != exp_paramTypes) {
        EMSG("Bad Parameters");
        ret = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    params[1].value.a = sub(params[0].value.a, params[0].value.b);

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
    case TA_CMD_ADD:
        ret = cmd_add(paramTypes, params);
        break;
    case TA_CMD_SUB:
        ret = cmd_sub(paramTypes, params);
        break;
    default:
        ret = TEE_ERROR_BAD_PARAMETERS;
    }

    DMSG("---");
    return ret;
}
