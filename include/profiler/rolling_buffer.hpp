#pragma once
#include <cstdint>
#include <algorithm>
#include <limits>
#include <cmath>
#include <array>
#include <cstddef>
namespace profiler{

class RollingStats {
public:
    RollingStats() noexcept { reset(); }

    __attribute__((always_inline))
    void update(int64_t value_ns) noexcept{
        count_++;

        if(value_ns <min_) min_ = value_ns;
        if(value_ns > max_) max_ = value_ns;

        const double delta=static_cast<double>(value_ns) - mean_;

        mean_+= delta/static_cast<double>(count_);
        const double delta2= static_cast<double>(value_ns) - mean_;
        m2_ +=delta* delta2;
    }

    uint64_t count() const noexcept { return count_; }
    double mean_ns() const noexcept {return mean_; }
    double variance_ns() const noexcept {
        return count_> 1? m2_ /static_cast<double>(count_ - 1): 0.0;
    }

    double stddev_ns() const noexcept {
        return std::sqrt(variance_ns());
    }

    int64_t min_ns() const noexcept { return min_; }
    int64_t max_ns() const noexcept { return max_; }

    template <typename Buffer>
    double percentile_ns(const Buffer& buf,double p ) const noexcept{
        if (buf.size() ==0) return 0.0;

        std::array<int64_t, Buffer::kCapacity> tmp{};
        size_t n =buf.size();
        size_t i=0;
        buf.for_each([&](int64_t v){ if (i < n) tmp[i++] = v; });
        const size_t k = std::min(
            n - 1,
            static_cast<size_t>((p / 100.0) * static_cast<double>(n - 1))
        );
        std::partial_sort(tmp.begin(), tmp.begin()+k + 1, tmp.begin()+n);
        return static_cast<double>(tmp[k]);
    }

    void reset() noexcept {
        count_=0;
        mean_ = 0.0;
        m2_=0.0;
        min_= std::numeric_limits<int64_t>::max();
        max_ = std::numeric_limits<int64_t>::min();
    }
private:
    uint64_t count_{0};
    double mean_{0.0};
    double m2_{0.0};
    int64_t min_{std::numeric_limits<int64_t>::max()};
    int64_t max_{std::numeric_limits<int64_t>::min()};

};
}