#include <stdio.h>
#include <string.h>

#include <tee_client_api.h>

#include <my_sec_store_ta.h>

#define BUF_SIZE    100

int main (void)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Context ctx = {0};
    TEEC_Session sess = {0};
    TEEC_Operation op = {0};
    TEEC_UUID uuid = TA_MY_SEC_STORE_UUID;
    uint32_t origin = 0;
    char file_name[] = "file1.txt";
    char new_file_name[] = "file2.txt";
    char write_buf[BUF_SIZE] = "Google Chrome is made possible by the Chromium open source project and other open source software.";
    char read_buf[BUF_SIZE] = {0};

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

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("TEEC_InvokeCommand(TA_CMD_TEST)...\n");

    memset(&op, 0, sizeof(op));
    ret = TEEC_InvokeCommand(&sess, TA_CMD_TEST, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_TEST) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
        goto L3;
    }
    printf("TEEC_InvokeCommand(TA_CMD_TEST) OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("TEEC_InvokeCommand(TA_CMD_CREATE)...\n");
    printf("file_name=%s, size=%lu\n", file_name, sizeof(file_name));

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = sizeof(file_name);

    ret = TEEC_InvokeCommand(&sess, TA_CMD_CREATE, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_CREATE) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
        goto L3;
    }
    printf("TEEC_InvokeCommand(TA_CMD_CREATE) OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("TEEC_InvokeCommand(TA_CMD_WRITE)...\n");
    printf("file_name=%s, size=%lu\n", file_name, sizeof(file_name));
    printf("write_buf=%s\n", write_buf);
    printf("size=%lu\n", sizeof(write_buf));

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = sizeof(file_name);
    op.params[1].tmpref.buffer = write_buf;
    op.params[1].tmpref.size = sizeof(write_buf);

    ret = TEEC_InvokeCommand(&sess, TA_CMD_WRITE, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_WRITE) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
        goto L3;
    }
    printf("TEEC_InvokeCommand(TA_CMD_WRITE) OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("TEEC_InvokeCommand(TA_CMD_READ)...\n");
    printf("file_name=%s, size=%lu\n", file_name, sizeof(file_name));
    printf("read_buf size=%d\n", BUF_SIZE);

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_OUTPUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = sizeof(file_name);
    op.params[1].tmpref.buffer = read_buf;
    op.params[1].tmpref.size = BUF_SIZE;

    ret = TEEC_InvokeCommand(&sess, TA_CMD_READ, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_READ) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
        goto L3;
    }

    printf("read_buf=%s\n", read_buf);
    printf("length=%lu\n", strlen(read_buf));
    printf("TEEC_InvokeCommand(TA_CMD_READ) OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////



L3:
    TEEC_CloseSession(&sess);
L2:
    TEEC_FinalizeContext(&ctx);
L1:
    return ret;
}

