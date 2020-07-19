#include <stdio.h>
#include <string.h>

#include <tee_client_api.h>

#include <my_add_lib_ta.h>
#include <algo.h>

int main (void)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Context ctx = {0};
    TEEC_Session sess = {0};
    TEEC_Operation op = {0};
    TEEC_UUID uuid = TA_MY_ADD_LIB_UUID;
    uint32_t origin = 0;
    long x = 5, y = 2, r = 0;

    r = add(x, y);
    printf("[CA] %ld + %ld = %ld\n", x, y, r);

    r = sub(x, y);
    printf("[CA] %ld - %ld = %ld\n", x, y, r);

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
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
                                     TEEC_VALUE_OUTPUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].value.a = x;
    op.params[0].value.b = y;

    ret = TEEC_InvokeCommand(&sess, TA_CMD_ADD, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", ret, origin);
        goto L3;
    }
    printf("[TA] %ld + %ld = %ld\n", x, y, (long)op.params[1].value.a);

    ret = TEEC_InvokeCommand(&sess, TA_CMD_ADD, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x\n", ret, origin);
        goto L3;
    }
    printf("[TA] %ld - %ld = %ld\n", x, y, (long)op.params[1].value.a);
    

L3:
    TEEC_CloseSession(&sess);
L2:
    TEEC_FinalizeContext(&ctx);
L1:
    return ret;
}

