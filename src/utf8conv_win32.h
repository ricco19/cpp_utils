#ifndef UTF8CONV_WIN32_H
#define UTF8CONV_WIN32_H

#include <string>
#include <windows.h>

namespace utf8conv {

// wstring -> string (narrow)
inline std::string narrow(const std::wstring &ws) {
    std::string retstr;
    // cast length to int, if < 1 return an empty string
    const auto len = static_cast<int>(ws.size());
    if (len <= 0) {
        return retstr;
    }
    // dry run to retrieve the destination buffer size requirement
    // again just return an empty string if < 1
    const auto sz = WideCharToMultiByte(CP_UTF8, 0, &ws[0], len, nullptr, 0,
                                        nullptr, nullptr);
    if (sz <= 0) {
        return retstr;
    }
    // reserve and write to a buffer
    retstr.reserve(sz);
    WideCharToMultiByte(CP_UTF8, 0, &ws[0], len, &retstr[0], sz, nullptr,
                        nullptr);
    return retstr;
}
inline std::string narrow(const wchar_t *ws) {
    std::string retstr;
    // cast length to int, if < 1 return an empty string
    const auto len = static_cast<int>(wcslen(ws));
    if (len <= 0) {
        return retstr;
    }
    // dry run to retrieve the destination buffer size requirement
    // again just return an empty string if < 1
    const auto sz =
        WideCharToMultiByte(CP_UTF8, 0, ws, len, nullptr, 0, nullptr, nullptr);
    if (sz <= 0) {
        return retstr;
    }
    // reserve and write to a buffer
    retstr.reserve(sz);
    WideCharToMultiByte(CP_UTF8, 0, ws, len, &retstr[0], sz, nullptr, nullptr);
    return retstr;
}

// string -> wstring (widen)
inline std::wstring widen(const std::string &s) {
    std::wstring retstr;
    // cast length to int, if < 1 return an empty string
    const auto len = static_cast<int>(s.size());
    if (len <= 0) {
        return retstr;
    }
    // dry run to retrieve the destination buffer size requirement
    // again just return an empty string if < 1
    const auto sz = MultiByteToWideChar(CP_UTF8, 0, &s[0], len, nullptr, 0);
    if (sz <= 0) {
        return retstr;
    }
    // reserve and write to a buffer
    retstr.reserve(sz);
    MultiByteToWideChar(CP_UTF8, 0, &s[0], len, &retstr[0], sz);
    return retstr;
}
inline std::wstring widen(const char *s) {
    std::wstring retstr;
    // cast length to int, if < 1 return an empty string
    const int len = static_cast<int>(strlen(s));
    if (len <= 0) {
        return retstr;
    }
    // dry run to retrieve the destination buffer size requirement
    // again just return an empty string if < 1
    const auto sz = MultiByteToWideChar(CP_UTF8, 0, s, len, nullptr, 0);
    if (sz <= 0) {
        return retstr;
    }
    // reserve and write to a buffer
    retstr.reserve(sz);
    MultiByteToWideChar(CP_UTF8, 0, s, len, &retstr[0], sz);
    return retstr;
}

} // namespace utf8conv

#endif
