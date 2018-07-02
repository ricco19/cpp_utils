// #include "benchmark/benchmark.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>

// static void BM_freefunc(benchmark::State &s) {
//     for (auto _ : s) {
//         utils::list_t list = utils::file_to_list("../docs/random_list.txt");
//         __asm volatile("" : : : "memory");
//         utils::list_shuffle(list);
//         __asm volatile("" : : : "memory");
//         utils::list_sort_naturally(list);
//         __asm volatile("" : : : "memory");
//     }
// }

// static void BM_class(benchmark::State &s) {
//     for (auto _ : s) {
//         utils::clist list;
//         list.from_file("../docs/random_list.txt");
//         __asm volatile("" : : : "memory");
//         list.shuffle();
//         __asm volatile("" : : : "memory");
//         list.sort_naturally();
//         __asm volatile("" : : : "memory");
//     }
// }

// static void BM_freefunc_oo(benchmark::State &s) {
//     utils::list_t list = utils::file_to_list("../docs/random_list.txt");
//     for (auto _ : s) {
//         utils::list_shuffle(list);
//         __asm volatile("" : : : "memory");
//         utils::list_sort_naturally(list);
//         __asm volatile("" : : : "memory");
//     }
// }

// static void BM_class_oo(benchmark::State &s) {
//     utils::clist list;
//     list.from_file("../docs/random_list.txt");
//     for (auto _ : s) {
//         list.shuffle();
//         __asm volatile("" : : : "memory");
//         list.sort_naturally();
//         __asm volatile("" : : : "memory");
//     }
// }

// BENCHMARK(BM_freefunc)->ReportAggregatesOnly(true);
// BENCHMARK(BM_freefunc_oo)->ReportAggregatesOnly(true);
// BENCHMARK(BM_class)->ReportAggregatesOnly(true);
// BENCHMARK(BM_class_oo)->ReportAggregatesOnly(true);

// BENCHMARK_MAIN();

int main(int argc, char *argv[]) {

    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }

    utils::print_file_info(argv[1]);

    // utils::list_t list = utils::file_to_list(argv[1]);
    // utils::list_shuffle(list);
    // utils::list_sort_naturally(list);

    // utils::print_list(list);

    return 0;
}
