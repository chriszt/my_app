#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <tee_client_api.h>
#include <tee_client_api_extensions.h>

#include <my_digest_ta.h>

int main (void)
{
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = MY_DIGEST_UUID;
    TEEC_Result res;
    uint32_t org;
    int i;

    char msg[] = "This is a test!!!";
    uint8_t *hash = NULL;
    uint32_t hash_size = 1;


    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
    }

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &org);
    if (res != TEEC_SUCCESS) {
        fprintf(stderr, "TEEC_OpenSession failed with code 0x%x origin 0x%x\n", res, org);
        goto l_1;
    }

    hash = (uint8_t *)malloc(hash_size);
    if (!hash) {
        fprintf(stderr, "Alloc memory failed\n");
        goto l_2;
    }

    memset(&op, 0, sizeof(TEEC_Operation));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_OUTPUT, 
                                     TEEC_NONE, 
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = msg;
    op.params[0].tmpref.size = strlen(msg);
    op.params[1].tmpref.buffer = hash;
    op.params[1].tmpref.size = hash_size;
    
    printf("cmd_id=%u\n", CMD_DIGEST);
    printf("pts=0x%x\n", op.paramTypes);
    printf("msg(%p)=%s\n", op.params[0].tmpref.buffer, (char *)op.params[0].tmpref.buffer);
    printf("msg size=%lu\n", op.params[0].tmpref.size);
    printf("hash(%p)\n", op.params[1].tmpref.buffer);
    printf("hash size=%lu\n", op.params[1].tmpref.size);

    res = TEEC_InvokeCommand(&sess, CMD_DIGEST, &op, &org);
    hash = op.params[1].tmpref.buffer;
    hash_size = op.params[1].tmpref.size;
    if ((res == TEEC_ERROR_SHORT_BUFFER) && (org == TEEC_ORIGIN_TRUSTED_APP)) {
        printf("realloc hash buf, new hash_size=%u\n", hash_size);
    } else if (res) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, org);
        goto l_3;
    }

    free(hash);
    hash = (uint8_t *)malloc(op.params[1].tmpref.size);
    if (!hash) {
        fprintf(stderr, "Alloc memory failed\n");
        goto l_2;
    }
    op.params[1].tmpref.buffer = hash;

    printf("cmd_id=%u\n", CMD_DIGEST);
    printf("pts=0x%x\n", op.paramTypes);
    printf("msg(%p) str=%s\n", op.params[0].tmpref.buffer, (char *)op.params[0].tmpref.buffer);
    printf("msg size=%lu\n", op.params[0].tmpref.size);
    printf("hash(%p)\n", op.params[1].tmpref.buffer);
    printf("new hash size=%lu\n", op.params[1].tmpref.size);

    res = TEEC_InvokeCommand(&sess, CMD_DIGEST, &op, &org);
    hash = op.params[1].tmpref.buffer;
    hash_size = op.params[1].tmpref.size;
    if (res) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", res, org);
        goto l_3;
    }

    for (i = 0; i < hash_size; i++) {
        printf("0x%02x ", hash[i]);
        if ((i+1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");

l_3:
    free(hash);
l_2:
    TEEC_CloseSession(&sess);
l_1:
    TEEC_FinalizeContext(&ctx);
    return 0;
}