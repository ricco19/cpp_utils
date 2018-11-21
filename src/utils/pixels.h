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
[[nodiscard]] constexpr int
pxfmt_components(const Pixel_Format fmt) noexcept {
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
[[nodiscard]] constexpr unsigned
pixels_pitch(const int w, const Pixel_Format fmt) noexcept {
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
[[nodiscard]] constexpr unsigned
pixels_size(int w, int h, Pixel_Format fmt) noexcept {
    if (h > PIXELS_MAX_DIM) {
        return 0;
    }
    return static_cast<unsigned>(h) * pixels_pitch(w, fmt);
}

// Pixels class, holds pixel buffer and conversion functions
class pixel_data {
  public:
    // Default construct - empty buffer and values
    pixel_data() = default;
    // Format, width, height - allocates buffer
    pixel_data(Pixel_Format fmt, int w, int h)
        : buf(pixels_size(w, h, fmt), 0)
        , format_{fmt}
        , width_{w}
        , height_{h}
        , is_valid_{verify_buf()} {}
    // Moves buffer and verifys
    pixel_data(bytes_t &&p, Pixel_Format fmt, int w, int h)
        : buf{p}
        , format_{fmt}
        , width_{w}
        , height_{h}
        , is_valid_{verify_buf()} {}
    // Copys buffer and verifys
    pixel_data(bytes_t p, Pixel_Format fmt, int w, int h)
        : buf{std::move(p)}
        , format_{fmt}
        , width_{w}
        , height_{h}
        , is_valid_{verify_buf()} {}
    // Simple getter functions
    [[nodiscard]] Pixel_Format format() const noexcept { return format_; }
    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }
    [[nodiscard]] bool is_valid() const noexcept { return is_valid_; }
    // Public functions
    void clear();
    void convert_to(Pixel_Format fmt);
    bytes_t buf{};

  private:
    // Internal variables
    Pixel_Format format_{Pixel_Format::Unknown};
    int width_{0};
    int height_{0};
    bool is_valid_{false};
    // Internal functions
    bool verify_buf();
    Pixel_Format rgb_to_grey();
    Pixel_Format rgba_to_grey();
};

// Resets the class back to empty/clean state
inline void pixel_data::clear() {
    format_ = Pixel_Format::Unknown;
    width_ = 0;
    height_ = 0;
    is_valid_ = false;
    buf.clear();
}

// Verifies the size of the buffer
inline bool pixel_data::verify_buf() {
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

// Pixel conversion delgation
inline void pixel_data::convert_to(Pixel_Format fmt) {
    // No conversion necessary
    if (fmt == format_) {
        return;
    }
    switch (fmt) {
    case Pixel_Format::GRAY:
        switch (format_) {
        case Pixel_Format::RGB:
            format_ = rgb_to_grey();
            break;
        case Pixel_Format::RGBA:
            format_ = rgba_to_grey();
            break;
        case Pixel_Format::GRAY:
        case Pixel_Format::Unknown:
        default:
            break;
        }
        break;
    case Pixel_Format::RGB:
    case Pixel_Format::RGBA:
    case Pixel_Format::Unknown:
    default:
        break;
    }
    verify_buf();
}

// Pixel conversion -> RGB to GREYSCALE
inline Pixel_Format pixel_data::rgb_to_grey() {
    // Since this is encapsulated we are trusting the size of the buffer
    constexpr auto dst_fmt = Pixel_Format::GRAY;
    const auto dst_sz = pixels_size(width_, height_, dst_fmt);
    // RGB to Greyscale algorithm
    for (unsigned i = 0; i < dst_sz; ++i) {
        const auto offs = i * 3;
        const auto r = static_cast<double>(buf[offs]) * 0.2126;
        const auto g = static_cast<double>(buf[offs + 1]) * 0.7152;
        const auto b = static_cast<double>(buf[offs + 2]) * 0.0722;
        buf[i] = static_cast<uint8_t>(r + g + b);
    }
    // Trim the rest of the vector
    buf.resize(dst_sz);
    return dst_fmt;
}

// Pixel conversion -> RGBA to GREYSCALE
inline Pixel_Format pixel_data::rgba_to_grey() {
    // Since this is encapsulated we are trusting the size of the buffer
    constexpr auto dst_fmt = Pixel_Format::GRAY;
    const auto dst_sz = pixels_size(width_, height_, dst_fmt);
    // RGBA to Greyscale algorithm
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

} // namespace utils

#endif
