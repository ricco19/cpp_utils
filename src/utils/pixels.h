/*
  pixels.h -- Image constants and generic helper functions
*/
#ifndef PIXELS_H
#define PIXELS_H

#include "utils/system.h"
#include <cstring>

namespace utils {

// Supported pixel formats
enum class Pixel_Format { Unknown = -1, RGB, RGBA, GRAY };
constexpr std::ostream &operator<<(std::ostream &os, const Pixel_Format fmt) {
    switch (fmt) {
    default:
    case Pixel_Format::Unknown:
        os << "Unknown";
        break;
    case Pixel_Format::RGB:
        os << "RGB";
        break;
    case Pixel_Format::RGBA:
        os << "RGBA";
        break;
    case Pixel_Format::GRAY:
        os << "Grayscale";
        break;
    }
    return os;
}

//
// Pixel calculations
//
// Maximum dimension for width and height of an image
constexpr int PIXELS_MAX_DIM = 10000;
// Get the number of bytes per pixel (components) for each format
// Returns -1 if invalid format
[[nodiscard]] constexpr int pxfmt_components(const Pixel_Format fmt) noexcept {
    switch (fmt) {
    default:
    case Pixel_Format::Unknown:
        break;
    case Pixel_Format::RGB:
        return 3;
    case Pixel_Format::RGBA:
        return 4;
    case Pixel_Format::GRAY:
        return 1;
    }
    return -1;
}
// Calculates the number of bytes in a single row of an image
// Returns 0 on error
[[nodiscard]] constexpr unsigned pixels_pitch(const int w,
                                              const Pixel_Format fmt) noexcept {
    if (w > PIXELS_MAX_DIM) {
        return 0;
    }
    const auto comp = pxfmt_components(fmt);
    if (comp < 0) {
        return 0;
    }
    return static_cast<unsigned>(w * comp);
}
// Calculates the total number of bytes in an image
// Returns 0 on error
[[nodiscard]] constexpr unsigned pixels_size(const int w, const int h,
                                             const Pixel_Format fmt) noexcept {
    if (h > PIXELS_MAX_DIM) {
        return 0;
    }
    return static_cast<unsigned>(h) * pixels_pitch(w, fmt);
}

// Pixels class, holds pixel buffer and conversion functions
class Pixels {
  public:
    // Default construct - empty buffer and values
    Pixels() = default;

    // Constructor -> Format, width, height - allocates buffer
    Pixels(const Pixel_Format fmt, const int w, const int h)
        : buf(pixels_size(w, h, fmt), 0)
        , format_{fmt}
        , width_{w}
        , height_{h}
        , is_valid_{verify_buf()} {}

    // Constructor -> Moves buffer and verifys
    Pixels(bytes_t &&p, const Pixel_Format fmt, const int w, const int h)
        : buf{p}
        , format_{fmt}
        , width_{w}
        , height_{h}
        , is_valid_{verify_buf()} {}

    // Constructor -> Copys buffer and verifys
    Pixels(bytes_t p, const Pixel_Format fmt, const int w, const int h)
        : buf{std::move(p)}
        , format_{fmt}
        , width_{w}
        , height_{h}
        , is_valid_{verify_buf()} {}

    bytes_t buf{};

    // Simple getters
    Pixel_Format format() const noexcept { return format_; }
    int width() const noexcept { return width_; }
    int height() const noexcept { return height_; }
    bool is_valid() const noexcept { return is_valid_; }

    void set_format(const Pixel_Format fmt) noexcept { format_ = fmt; };

    // Resets the class back to empty/clean state
    void clear() {
        format_ = Pixel_Format::Unknown;
        width_ = 0;
        height_ = 0;
        is_valid_ = false;
        buf.clear();
    }

