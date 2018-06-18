//
// Created by philipp on 24.05.18.
//

#ifndef A2_TESTFRAMEWORK_H
#define A2_TESTFRAMEWORK_H

#include "stdio.h"
#include "stdlib.h"
#include <string.h>

#define TEST_INIT \
    char * currTestName;

#define TEST_SUCCESS \
    printf("%sTest '%s' passed!\n", "\x1B[32m", currTestName);

#define TEST_FAILURE \
    printf("%sTest '%s' failed!\nFile: %s\nLine: %d\n", "\x1B[31m", currTestName, __FILE__, __LINE__); \
    exit(-1);

#define TEST_BEGIN(testName) \
    currTestName = testName;

#define TEST_END \
    TEST_SUCCESS;

#define TEST_EQ(a, b) \
    if (a != b) { TEST_FAILURE; }

#define TEST_NEQ(a, b) \
    if (a == b) { TEST_FAILURE; }

#define TEST_STREQ(a, b) \
    if (strcmp(a, b) != 0) { TEST_FAILURE; }

#define TEST_STRNEQ(a, b) \
    if (strcmp(a, b) == 0) { TEST_FAILURE; }

#define TEST_TRUE(a) \
    if (!a) { TEST_FAILURE; }

#define TEST_FALSE(a) \
    if (a) { TEST_FAILURE; }



#endif //A2_TESTFRAMEWORK_H
