#pragma once
#include <cstdint>
#include <cstring>
#include <algorithm>

namespace profiler{

class LogHistogram{
public:
    LogHistogram() noexcept { reset(); }

    __attribute__((always_inline))
    void record(int64_t value_ns) noexcept {
        if (value_ns<= 0) {
            buckets_[0]++;
            return;
        }

        const int bucket= 63- __builtin_clzll(static_cast<uint64_t>(value_ns));
        buckets_[bucket]++;
        total_count_++;
    }

    __attribute__((always_inline))
    int64_t percentile(double p) const noexcept {
        if(total_count_ ==0) return 0;

        const uint64_t target= static_cast<uint64_t>((p /100.0)* total_count_);
        uint64_t cumulative=0;

        for(int i =0; i < 64; ++i) {
            cumulative+= buckets_[i];
            if (cumulative >= target) {
                return (i == 0)? 0 : (1LL <<i);
            }
        }
        return (1LL << 63);
    }

    int64_t min_ns() const noexcept{ return min_ns_;}
    int64_t max_ns() const noexcept { return max_ns_; }
    uint64_t count() const noexcept{ return total_count_;}

    void reset() noexcept{
        std::memset(buckets_, 0, sizeof(buckets_));
        total_count_= 0;
        min_ns_ = INT64_MAX;
        max_ns_= INT64_MIN;
    }

private:
    uint64_t buckets_[64];
    uint64_t total_count_{0};
    int64_t min_ns_{INT64_MAX};
    int64_t max_ns_{INT64_MIN};
};
};