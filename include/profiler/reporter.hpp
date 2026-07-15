#pragma once
#include "profiler.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <atomic>
#include <functional>


namespace profiler {

class Reporter {
public:
    using Callback = std::function<void(const Profiler::Report&)>;

    explicit Reporter(std::chrono::milliseconds interval= std::chrono::milliseconds(1000))
        : interval_(interval), running_(false) {}
    
    void register_profiler(Profiler& p) {
        profilers_.push_back(&p);
    }
    void set_callback(Callback cb){ callback_= std::move(cb);}

    void start() {
        running_ = true;
        thread_= std::thread([this]() { run();});
    }

    void stop() {
        running_= false;
        if(thread_.joinable()) thread_.join();
    }

    static void print_report(const Profiler::Report& r) {
        std::cout << std::left << std::setw(16) << r.name
            << " events = " << std::setw(10) << r.events
            << " mean = " << std::setw(10) << std::fixed<< std::setprecision(2) <<r.mean_us << " µs"
            << " p50 = " << std::setw(10) <<r.p50_us << " µs"
            << " p99 = " << std::setw(10) <<r.p99_us << " µs"
            << " p99.9 = " << std::setw(10) <<r.p999_us << " µs"
            << " min = " << std::setw(10) <<r.min_ns << " ns"
            << " max = " << std::setw(10) <<r.max_ns << " ns" << "\n";
    }
private:
    void run() {
        std::this_thread::sleep_for(interval_);
        for (auto* p: profilers_) {
            auto r= p->report();
            print_report(r);
            if(callback_) callback_(r);
        }
    }

    std::chrono::milliseconds interval_;
    std::vector<Profiler*> profilers_;
    Callback callback_;
    std::thread thread_;
    std::atomic<bool> running_;
};
};