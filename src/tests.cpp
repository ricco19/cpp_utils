// #include "natcmp.h"
// #include "benchmark/benchmark.h"
// #include <algorithm>
// #include <cstring>
// #include <iostream>
// #include <string>
// #include <vector>

// static void BM_baseline(benchmark::State &s) {
//     for (auto _ : s) {
//         std::vector<std::string> list{{"2string"}, {"3string"}, {"20string"},
//                                       {"st2ring"}, {"st3ring"}, {"st20ring"},
//                                       {"string2"}, {"string3"},
//                                       {"string20"}};
//         __asm volatile("" : : : "memory");
//     }
// }

// static void BM_defsort(benchmark::State &s) {
//     for (auto _ : s) {
//         std::vector<std::string> list{{"2string"}, {"3string"}, {"20string"},
//                                       {"st2ring"}, {"st3ring"}, {"st20ring"},
//                                       {"string2"}, {"string3"},
//                                       {"string20"}};
//         std::sort(list.begin(), list.end());
//         __asm volatile("" : : : "memory");
//     }
// }

// static void BM_strcmp(benchmark::State &s) {
//     for (auto _ : s) {
//         std::vector<std::string> list{{"2string"}, {"3string"}, {"20string"},
//                                       {"st2ring"}, {"st3ring"}, {"st20ring"},
//                                       {"string2"}, {"string3"},
//                                       {"string20"}};
//         std::sort(list.begin(), list.end(),
//                   [](const std::string &lhs, const std::string &rhs) {
//                       return strcmp(&lhs[0], &rhs[0]) < 0;
//                   });
//         __asm volatile("" : : : "memory");
//     }
// }
// #ifdef _WIN32
// static void BM_natcmp(benchmark::State &s) {
//     for (auto _ : s) {
//         std::vector<std::wstring> list{{L"2string"}, {L"3string"},
//         {L"20string"},
//                                       {L"st2ring"}, {L"st3ring"},
//                                       {L"st20ring"}, {L"string2"},
//                                       {L"string3"}, {L"string20"}};
//         std::sort(list.begin(), list.end(),
//                   [](const std::wstring &lhs, const std::wstring &rhs) {
//                       return natcmp::natcmp(lhs, rhs) < 0;
//                   });
//         __asm volatile("" : : : "memory");
//     }
// }
// #else
// static void BM_natcmp(benchmark::State &s) {
//     for (auto _ : s) {
//         std::vector<std::string> list{{"2string"}, {"3string"}, {"20string"},
//                                       {"st2ring"}, {"st3ring"}, {"st20ring"},
//                                       {"string2"}, {"string3"},
//                                       {"string20"}};
//         std::sort(list.begin(), list.end(),
//                   [](const std::string &lhs, const std::string &rhs) {
//                       return natcmp::natcmp(lhs, rhs) < 0;
//                   });
//         __asm volatile("" : : : "memory");
//     }
// }
// #endif
// BENCHMARK(BM_baseline)->ReportAggregatesOnly(true);
// BENCHMARK(BM_defsort)->ReportAggregatesOnly(true);
// BENCHMARK(BM_strcmp)->ReportAggregatesOnly(true);
// BENCHMARK(BM_natcmp)->ReportAggregatesOnly(true);

// BENCHMARK_MAIN();

#include "natcmp.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> list{
        {"2string"},     {"2string"},
        {"    3string"}, {"20string    "},
        {"st2ring"},     {"st3ring"},
        {"st20ring"},    {"?????????stdddr h gfh hingfds2"},
        {"string3"},     {"str  in g 20"}};

    std::cout << "sort() with natcmp\n";
    std::sort(list.begin(), list.end(),
              [](const std::string &lhs, const std::string &rhs) {
                  return natcmp::natcmp(lhs, rhs) < 0;
              });

    for (const auto &s : list) {
        std::cout << s << '\n';
    }
    std::cout << std::endl;

    return 0;
}
