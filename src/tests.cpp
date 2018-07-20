#include "benchmark/benchmark.h"
//#include "image/tiff.h"
#include "utils/natcmp.h"
#include "utils/quickrng.h"
#include "utils/system.h"
#include "utils/timer.h"
#include <iostream>


static void BM_read8u(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    uint8_t i = 0;
    for (auto _ : s) {
        i = f.read_8u(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read8s(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    int8_t i = 0;
    for (auto _ : s) {
        i = f.read_8s(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read16u(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    uint16_t i = 0;
    for (auto _ : s) {
        i = f.read_16u(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read16s(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    int16_t i = 0;
    for (auto _ : s) {
        i = f.read_16s(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read16u_sw(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    uint16_t i = 0;
    for (auto _ : s) {
        i = f.read_16u(0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read16s_sw(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    int16_t i = 0;
    for (auto _ : s) {
        i = f.read_16s(0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read32u(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    uint32_t i = 0;
    for (auto _ : s) {
        i = f.read_32u(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read32s(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    int32_t i = 0;
    for (auto _ : s) {
        i = f.read_32s(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read32u_sw(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    uint32_t i = 0;
    for (auto _ : s) {
        i = f.read_32u(0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read32s_sw(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    int32_t i = 0;
    for (auto _ : s) {
        i = f.read_32s(0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read64u(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    uint64_t i = 0;
    for (auto _ : s) {
        i = f.read_64u(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read64s(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    int64_t i = 0;
    for (auto _ : s) {
        i = f.read_64s(0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read64u_sw(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    uint64_t i = 0;
    for (auto _ : s) {
        i = f.read_64u(0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_read64s_sw(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    int64_t i = 0;
    for (auto _ : s) {
        i = f.read_64s(0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_readstr(benchmark::State &s) {
    utils::binread f("../docs/tiff_samples/MARBLES.TIF");
    std::string str;
    for (auto _ : s) {
        str = f.read_str(0, 12);
        asm volatile("" : : : "memory");
    }
}

static void BM_temp_32u(benchmark::State &s) {
    const auto buf = utils::file_binread("../docs/tiff_samples/MARBLES.TIF");
    uint32_t i = 0;
    for (auto _ : s) {
        i = utils::brtest<uint32_t>(buf, 0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_temp_32s(benchmark::State &s) {
    const auto buf = utils::file_binread("../docs/tiff_samples/MARBLES.TIF");
    int32_t i = 0;
    for (auto _ : s) {
        i = utils::brtest<int32_t>(buf, 0);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_temp_32u_sw(benchmark::State &s) {
    const auto buf = utils::file_binread("../docs/tiff_samples/MARBLES.TIF");
    uint32_t i = 0;
    for (auto _ : s) {
        i = utils::brtest<uint32_t>(buf, 0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}

static void BM_temp_32s_sw(benchmark::State &s) {
    const auto buf = utils::file_binread("../docs/tiff_samples/MARBLES.TIF");
    int32_t i = 0;
    for (auto _ : s) {
        i = utils::brtest<int32_t>(buf, 0, true);
        asm volatile("" : : : "memory");
    }
    if (i == 0) {
        i = 1;
    }
}


BENCHMARK(BM_read8u)->ReportAggregatesOnly(true);
BENCHMARK(BM_read8s)->ReportAggregatesOnly(true);

BENCHMARK(BM_read16u)->ReportAggregatesOnly(true);
BENCHMARK(BM_read16s)->ReportAggregatesOnly(true);
BENCHMARK(BM_read16u_sw)->ReportAggregatesOnly(true);
BENCHMARK(BM_read16s_sw)->ReportAggregatesOnly(true);

BENCHMARK(BM_read32u)->ReportAggregatesOnly(true);
BENCHMARK(BM_read32s)->ReportAggregatesOnly(true);
BENCHMARK(BM_read32u_sw)->ReportAggregatesOnly(true);
BENCHMARK(BM_read32s_sw)->ReportAggregatesOnly(true);

BENCHMARK(BM_temp_32u)->ReportAggregatesOnly(true);
BENCHMARK(BM_temp_32s)->ReportAggregatesOnly(true);
BENCHMARK(BM_temp_32u_sw)->ReportAggregatesOnly(true);
BENCHMARK(BM_temp_32s_sw)->ReportAggregatesOnly(true);

BENCHMARK(BM_read64u)->ReportAggregatesOnly(true);
BENCHMARK(BM_read64s)->ReportAggregatesOnly(true);
BENCHMARK(BM_read64u_sw)->ReportAggregatesOnly(true);
BENCHMARK(BM_read64s_sw)->ReportAggregatesOnly(true);

BENCHMARK(BM_readstr)->ReportAggregatesOnly(true);




BENCHMARK_MAIN();

/*
int main(int argc, char *argv[]) {

    if (argc <= 1) {
        std::cout << "Nothing to do!\n";
        return 0;
    }

    utils::print_file_info(argv[1]);

    // image::tiff_open tif(argv[1]);
    // if (tif.is_tiff()) {
    //     std::cout << " -> Loaded TIFF!" << '\n';
    // } else {
    //     std::cout << " -> Not a TIFF." << '\n';
    // }

    utils::binread test(argv[1]);

    std::cout << "size = " << test.size() << "\n\n";

    // std::cout << "read_8u       : " << test.read_8u(0) << '\n';
    // std::cout << "read_8s       : " << test.read_8s(0) << '\n';
    // std::cout << std::endl;

    // std::cout << "read_16u      : " << test.read_16u(0) << '\n';
    // std::cout << "read_16u safe : " << test.read_16u_safe(0) << '\n';
    // std::cout << "read_16u swap : " << test.read_16u(0, true) << '\n';
    // std::cout << "read_16s      : " << test.read_16s(0) << '\n';
    // std::cout << "read_16s safe : " << test.read_16s_safe(0) << '\n';
    // std::cout << "read_16s swap : " << test.read_16s(0, true) << '\n';
    // std::cout << std::endl;

    std::cout << "read_32u      : " << test.read_32u(0) << '\n';
    std::cout << "read_32u swap : " << test.read_32u(0, true) << '\n';
    std::cout << "read_32s      : " << test.read_32s(0) << '\n';
    std::cout << "read_32s swap : " << test.read_32s(0, true) << '\n';
    std::cout << std::endl;

    std::cout << "read_32u      : " << utils::brtest<uint32_t>(test.buf_, 0)
              << '\n';
    std::cout << "read_32s      : " << utils::brtest<std::string>(test.buf_, 0)
              << '\n';
    std::cout << std::endl;

    // std::cout << "read_64u      : " << test.read_64u(0) << '\n';
    // std::cout << "read_64u safe : " << test.read_64u_safe(0) << '\n';
    // std::cout << "read_64u swap : " << test.read_64u(0, true) << '\n';
    // std::cout << "read_64s      : " << test.read_64s(0) << '\n';
    // std::cout << "read_64s safe : " << test.read_64s_safe(0) << '\n';
    // std::cout << "read_64s swap : " << test.read_64s(0, true) << '\n';
    // std::cout << std::endl;

    // utils::list_t list = utils::file_to_list(argv[1]);
    // utils::list_shuffle(list);
    // utils::list_sort_naturally(list);

    // utils::print_list(list);

    return 0;
}
*/
