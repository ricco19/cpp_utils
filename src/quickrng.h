#ifndef QUICKRNG_H
#define QUICKRNG_H

#include <random>
#ifdef __MINGW32__
#include <chrono>
#endif

namespace quickrng {

inline char rng_alnum() {
    static constexpr char chrs[] =
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    static constexpr auto numchars = sizeof(chrs) - 2;
#ifdef __MINGW32__
    using ms_t = std::chrono::duration<uint_fast32_t, std::milli>;
    using clk_t = std::chrono::high_resolution_clock;
    thread_local static std::mt19937 rng{
        ms_t(clk_t::now().time_since_epoch().count()).count()};
#else
    thread_local static std::mt19937 rng{std::random_device{}()};
#endif
    thread_local static std::uniform_int_distribution<int> uid{0, numchars};
    return chrs[uid(rng)];
}

inline int rng_int(const int range_low = std::numeric_limits<int>::min(),
                   const int range_high = std::numeric_limits<int>::max()) {
    if (range_low > range_high) {
        return 0;
    }
#ifdef __MINGW32__
    using ms_t = std::chrono::duration<uint_fast32_t, std::milli>;
    using clk_t = std::chrono::high_resolution_clock;
    thread_local static std::mt19937 rng{
        ms_t(clk_t::now().time_since_epoch().count()).count()};
#else
    thread_local static std::mt19937 rng{std::random_device{}()};
#endif
    return std::uniform_int_distribution<int>{range_low, range_high}(rng);
}

inline std::string rng_str(std::string::size_type len = 16) {
    std::string retstr{};
    if (len <= 0) {
        return retstr;
    }
    retstr.reserve(len);
    while (--len != 0u) {
        retstr.push_back(rng_alnum());
    }
    return retstr;
}

} // namespace quickrng
#endif
