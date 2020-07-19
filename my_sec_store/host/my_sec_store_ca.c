#include <stdio.h>
#include <string.h>

#include <tee_client_api.h>

#include <my_sec_store_ta.h>

int main (void)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Context ctx = {0};
    TEEC_Session sess = {0};
    TEEC_Operation op = {0};
    TEEC_UUID uuid = TA_MY_SEC_STORE_UUID;
    uint32_t origin = 0;

    long x = 5, y = 2, r = 0;

    ret = TEEC_InitializeContext(NULL, &ctx);
    if (ret) {
        fprintf(stderr, "TEEC_InitializeContext failed with code 0x%x\n", ret);
        goto L1;
    }

    ret = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_OpenSession failed with code 0x%x origin 0x%x\n", ret, origin);
        goto L2;
    }

    ret = TEEC_InvokeCommand(&sess, TA_CMD_TEST, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", ret, origin);
        goto L3;
    }

L3:
    TEEC_CloseSession(&sess);
L2:
    TEEC_FinalizeContext(&ctx);
L1:
    return ret;
}

