#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

#include <my_test_ta.h> /* To get the TA_YL_UUID define */

#define TA_UUID				TA_MY_TEST_UUID

#define TA_FLAGS			TA_FLAG_EXEC_DDR

#define TA_STACK_SIZE			(2 * 1024)

#define TA_DATA_SIZE			(32 * 1024)

#define TA_VERSION	"1.0"

#define TA_DESCRIPTION	"My Test Internal Trusted Application"

#define TA_CURRENT_TA_EXT_PROPERTIES \
    { "org.linaro.optee.examples.my.test.property1", \
	USER_TA_PROP_TYPE_STRING, \
        "YL INT TA" }, \
    { "org.linaro.optee.examples.my.test.property2", \
	USER_TA_PROP_TYPE_U32, &(const uint32_t){ 0x0010 } }

#endif
