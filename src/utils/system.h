#ifndef SYSTEM_H
#define SYSTEM_H

#include "utils/natcmp.h"
#include "utils/timer.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <type_traits>
#include <vector>

#ifdef _WIN32
#include "utils/utf8conv_win32.h"
#include <windows.h>
#else
#include <cstring>
#include <sys/stat.h>
#endif

namespace utils {

// Using alias (typedefs) for common types
using bytes_t = std::vector<uint8_t>;
using list_t = std::vector<std::string>;
// Constants
constexpr auto MAX_FILE_SIZE = 512'000'000;
enum class file_info { no_exist, is_file, is_dir, is_link };
enum class file_ext { unknown = -1, jpeg, tiff };

// Internal functions
namespace internal {
inline bool is_crlf(const std::string &s) {
    const auto pos = s.find('\n');
    return (pos > 0) && (s.at(pos - 1) == '\r');
}
inline bool has_bom(const std::string &s) {
    return (s.size() > 3) && (s[0] == '\xEF') && (s[1] == '\xBB') &&
           (s[2] == '\xBF');
}
inline int biggest_int(int a, int b) {
    if (a > b) {
        return a;
    }
    return b;
}
} // namespace internal

// Reads entire file into a memory buffer (vector)
inline bytes_t file_binread(const char *filename) {
    // Try to open the file
    constexpr std::ios_base::openmode open_mode =
        std::ios::in | std::ios::binary | std::ios::ate;
    std::ifstream fst(filename, open_mode);
    if (!fst.is_open()) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Error opening file.\n";
        return bytes_t{};
    }
    // Get size of the file, ignore <= 0 > max_size
    const auto filesz = static_cast<int64_t>(fst.tellg());
    if (filesz <= 0 || filesz > MAX_FILE_SIZE) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Invalid file size (" << filesz
                  << ")! Max = " << MAX_FILE_SIZE << '\n';
        return bytes_t{};
    }
    // Read entire file into a string buffer
    fst.seekg(std::ios::beg);
    bytes_t buf(static_cast<unsigned>(filesz));
    fst.read(reinterpret_cast<char *>(&buf[0]), filesz); // NOLINT
    fst.close();
    return buf;
}
// Reads file into memory buffer (vector) with given offsets
inline bytes_t file_binread(const char *filename, int beg, int end) {
    // Make sure end - beg > 0
    const auto bufsz = end - beg;
    if (bufsz <= 0) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Invalid range specified.\n";
        return bytes_t{};
    }
    // Try to open the file
    constexpr std::ios_base::openmode open_mode =
        std::ios::in | std::ios::binary | std::ios::ate;
    std::ifstream fst(filename, open_mode);
    if (!fst.is_open()) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Error opening file.\n";
        return bytes_t{};
    }
    // Get size of the file, ignore <= 0 > max_size
    const auto filesz = static_cast<int64_t>(fst.tellg());
    if (filesz < end) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Invalid file size!" << '\n';
        return bytes_t{};
    }
    // Read file range into a string buffer
    fst.seekg(std::ios::beg + beg);
    bytes_t buf(static_cast<unsigned>(bufsz));
    fst.read(reinterpret_cast<char *>(&buf[0]), bufsz); // NOLINT
    fst.close();
    return buf;
}

