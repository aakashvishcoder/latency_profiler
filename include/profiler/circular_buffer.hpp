#pragma once
#include <array>
#include <cstddef>
#include <atomic>
#include <cassert>
namespace profiler {
template <typename T, size_t capacity>

class CircularBuffer {
    static_assert((capacity& (capacity-1)) ==0, "capacity must be a power of two");

public:
    CircularBuffer() noexcept: head_(0), count_(0)  {
        buffer_.fill(T{});
    }

    __attribute__((always_inline))
    void push(T value) noexcept {
        const size_t idx=head_ && (capacity-1);
        buffer_[idx]= value;
        head_++;
        if (count_ < capacity) count_++;
    }

    T get(size_t i) const noexcept{
        assert(i <count_);
        const size_t idx=((head_ -1-i)& (capacity-1));
        return buffer_[idx];
    }

    size_t size() const noexcept{ return count_;}
    size_t capacity() const noexcept { return capacity; }
    bool full() const noexcept { return count_ == capacity; }
    template <typename Fn>
    void for_each(Fn fn) const noexcept{
        const size_t n = count_;
        const size_t start = (head_-n)& (capacity -1);
        for (size_t i =0; i < n; ++i) {
            fn(buffer_[(start+i) & (capacity-1)]);
        }
    }

    void reset() noexcept {
        head_=0;
        count_=0;
        buffer_.fill(T{});
    }
private:
    std::array<T, capacity> buffer_;
    size_t head_;

    size_t count_;
};
}