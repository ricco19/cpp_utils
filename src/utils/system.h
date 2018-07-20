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
#include "utils/utf8conv_win32.h"
#include <windows.h>
#else
#include <cstring>
#include <sys/stat.h>
#endif

namespace utils {

using bytearr_t = std::vector<uint8_t>;
using list_t = std::vector<std::string>;
static constexpr int MAX_FILE_READ_SZ = 10'485'760;
enum : int { FINFO_NOEXIST, FINFO_IS_FILE, FINFO_IS_DIR, FINFO_IS_LINK };
enum : int { DL_SHOW_HIDDEN = 0x01, DL_RECURSE = 0x02 };
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

inline bytearr_t file_binread(const char *filename,
                              const int max_size = MAX_FILE_READ_SZ) {
    // Try to open the file
    constexpr std::ios_base::openmode open_mode =
        std::ios::in | std::ios::binary | std::ios::ate;
    std::ifstream fst(filename, open_mode);
    if (!fst.is_open()) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Error opening file.\n";
        return bytearr_t{};
    }
    // Get size of the file, ignore <= 0 > max_size
    const auto filesize = static_cast<int64_t>(fst.tellg());
    if (filesize <= 0 || filesize > max_size) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Invalid file size (" << filesize
                  << ")! Max = " << max_size << '\n';
        return bytearr_t{};
    }
    // Read entire file into a string buffer
    fst.seekg(std::ios::beg);
    bytearr_t buf(static_cast<unsigned>(filesize));
    fst.read(reinterpret_cast<char *>(&buf[0]), filesize);
    fst.close();
    return buf;
}

class binread {
  public:
    binread() = delete;
    binread(const char *file)
        : buf_{file_binread(file)} {}
    bytearr_t buf_;
    size_t size() const { return buf_.size(); }
    uint8_t read_8u(const unsigned offset) const;
    uint16_t read_16u(const unsigned offset, const bool bswap) const;
    uint32_t read_32u(const unsigned offset, const bool bswap) const;
    uint64_t read_64u(const unsigned offset, const bool bswap) const;
    int8_t read_8s(const unsigned offset) const;
    int16_t read_16s(const unsigned offset, const bool bswap) const;
    int32_t read_32s(const unsigned offset, const bool bswap) const;
    int64_t read_64s(const unsigned offset, const bool bswap) const;
    std::string read_str(const unsigned offset, const unsigned len) const;

  private:
};

template <class IntType>
IntType brtest(const std::vector<uint8_t> &buf, const unsigned offset,
               const bool bswap = false) {
    const auto sz = sizeof(IntType);
    if (offset + sz > buf.size()) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    const auto val = *reinterpret_cast<const IntType *>(&buf[offset]);
    if (bswap) {
        switch (sz) {
       /* case 8:
            return (((val & 0xFF00000000000000) >> 56) |
                    ((val & 0x00FF000000000000) >> 40) |
                    ((val & 0x0000FF0000000000) >> 24) |
                    ((val & 0x000000FF00000000) >> 8) |
                    ((val & 0x00000000FF000000) << 8) |
                    ((val & 0x0000000000FF0000) << 24) |
                    ((val & 0x000000000000FF00) << 40) |
                    ((val & 0x00000000000000FF) << 56));*/
        case 4:
            return (((val & 0xFF000000) >> 24) | ((val & 0x00FF0000) >> 8) |
                    ((val & 0x0000FF00) << 8) | ((val & 0x000000FF) << 24));
        case 2:
            return static_cast<IntType>(((val & 0x00FF) >> 8) | (val << 8));
        case 1:
        default:
            break;
        }
    }
    return val;
}
// binread<typename>(std::vector<uint8_t>, offset)

