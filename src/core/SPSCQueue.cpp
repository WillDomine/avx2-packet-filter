// Single-Producer Single-Consumer Lock-Free Queue Implementation
// Will Domine
#include <atomic>
#include <optional>
#include <vector>

template<typename T, size_t Size>
class SPSCQueue {
    static_assert((Size & (Size - 1)) == 0, "Size must be a power of 2");
    public:
        // Constructor initializes the buffer and sets head and tail to 0
        SPSCQueue() {
            buffer.resize(Size);
        }

        // Call to get a slot for writing, returns nullptr if the queue is full
        T* get_write_slot() {
            size_t tail = tail.load(std::memory_order_relaxed);
            size_t head = head.load(std::memory_order_acquire);
            if ((tail - head) >= Size) {
                return nullptr; // Queue is full
            }
            return &buffer[tail & Mask];
        }
        // Call after writing to the slot returned by get_write_slot
        void advance_tail() {
            size_t tail = tail.load(std::memory_order_relaxed);
            tail.store(tail + 1, std::memory_order_release);
        }

        // Call to get a slot for reading, returns nullptr if the queue is empty
        T* peek_read_slot() {
            size_t head = head.load(std::memory_order_relaxed);
            size_t tail = tail.load(std::memory_order_acquire);
            if (head == tail) {
                return nullptr; // Queue is empty
            }
            return &buffer[head & Mask];
        }
        // Call after reading from the slot returned by peek_read_slot
        void advance_head() {
            size_t head = head.load(std::memory_order_relaxed);
            head.store(head + 1, std::memory_order_release);
        }


    private:
        // Mask for wrapping indices, basically used like modulo but faster bitwise
        static constexpr size_t Mask = Size - 1;
        // The buffer for storing the elements, aligned to the cache line.
        std::vector<T> buffer;
        // Align head and tail on separate cache lines to avoid false sharing.
        alignas(64) std::atomic<size_t> head = {0};
        char padding[64]; // Padding to ensure head and tail are separated by at least one cache line
        alignas(64) std::atomic<size_t> tail = {0};
     
};