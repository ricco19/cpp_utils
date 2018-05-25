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

// BENCHMARK(BM_rngstr)->RangeMultiplier(4)->Range(4, 4<<10)->ReportAggregatesOnly(true);
// BENCHMARK(BM_rngstr2)->RangeMultiplier(4)->Range(4, 4<<10)->ReportAggregatesOnly(true);
// BENCHMARK(BM_rngstr3)->RangeMultiplier(4)->Range(4, 4<<10)->ReportAggregatesOnly(true);
// BENCHMARK(BM_strcr)->RangeMultiplier(4)->Range(4, 4<<10)->ReportAggregatesOnly(true);

// BENCHMARK_MAIN();

#include "quickrng.h"
#include "natcmp.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {

    for (int loops = 0; loops <= 1024; ++loops) {
        std::vector<std::string> list;
        const int sz = quickrng::rng_int(128, 2048);
        list.reserve(sz);
        for (int i = 0; i < sz; ++i)
            list.push_back(quickrng::rng_str(quickrng::rng_int(16, 1024)));
        __asm volatile("" : : : "memory");
            std::sort(list.begin(), list.end(),
              [](const std::string &lhs, const std::string &rhs) {
                  return natcmp::natcmp(lhs, rhs) < 0;
              });
        __asm volatile("" : : : "memory");

    }

    // std::vector<std::string> list {
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()},
    //     {quickrng::rng_str()}
    // };

    // std::cout << "sort() with natcmp\n";
    // std::sort(list.begin(), list.end(),
    //           [](const std::string &lhs, const std::string &rhs) {
    //               return natcmp::natcmp(lhs, rhs) < 0;
    //           });

    // for (const auto &s : list) {
    //     std::cout << s << '\n';
    // }
    // std::cout << std::endl;

    // for (int i = 0; i < 10; ++i) {
    //     std::cout << quickrng::rng_int() << '\n';
    // }




    return 0;
}
