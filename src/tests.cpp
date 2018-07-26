//#include "benchmark/benchmark.h"
#include "image/tiff.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>

/*
#ifdef _WIN32
const char *testfile = "S:/Projects/cpp_utils/docs/tiff_samples/MARBLES.TIF";
#else
const char *testfile =
    "/home/brian/Storage/Projects/cpp_utils/docs/tiff_samples/MARBLES.TIF";
#endif
const utils::bytearr_t buf{utils::file_binread(testfile)};

static void BM_sh64(benchmark::State &s) {
    for (auto _ : s) {
        utils::bytes_to_int<int64_t>(buf, 0);
        asm volatile("" : : : "memory");
    }
}

static void BM_mc64(benchmark::State &s) {
    for (auto _ : s) {
        utils::bytes_to_int_mc<int64_t>(buf, 0);
        asm volatile("" : : : "memory");
    }
}

BENCHMARK(BM_sh64)->ReportAggregatesOnly(true);
BENCHMARK(BM_mc64)->ReportAggregatesOnly(true);

BENCHMARK_MAIN();
*/

int main(int argc, char *argv[]) {

    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }

    utils::print_file_info(argv[1]);

    image::tiff_open tif(argv[1]);
    if (tif.is_tiff()) {
        std::cout << " -> dats a tiff\n";
    } else {
        std::cout << " -> not a tiff bro\n";
        return 0;
    }

    for (const auto ent : tif.ifd.entry) {
        std::cout << ent.tag << '\n';
    }

    return 0;
}
