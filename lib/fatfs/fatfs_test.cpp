#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

TEST_GROUP (FatFS) {
    FATFS SDC_FS;
    void setup()
    {
        const auto mode = 0; // partition disk
        const auto cluster_size = 4 * 1024;
        f_mount(&SDC_FS, "", 0);
        f_mkfs("0:", mode, cluster_size);
    }
};

TEST (FatFS, CanFormat) {
    const auto mode = 0; // partition disk
    const auto cluster_size = 4 * 1024;
    auto res = f_mkfs("0:", mode, cluster_size);
    CHECK_EQUAL_TEXT(FR_OK, res, "Can not create FatFS");
}

TEST (FatFS, CanWriteThenRead) {
    // First write to a file
    {
        const char msg[] = "hello";
        FIL file;
        unsigned written_len;

        auto res = f_open(&file, "/foo.txt", FA_WRITE | FA_CREATE_NEW);
        CHECK_EQUAL_TEXT(FR_OK, res, "Could not open file for writing");

        f_write(&file, msg, sizeof(msg), &written_len);
        CHECK_EQUAL(sizeof(msg), written_len);

        f_close(&file);
    }

    // Then read back from it
    {
        FIL file;
        char msg[6];
        auto read_len = 0u;

        auto res = f_open(&file, "/foo.txt", FA_READ);
        CHECK_EQUAL_TEXT(FR_OK, res, "Could not open file for reading");

        f_read(&file, msg, sizeof(msg), &read_len);
        f_close(&file);

        CHECK_EQUAL(sizeof(msg), read_len);
        STRCMP_EQUAL("hello", msg);
    }
}
