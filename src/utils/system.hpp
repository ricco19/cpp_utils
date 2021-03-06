#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include "utils/natcmp.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

#ifdef __MINGW32__
#include "utils/timer.h"
#endif
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

// A very simple toupper function that works on ascii chars a-z only
// The uint64 version saves some readibility
constexpr int toupper_ascii(const int ch) noexcept {
    if ((ch >= 97) && (ch <= 122)) {
        return ch - 32;
    }
    return ch;
}
constexpr uint64_t toupper_ascii_uint64(const int ch) noexcept {
    if ((ch >= 97) && (ch <= 122)) {
        return static_cast<uint64_t>(ch - 32);
    }
    return static_cast<uint64_t>(ch);
}

// Converts first 8 bytes of a string to a 64 bit unsigned integer
// Case insenstive, only works on ascii chars a-z
constexpr uint64_t str_to_uint64(const std::string_view str) {
    // Yikes: unrolled loop and ugly casts
    switch (str.size()) {
    case 0:
        return 0;
    case 1:
        return toupper_ascii_uint64(str[0]);
    case 2:
        return (toupper_ascii_uint64(str[0]) << 0) |
               (toupper_ascii_uint64(str[1]) << 8);
        break;
    case 3:
        return (toupper_ascii_uint64(str[0]) << 0) |
               (toupper_ascii_uint64(str[1]) << 8) |
               (toupper_ascii_uint64(str[2]) << 16);
        break;
    case 4:
        return (toupper_ascii_uint64(str[0]) << 0) |
               (toupper_ascii_uint64(str[1]) << 8) |
               (toupper_ascii_uint64(str[2]) << 16) |
               (toupper_ascii_uint64(str[3]) << 24);
        break;
    case 5:
        return (toupper_ascii_uint64(str[0]) << 0) |
               (toupper_ascii_uint64(str[1]) << 8) |
               (toupper_ascii_uint64(str[2]) << 16) |
               (toupper_ascii_uint64(str[3]) << 24) |
               (toupper_ascii_uint64(str[4]) << 32);
        break;
    case 6:
        return (toupper_ascii_uint64(str[0]) << 0) |
               (toupper_ascii_uint64(str[1]) << 8) |
               (toupper_ascii_uint64(str[2]) << 16) |
               (toupper_ascii_uint64(str[3]) << 24) |
               (toupper_ascii_uint64(str[4]) << 32) |
               (toupper_ascii_uint64(str[5]) << 40);
        break;
    case 7:
        return (toupper_ascii_uint64(str[0]) << 0) |
               (toupper_ascii_uint64(str[1]) << 8) |
               (toupper_ascii_uint64(str[2]) << 16) |
               (toupper_ascii_uint64(str[3]) << 24) |
               (toupper_ascii_uint64(str[4]) << 32) |
               (toupper_ascii_uint64(str[5]) << 40) |
               (toupper_ascii_uint64(str[6]) << 48);
        break;
    case 8:
    default:
        break;
    }
    return (toupper_ascii_uint64(str[0]) << 0) |
           (toupper_ascii_uint64(str[1]) << 8) |
           (toupper_ascii_uint64(str[2]) << 16) |
           (toupper_ascii_uint64(str[3]) << 24) |
           (toupper_ascii_uint64(str[4]) << 32) |
           (toupper_ascii_uint64(str[5]) << 40) |
           (toupper_ascii_uint64(str[6]) << 48) |
           (toupper_ascii_uint64(str[7]) << 56);
}

