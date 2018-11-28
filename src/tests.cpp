#include "utils/image.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>

#include "benchmark/benchmark.h"

static void BM_baseline(benchmark::State &s, const char *fn) {
    const auto jpeg = utils::jpeg(fn);
    const auto p = jpeg.get_pixels();
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::pc_rgb_to_gray_dry(p.buf));
        benchmark::ClobberMemory();
    }
}

static void BM_baseline2(benchmark::State &s, const char *fn) {
    const auto jpeg = utils::jpeg(fn);
    const auto p = jpeg.get_pixels();
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::pc_rgb_to_gray_dry2(p.buf));
        benchmark::ClobberMemory();
    }
}

static void BM_to_gray(benchmark::State &s, const char *fn) {
    const auto jpeg = utils::jpeg(fn);
    const auto p = jpeg.get_pixels();
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::pc_rgb_to_gray(p.buf));
        benchmark::ClobberMemory();
    }
}

static void BM_to_gray2(benchmark::State &s, const char *fn) {
    const auto jpeg = utils::jpeg(fn);
    const auto p = jpeg.get_pixels();
    for (auto _ : s) {
        benchmark::DoNotOptimize(utils::pc_rgb_to_gray2(p.buf));
        benchmark::ClobberMemory();
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    benchmark::RegisterBenchmark("BASELINE I0", &BM_baseline, argv[1]);
    benchmark::RegisterBenchmark("BASELINE IS", &BM_baseline2, argv[1]);
    benchmark::RegisterBenchmark("DOUBLE CALC", &BM_to_gray, argv[1]);
    benchmark::RegisterBenchmark("SIMPLE AVG", &BM_to_gray2, argv[1]);
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

    std::cout << argv[1] << '\n';
    std::cout << "  Info: " << finfo << '\n';
    std::cout << "  Extension: " << ext << '\n';

    auto jpeg = utils::jpeg(argv[1]);
    if (jpeg.is_jpeg()) {
        std::cout << "  JPEG Info:\n";
        std::cout << "    Width: " << jpeg.width() << '\n';
        std::cout << "    Height: " << jpeg.height() << '\n';
        std::cout << "    Subsampling: " << jpeg.subsamp_sv() << '\n';
        std::cout << "    Color Space: " << jpeg.colorspace_sv() << '\n';
        auto p = jpeg.get_pixels();
        if (p.is_valid()) {
            std::cout << "    Pixels: " << p.format() << " (" << p.buf.size()
                      << ")\n";
            auto pc = utils::pc_rgb_to_gray(p.buf);
            std::cout << "    Pixels: "
                      << "??"
                      << " (" << pc.size() << ")\n";
        }
    }

    return 0;
}
*/
