#include "utils/image.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>

#include "benchmark/benchmark.h"

static void BM_baseline(benchmark::State &s, const char *fn) {
    auto jpeg = utils::jpeg(fn);
    for (auto _ : s) {
        if (jpeg.is_jpeg()) {
            auto p = jpeg.get_pixels();

        }

        benchmark::ClobberMemory();
    }
}

static void BM_to_rgb(benchmark::State &s, const char *fn) {
    auto jpeg = utils::jpeg(fn);
    for (auto _ : s) {
        if (jpeg.is_jpeg()) {
            auto p = jpeg.get_pixels();
            p.convert_to(utils::Pixel_Format::RGB);

        }

        benchmark::ClobberMemory();
    }
}

static void BM_to_rgba(benchmark::State &s, const char *fn) {
    auto jpeg = utils::jpeg(fn);
    for (auto _ : s) {
        if (jpeg.is_jpeg()) {
            auto p = jpeg.get_pixels();
            p.convert_to(utils::Pixel_Format::RGBA);

        }
        benchmark::ClobberMemory();
    }
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    benchmark::RegisterBenchmark("BASELINE", &BM_baseline, argv[1]);
    benchmark::RegisterBenchmark("TO RGB", &BM_to_rgb, argv[1]);
    benchmark::RegisterBenchmark("TO RGBA", &BM_to_rgba, argv[1]);
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
            std::cout << "    Pixels: " << p.format()
                      << " (" << p.buf.size() << ")\n";
            p.convert_to(utils::Pixel_Format::RGBA);
            std::cout << "    Pixels: " << p.format()
                      << " (" << p.buf.size() << ")\n";
        }
    }

    return 0;
}
*/
