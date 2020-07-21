#include <stdio.h>
#include <string.h>

#include <tee_client_api.h>

#include <my_sec_store_ta.h>

#define BUF_SIZE    100

TEEC_Result init (TEEC_Context *ctx, TEEC_Session *sess);
void finish (TEEC_Context *ctx, TEEC_Session *sess);
TEEC_Result ivk_test (TEEC_Session *sess);
TEEC_Result ivk_create (TEEC_Session *sess, char *file_name, uint32_t file_name_size);
TEEC_Result ivk_write (TEEC_Session *sess, char *file_name, uint32_t file_name_size, char *write_buf);
TEEC_Result ivk_read (TEEC_Session *sess, char *file_name, uint32_t file_name_size, char *read_buf);
TEEC_Result ivk_rename (TEEC_Session *sess, char *old_file_name, uint32_t old_file_name_size,
                                            char *new_file_name, uint32_t new_file_name_size);
TEEC_Result ivk_truncate (TEEC_Session *sess, char *file_name, uint32_t file_name_size, uint32_t new_size);
TEEC_Result ivk_delete (TEEC_Session *sess, char *file_name, uint32_t file_name_size);

TEEC_Result init (TEEC_Context *ctx, TEEC_Session *sess)
{
    TEEC_Result ret = TEEC_SUCCESS;
    uint32_t origin = 0;
    TEEC_UUID uuid = TA_MY_SEC_STORE_UUID;
    

    ret = TEEC_InitializeContext(NULL, ctx);
    if (ret) {
        fprintf(stderr, "TEEC_InitializeContext failed with code 0x%x\n", ret);
        goto L1;
    }

    ret = TEEC_OpenSession(ctx, sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_OpenSession failed with code 0x%x origin 0x%x\n", ret, origin);
        goto L2;
    }

    return ret;

L2:
    TEEC_FinalizeContext(ctx);
L1:
    return ret;
}

/*
 * [NOTE] the close function can be auto run without the main function call it.
 * 
void close (TEEC_Context *ctx, TEEC_Session *sess)
{
    printf("++++++++ 1 ++++++++\n");
//    TEEC_CloseSession(sess);
//    TEEC_FinalizeContext(ctx);
}
*/

void finish (TEEC_Context *ctx, TEEC_Session *sess)
{
    TEEC_CloseSession(sess);
    TEEC_FinalizeContext(ctx);
}

TEEC_Result ivk_test (TEEC_Session *sess)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Operation op = {0};
    uint32_t origin = 0;

    ret = TEEC_InvokeCommand(sess, TA_CMD_TEST, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_TEST) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
    }

    return ret;
}

TEEC_Result ivk_create (TEEC_Session *sess, char *file_name, uint32_t file_name_size)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Operation op = {0};
    uint32_t origin = 0;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = file_name_size;

    ret = TEEC_InvokeCommand(sess, TA_CMD_CREATE, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_CREATE) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
    }

    return ret;
}

TEEC_Result ivk_write (TEEC_Session *sess, char *file_name, uint32_t file_name_size, char *write_buf)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Operation op = {0};
    uint32_t origin = 0;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = file_name_size;
    op.params[1].tmpref.buffer = write_buf;
    op.params[1].tmpref.size = strlen(write_buf) + 1;

    ret = TEEC_InvokeCommand(sess, TA_CMD_WRITE, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_WRITE) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
    }

    return ret;
}

TEEC_Result ivk_read (TEEC_Session *sess, char *file_name, uint32_t file_name_size, char *read_buf)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Operation op = {0};
    uint32_t origin = 0;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_OUTPUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = file_name_size;
    op.params[1].tmpref.buffer = read_buf;
    op.params[1].tmpref.size = BUF_SIZE;

    ret = TEEC_InvokeCommand(sess, TA_CMD_READ, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_READ) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
    }

    return ret;
}

TEEC_Result ivk_rename (TEEC_Session *sess, char *old_file_name, uint32_t old_file_name_size,
                                            char *new_file_name, uint32_t new_file_name_size)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Operation op = {0};
    uint32_t origin = 0;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = old_file_name;
    op.params[0].tmpref.size = old_file_name_size;
    op.params[1].tmpref.buffer = new_file_name;
    op.params[1].tmpref.size = new_file_name_size;

    ret = TEEC_InvokeCommand(sess, TA_CMD_RENAME, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_RENAME) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
    }

    return ret;
}

TEEC_Result ivk_truncate (TEEC_Session *sess, char *file_name, uint32_t file_name_size, uint32_t new_size)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Operation op = {0};
    uint32_t origin = 0;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_VALUE_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = file_name_size;
    op.params[1].value.a = new_size;

    ret = TEEC_InvokeCommand(sess, TA_CMD_TRUNCATE, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_TRUNCATE) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
    }

    return ret;
}

