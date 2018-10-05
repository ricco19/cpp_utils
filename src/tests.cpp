#include "image/image.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>

// #include "benchmark/benchmark.h"

// static void BM_loadimg(benchmark::State &s, const char *fn) {
//     size_t sz = 0;
//     const auto img = image::load_image{fn};
//     for (auto _ : s) {
//         if (img.is_image()) {
//             const auto p = img.get_pixels(image::FORMAT_GREY);
//             sz = p.size();
//         }
//         benchmark::ClobberMemory();
//     }
//     if (sz == 0)
//         sz = 1;
//     benchmark::ClobberMemory();
// }

// static void BM_loadimg2(benchmark::State &s, const char *fn) {
//     size_t sz = 0;
//     const auto img = image::load_image{fn};
//     for (auto _ : s) {
//         if (img.is_image()) {
//             const auto p = img.get_pixels(image::FORMAT_RGBA);
//             sz = p.size();
//         }
//         benchmark::ClobberMemory();
//     }
//     if (sz == 0)
//         sz = 1;
//     benchmark::ClobberMemory();
// }

// static void BM_loadj(benchmark::State &s, const char *fn) {

//     const auto data = utils::file_binread(fn);
//     tjhandle hand = tjInitDecompress();
//     int w{};
//     int h{};
//     int ds{};
//     int ds2{};
//     tjDecompressHeader3(hand, &data[0], data.size(), &w, &h, &ds, &ds2);
//     const auto sz = static_cast<size_t>(w * h);
//     size_t tsz{};
//     for (auto _ : s) {
//         utils::bytearr_t pixels(sz, 0);
//         tjDecompress2(hand, &data[0], data.size(), &pixels[0], w, w, h,
//                       TJPF_GRAY, TJFLAG_NOREALLOC | TJFLAG_FASTDCT);
//         tsz = pixels.size();
//         benchmark::ClobberMemory();
//     }
//     tjDestroy(hand);
//     if (tsz == 0)
//         tsz = 1;
//     benchmark::ClobberMemory();
// }

// static void BM_loadjsl(benchmark::State &s, const char *fn) {
//     size_t tsz{};
//     for (auto _ : s) {
//         const auto data = utils::file_binread(fn);
//         tjhandle hand = tjInitDecompress();
//         int w{};
//         int h{};
//         int ds{};
//         int ds2{};
//         tjDecompressHeader3(hand, &data[0], data.size(), &w, &h, &ds, &ds2);
//         const auto sz = static_cast<size_t>(w * h);

//         utils::bytearr_t pixels(sz, 0);
//         tjDecompress2(hand, &data[0], data.size(), &pixels[0], w, w, h,
//                       TJPF_GRAY, TJFLAG_NOREALLOC | TJFLAG_FASTDCT);
//         tsz = pixels.size();
//         benchmark::ClobberMemory();
//         tjDestroy(hand);
//     }

//     if (tsz == 0)
//         tsz = 1;
//     benchmark::ClobberMemory();
// }

// static void BM_load(benchmark::State &s, const char *fn) {
//     size_t tsz{};
//     for (auto _ : s) {
//         const auto data = utils::file_binread(fn);
//         tsz = data.size();
//         if (tsz == 0)
//             tsz = 1;
//         benchmark::ClobberMemory();
//     }
// }

// int main(int argc, char **argv) {

//     benchmark::RegisterBenchmark("JPEG RGBA", &BM_loadimg2,
//                                  "/home/geeklab/acf_test/docs/test.jpg");
//     benchmark::RegisterBenchmark("JPEG", &BM_loadimg,
//                                  "/home/geeklab/acf_test/docs/test.jpg");
//     benchmark::RegisterBenchmark("TIFF", &BM_loadimg,
//                                  "/home/geeklab/acf_test/docs/test.tif");
//     benchmark::RegisterBenchmark("JPEG RAW", &BM_loadj,
//                                  "/home/geeklab/acf_test/docs/test.jpg");
//     benchmark::RegisterBenchmark("JPEG RAW SLOW", &BM_loadjsl,
//                                  "/home/geeklab/acf_test/docs/test.jpg");
//     benchmark::RegisterBenchmark("FILE LOAD", &BM_load,
//                                  "/home/geeklab/acf_test/docs/test.jpg");

//     benchmark::Initialize(&argc, argv);
//     benchmark::RunSpecifiedBenchmarks();
// }

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }
    // utils::print_file_info(argv[1]);
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
