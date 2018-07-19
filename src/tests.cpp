//#include "benchmark/benchmark.h"
#include "image/tiff.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>

// static void BM_checkend(benchmark::State &s) {
//     for (auto _ : s) {
//         benchmark::DoNotOptimize(utils::is_big_endian());
//     }
// }

// BENCHMARK(BM_checkend)->ReportAggregatesOnly(true);

// BENCHMARK_MAIN();


int main(int argc, char *argv[]) {

    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }

    utils::print_file_info(argv[1]);

    image::tiff_open tif(argv[1]);
    if (tif.is_tiff()) {
        std::cout << " -> Loaded TIFF!" << '\n';
    } else {
        std::cout << " -> Not a TIFF." << '\n';
    }


    // utils::list_t list = utils::file_to_list(argv[1]);
    // utils::list_shuffle(list);
    // utils::list_sort_naturally(list);

    // utils::print_list(list);

    return 0;
}
