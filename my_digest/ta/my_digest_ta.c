#include <string.h>

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <my_digest_ta.h>

// reference <tee_api_defines.h> digest algorithm.
#define CURRENT_DIGEST_ALG    TEE_ALG_SHA512

void printf_op_info (TEE_OperationInfo *);

TEE_Result TA_CreateEntryPoint (void)
{
    printf("*** b6c53aba-9669-4668-a7f2-205629d00f88.ta\n");
    printf("*** %s()\n", __func__);
    return TEE_SUCCESS;
}

void TA_DestroyEntryPoint (void)
{
    printf("*** %s()\n", __func__);
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t pts,
                                    TEE_Param params[TEE_NUM_PARAMS],
                                    void **sess_ctx)
{
    (void) pts;
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

void printf_op_info (TEE_OperationInfo *op_info)
{
    printf("+++ %s() +++\n", __func__);
    printf("algorithm=0x%x\n", op_info->algorithm);
    printf("mode=0x%x\n", op_info->mode);
    printf("maxKeySize=%u\n", op_info->maxKeySize);
    printf("operationClass=0x%x\n", op_info->operationClass);
    printf("keySize=%u\n", op_info->keySize);
    printf("requiredKeyUsage=0x%x\n", op_info->requiredKeyUsage);
    printf("digestLength=%u\n", op_info->digestLength);
    printf("handleState=0x%x\n", op_info->handleState);
    printf("------------------------\n");
}

TEE_Result TA_InvokeCommandEntryPoint(void *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t pts,
                                      TEE_Param params[TEE_NUM_PARAMS])
{
    TEE_Result res;
    void *msg = NULL;
    uint32_t msg_size = 0;
    void *hash = NULL;
    uint32_t hash_size = 0;
    TEE_OperationHandle op = TEE_HANDLE_NULL;
//  TEE_OperationInfo op_info = {0};
    uint32_t exp_pts = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                       TEE_PARAM_TYPE_MEMREF_OUTPUT,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE);
    (void) sess_ctx;

    printf("*** %s()\n", __func__);
    printf("cmd_id=%u\n", cmd_id);
    printf("exp_pts=0x%x, pts=0x%x\n", exp_pts, pts);
    if (exp_pts != pts) {
        printf("*** bad parameters. ***\n");
        return TEE_ERROR_BAD_PARAMETERS;
    }
    
    msg = params[0].memref.buffer;
    msg_size = params[0].memref.size;
    hash = params[1].memref.buffer;
    hash_size = params[1].memref.size;

    printf("msg(%p)=%s\n", msg, (char *)msg);
    printf("msg size=%u\n", msg_size);
    printf("hash=%p, hash_size=%u\n", hash, hash_size);
    
/*  memset(&op_info, 0, sizeof(TEE_OperationInfo));
    TEE_GetOperationInfo(op, &op_info);
    printf_op_info(&op_info);*/

    res = TEE_AllocateOperation(&op, CURRENT_DIGEST_ALG, TEE_MODE_DIGEST, 0);
/*  memset(&op_info, 0, sizeof(TEE_OperationInfo));
    TEE_GetOperationInfo(op, &op_info);
    printf_op_info(&op_info);*/
    if (res) {
        printf("TEE_AllocateOperation() failed. Code=0x%x\n", res);
        return res;
    }
    
    printf("*** ok1 ***\n");
    
    TEE_DigestUpdate(op, msg, msg_size);
/*  memset(&op_info, 0, sizeof(TEE_OperationInfo));
    TEE_GetOperationInfo(op, &op_info);
    printf_op_info(&op_info);*/
    
    printf("*** ok2 ***\n");
    
    res = TEE_DigestDoFinal(op, NULL, 0, hash, &hash_size);
/*  memset(&op_info, 0, sizeof(TEE_OperationInfo));
    TEE_GetOperationInfo(op, &op_info);
    printf_op_info(&op_info);*/
    if (res == TEE_ERROR_SHORT_BUFFER) {
        printf("need realloc hash_size=%u\n", hash_size);
        params[1].memref.size = hash_size;
    } else if (res) {
        printf("TEE_DigestDoFinal() failed. Code=0x%x\n", res);
    }

    printf("*** ok3 ***\n");
    
    TEE_FreeOperation(op);
/*  memset(&op_info, 0, sizeof(TEE_OperationInfo));
    TEE_GetOperationInfo(op, &op_info);
    printf_op_info(&op_info);*/
    op = TEE_HANDLE_NULL;
    return res;
}