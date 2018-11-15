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




int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    benchmark::RegisterBenchmark("BASE", &BM_ext, argv[1]);
    benchmark::RegisterBenchmark("NO INLINE", &BM_ext_noi, argv[1]);
    benchmark::RegisterBenchmark("BASE2", &BM_ext, argv[1]);
    benchmark::RegisterBenchmark("NO INLINE2", &BM_ext_noi, argv[1]);
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}
*/

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    utils::print_file_info(argv[1]);


    const auto ext = utils::get_file_ext(argv[1]);
    utils::print_file_ext(ext);




    // const auto img = image::load_image{argv[1]};
    // if (img.is_image()) {
    //     std::cout << "  This is an image!\n";
    //     std::cout << "    w: " << img.width() << ", h: " << img.height()
    //               << '\n';
    //     const auto p = img.get_pixels(image::FORMAT_GREY);
    //     std::cout << "    size: " << p.size() << '\n';
    // } else {
    //     std::cout << "  Not an image.\n";
    // }
    return 0;
}
