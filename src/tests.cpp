#include "utils/image.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>
/*
#include "benchmark/benchmark.h"

static void BM_ext(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext(fn));
        benchmark::ClobberMemory();
    }
}

static void BM_ext_noi(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext_noi(fn));
        benchmark::ClobberMemory();
    }
}

static void BM_ext_2(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext_2(fn));
        benchmark::ClobberMemory();
    }
}

static void BM_ext_3(benchmark::State &s, const char *fn) {
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::get_file_ext_3(fn));
        benchmark::ClobberMemory();
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    benchmark::RegisterBenchmark("GETEXT", &BM_ext, argv[1]);
    benchmark::RegisterBenchmark("NO INLINE", &BM_ext_noi, argv[1]);
    benchmark::RegisterBenchmark("V 2", &BM_ext_2, argv[1]);
    benchmark::RegisterBenchmark("V 3", &BM_ext_3, argv[1]);
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



    const auto ext = utils::get_file_ext(argv[1]);


    std::cout << argv[1] << '\n';
    std::cout << "  Info: " << file_info_str(finfo) << '\n';
    std::cout << "  Extension: " << file_ext_str(ext) << '\n';


    auto jpeg = utils::jpeg(argv[1]);
    if (jpeg.is_jpeg()) {
        std::cout << "  JPEG Info:\n";
        std::cout << "    Width = " << jpeg.width() << '\n';
        std::cout << "    Height = " << jpeg.height() << '\n';
        std::cout << "    Sub-sampling = " << jpeg.subsamp() << '\n';
        std::cout << "    Color Space = " << jpeg.colorspace() << '\n';
    }



    return 0;

}
