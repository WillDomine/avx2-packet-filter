// Packet block structure for network packet processing
// Will Domine
#pragma once
#include <cstdint>
#include <array>

//The maximum size of the packet data, including the header (length, timestamp, flags).
constexpr size_t MAX_PACKET_SIZE = 2048;

//Align on the cache line size to improve performance when processing packets in batches(SIMD).
struct alignas(64) PacketBlock {
    uint32_t length;
    uint32_t timestamp;
    uint32_t flags;
    uint8_t data[MAX_PACKET_SIZE - 12];
};

//Ensure packet size is set at the max packet size to avoid buffer overflows
static_assert(sizeof(PacketBlock) == MAX_PACKET_SIZE, "PacketBlock size must be equal to MAX_PACKET_SIZE");
