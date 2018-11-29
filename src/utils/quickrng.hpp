#ifndef QUICKRNG_HPP
#define QUICKRNG_HPP

#include <random>
#include <string>
#include <string_view>
#ifdef __MINGW32__
#include "utils/timer.hpp"
#endif

namespace utils {

inline char rng_alnum() {
    constexpr std::string_view chrs{
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"};
    constexpr auto numchars = chrs.size() - 1;
#ifdef __MINGW32__
    thread_local static std::mt19937 rng{the_time()};
#else
    thread_local static std::mt19937 rng{std::random_device{}()};
#endif
    thread_local static std::uniform_int_distribution<int> uid{0, numchars};
    return chrs.at(static_cast<size_t>(uid(rng)));
}

inline int rng_int(const int range_low = std::numeric_limits<int>::min(),
                   const int range_high = std::numeric_limits<int>::max()) {
    if (range_low > range_high) {
        return 0;
    }
#ifdef __MINGW32__
    thread_local static std::mt19937 rng{the_time()};
#else
    thread_local static std::mt19937 rng{std::random_device{}()};
#endif
    return std::uniform_int_distribution<int>{range_low, range_high}(rng);
}

inline std::string rng_str(unsigned len = 16) {
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

} // namespace utils
#endif