    // Convert pixel formats
    void convert_to(const Pixel_Format fmt) {
        // No conversion necessary
        if ((fmt == format_) || buf.empty()) {
            return;
        }
        switch (fmt) {
        case Pixel_Format::GRAY:
            switch (format_) {
            case Pixel_Format::RGB:
                format_ = rgb_to_gray();
                break;
            case Pixel_Format::RGBA:
                format_ = rgba_to_gray();
                break;
            case Pixel_Format::GRAY:
            case Pixel_Format::Unknown:
            default:
                break;
            }
            break;
        case Pixel_Format::RGB:
            switch (format_) {
            case Pixel_Format::GRAY:
                format_ = gray_to_rgb();
                break;
            case Pixel_Format::RGBA:
            case Pixel_Format::RGB:
            case Pixel_Format::Unknown:
            default:
                break;
            }
            break;
        case Pixel_Format::RGBA:
            switch (format_) {
            case Pixel_Format::GRAY:
                format_ = gray_to_rgba();
                break;
            case Pixel_Format::RGBA:
            case Pixel_Format::RGB:
            case Pixel_Format::Unknown:
            default:
                break;
            }
            break;
        case Pixel_Format::Unknown:
        default:
            break;
        }
        verify_buf();
    }

  private:
    Pixel_Format format_{Pixel_Format::Unknown};
    int width_{0};
    int height_{0};
    bool is_valid_{false};

    // Verifies the size of the buffer
    bool verify_buf() {
        if (buf.empty()) {
            clear();
            return false;
        }
        if (buf.size() != pixels_size(width_, height_, format_)) {
            clear();
            return false;
        }
        return true;
    }

    // Pixel conversion -> RGB to GRAYSCALE
    Pixel_Format rgb_to_gray() {
        // Get number of components of source pixels
        constexpr auto src_fmt = Pixel_Format::RGB;
        constexpr auto comps = pxfmt_components(src_fmt);

        std::cout << "beg: ";
        for (uint64_t i = 0; i < 12; ++i) {
            std::cout << static_cast<int>(buf[i]) << ',';
        }
        std::cout << '\n';
        std::cout << "end: ";
        for (uint64_t i = buf.size() - 12; i < buf.size(); ++i) {
            std::cout << static_cast<int>(buf[i]) << ',';
        }
        std::cout << '\n';

        // Loop over all components and flatten to a single GRAY component
        auto dst_it = std::begin(buf);
        for (auto it = std::cbegin(buf); it < std::cend(buf); it += comps) {
            const auto r = static_cast<double>(*it) * 0.2126;
            const auto g = static_cast<double>(*(it + 1)) * 0.7152;
            const auto b = static_cast<double>(*(it + 2)) * 0.0722;
            *dst_it = static_cast<uint8_t>(r + g + b);
            dst_it++;
        }

        // Calculate new size and trim
        constexpr auto dst_fmt = Pixel_Format::GRAY;
        const auto dst_sz = pixels_size(width_, height_, dst_fmt);
        buf.resize(dst_sz);

        std::cout << "beg: ";
        for (uint64_t i = 0; i < 4; ++i) {
            std::cout << static_cast<int>(buf[i]) << ',';
        }
        std::cout << '\n';
        std::cout << "end: ";
        for (uint64_t i = buf.size() - 4; i < buf.size(); ++i) {
            std::cout << static_cast<int>(buf[i]) << ',';
        }
        std::cout << '\n';

        return dst_fmt;
    }

    // Pixel conversion -> RGBA to GRAYSCALE
    Pixel_Format rgba_to_gray() {
        // Since this is encapsulated we are trusting the size of the buffer
        constexpr auto dst_fmt = Pixel_Format::GRAY;
        const auto dst_sz = pixels_size(width_, height_, dst_fmt);
        // RGBA to Grayscale algorithm
        for (unsigned i = 0; i < dst_sz; ++i) {
            const auto offs = i * 4;
            const auto r = static_cast<double>(buf[offs]) * 0.2126;
            const auto g = static_cast<double>(buf[offs + 1]) * 0.7152;
            const auto b = static_cast<double>(buf[offs + 2]) * 0.0722;
            const auto lum = static_cast<double>(buf[offs + 3]) / 255.0;
            buf[i] = static_cast<uint8_t>((r + g + b) * lum);
        }
        // Trim the rest of the vector
        buf.resize(dst_sz);
        return dst_fmt;
    }

