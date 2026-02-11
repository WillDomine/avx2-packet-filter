#pragma once    
#include <cstdint>
#include <new>

//Structure on 32 byte line allowing for 2 Packets per cache line. 
struct alignas(32) Packet {
    //4 Byte [Admin:1][Op:7][Seq:16][Flags:8]
    uint32_t header;

    //This is just test data for example in a game server.
    float x,y,z;
    uint8_t data[16];
};

const uint32_t MASK_ADMIN = 0x80000000;
const uint32_t MASK_OPCODE = 0x7F000000;