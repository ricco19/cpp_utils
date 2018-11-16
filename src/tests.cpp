#include "utils/image.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>

#include "benchmark/benchmark.h"

static void BM_ext(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext(fn));
        benchmark::ClobberMemory();
    }
}

static void BM_extce(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_extce(fn));
        benchmark::ClobberMemory();
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    benchmark::RegisterBenchmark("STRRCHR", &BM_ext, argv[1]);
    benchmark::RegisterBenchmark("SV CE", &BM_extce, argv[1]);
    benchmark::RegisterBenchmark("STRRCHR 2", &BM_ext, argv[1]);
    benchmark::RegisterBenchmark("SV CE 2", &BM_extce, argv[1]);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}

/*
int main(int argc, char *argv[]) {

    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }

    const auto finfo = utils::get_file_info(argv[1]);
    const auto ext = utils::get_file_ext(argv[1]);
    const auto extce = utils::get_file_extce(argv[1]);

    std::cout << argv[1] << '\n';
    std::cout << "  Info: " << finfo << '\n';
    std::cout << "  Extension: " << ext << '\n';
    std::cout << "  Extension: " << extce << '\n';

    return 0;

}
*/
