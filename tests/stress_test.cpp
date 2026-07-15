#include "../include/profiler/profiler.hpp"
#include "../include/profiler/reporter.hpp"
#include <iostream>
#include <thread>
#include <vector>
#include <random>

using namespace profiler;

void simulate_trading_op(Profiler& prof, std::mt19937& rng) {
    auto t0 = prof.start();

    std::lognormal_distribution<double> dist(6.5, 1.2);  
    int64_t work_ns = static_cast<int64_t>(dist(rng));
    volatile int64_t sink = 0;  
    for (int64_t i = 0; i < work_ns / 10; ++i) sink += i;

    prof.stop(t0);
}

int main() {
    constexpr int NUM_THREADS   = 8;
    constexpr int EVENTS_PER_THREAD = 1'000'000;

    std::vector<Profiler> profilers;
    profilers.reserve(NUM_THREADS);
    for (int i = 0; i < NUM_THREADS; ++i)
        profilers.emplace_back("thread_" + std::to_string(i));


    Reporter reporter(std::chrono::milliseconds(500));
    for (auto& p : profilers) reporter.register_profiler(p);
    reporter.start();

    std::cout << "=== Stress Test: " << NUM_THREADS << " threads × "
              << EVENTS_PER_THREAD << " events ===\n\n";

    auto wall_start = Timestamp::now();

    std::vector<std::thread> workers;
    for (int t = 0; t < NUM_THREADS; ++t) {
        workers.emplace_back([&profilers, t]() {
            std::mt19937 rng(42 + t);
            for (int i = 0; i < EVENTS_PER_THREAD; ++i) {
                simulate_trading_op(profilers[t], rng);
            }
        });
    }

    for (auto& w : workers) w.join();

    auto wall_end = Timestamp::now();
    reporter.stop();

    double wall_us = Timestamp::elapsed_us(wall_start, wall_end);
    uint64_t total_events = NUM_THREADS * EVENTS_PER_THREAD;

    std::cout << "\n=== Results ===\n";
    std::cout << "Total events:     " << total_events << "\n";
    std::cout << "Wall time:        " << wall_us / 1e6 << " s\n";
    std::cout << "Throughput:       " << total_events / (wall_us / 1e6) / 1e6
              << " M events/sec\n";

    {
        Profiler overhead_profiler("overhead_measurement");
        constexpr int N = 10'000'000;
        auto t0 = Timestamp::now();
        for (int i = 0; i < N; ++i) {
            auto s = overhead_profiler.start();
            overhead_profiler.stop(s);
        }
        auto t1 = Timestamp::now();
        double total_ns = Timestamp::elapsed_ns(t0, t1);
        std::cout << "\nProfiler overhead:  " << total_ns / N << " ns per start/stop pair\n";
    }

    return 0;
}