// Read binary data from a buffer into an integral type
template <class IntType>
IntType read_int(const bytes_t &buf, unsigned offset, bool bswap = false) {
    // Bounds checking
    constexpr auto sz = sizeof(IntType);
    if (offset + sz > buf.size()) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    // Use memcpy to copy the data
    IntType val{};
    memcpy(&val, &buf[offset], sz);
    // Swap if necessary, ugly but optimizes to a single bswap
    if (bswap) {
        switch (sz) {
        case 8ULL:
            val = static_cast<IntType>(
                ((static_cast<uint64_t>(val) & 0xFF00000000000000) >> 56) |
                ((static_cast<uint64_t>(val) & 0x00FF000000000000) >> 40) |
                ((static_cast<uint64_t>(val) & 0x0000FF0000000000) >> 24) |
                ((static_cast<uint64_t>(val) & 0x000000FF00000000) >> 8) |
                ((static_cast<uint64_t>(val) & 0x00000000FF000000) << 8) |
                ((static_cast<uint64_t>(val) & 0x0000000000FF0000) << 24) |
                ((static_cast<uint64_t>(val) & 0x000000000000FF00) << 40) |
                ((static_cast<uint64_t>(val) & 0x00000000000000FF) << 56));
            break;
        case 4ULL:
            val = static_cast<IntType>(
                ((static_cast<uint32_t>(val) & 0xFF000000) >> 24) |
                ((static_cast<uint32_t>(val) & 0x00FF0000) >> 8) |
                ((static_cast<uint32_t>(val) & 0x0000FF00) << 8) |
                ((static_cast<uint32_t>(val) & 0x000000FF) << 24));
            break;
        case 2ULL:
            val = static_cast<IntType>(
                ((static_cast<uint16_t>(val) & 0xFF00) >> 8) |
                ((static_cast<uint16_t>(val) & 0x00FF) << 8));
            break;
        case 1ULL:
        default:
            break;
        }
    }
    return val;
}

// Shuffles a list using a random seed based on time
inline void list_shuffle(list_t &list) {
#ifdef __MINGW32__ // mingws random_device is broken
    thread_local static std::mt19937 rng{the_time()};
#else
    thread_local static std::mt19937 rng{std::random_device{}()};
#endif
    std::shuffle(list.begin(), list.end(), rng);
}
// Sorts a list "naturally", ie for a file list
inline void list_sort_naturally(list_t &list) {
    std::sort(list.begin(), list.end(),
              [](const std::string &lhs, const std::string &rhs) {
                  return natcmp(lhs, rhs) < 0;
              });
}
// Prints a list to console for debugging
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

// Determines if a file exists, and what type of file it is
#ifdef _WIN32
inline file_info get_file_info(const char *path) noexcept {
    // Check for null/empty
    if (path == nullptr) {
        return file_info::no_exist;
    }
    const auto len = strlen(path);
    if (len <= 0) {
        return file_info::no_exist;
    }
    // Try to get attributes
    const auto res = GetFileAttributesW(&widen(path)[0]);
    // Check for error, most likely from non-existance
    if (res == INVALID_FILE_ATTRIBUTES) {
        return file_info::no_exist;
    }
    // In windows, if it's not a directory it's a file
    if ((res & static_cast<DWORD>(FILE_ATTRIBUTE_DIRECTORY)) != 0u) {
        return file_info::is_dir;
    }
    return file_info::is_file;
}
#else
inline file_info get_file_info(const char *path) noexcept {
    // Check for null/empty
    if (path == nullptr) {
        return file_info::no_exist;
    }
    const auto len = strlen(path);
    if (len <= 0) {
        return file_info::no_exist;
    }
    // Stat the file, if this fails probably doesnt exist
    struct stat statbuf {};
    if (lstat(path, &statbuf) != 0) {
        return file_info::no_exist;
    }
    // Check flags
    switch (statbuf.st_mode & static_cast<unsigned>(S_IFMT)) {
    case S_IFDIR:
        return file_info::is_dir;
    case S_IFLNK:
        return file_info::is_link;
    case S_IFREG:
        return file_info::is_file;
    default:
        break;
    }
    return file_info::no_exist;
}
#endif
inline void print_file_info(const char *path) {
    std::cout << "Getting file info -> " << path << '\n';
    switch (get_file_info(path)) {
    default:
    case file_info::no_exist:
        std::cout << "  File does not exist.\n";
        break;
    case file_info::is_file:
        std::cout << "  This is a file.\n";
        break;
    case file_info::is_dir:
        std::cout << "  This is a folder.\n";
        break;
    case file_info::is_link:
        std::cout << "  This is a symbolic link.\n";
        break;
    }
}

// Determine some "known" file extensions
inline file_ext get_file_ext(const char *filename) {
    const auto ext = strrchr(filename, '.');
    if (ext == nullptr) {
        return file_ext::unknown;
    }
    // JPEG
    if ((strncasecmp(ext, ".jpg", 4) == 0) ||
        (strncasecmp(ext, ".jpeg", 5) == 0)) {
        return file_ext::jpeg;
    }
    // TIFF
    if ((strncasecmp(ext, ".tif", 4) == 0) ||
        (strncasecmp(ext, ".tiff", 5) == 0)) {
        return file_ext::tiff;
    }
    return file_ext::unknown;
}