uint8_t binread::read_8u(const unsigned offset) const {
    if (offset > buf_.size() - sizeof(uint8_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    return buf_[offset];
}

int8_t binread::read_8s(const unsigned offset) const {
    if (offset > buf_.size() - sizeof(int8_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    return buf_[offset];
}

uint16_t binread::read_16u(const unsigned offset,
                           const bool bswap = false) const {
    if (offset > buf_.size() - sizeof(uint16_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    const auto val = reinterpret_cast<const uint16_t *>(&buf_[offset])[0];
    if (bswap) {
        return static_cast<uint16_t>(((val & 0x00FF) >> 8) | (val << 8));
    }
    return val;
}

int16_t binread::read_16s(const unsigned offset,
                          const bool bswap = false) const {
    if (offset > buf_.size() - sizeof(int16_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    const auto val = reinterpret_cast<const int16_t *>(&buf_[offset])[0];
    if (bswap) {
        return static_cast<int16_t>(((val & 0x00FF) >> 8) | (val << 8));
    }
    return val;
}

uint32_t binread::read_32u(const unsigned offset,
                           const bool bswap = false) const {
    if (offset > buf_.size() - sizeof(uint32_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    const auto val = reinterpret_cast<const uint32_t *>(&buf_[offset])[0];
    if (bswap) {
        return (((val & 0xFF000000) >> 24) | ((val & 0x00FF0000) >> 8) |
                ((val & 0x0000FF00) << 8) | ((val & 0x000000FF) << 24));
    }
    return val;
}

int32_t binread::read_32s(const unsigned offset,
                          const bool bswap = false) const {
    if (offset > buf_.size() - sizeof(int32_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    const auto val = reinterpret_cast<const int32_t *>(&buf_[offset])[0];
    if (bswap) {
        return (((val & 0xFF000000) >> 24) | ((val & 0x00FF0000) >> 8) |
                ((val & 0x0000FF00) << 8) | ((val & 0x000000FF) << 24));
    }
    return val;
}

uint64_t binread::read_64u(const unsigned offset,
                           const bool bswap = false) const {
    if (offset > buf_.size() - sizeof(uint64_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    const auto val = reinterpret_cast<const uint64_t *>(&buf_[offset])[0];
    if (bswap) {
        return (((val & 0xFF00000000000000) >> 56) |
                ((val & 0x00FF000000000000) >> 40) |
                ((val & 0x0000FF0000000000) >> 24) |
                ((val & 0x000000FF00000000) >> 8) |
                ((val & 0x00000000FF000000) << 8) |
                ((val & 0x0000000000FF0000) << 24) |
                ((val & 0x000000000000FF00) << 40) |
                ((val & 0x00000000000000FF) << 56));
    }
    return val;
}

int64_t binread::read_64s(const unsigned offset,
                          const bool bswap = false) const {
    if (offset > buf_.size() - sizeof(int64_t)) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    const auto val = reinterpret_cast<const int64_t *>(&buf_[offset])[0];
    if (bswap) {
        return (((val & 0xFF00000000000000) >> 56) |
                ((val & 0x00FF000000000000) >> 40) |
                ((val & 0x0000FF0000000000) >> 24) |
                ((val & 0x000000FF00000000) >> 8) |
                ((val & 0x00000000FF000000) << 8) |
                ((val & 0x0000000000FF0000) << 24) |
                ((val & 0x000000000000FF00) << 40) |
                ((val & 0x00000000000000FF) << 56));
    }
    return val;
}

std::string binread::read_str(const unsigned offset, const unsigned len) const {
    if (offset > buf_.size() - len) {
        std::cerr << "[ERROR] Cannot binary read data, would overflow!\n";
        return 0;
    }
    std::string str;
    str.reserve(len);
    str.append(reinterpret_cast<const char *>(&buf_[offset]), len);
    return str;
}

inline std::string file_to_string(const char *filename) {
    // Try to open the file
    constexpr std::ios_base::openmode open_mode =
        std::ios::in | std::ios::binary | std::ios::ate;
    std::ifstream fst(filename, open_mode);
    if (!fst.is_open()) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Error opening file.\n";
        return std::string{};
    }
    // Get size of the file, ignore <= 0 > MAX_FILE_READ_SZ
    const auto filesize = static_cast<int64_t>(fst.tellg());
    if (filesize <= 0 || filesize > MAX_FILE_READ_SZ) {
        std::cerr << "Cannot open file for input: " << filename
                  << "\n -> Invalid file size (" << filesize
                  << ")! Max = " << MAX_FILE_READ_SZ << '\n';
        return std::string{};
    }
    // Read entire file into a string buffer
    fst.seekg(std::ios::beg);
    std::string buf(static_cast<unsigned>(filesize), 0);
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
inline int get_file_info(const char *path) noexcept {
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
inline list_t list_dir(const char *path, const int opt = 0) {
    // Check for NULL
    list_t list{};
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
