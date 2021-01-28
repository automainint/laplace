#include "../core/utils.h"
#include <gtest/gtest.h>

using namespace laplace;

TEST(laplace_core, utils_read_bytes)
{
    const vbyte a { 0xff, 0x10, 0x30 };

    EXPECT_EQ(rd<uint8_t>(a, 0), 0xff);
    EXPECT_EQ(rd<uint8_t>(a, 1), 0x10);
    EXPECT_EQ(rd<uint8_t>(a, 2), 0x30);

    EXPECT_EQ(rd<uint16_t>(a, 0), 0x10ff);
    EXPECT_EQ(rd<uint16_t>(a, 1), 0x3010);
}

TEST(laplace_core, utils_write_bytes)
{
    vbyte a { 0, 0, 0 };

    wr<uint8_t>(a, 0, 0xff);
    wr<uint16_t>(a, 1, 0x3010);

    EXPECT_EQ(rd<uint8_t>(a, 0), 0xff);
    EXPECT_EQ(rd<uint8_t>(a, 1), 0x10);
    EXPECT_EQ(rd<uint8_t>(a, 2), 0x30);

    EXPECT_EQ(rd<uint16_t>(a, 0), 0x10ff);
    EXPECT_EQ(rd<uint16_t>(a, 1), 0x3010);
}

TEST(laplace_core, utils_identefier)
{
    if constexpr (sizeof(size_t) == 8)
    {
        uint64_t a = 0x1020304050607080;
        uint64_t b = 0x2030405060708090;

        EXPECT_EQ(as_index(a), 0x1020304050607080);
        EXPECT_EQ(as_index(b), 0x2030405060708090);
    }

    if constexpr (sizeof(size_t) == 4)
    {
        uint64_t a = 0x10203040;
        uint64_t b = 0x20304050;

        EXPECT_EQ(as_index(a), 0x10203040);
        EXPECT_EQ(as_index(b), 0x20304050);
    }
}
