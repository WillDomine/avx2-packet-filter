# AVX2 High-Performance Packet Filter

A header-only C++ library utilizing **AVX2 SIMD intrinsics** to filter network packets with **0.76ns latency**.

![Language](https://img.shields.io/badge/C++-20-blue.svg)
![Architecture](https://img.shields.io/badge/Arch-AVX2-red.svg)
![Latency](https://img.shields.io/badge/Latency-0.7ns-brightgreen.svg)

## 🚀 Performance
Benchmarked on **Intel Core i9-14900K** (5.7 GHz) using Google Benchmark.

| Implementation | Time (ns) | Throughput (PPS) | Speedup |
| :--- | :--- | :--- | :--- |
| **Scalar (Standard C++)** | 1.86 ns | ~0.5 Billion | 1.0x |
| **AVX2 (SIMD)** | **0.76 ns** | **~1.3 Billion** | **2.44x** |

> **Key Result:** The AVX2 implementation processes a batch of 8 packets in less than a single nanosecond by leveraging the `_mm256_i32gather_epi32` instruction to minimize cache misses.

## 🛠 Architecture

### The Packet
Packets are 32-byte aligned structures. This alignment ensures that exactly two packets fit into a single 64-byte CPU cache line, minimizing memory fetches.

```cpp
struct alignas(32) Packet {
    uint32_t header; // [Admin:1][Op:7][Seq:16][Flags:8]
    float x, y, z;
    uint8_t data[16];
};
```

### Library Integration
Include the header in your C++ project.
```cpp
#include "include/filter.hpp"

// Setup batch of packets (Must be 32-byte aligned)
Packet* batch = ...; 

// Returns an 8-bit mask (1 = Drop, 0 = Keep)
uint8_t drop_mask = filter_batch_8_avx2(batch);
```

### Running Demo Server
This project includes an io_uring TCP server to demonstrate the filter in a zero-copy environment.
```bash
mkdir build && cd build
cmake ..
make

# Run the benchmark
./run_benchmarks

# Run the server (Requires sudo for IORING_SETUP_SQPOLL)
sudo ./demo_server
```