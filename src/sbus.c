#include "sbus.h"

#define SBUS_HEADER       0x0f
#define SBUS_FOOTER       0x00
#define SBUS_CHANNEL_MASK (0x7FF)

bool sbus_parse(uint8_t bytes[25], SBUSPacket *pkt)
{
    if (bytes[0] != SBUS_HEADER || bytes[24] != SBUS_FOOTER) {
        return false;
    }

    /* Parse channels, 11 bits each. */
    pkt->channels[0]  = ((bytes[1]    | bytes[2] << 8) & SBUS_CHANNEL_MASK);
    pkt->channels[1]  = ((bytes[2] >> 3 | bytes[3] << 5) & SBUS_CHANNEL_MASK);
    pkt->channels[2]  = ((bytes[3] >> 6 | bytes[4] << 2 | bytes[5] << 10) & SBUS_CHANNEL_MASK);
    pkt->channels[3]  = ((bytes[5] >> 1 | bytes[6] << 7) & SBUS_CHANNEL_MASK);
    pkt->channels[4]  = ((bytes[6] >> 4 | bytes[7] << 4) & SBUS_CHANNEL_MASK);
    pkt->channels[5]  = ((bytes[7] >> 7 | bytes[8] << 1 | bytes[9] << 9) & SBUS_CHANNEL_MASK);
    pkt->channels[6]  = ((bytes[9] >> 2 | bytes[10] << 6) & SBUS_CHANNEL_MASK);
    pkt->channels[7]  = ((bytes[10] >> 5 | bytes[11] << 3) & SBUS_CHANNEL_MASK);
    pkt->channels[8]  = ((bytes[12]   | bytes[13] << 8) & SBUS_CHANNEL_MASK);
    pkt->channels[9]  = ((bytes[13] >> 3 | bytes[14] << 5) & SBUS_CHANNEL_MASK);
    pkt->channels[10] = ((bytes[14] >> 6 | bytes[15] << 2 | bytes[16] << 10) & SBUS_CHANNEL_MASK);
    pkt->channels[11] = ((bytes[16] >> 1 | bytes[17] << 7) & SBUS_CHANNEL_MASK);
    pkt->channels[12] = ((bytes[17] >> 4 | bytes[18] << 4) & SBUS_CHANNEL_MASK);
    pkt->channels[13] = ((bytes[18] >> 7 | bytes[19] << 1 | bytes[20] << 9) & SBUS_CHANNEL_MASK);
    pkt->channels[14] = ((bytes[20] >> 2 | bytes[21] << 6) & SBUS_CHANNEL_MASK);
    pkt->channels[15] = ((bytes[21] >> 5 | bytes[22] << 3) & SBUS_CHANNEL_MASK);

    /* Parse last byte containing aux informations */
    pkt->channel16 = (bytes[23] & 0x80) == 0x80;
    pkt->channel17 = (bytes[23] & 0x40) == 0x40;
    pkt->frame_lost = (bytes[23] & 0x20) == 0x20;
    pkt->failsafe = (bytes[23] & 0x10) == 0x10;
    return true;
}

