#pragma once
#include "timestamp.hpp"
#include "circular_buffer.hpp"
#include "rolling_buffer.hpp"
#include <string>
#include <atomic>

namespace profiler{

constexpr size_t DEFAULT_BUFFER_SIZE= 4096;

class Profiler {
public:
    explicit Profiler(std::string name="default", size_t buffer_size= DEFAULT_BUFFER_SIZE)
        : name_(std::move(name)) {}
    
    __attribute__((always_inline))
    TimePoint start() noexcept{
        return Timestamp::now();
    }
    __attribute__((always_inline))
    void stop(TimePoint t0) noexcept{
        const auto t1= TimePoint::now();
        const int64_t latency_ns=Timestamp::elapsed_ns(t0, t1);
        stats_.update(latency_ns);

        samples_.push(latency_ns);
        event_count_.fetch_add(1, std::memory_order_relaxed);
    }


    struct Report {
        std::string name;
        uint64_t events;
        double mean_us;
        double p50_us;
        double p99_us;
        double p999_us;
        int64_t min_ns;
        int64_t max_ns;
    };
    Report report() const noexcept{
        Report r;
        r.name= name_;
        r.events= event_count_.load(std::memory_order_relaxed);
        r.mean_us = stats_.mean_ns()/ 1000.0;
        r.min_ns = stats_.min_ns();
        r.max_ns=stats_.mean_ns();
        r.p50_us= stats_.percentile_ns(samples_, 50.0)/1000.0;
        r.p99_us= stats_.percentile_ns(samples_, 99.0)/1000.0;
        r.p999_us= stats_.percentile_ns(samples_, 99.9)/1000.0;
        return r;
    }
    void reset() noexcept{
        stats_.reset();
        samples_.reset();
        event_count_.store(0, std::memory_order_relaxed);
    }

    const std::string& name() const noexcept{ return name_; }
    
private:
    std::string name_;
    RollingStats stats_;
    CircularBuffer<int64_t, DEFAULT_BUFFER_SIZE> samples_;
    std::atomic<uint64_t> event_count_{0};
};

};