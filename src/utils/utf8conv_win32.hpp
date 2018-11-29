#ifndef UTF8CONV_WIN32_HPP
#define UTF8CONV_WIN32_HPP

#include <string>
#include <windows.h>
#include <iostream>

namespace utils {

// wstring -> string (narrow)
inline std::string narrow(std::wstring_view wsv) {
    // cast length to int, if < 1 return an empty string
    const auto len = static_cast<int>(wsv.size());
    if (len <= 0) {
        return std::string{};
    }
    // dry run to retrieve the destination buffer size requirement
    // again just return an empty string if < 1
    const auto sz = WideCharToMultiByte(CP_UTF8, 0, &wsv[0], len, nullptr, 0,
                                        nullptr, nullptr);
    if (sz <= 0) {
        return std::string{};
    }
    // reserve and write to a buffer
    std::string retstr(static_cast<size_t>(sz));
    WideCharToMultiByte(CP_UTF8, 0, &wsv[0], len, &retstr[0], sz, nullptr,
                        nullptr);
    return retstr;
}

// string -> wstring (widen)
inline std::wstring widen(std::string_view sv) {
    // cast length to int, if < 1 return an empty string
    const auto len = static_cast<int>(sv.size());
    if (len <= 0) {
        return std::wstring{};
    }
    // dry run to retrieve the destination buffer size requirement
    // again just return an empty string if < 1
    const auto sz = MultiByteToWideChar(CP_UTF8, 0, &sv[0], len, nullptr, 0);
    if (sz <= 0) {
        return std::wstring{};
    }
    // reserve and write to a buffer
    std::wstring retstr(static_cast<size_t>(sz))
    MultiByteToWideChar(CP_UTF8, 0, &sv[0], len, &retstr[0], sz);
    return retstr;
}


} // namespace utils

#endif
