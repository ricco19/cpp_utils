/*
  pixels.h -- Image constants and generic helper functions
*/
#ifndef PIXELS_H
#define PIXELS_H

#include "utils/system.h"
#include <cstring>

namespace utils {

// Maximum dimension for width and height of an image
constexpr int PIXELS_MAX_DIM = 10000;

// Supported pixel formats
enum class pixel_format { invalid = -1, rgb, rgba, gray, grey };

// Get the number of bytes per pixel (components) for each format
// Returns -1 if invalid format
[[nodiscard]] constexpr int pxfmt_components(pixel_format fmt) noexcept {
    switch (fmt) {
    default:
    case pixel_format::invalid:
        break;
    case pixel_format::rgb:
        return 3;
    case pixel_format::rgba:
        return 4;
    case pixel_format::gray:
    case pixel_format::grey:
        return 1;
    }
    return -1;
}

// Calculates the number of bytes in a single row of an image
// Returns 0 on error
[[nodiscard]]
constexpr unsigned pixels_pitch(int w, pixel_format fmt) noexcept {
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
[[nodiscard]]
constexpr unsigned pixels_size(int w, int h, pixel_format fmt) noexcept {
    if (h > PIXELS_MAX_DIM) {
        return 0;
    }
    return static_cast<unsigned>(h) * pixels_pitch(w, fmt);
}

// Pixels class, holds pixel buffer and conversion functions
class pixels {
  public:
    // Default construct - empty buffer and values
    pixels() = default;
    // Format, width, height - allocates buffer
    pixels(pixel_format fmt, int w, int h)
        : format_{fmt}
        , width_{w}
        , height_{h}
        , buf_(pixels_size(w, h, fmt), 0)
        , is_valid_{verify_buf()} {}
    // Moves buffer and verifys
    pixels(bytes_t &&p, pixel_format fmt, int w, int h)
        : format_{fmt}
        , width_{w}
        , height_{h}
        , buf_{p}
        , is_valid_{verify_buf()} {}
    // Copys buffer and verifys
    pixels(bytes_t p, pixel_format fmt, int w, int h)
        : format_{fmt}
        , width_{w}
        , height_{h}
        , buf_{std::move(p)}
        , is_valid_{verify_buf()} {}
    // Simple getter functions
    [[nodiscard]] pixel_format format() const noexcept { return format_; }
    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }
    [[nodiscard]] bytes_t const &buf() const noexcept { return buf_; }
    [[nodiscard]] bool is_valid() const noexcept { return is_valid_; }
    // Public functions
    void clear();
    void convert_to(pixel_format fmt);

  private:
    // Internal variables
    pixel_format format_{pixel_format::invalid};
    int width_{0};
    int height_{0};
    bytes_t buf_{};
    bool is_valid_{false};
    // Internal functions
    bool verify_buf();
    pixel_format rgb_to_grey();
    pixel_format rgba_to_grey();
};

// Resets the class back to empty/clean state
inline void pixels::clear() {
    format_ = pixel_format::invalid;
    width_ = 0;
    height_ = 0;
    is_valid_ = false;
    buf_.clear();
}

// Verifies the size of the buffer
inline bool pixels::verify_buf() {
    if (buf_.empty()) {
        clear();
        return false;
    }
    if (buf_.size() != pixels_size(width_, height_, format_)) {
        clear();
        return false;
    }
    return true;
}

// Pixel conversion delgation
inline void pixels::convert_to(pixel_format fmt) {
    // No conversion necessary
    if (fmt == format_) {
        return;
    }
    switch (fmt) {
    case pixel_format::gray:
    case pixel_format::grey:
        switch (format_) {
        case pixel_format::rgb:
            format_ = rgb_to_grey();
            break;
        case pixel_format::rgba:
            format_ = rgba_to_grey();
            break;
        case pixel_format::gray:
        case pixel_format::grey:
        case pixel_format::invalid:
        default:
            break;
        }
        break;
    case pixel_format::rgb:
    case pixel_format::rgba:
    case pixel_format::invalid:
    default:
        break;
    }
    verify_buf();
}

// Pixel conversion -> RGB to GREYSCALE
inline pixel_format pixels::rgb_to_grey() {
    // Since this is encapsulated we are trusting the size of the buffer
    constexpr auto dst_fmt = pixel_format::grey;
    const auto dst_sz = pixels_size(width_, height_, dst_fmt);
    // RGB to Greyscale algorithm
    for (unsigned i = 0; i < dst_sz; ++i) {
        const auto offs = i * 3;
        const auto r = static_cast<double>(buf_[offs]) * 0.2126;
        const auto g = static_cast<double>(buf_[offs + 1]) * 0.7152;
        const auto b = static_cast<double>(buf_[offs + 2]) * 0.0722;
        buf_[i] = static_cast<uint8_t>(r + g + b);
    }
    // Trim the rest of the vector
    buf_.resize(dst_sz);
    return dst_fmt;
}

// Pixel conversion -> RGBA to GREYSCALE
inline pixel_format pixels::rgba_to_grey() {
    // Since this is encapsulated we are trusting the size of the buffer
    constexpr auto dst_fmt = pixel_format::grey;
    const auto dst_sz = pixels_size(width_, height_, dst_fmt);
    // RGBA to Greyscale algorithm
    for (unsigned i = 0; i < dst_sz; ++i) {
        const auto offs = i * 4;
        const auto r = static_cast<double>(buf_[offs]) * 0.2126;
        const auto g = static_cast<double>(buf_[offs + 1]) * 0.7152;
        const auto b = static_cast<double>(buf_[offs + 2]) * 0.0722;
        const auto lum = static_cast<double>(buf_[offs + 3]) / 255.0;
        buf_[i] = static_cast<uint8_t>((r + g + b) * lum);
    }
    // Trim the rest of the vector
    buf_.resize(dst_sz);
    return dst_fmt;
}

} // namespace utils

#endif
