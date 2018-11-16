#include "utils/image.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>
/*
#include "benchmark/benchmark.h"

static void BM_ext_strrchr(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext_strrchr(fn));
        benchmark::ClobberMemory();
    }
}

static void BM_ext_strrchr2(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext_strrchr2(fn));
        benchmark::ClobberMemory();
    }
}

static void BM_ext_svflo(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext_svflo(fn));
        benchmark::ClobberMemory();
    }
}

static void BM_ext_svrfind(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext_svrfind(fn));
        benchmark::ClobberMemory();
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    benchmark::RegisterBenchmark("STRRCHR", &BM_ext_strrchr, argv[1]);
    benchmark::RegisterBenchmark("STRRCHR2", &BM_ext_strrchr2, argv[1]);
    benchmark::RegisterBenchmark("SV FLO", &BM_ext_svflo, argv[1]);
    benchmark::RegisterBenchmark("SV RFIND", &BM_ext_svrfind, argv[1]);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}
*/

int main(int argc, char *argv[]) {

    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }

    const auto finfo = utils::get_file_info(argv[1]);



    const utils::file_ext ext[] = {
        utils::get_file_ext_strrchr(argv[1]),
        utils::get_file_ext_strrchr2(argv[1]),
        utils::get_file_ext_svflo(argv[1]),
        utils::get_file_ext_svrfind(argv[1]),
        utils::get_file_ext_manual(argv[1])
    };

    std::cout << argv[1] << '\n';
    std::cout << "  Info: " << finfo << '\n';
    for (auto e : ext) {
        std::cout << "  Extension: " << e << '\n';
    }

    return 0;

}
