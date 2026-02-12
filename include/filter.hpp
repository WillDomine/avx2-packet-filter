#pragma once
#include "../include/packet.hpp"
#include <immintrin.h>
#include <vector>
#include <iostream>

//Filters the packets in batches of 8 using AVX2
inline uint8_t filter_batch_8_avx2(Packet* packets) {

    //Precalc the index for 32-byte packets we are looking for header for the flags
    const __m256i INDICIES = _mm256_setr_epi32(0,8,16,24,32,40,48,56);

    //Admin Bit Mask for all 8 slots
    const __m256i VEC_MASK_ADMIN = _mm256_set1_epi32(MASK_ADMIN);

    //Gather the 8 packets
    __m256i headers = _mm256_i32gather_epi32(
        (int const*)packets,
        INDICIES,
        4
    );

    //Check if the admin bit is set. This does the bitwise
    __m256i admin_bits = _mm256_and_si256(headers, VEC_MASK_ADMIN);

    //Compare against the mask returns the boolean
    __m256i results = _mm256_cmpeq_epi32(admin_bits, VEC_MASK_ADMIN);

    //Compress into a 8 bit mask for the results
    int mask = _mm256_movemask_ps(_mm256_castsi256_ps(results));

    return (uint8_t)mask;
};