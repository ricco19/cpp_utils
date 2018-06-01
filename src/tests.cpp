// #include "natcmp.h"
// #include "benchmark/benchmark.h"
// #include "quickrng.h"
// #include <algorithm>
// #include <cstring>
// #include <iostream>
// #include <string>
// #include <vector>

// static void BM_strcr(benchmark::State &s) {
//     for (auto _ : s) {
//         std::string str{};
//         str.reserve(s.range(0));
//         for (int i = 0; i < s.range(0); ++i)
//             str.push_back('a');
//         __asm volatile("" : : : "memory");
//     }
//     s.SetLabel(std::to_string(s.range(0)));
// }

// static void BM_rngstr(benchmark::State &s) {
//     for (auto _ : s) {
//         benchmark::DoNotOptimize(quickrng::rng_str(s.range(0)));
//     }
//     s.SetLabel(std::to_string(s.range(0)));
// }

// static void BM_rngstr2(benchmark::State &s) {
//     for (auto _ : s) {
//         benchmark::DoNotOptimize(quickrng::rng_str2(s.range(0)));
//     }
//     s.SetLabel(std::to_string(s.range(0)));
// }
// static void BM_rngstr3(benchmark::State &s) {
//     for (auto _ : s) {
//         benchmark::DoNotOptimize(quickrng::rng_str3(s.range(0)));
//     }
//     s.SetLabel(std::to_string(s.range(0)));
// }

// BENCHMARK(BM_rngstr)->RangeMultiplier(4)->Range(4,
// 4<<10)->ReportAggregatesOnly(true);
// BENCHMARK(BM_rngstr2)->RangeMultiplier(4)->Range(4,
// 4<<10)->ReportAggregatesOnly(true);
// BENCHMARK(BM_rngstr3)->RangeMultiplier(4)->Range(4,
// 4<<10)->ReportAggregatesOnly(true);
// BENCHMARK(BM_strcr)->RangeMultiplier(4)->Range(4,
// 4<<10)->ReportAggregatesOnly(true);

// BENCHMARK_MAIN();

#include "natcmp.h"
#include "quickrng.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

class stopwatch {
    using timepoint_t = std::chrono::high_resolution_clock::time_point;
    using ns_t = std::chrono::nanoseconds;

  private:
    timepoint_t start_ = timepoint_t();
    timepoint_t stop_ = timepoint_t();
    ns_t delta_ = std::chrono::nanoseconds(0);
    bool was_stopped_ = false;

  public:
    inline void start() noexcept {
        was_stopped_ = false;
        start_ = std::chrono::high_resolution_clock::now();
    }
    inline void stop() noexcept {
        stop_ = std::chrono::high_resolution_clock::now();
        was_stopped_ = true;
        delta_ = std::chrono::duration_cast<ns_t>(stop_ - start_);
    }
    inline ns_t delta() const noexcept {
        if (!was_stopped_) {
            return std::chrono::nanoseconds(0);
        }
        return delta_;
    }
};

int main() {
    for (int loops = 0; loops <= 1024; ++loops) {
        std::vector<std::string> list;
        const int sz = utils::rng_int(128, 2048);
        list.reserve(sz);
        for (int i = 0; i < sz; ++i)
            list.push_back(utils::rng_str(utils::rng_int(16, 1024)));
        std::sort(list.begin(), list.end(),
                  [](const std::string &lhs, const std::string &rhs) {
                      return utils::natcmp(lhs, rhs) < 0;
                  });
    }

    return 0;
}
