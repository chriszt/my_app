#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <tee_client_api.h>
#include <tee_client_api_extensions.h>

#include <my_aes_ta.h>

void print_mem (uint8_t *mem, uint32_t mem_size);

void print_mem (uint8_t *mem, uint32_t mem_size)
{
    uint32_t i;

    if (mem == NULL || mem_size == 0) {
        printf("Bad Parameters\n");       
        return;
    }
    
    printf("addr:%p, size=%u\n", mem, mem_size);

    for (i = 0; i < mem_size; ++i) {
        printf("%02x ", mem[i]);
        if ((i+1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");

}

int main (void)
{
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = MY_AES_UUID;
    TEEC_Result res;
    uint32_t org;

    char msg[] = "Come On! You! You! check now,jian bing guo zi lai yi tao! yeah!";  // sizeof(msg) = 64
    uint8_t *enc_buf = NULL;
    uint32_t enc_buf_size = 0;
    uint8_t *dec_buf = NULL;
    uint32_t dec_buf_size = 0;

    printf("msg=%s\n", msg);
    print_mem((uint8_t *)msg, sizeof(msg));

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res) {
        fprintf(stderr, "TEEC_InitializeContext failed with code 0x%x", res);
        goto L1;
    }

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &org);
    if (res) {
        fprintf(stderr, "TEEC_OpenSession failed with code 0x%x origin 0x%x\n", res, org);
        goto L2;
    }

    memset(&op, 0, sizeof(TEEC_Operation));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_OUTPUT, 
                                     TEEC_NONE, 
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = msg;
    op.params[0].tmpref.size = sizeof(msg);

    res = TEEC_InvokeCommand(&sess, CMD_ENC, &op, &org);
    if ((res == TEEC_ERROR_SHORT_BUFFER) && (org == TEEC_ORIGIN_TRUSTED_APP)) {
        enc_buf_size = op.params[1].tmpref.size;
        printf("enc_buf_size=%u\n", enc_buf_size);
    } else if (res) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, org);
        goto L3;
    }

    enc_buf = (uint8_t *)malloc(enc_buf_size);
    if (!enc_buf) {
        fprintf(stderr, "Allocate memory failed\n");
        goto L3;
    }
    memset(enc_buf, 0, enc_buf_size);

    op.params[1].tmpref.buffer = enc_buf;
    op.params[1].tmpref.size = enc_buf_size;

    res = TEEC_InvokeCommand(&sess, CMD_ENC, &op, &org);
    if (res) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, org);
        goto L3;
    }

    print_mem(enc_buf, enc_buf_size);

    /////////////////////////////////////////////////////////////////////////////////////

    memset(&op, 0, sizeof(TEEC_Operation));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_OUTPUT, 
                                     TEEC_NONE, 
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = enc_buf;
    op.params[0].tmpref.size = enc_buf_size;

    res = TEEC_InvokeCommand(&sess, CMD_DEC, &op, &org);
    if ((res == TEEC_ERROR_SHORT_BUFFER) && (org == TEEC_ORIGIN_TRUSTED_APP)) {
        dec_buf_size = op.params[1].tmpref.size;
        printf("dec_buf_size=%u\n", dec_buf_size);
    } else if (res) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, org);
        goto L4;
    }

    dec_buf = (uint8_t *)malloc(dec_buf_size);
    if (!dec_buf) {
        fprintf(stderr, "Allocate memory failed\n");
        goto L4;
    }
    memset(dec_buf, 0, dec_buf_size);

    op.params[1].tmpref.buffer = dec_buf;
    op.params[1].tmpref.size = dec_buf_size;

    res = TEEC_InvokeCommand(&sess, CMD_DEC, &op, &org);
    if (res) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, org);
        goto L4;
    }

    print_mem(dec_buf, dec_buf_size);
    printf("dec_buf=%s\n", dec_buf);


    free(dec_buf);
    dec_buf = NULL;
L4:
    free(enc_buf);
    enc_buf = NULL;
L3:
    TEEC_CloseSession(&sess);
L2:
    TEEC_FinalizeContext(&ctx);
L1:
    return res;
}