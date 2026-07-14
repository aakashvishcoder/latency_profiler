#pragma once
#include <chrono>
#include <cstdint>


namespace profiler{

using Clock= std::chrono::steady_clock;
using TimePoint = Clock::time_point;
using Nanoseconds= std::chrono::nanoseconds;
using Microseconds=std::chrono::microseconds;

class Timestamp{
public:
    __attribute__((always_inline))
    static TimePoint now() noexcept{
        return Clock::now();
    }
    __attribute__((always_inline))
    static int64_t elapsed_ns(TimePoint start, TimePoint end) noexcept {
        return std::chrono::duration_cast<Nanoseconds>(end-start).count();
    }
};

class ScopedTimer {
public:
    explicit ScopedTimer(TimePoint& out_start) noexcept
        : start_(Timestamp::now()), out_(out_start) {}
    
    TimePoint start() noexcept {return start_;}
private:
    TimePoint start_;
    TimePoint& out_;
};
};