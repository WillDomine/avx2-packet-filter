#include <liburing.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <bitset>
#include "../include/packet.hpp"
#include "../include/filter.hpp"

#define PORT 9000
#define QUEUE_DEPTH 256
#define BUFFER_SIZE 1024

int main() {
    size_t batch_count = 8;
    size_t total_size = sizeof(Packet) * batch_count;
    
    // Note: C++17/23 aligned_alloc. 
    void* mem = std::aligned_alloc(32, total_size);
    if (!mem) {
        std::cerr << "Memory allocation failed!" << std::endl;
        return 1;
    }
    
    Packet* packet_batch = static_cast<Packet*>(mem);

    // 2. Generate Data (Simulate Network Traffic)
    std::cout << "--- Generating 8 Packets ---" << std::endl;
    for(int i=0; i < 8; i++) {
        // Clear header
        packet_batch[i].header = 0; 
        
        // Mark Packets 2 and 5 as "Malicious" (Admin Bit Set)
        if (i == 2 || i == 5) {
            packet_batch[i].header |= MASK_ADMIN;
            std::cout << "Packet " << i << ": [BAD]  Admin Bit Set" << std::endl;
        } else {
            // Set some random sequence number (lower bits)
            packet_batch[i].header |= (i + 100); 
            std::cout << "Packet " << i << ": [GOOD] Normal Player" << std::endl;
        }
    }

    // 3. The "Hot Path" - Run the SIMD Filter
    std::cout << "\n--- Executing AVX2 Filter (Single Cycle) ---" << std::endl;
    
    // This function call replaces a loop of 8 'if' statements
    uint8_t drop_mask = filter_batch_8_avx2(packet_batch);

    // 4. Process Results
    std::cout << "Result Bitmask: " << std::bitset<8>(drop_mask) << " (Binary)" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    if (drop_mask == 0) {
        std::cout << "Fast Path: All packets valid. No further checks needed." << std::endl;
    } else {
        // We found bad packets. Now we iterate the mask (cheap bitwise logic)
        // to find *which* ones to drop.
        for(int i=0; i<8; i++) {
            // Check bit 'i'
            bool drop = (drop_mask >> i) & 1;
            
            if (drop) {
                std::cout << "Action: DROP Packet " << i << " (Caught by SIMD)" << std::endl;
            } else {
                std::cout << "Action: KEEP Packet " << i << std::endl;
            }
        }
    }

    // Cleanup
    std::free(mem);
    return 0;
}