#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <tee_client_api.h>
#include <tee_client_api_extensions.h>

#include <my_random_ta.h>

int main (void)
{
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = MY_RANDOM_UUID;
    TEEC_Result res;
    uint32_t origin;
    
    int i;
    uint8_t *uuid_buf = NULL;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
    }

    res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (res != TEEC_SUCCESS) {
        TEEC_FinalizeContext(&ctx);
        errx(1, "TEEC_OpenSession failed with code 0x%x origin 0x%x", res, origin);
    }

    uuid_buf = (uint8_t *)malloc(RAND_UUID_LEN);
    if (!uuid_buf) {
        TEEC_CloseSession(&sess);
        TEEC_FinalizeContext(&ctx);
        errx(1, "Malloc Memory Failed");
    }

    memset(&op, 0, sizeof(TEEC_Operation));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = uuid_buf;
    op.params[0].tmpref.size = RAND_UUID_LEN;

    printf("cmdID=%u\n", GEN_MY_RANDOM);
    printf("paramTypes=%u\n", op.paramTypes);
    printf("buffer=%p, size=%lu\n", op.params[0].tmpref.buffer, op.params[0].tmpref.size);

    res = TEEC_InvokeCommand(&sess, GEN_MY_RANDOM, &op, &origin);
    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, origin);
    }
    
    for (i = 0; i < RAND_UUID_LEN; ++i) {
        printf("0x%02x ", uuid_buf[i]);
        if ((i+1) % 8 == 0) {
            printf("\n");
        }
    }
    printf("\n");

    if (uuid_buf) {
        free(uuid_buf);
        uuid_buf = NULL;
    }
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);

    return 0;
}