#ifndef SYSTEM_H
#define SYSTEM_H

#include "utils/natcmp.h"
#include "utils/timer.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#else
#include <cstring>
#include <sys/stat.h>
#endif

namespace utils {

using list_t = std::vector<std::string>;
static constexpr int MAX_FILE_READ_SZ = 10'485'760;
enum : int { FINFO_NOEXIST, FINFO_IS_FILE, FINFO_IS_DIR, FINFO_IS_LINK };

namespace internal {
inline bool is_crlf(const std::string &s) {
    const auto pos = s.find('\n');
    return (pos > 0) && (s.at(pos - 1) == '\r');
}

inline bool has_bom(const std::string &s) {
    return (s.size() > 3) && (s[0] == '\xEF') && (s[1] == '\xBB') &&
           (s[2] == '\xBF');
}
} // namespace internal

inline std::string file_to_string(const char *filename) {
    // Try to open the file
    constexpr std::ios_base::openmode open_mode =
        std::ios::in | std::ios::binary | std::ios::ate;
    std::ifstream fst(filename, open_mode);
    if (!fst.is_open()) {
        return std::string{};
    }
    // Get size of the file, ignore <= 0 > MAX_FILE_READ_SZ
    const auto filesize = static_cast<int64_t>(fst.tellg());
    if (filesize <= 0 || filesize > MAX_FILE_READ_SZ) {
        return std::string{};
    }
    // Read entire file into a string buffer
    fst.seekg(std::ios::beg);
    std::string buf(static_cast<unsigned int>(filesize), 0);
    fst.read(&buf[0], filesize);
    fst.close();
    return buf;
}

inline list_t list_from_filelines(const char *filename) {
    list_t list{};
    // Load into a temp string buffer first, this is actually faster
    const std::string buf = file_to_string(filename);
    if (buf.empty()) {
        return list;
    }
    // Check for byte order mark
    size_t start_pos = 0;
    if (internal::has_bom(buf)) {
        start_pos = 3;
    }
    // Loop through string buffer looking for delimiter
    size_t end_pos = 0;
    if (internal::is_crlf(buf)) {
        while (true) {
            end_pos = buf.find("\r\n", start_pos, 2);
            if (end_pos == std::string::npos) {
                break;
            }
            list.emplace_back(buf.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos + 2;
        }
    } else {
        while (true) {
            end_pos = buf.find('\n', start_pos);
            if (end_pos == std::string::npos) {
                break;
            }
            list.emplace_back(buf.substr(start_pos, end_pos - start_pos));
            start_pos = end_pos + 1;
        }
    }
    // There may be (probably is) one more trailing entry
    // This will ignore if the last entry is null
    if (start_pos < buf.size()) {
        list.emplace_back(buf.substr(start_pos, buf.size() - start_pos));
    }
    return list;
}

inline void list_shuffle(list_t &list) {
#ifdef __MINGW32__
    thread_local static std::mt19937 rng{the_time()};
#else
    thread_local static std::mt19937 rng{std::random_device{}()};
#endif
    std::shuffle(list.begin(), list.end(), rng);
}

inline void list_sort(list_t &list) { std::sort(list.begin(), list.end()); }

inline void list_sort_naturally(list_t &list) {
    std::sort(list.begin(), list.end(),
              [](const std::string &lhs, const std::string &rhs) {
                  return natcmp(lhs, rhs) < 0;
              });
}

inline void list_reverse(list_t &list) {
    std::reverse(list.begin(), list.end());
}

inline void list_print(const list_t &list) {
    if (list.empty()) {
        std::cout << "List is empty!\n";
        return;
    }
    std::cout << "Printing list -> " << list.size() << " entries.\n";
    std::cout << "--- START OF LIST ---\n";
    for (const auto &s : list) {
        std::cout << s.size() << " : " << s << '\n';
    }
    std::cout << "--- END OF LIST ---\n";
}

#ifdef _WIN32
int get_file_info(const char *path) noexcept {
    // Check for null/empty
    if (path == nullptr) {
        return FINFO_NOEXIST;
    }
    const auto len = strlen(path);
    if (len <= 0) {
        return FINFO_NOEXIST;
    }
    // Try to get attributes
    const auto res = GetFileAttributesW(&widen(path)[0]);
    // Check for error, most likely from non-existance
    if (res == INVALID_FILE_ATTRIBUTES) {
        return FINFO_NOEXIST;
    }
    // In windows, if it's not a directory it's a file
    if ((res & static_cast<DWORD>(FILE_ATTRIBUTE_DIRECTORY)) != 0u) {
        return FINFO_IS_DIR;
    }
    return FINFO_IS_FILE;
}
#else
inline int get_file_info(const char *path) noexcept {
    // Check for null/empty
    if (path == nullptr) {
        return FINFO_NOEXIST;
    }
    const auto len = strlen(path);
    if (len <= 0) {
        return FINFO_NOEXIST;
    }
    // Stat the file, if this fails probably doesnt exist
    struct stat statbuf {};
    if (lstat(path, &statbuf) != 0) {
        return FINFO_NOEXIST;
    }
    // Check flags
    switch (statbuf.st_mode & static_cast<unsigned>(S_IFMT)) {
    case S_IFDIR:
        return FINFO_IS_DIR;
    case S_IFLNK:
        return FINFO_IS_LINK;
    case S_IFREG:
        return FINFO_IS_FILE;
    default:
        break;
    }
    return FINFO_NOEXIST;
}
#endif

inline void print_file_info(const char *path) {
    std::cout << "Getting file info -> " << path << '\n';
    switch (get_file_info(path)) {
    default:
    case FINFO_NOEXIST:
        std::cout << "  File does not exist.\n";
        break;
    case FINFO_IS_FILE:
        std::cout << "  This is a file.\n";
        break;
    case FINFO_IS_DIR:
        std::cout << "  This is a folder.\n";
        break;
    case FINFO_IS_LINK:
        std::cout << "  This is a symbolic link.\n";
        break;
    }
}

} // namespace utils

#endif