TEEC_Result ivk_delete (TEEC_Session *sess, char *file_name, uint32_t file_name_size)
{
    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Operation op = {0};
    uint32_t origin = 0;

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
                                     TEEC_NONE,
                                     TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = file_name;
    op.params[0].tmpref.size = file_name_size;

    ret = TEEC_InvokeCommand(sess, TA_CMD_DELETE, &op, &origin);
    if (ret) {
        fprintf(stderr, "TEEC_InvokeCommand(TA_CMD_DELETE) failed. code=0x%x, origin=0x%x\n",
            ret, origin);
    }

    return ret;
}

int main (void)
{
    char file_name[] = "file1.txt";
    char new_file_name[] = "file2.txt";
    char write_buf[BUF_SIZE] = "Google Chrome is made possible by the Chromium open source project and other open source software.";
    char read_buf[BUF_SIZE] = {0};

    TEEC_Result ret = TEEC_SUCCESS;
    TEEC_Context ctx = {0};
    TEEC_Session sess = {0};

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("Connect to TA...\n");
    ret = init(&ctx, &sess);
    if (ret) {
        fprintf(stderr, "Connect to TA FAILED. ret=0x%x\n", ret);
        goto L1;
    }
    printf("Connect to TA OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("\ninvoke TA_CMD_TEST...\n");
    ret = ivk_test(&sess);
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_TEST FAILED.\n");
        goto L2;
    }
    printf("invoke TA_CMD_TEST OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("\ninvoke TA_CMD_CREATE...\n");
    printf("file_name=%s, size=%lu\n", file_name, sizeof(file_name));
    ret = ivk_create(&sess, file_name, sizeof(file_name));
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_CREATE FAILED.\n");
        goto L2;
    }
    printf("invoke TA_CMD_CREATE OK\n");


    printf("\ninvoke TA_CMD_WRITE...\n");
    printf("file_name=%s, size=%lu\n", file_name, sizeof(file_name));
    printf("write_buf=%s\n", write_buf);
    printf("size=%ld\n", strlen(write_buf)+1);
    ret = ivk_write(&sess, file_name, sizeof(file_name), write_buf);
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_WRITE FAILED.\n");
        goto L2;
    }
    printf("invoke TA_CMD_WRITE OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("\ninvoke TA_CMD_READ...\n");
    printf("file_name=%s, size=%lu\n", file_name, sizeof(file_name));
    printf("read_buf size=%d\n", BUF_SIZE);
    ret = ivk_read(&sess, file_name, sizeof(file_name), read_buf);
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_READ FAILED.\n");
        goto L2;
    }
    printf("read_buf=%s\n", read_buf);
    printf("length=%lu\n", strlen(read_buf));
    printf("invoke TA_CMD_READ OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("\ninvoke TA_CMD_RENAME...\n");
    printf("file_name=%s, size=%lu\n", file_name, sizeof(file_name));
    printf("new_file_name=%s, size=%lu\n", new_file_name, sizeof(new_file_name));

    ret = ivk_rename(&sess, file_name, sizeof(file_name), new_file_name, sizeof(new_file_name));
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_RENAME FAILED.\n");
        goto L2;
    }
    printf("invoke TA_CMD_RENAME OK\n");

    printf("\ninvoke TA_CMD_READ...\n");
    printf("new_file_name=%s, size=%lu\n", new_file_name, sizeof(new_file_name));
    memset(&read_buf, 0, BUF_SIZE);
    ret = ivk_read(&sess, new_file_name, sizeof(new_file_name), read_buf);
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_READ FAILED.\n");
        goto L2;
    }
    printf("read_buf=%s\n", read_buf);
    printf("length=%lu\n", strlen(read_buf));
    printf("invoke TA_CMD_READ OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("\ninvoke TA_CMD_TRUNCATE...\n");
    printf("new_file_name=%s, size=%lu\n", new_file_name, sizeof(new_file_name));
    ret = ivk_truncate(&sess, new_file_name, sizeof(new_file_name), 10);
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_TRUNCATE FAILED.\n");
        goto L2;
    }
    printf("invoke TA_CMD_TRUNCATE OK\n");

    printf("\ninvoke TA_CMD_READ...\n");
    printf("new_file_name=%s, size=%lu\n", new_file_name, sizeof(new_file_name));
    memset(&read_buf, 0, BUF_SIZE);
    ret = ivk_read(&sess, new_file_name, sizeof(new_file_name), read_buf);
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_READ FAILED.\n");
        goto L2;
    }
    printf("read_buf=%s\n", read_buf);
    printf("length=%lu\n", strlen(read_buf));
    printf("invoke TA_CMD_READ OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

    printf("\ninvoke TA_CMD_DELETE...\n");
    printf("new_file_name=%s, size=%lu\n", new_file_name, sizeof(new_file_name));
    ret = ivk_delete(&sess, new_file_name, sizeof(new_file_name));
    if (ret) {
        fprintf(stderr, "invoke TA_CMD_DELETE FAILED.\n");
        goto L2;
    }
    printf("invoke TA_CMD_DELETE OK\n");

    //////////////////////////////////////////////////////////////////////////////////////////

L2:
    printf("\nFinish TA...\n");
    finish(&ctx, &sess);
    printf("Finish TA OK\n");
L1:
    return ret;
}

