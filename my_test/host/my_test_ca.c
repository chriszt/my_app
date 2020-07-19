#include <stdio.h>
#include <string.h>

#include <tee_client_api.h>

#include <my_test_ta.h>

int main (void)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Context ctx = {0};
    TEEC_Session sess = {0};
    TEEC_Operation op = {0};
    TEEC_UUID uuid = TA_MY_TEST_UUID;
    uint32_t origin = 0;

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

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = 2;
    op.params[0].value.b = 1;
    fprintf(stdout, "%u + %u = ?\n", op.params[0].value.a, op.params[0].value.b);

    ret = TEEC_InvokeCommand(&sess, TA_MY_TEST_CMD_INC, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", ret, origin);
        goto L3;
    }
    fprintf(stdout, "Result = %u\n", op.params[0].value.a);

    op.params[0].value.a = 2;
    op.params[0].value.b = 1;
    fprintf(stdout, "%u - %u = ?\n", op.params[0].value.a, op.params[0].value.b);
    ret = TEEC_InvokeCommand(&sess, TA_MY_TEST_CMD_DEC, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", ret, origin);
        goto L3;
    }
    fprintf(stdout, "Result = %u\n", op.params[0].value.a);

L3:
    TEEC_CloseSession(&sess);
L2:
    TEEC_FinalizeContext(&ctx);
L1:
    return ret;
}

