#include <string.h>

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <my_aes_ta.h>

///////////////////////////////////////////////////////////////

static const uint32_t g_AES_128_KEY_SIZE = 128;

static const uint8_t g_AES_128_KEY[] = 
{
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff
};

static const uint8_t g_AES_128_IV[] = 
{
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

///////////////////////////////////////////////////////////////

void print_mem (uint8_t *mem, uint32_t mem_size);
TEE_Result enc (void *sess_ctx, uint32_t pts, TEE_Param *params);
TEE_Result dec (void *sess_ctx, uint32_t pts, TEE_Param *params);
TEE_Result do_cipher (TEE_OperationMode op_mode, void *sess_ctx,
        uint32_t pts, TEE_Param *params);

///////////////////////////////////////////////////////////////

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

TEE_Result TA_OpenSessionEntryPoint (uint32_t pts, 
        TEE_Param params[TEE_NUM_PARAMS], void **sess_ctx)
{
    (void) pts;
    (void) params;
    (void) sess_ctx;

    DMSG("+++");

    // TODO

    DMSG("---");
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint (void *sess_ctx)
{
    (void) sess_ctx;
    
    DMSG("+++");

    // TODO

    DMSG("---");
}

void print_mem (uint8_t *mem, uint32_t mem_size)
{
    uint32_t i;

    if (mem == NULL || mem_size == 0) {
        EMSG("Bad Parameters");
        return;
    }
    
    IMSG("addr:%p, size=%u", mem, mem_size);

    for (i = 0; i < mem_size; ++i) {
        IMSG("%02x ", mem[i]);
        if ((i+1) % 8 == 0) {
            IMSG("\n");
        }
    }
    IMSG("\n");
}

TEE_Result enc (void *sess_ctx, uint32_t pts, TEE_Param *params)
{
    return do_cipher(TEE_MODE_ENCRYPT, sess_ctx, pts, params);
}

TEE_Result dec (void *sess_ctx, uint32_t pts, TEE_Param *params)
{
    return do_cipher(TEE_MODE_DECRYPT, sess_ctx, pts, params);
}

TEE_Result do_cipher (TEE_OperationMode op_mode, void *sess_ctx,
        uint32_t pts, TEE_Param *params)
{
    TEE_Result res;
    TEE_OperationHandle oper_hnd = TEE_HANDLE_NULL;
    TEE_ObjectHandle key_hnd = TEE_HANDLE_NULL;
    TEE_Attribute attr;
    uint8_t *src = NULL;
    uint32_t src_size = 0;
    uint8_t *dst = NULL;
    uint32_t dst_size = 0;

    uint32_t exp_pts = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                       TEE_PARAM_TYPE_MEMREF_OUTPUT,
                                       TEE_PARAM_TYPE_NONE,
                                       TEE_PARAM_TYPE_NONE);
    
    DMSG("+++");

    if (exp_pts != pts) {
        EMSG("Bad Parameters.");
        res = TEE_ERROR_BAD_PARAMETERS;
        goto L1;
    }

    (void) sess_ctx;
    
    src = params[0].memref.buffer;
    src_size = params[0].memref.size;
    dst = params[1].memref.buffer;
    dst_size = params[1].memref.size;

    IMSG("dst=%p, size=%u\n", dst, dst_size);

    IMSG("1. Allocate operation handle\n");
    res = TEE_AllocateOperation(&oper_hnd, TEE_ALG_AES_CBC_NOPAD, op_mode, g_AES_128_KEY_SIZE);
    if (res) {
        EMSG("TEE_AllocateOperation() failed. code=0x%x\n", res);
        goto L1;
    }

    IMSG("2. Allocate transient object handle\n");
    res = TEE_AllocateTransientObject(TEE_TYPE_AES, g_AES_128_KEY_SIZE, &key_hnd);
    if (res) {
        EMSG("TEE_AllocateOperation() failed. code=0x%x\n", res);
        goto L2;
    }

    IMSG("3. Initial attribute\n");
    TEE_InitRefAttribute(&attr, TEE_ATTR_SECRET_VALUE, g_AES_128_KEY, g_AES_128_KEY_SIZE/8);
    
    IMSG("4. attributes -> object handle\n");
    res = TEE_PopulateTransientObject(key_hnd, &attr, 1);
    if (res) {
        EMSG("TEE_PopulateTransientObject() failed. code=0x%x\n", res);
        goto L3;
    }

    IMSG("5. object handle -> operation handle\n");
    res = TEE_SetOperationKey(oper_hnd, key_hnd);
    if (res) {
        EMSG("TEE_SetOperationKey() failed. code=0x%x\n", res);
        goto L3;
    }

    IMSG("6. IV -> operation handle\n");
    TEE_CipherInit(oper_hnd, g_AES_128_IV, g_AES_128_KEY_SIZE/8);

    IMSG("7. Do Cihper");
    res = TEE_CipherUpdate(oper_hnd, src, src_size, dst, &dst_size);
    if (res == TEE_ERROR_SHORT_BUFFER) {
        EMSG("TEE_CipherUpdate() failed. Short Buffer, need dst_size=%u\n", dst_size);
        params[1].memref.size = dst_size;
        goto L3;
    }
    IMSG("dst=%p, size=%u", dst, dst_size);

L3:
    TEE_FreeTransientObject(key_hnd);
    key_hnd = TEE_HANDLE_NULL;
L2:
    TEE_FreeOperation(oper_hnd);
    oper_hnd = TEE_HANDLE_NULL;
L1:
    DMSG("---");
    return res;
}



TEE_Result TA_InvokeCommandEntryPoint (void *sess_ctx, uint32_t cmd_id,
        uint32_t pts, TEE_Param params[TEE_NUM_PARAMS])
{
    DMSG("+++");

    switch (cmd_id) {
    case CMD_ENC:
        return enc(sess_ctx, pts, params);
    case CMD_DEC:
        return dec(sess_ctx, pts, params);
    default:
        return TEE_ERROR_NOT_SUPPORTED;
    }
}