/*
///////////////////////////////
// Windows directory listing //
///////////////////////////////
#ifdef _WIN32

// Directory handle paired with WIN32_FIND_DATA
struct dirhand_t {
    HANDLE handle;
    WIN32_FIND_DATAW find_data;
};

// Files that begin with . arent technically hidden on windows
inline bool is_hidden(const WIN32_FIND_DATAW &fdata) {
    if (fdata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) {
        return true;
    }
    return (fdata.cFileName[0] == L'.') ? true : false;
}

// A few cases we always hide: . and ..
// $RECYCLE.BIN and System Volume Information
inline bool is_forcehidden(const wchar_t *wstr) {
    switch (wcslen(wstr)) {
    case 1: // . ?
        if (wstr[0] == L'.') {
            return true;
        }
        break;
    case 2: // .. ?
        if ((wstr[1] == L'.') && (wstr[0] == L'.')) {
            return true;
        }
        break;
    case 12: // $RECYCLE.BIN?
        if (wcsncmp(&wstr[0], L"$RECYCLE.BIN", 12) == 0) {
            return true;
        }
        break;
    case 25: // System Volume Information?
        if (wcsncmp(&wstr[0], L"System Volume Information", 25) == 0) {
            return true;
        }
        break;
    default:
        return false;
    }
    return false;
}

// Converts path to a string and removes/replaces invalid characters
inline std::string fix_path(const char *in_path) {
    std::string path{in_path};
    while (!path.empty()) {
        switch (path.back()) {
        case '\\':
        case '/':
        case '*':
        case '.':
        case '~':
        case '#':
        case '%':
        case '&':
        case '{':
        case '}':
        case ':':
        case '<':
        case '>':
        case '?':
        case '+':
        case '|':
        case '\"':
            path.pop_back();
            break;
        default:
            // Use backslashes on windows
            std::replace(path.begin(), path.end(), '/', '\\');
            return path;
        }
    }
    return path;
}

// Trys to get a directory handle/find data
inline dirhand_t get_dir_handle(const char *path) {
    // Initalize return struct
    dirhand_t ret = {INVALID_HANDLE_VALUE,
                     {0, {0, 0}, {0, 0}, {0, 0}, 0, 0, 0, 0, L"", L""}};
    // To start, try to properly format the string
    std::string fixed_path{fix_path(path)};
    // Make sure it's a directory
    if (get_file_info(fixed_path.c_str()) != FINFO_IS_DIR) {
        std::cerr << fixed_path << " is not a directory!\n";
        return ret;
    }
    // Append wildcard to path
    fixed_path.append("\\*");
    // Try to get handle
    ret.handle = FindFirstFileExW(&widen(fixed_path)[0], FindExInfoBasic,
                                  &ret.find_data, FindExSearchNameMatch,
                                  nullptr, FIND_FIRST_EX_LARGE_FETCH);
    return ret;
}

// Get's directory contents and returns a list of strings
inline list list_dir(const char *path, const int opt = 0) {
    // Check for NULL
    list list{};
    if (path == nullptr) {
        std::cerr << "NULL path\n";
        return list;
    }
    // Try to get directory handle
    dirhand_t dir = get_dir_handle(path);
    if (dir.handle == INVALID_HANDLE_VALUE) {
        std::cerr << "Cannot get directory handle!\n";
        return list;
    }
    // Set some bools so we don't every loop
    const bool show_hidden = opt & DL_SHOW_HIDDEN;
    const bool recurse = opt & DL_RECURSE;
    // Loop over each entry
    while (FindNextFileW(dir.handle, &dir.find_data) != 0) {
        // Ignore "bad" entries
        if (is_forcehidden(dir.find_data.cFileName)) {
            continue;
        }
        // Ignore hidden entries - optional
        if (!show_hidden && is_hidden(dir.find_data)) {
            continue;
        }
        // Directory: ignore or recurse
        if (dir.find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (recurse) {
                // recurse here
            }
            continue;
        }

        list.emplace_back(narrow(dir.find_data.cFileName));
    }
    return list;
}
#endif
*/
} // namespace utils

#endif
