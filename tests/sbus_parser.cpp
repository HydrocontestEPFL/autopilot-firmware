#include <CppUTest/TestHarness.h>
#include <CppUTestExt/MockSupport.h>

#include "sbus.h"

TEST_GROUP(SBUSParser)
{
    uint8_t bytes[25];
    SBUSPacket packet;

    void setup(void)
    {
        // Minimum sbus bytes is just a header (0x0f) and all zeroes
        bytes[0] = 0x0f;

        for (int i = 1; i < 25; i++) {
            bytes[i] = 0;
        }
    }

};

TEST(SBUSParser, CanParseBasicPacket)
{
    auto res = sbus_parse(bytes, &packet);
    CHECK_TRUE(res);
}

TEST(SBUSParser, InvalidHeader)
{
    bytes[0] = 0x42; // invalid header
    auto res = sbus_parse(bytes, &packet);
    CHECK_FALSE(res);
}

TEST(SBUSParser, InvalidFooter)
{
    bytes[24] = 0x42; // invalid footer
    auto res = sbus_parse(bytes, &packet);
    CHECK_FALSE(res);
}

TEST(SBUSParser, ParseDigitalChannel16)
{
    bytes[23] = 0x80; // channel 16 is enabled
    sbus_parse(bytes, &packet);
    CHECK_TRUE(packet.channel16);
}

TEST(SBUSParser, ParseDigitalChannel17)
{
    bytes[23] = 0x40; // channel 17 is enabled
    sbus_parse(bytes, &packet);
    CHECK_TRUE(packet.channel17);
}

TEST(SBUSParser, ParseFrameLost)
{
    bytes[23] = 0x20; // Frame lost
    sbus_parse(bytes, &packet);
    CHECK_TRUE(packet.frame_lost);
}

TEST(SBUSParser, ParseChannel0)
{
    bytes[1] = 0xFF;
    bytes[2] = 0x7;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[0]);
}

TEST(SBUSParser, ParseChannel1)
{
    bytes[2] = 0xF8;
    bytes[3] = 0x3F;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[1]);
}

TEST(SBUSParser, ParseChannel2)
{
    bytes[3] = 0xC0;
    bytes[4] = 0xFF;
    bytes[5] = 0x1;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[2]);
}

TEST(SBUSParser, ParseChannel3)
{
    bytes[5] = 0xFE;
    bytes[6] = 0xF;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[3]);
}

TEST(SBUSParser, ParseChannel4)
{
    bytes[6] = 0xF0;
    bytes[7] = 0x7F;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[4]);
}

TEST(SBUSParser, ParseChannel5)
{
    bytes[7] = 0x80;
    bytes[8] = 0xFF;
    bytes[9] = 0x3;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[5]);
}

TEST(SBUSParser, ParseChannel6)
{
    bytes[9] = 0xFC;
    bytes[10] = 0x1F;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[6]);
}

TEST(SBUSParser, ParseChannel7)
{
    bytes[10] = 0xE0;
    bytes[11] = 0xFF;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[7]);
}

TEST(SBUSParser, ParseChannel8)
{
    bytes[12] = 0xFF;
    bytes[13] = 0x7;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[8]);
}

TEST(SBUSParser, ParseChannel9)
{
    bytes[13] = 0xF8;
    bytes[14] = 0x3F;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[9]);
}

TEST(SBUSParser, ParseChannel10)
{
    bytes[14] = 0xC0;
    bytes[15] = 0xFF;
    bytes[16] = 0x1;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[10]);
}

TEST(SBUSParser, ParseChannel11)
{
    bytes[16] = 0xFE;
    bytes[17] = 0xF;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[11]);
}

TEST(SBUSParser, ParseChannel12)
{
    bytes[17] = 0xF0;
    bytes[18] = 0x7F;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[12]);
}

TEST(SBUSParser, ParseChannel13)
{
    bytes[18] = 0x80;
    bytes[19] = 0xFF;
    bytes[20] = 0x3;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[13]);
}

TEST(SBUSParser, ParseChannel14)
{
    bytes[20] = 0xFC;
    bytes[21] = 0x1F;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[14]);
}

TEST(SBUSParser, ParseChannel15)
{
    bytes[21] = 0xE0;
    bytes[22] = 0xFF;

    sbus_parse(bytes, &packet);
    CHECK_EQUAL(2047, packet.channels[15]);
}