    // Pixel conversion -> GRAYSCALE to RGB
    Pixel_Format gray_to_rgb() {
        // Calculate the new size of the buffer and resize it
        constexpr auto dst_fmt = Pixel_Format::RGB;
        const auto dst_sz = pixels_size(width_, height_, dst_fmt);
        buf.resize(dst_sz);

        // Get an const iterator to the end of the current data
        constexpr auto src_fmt = Pixel_Format::GRAY;
        const auto src_sz = pixels_size(width_, height_, src_fmt);
        auto src_end_it = std::cbegin(buf) + src_sz - 1;

        // Fill in the new back of the buffer
        constexpr auto comps = pxfmt_components(dst_fmt);
        for (auto it = std::rbegin(buf); it != std::rend(buf); it += comps) {
            std::fill(it, it + comps, *src_end_it);
            --src_end_it;
        }
        return dst_fmt;
    }

    // Pixel conversion -> GRAYSCALE to RGBA
    Pixel_Format gray_to_rgba() {
        // Calculate the new size of the buffer and resize it
        constexpr auto dst_fmt = Pixel_Format::RGBA;
        const auto dst_sz = pixels_size(width_, height_, dst_fmt);
        buf.resize(dst_sz);

        // std::cout << "beg: ";
        // for (uint64_t i = 0; i < 4; ++i) {
        //     std::cout << static_cast<int>(buf[i]) << ',';
        // }
        // std::cout << '\n';
        // std::cout << "end: ";
        // for (uint64_t i = buf.size() - 4; i < buf.size(); ++i) {
        //     std::cout << static_cast<int>(buf[i]) << ',';
        // }
        // std::cout << '\n';

        // Get an const iterator to the end of the current data
        constexpr auto src_fmt = Pixel_Format::GRAY;
        const auto src_sz = pixels_size(width_, height_, src_fmt);
        auto src_end_it = std::cbegin(buf) + src_sz - 1;

        // Fill in the back
        constexpr auto comps = pxfmt_components(dst_fmt);
        for (auto it = std::rbegin(buf); it < std::rend(buf); it += comps) {
            *it = 0xFF;
            std::fill(it + 1, it + comps, *src_end_it);
            --src_end_it;
        }

        // std::cout << "beg: ";
        // for (uint64_t i = 0; i < 16; ++i) {
        //     std::cout << static_cast<int>(buf[i]) << ',';
        // }
        // std::cout << '\n';
        // std::cout << "end: ";
        // for (uint64_t i = buf.size() - 16; i < buf.size(); ++i) {
        //     std::cout << static_cast<int>(buf[i]) << ',';
        // }
        // std::cout << '\n';

        return dst_fmt;
    }
};

bytes_t pc_rgb_to_gray_dry(const bytes_t &srcbuf) {
    // Get number of components of source pixels
    constexpr auto src_fmt = Pixel_Format::RGB;
    constexpr auto src_comps = pxfmt_components(src_fmt);

    // We can safely cast to a signed integer here
    const auto src_size = static_cast<int>(srcbuf.size());

    // Make sure we have at least 1 pixel
    if (src_size < src_comps) {
        return bytes_t{};
    }

    // Get the size of the destination vector and reserve memory
    constexpr auto dst_fmt = Pixel_Format::GRAY;
    constexpr auto dst_comps = pxfmt_components(dst_fmt);
    const auto dst_size = (src_size / src_comps) * dst_comps;
    bytes_t dstbuf(static_cast<unsigned>(dst_size), 0);

    return dstbuf;
}

bytes_t pc_rgb_to_gray_dry2(const bytes_t &srcbuf) {
    // Get number of components of source pixels
    constexpr auto src_fmt = Pixel_Format::RGB;
    constexpr auto src_comps = pxfmt_components(src_fmt);

    // We can safely cast to a signed integer here
    const auto src_size = static_cast<int>(srcbuf.size());

    // Make sure we have at least 1 pixel
    if (src_size < src_comps) {
        return bytes_t{};
    }

    // Get the size of the destination vector and reserve memory
    constexpr auto dst_fmt = Pixel_Format::GRAY;
    constexpr auto dst_comps = pxfmt_components(dst_fmt);
    const auto dst_size = (src_size / src_comps) * dst_comps;
    bytes_t dstbuf(static_cast<unsigned>(dst_size));

    return dstbuf;
}

bytes_t pc_rgb_to_gray(const bytes_t &srcbuf) {
    // Get number of components of source pixels
    constexpr auto src_fmt = Pixel_Format::RGB;
    constexpr auto src_comps = pxfmt_components(src_fmt);

    // We can safely cast to a signed integer here
    const auto src_size = static_cast<int>(srcbuf.size());

    // Make sure we have at least 1 pixel
    if (src_size < src_comps) {
        return bytes_t{};
    }

    // Get the size of the destination vector and 0 initialize
    constexpr auto dst_fmt = Pixel_Format::GRAY;
    constexpr auto dst_comps = pxfmt_components(dst_fmt);
    const auto dst_size = (src_size / src_comps) * dst_comps;
    bytes_t dstbuf(static_cast<unsigned>(dst_size), 0);

    // std::cout << "beg: ";
    // for (uint64_t i = 0; i < 12; ++i) {
    //     std::cout << static_cast<int>(srcbuf[i]) << ',';
    // }
    // std::cout << '\n';
    // std::cout << "end: ";
    // for (uint64_t i = srcbuf.size() - 12; i < srcbuf.size(); ++i) {
    //     std::cout << static_cast<int>(srcbuf[i]) << ',';
    // }
    // std::cout << '\n';

    // Loop over all components and flatten to a single GRAY component
    auto dst_it = std::begin(dstbuf);
    for (auto it = std::cbegin(srcbuf); it < std::cend(srcbuf);
         it += src_comps) {
        const auto r = static_cast<double>(*it) * 0.2126;
        const auto g = static_cast<double>(*(it + 1)) * 0.7152;
        const auto b = static_cast<double>(*(it + 2)) * 0.0722;
        *dst_it = static_cast<uint8_t>(r + g + b);
        dst_it++;
    }

    // std::cout << "beg: ";
    // for (uint64_t i = 0; i < 4; ++i) {
    //     std::cout << static_cast<int>(dstbuf[i]) << ',';
    // }
    // std::cout << '\n';
    // std::cout << "end: ";
    // for (uint64_t i = dstbuf.size() - 4; i < dstbuf.size(); ++i) {
    //     std::cout << static_cast<int>(dstbuf[i]) << ',';
    // }
    // std::cout << '\n';

    return dstbuf;
}

bytes_t pc_rgb_to_gray2(const bytes_t &srcbuf) {
    // Get number of components of source pixels
    constexpr auto src_fmt = Pixel_Format::RGB;
    constexpr auto src_comps = pxfmt_components(src_fmt);

    // We can safely cast to a signed integer here
    const auto src_size = static_cast<int>(srcbuf.size());

    // Make sure we have at least 1 pixel
    if (src_size < src_comps) {
        return bytes_t{};
    }

    // Get the size of the destination vector and 0 initialize
    constexpr auto dst_fmt = Pixel_Format::GRAY;
    constexpr auto dst_comps = pxfmt_components(dst_fmt);
    const auto dst_size = (src_size / src_comps) * dst_comps;
    bytes_t dstbuf(static_cast<unsigned>(dst_size), 0);

    // std::cout << "beg: ";
    // for (uint64_t i = 0; i < 12; ++i) {
    //     std::cout << static_cast<int>(srcbuf[i]) << ',';
    // }
    // std::cout << '\n';
    // std::cout << "end: ";
    // for (uint64_t i = srcbuf.size() - 12; i < srcbuf.size(); ++i) {
    //     std::cout << static_cast<int>(srcbuf[i]) << ',';
    // }
    // std::cout << '\n';

    // Loop over all components and flatten to a single GRAY component
    auto dst_it = std::begin(dstbuf);
    for (auto it = std::cbegin(srcbuf); it < std::cend(srcbuf);
         it += src_comps) {
        const auto avg = std::accumulate(it, it + src_comps, 0) / src_comps;
        *dst_it = static_cast<uint8_t>(avg);
        dst_it++;
    }

    // std::cout << "beg: ";
    // for (uint64_t i = 0; i < 4; ++i) {
    //     std::cout << static_cast<int>(dstbuf[i]) << ',';
    // }
    // std::cout << '\n';
    // std::cout << "end: ";
    // for (uint64_t i = dstbuf.size() - 4; i < dstbuf.size(); ++i) {
    //     std::cout << static_cast<int>(dstbuf[i]) << ',';
    // }
    // std::cout << '\n';

    return dstbuf;
}

} // namespace utils

#endif
