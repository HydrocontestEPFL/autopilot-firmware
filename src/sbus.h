#ifndef SBUS_H
#define SBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "messages/SBUSPacket.pb.h"

/** Parses an SBUS packet made of 25 bytes.
 *
 * @param [in] bytes The byte array, must be 25 bytes.
 * @param [out] decoded_packet Destination where the decoded value will be stored.
 *
 * @returns True if the bytes form a valid sbus packet, false otherwise.
 */
bool sbus_parse(uint8_t bytes[25], SBUSPacket *pkt);

#ifdef __cplusplus
}
#endif

#endif