// Read binary data from a buffer into an integral type
template <class IntType>
[[nodiscard]] IntType read_int(const bytes_t &buf, const unsigned offset,
                               const bool bswap = false) {
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

//
// Determine if a file or folder exists
// TODO: test std::filesystem performance
//
enum class File_Info {
    Unknown = -1,
    Is_File,
    Is_Dir,
    Is_Link
};
constexpr std::ostream &operator<<(std::ostream &os, const File_Info info) {
    switch (info) {
    default:
    case File_Info::Unknown:
        os << "Unknown";
        break;
    case File_Info::Is_File:
        os << "File";
        break;
    case File_Info::Is_Dir:
        os << "Directory";
        break;
    case File_Info::Is_Link:
        os << "Symbolic Link";
        break;
    }
    return os;
}
// Determines if a file exists, and what type of file it is
#ifdef _WIN32
inline File_Info get_file_info(const char *path) noexcept {
    // Check for null/empty
    if (path == nullptr) {
        return File_Info::Unknown;
    }
    const auto len = strlen(path);
    if (len <= 0) {
        return File_Info::Unknown;
    }
    // Try to get attributes
    const auto res = GetFileAttributesW(widen(path).data());
    // Check for error, most likely from non-existance
    if (res == INVALID_FILE_ATTRIBUTES) {
        return File_Info::Unknown;
    }
    // In windows, if it's not a directory it's a file
    if ((res & static_cast<DWORD>(FILE_ATTRIBUTE_DIRECTORY)) != 0u) {
        return File_Info::Is_Dir;
    }
    return File_Info::Is_File;
}
#else
inline File_Info get_file_info(const char *path) noexcept {
    // Stat the file, if this fails probably doesnt exist
    struct stat statbuf {};
    if (lstat(path, &statbuf) != 0) {
        return File_Info::Unknown;
    }
    // Check flags
    switch (statbuf.st_mode & static_cast<unsigned>(S_IFMT)) {
    case S_IFDIR:
        return File_Info::Is_Dir;
    case S_IFLNK:
        return File_Info::Is_Link;
    case S_IFREG:
        return File_Info::Is_File;
    default:
        break;
    }
    return File_Info::Unknown;
}
#endif

//
// Naive file (string) extension checker
//
enum class File_Ext { Unknown = -1, JPEG, TIFF, GIF, PNG, BMP };
constexpr std::ostream &operator<<(std::ostream &os, const File_Ext ext) {
    switch (ext) {
    default:
    case File_Ext::Unknown:
        os << "Unknown";
        break;
    case File_Ext::JPEG:
        os << "JPEG Image";
        break;
    case File_Ext::TIFF:
        os << "TIFF Image";
        break;
    case File_Ext::GIF:
        os << "GIF Image";
        break;
    case File_Ext::PNG:
        os << "PNG Image";
        break;
    case File_Ext::BMP:
        os << "BMP Image";
        break;
    }
    return os;
}
// Trys to extract extension by finding '.' then converts to int64_t
constexpr uint64_t get_ext_code(const std::string_view sv) {
    // Maximum length of an extension -> 8 bytes
    constexpr auto MAX_EXT_LEN = sizeof(uint64_t);
    if (sv.size() < MAX_EXT_LEN) {
        const auto pos = sv.find_last_of('.');
        if (pos == std::string_view::npos) {
            return 0;
        }
        return str_to_uint64(&sv[pos + 1]);
    }
    const auto ss = sv.substr(sv.size() - MAX_EXT_LEN, MAX_EXT_LEN);
    const auto pos = ss.find_last_of('.');
    if (pos == std::string_view::npos) {
        return 0;
    }
    return str_to_uint64(&ss[pos + 1]);
}
// Case insensitive file extension checker
constexpr File_Ext get_file_ext(const std::string_view filename) {
    // "magic numbers", 8 bytes converted to 64 bit int
    constexpr auto EXTTYPE_JPEG = str_to_uint64("JPEG");
    constexpr auto EXTTYPE_JPG = str_to_uint64("JPG");
    constexpr auto EXTTYPE_TIFF = str_to_uint64("TIFF");
    constexpr auto EXTTYPE_TIF = str_to_uint64("TIF");
    constexpr auto EXTTYPE_GIF = str_to_uint64("GIF");
    constexpr auto EXTTYPE_PNG = str_to_uint64("PNG");
    constexpr auto EXTTYPE_BMP = str_to_uint64("BMP");
    // Extract integer value from the string so we can switch it
    switch (get_ext_code(filename)) {
    case EXTTYPE_GIF:
        return File_Ext::GIF;
    case EXTTYPE_PNG:
        return File_Ext::PNG;
    case EXTTYPE_BMP:
        return File_Ext::BMP;
    case EXTTYPE_JPG:
    case EXTTYPE_JPEG:
        return File_Ext::JPEG;
    case EXTTYPE_TIF:
    case EXTTYPE_TIFF:
        return File_Ext::TIFF;
    default:
        break;
    }
    return File_Ext::Unknown;
}

//
// Read a file from disk into memory (vector of bytes)
//
inline bytes_t file_binread(const char *filename) {
    // Maximum size of a file we want to operate on -> 512mb
    constexpr std::streamoff MAX_FILE_SIZE = 512'000'000;
    // Try to open the file, readonly, binary, seek to end
    constexpr auto omode = std::ios::in | std::ios::binary | std::ios::ate;
    std::ifstream fst(filename, omode);
    if (!fst.is_open()) {
        std::cerr << "[ERROR] Cannot open file for input: " << filename
                  << "\n -> Error opening file.\n";
        return bytes_t{};
    }
    // Get size of the file, ignore <= 0 > max_size
    const auto filesz = static_cast<std::streamoff>(fst.tellg());
    if (filesz <= 0 || filesz > MAX_FILE_SIZE) {
        std::cerr << "[ERROR] Cannot open file for input: " << filename
                  << "\n -> Invalid file size (" << filesz
                  << ")! Max = " << MAX_FILE_SIZE << '\n';
        return bytes_t{};
    }
    // Read entire file into a string buffer
    fst.seekg(std::ios::beg);
    bytes_t buf(static_cast<uint64_t>(filesz));
    fst.read(reinterpret_cast<char *>(buf.data()), filesz); // NOLINT
    fst.close();
    return buf;
}
inline bytes_t file_binread(const char *filename, const std::streamoff beg,
                            const std::streamoff end) {
    // Make sure end - beg > 0
    const auto bufsz = end - beg;
    if (bufsz <= 0) {
        std::cerr << "[ERROR] Cannot open file for input: " << filename
                  << "\n -> Invalid range specified.\n";
        return bytes_t{};
    }
    // Try to open the file, readonly, binary, seek to end
    constexpr auto omode = std::ios::in | std::ios::binary | std::ios::ate;
    std::ifstream fst(filename, omode);
    if (!fst.is_open()) {
        std::cerr << "[ERROR] Cannot open file for input: " << filename
                  << "\n -> Error opening file.\n";
        return bytes_t{};
    }
    // Get size of the file, ignore <= 0 > end
    const auto filesz = static_cast<std::streamoff>(fst.tellg());
    if (filesz < end) {
        std::cerr << "[ERROR] Cannot open file for input: " << filename
                  << "\n -> Invalid file size!" << '\n';
        return bytes_t{};
    }
    // Read file range into a string buffer
    fst.seekg(std::ios::beg + beg);
    bytes_t buf(static_cast<uint64_t>(bufsz));
    fst.read(reinterpret_cast<char *>(buf.data()), bufsz); // NOLINT
    fst.close();
    return buf;
}

// List operations
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

/*
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
