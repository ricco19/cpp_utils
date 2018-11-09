/*
  pixels.h -- Image constants and generic helper functions
*/
#ifndef PIXELS_H
#define PIXELS_H

#include "utils/system.h"
#include <cstring>

namespace image {

// Supported pixel formats
enum class pxfmt : int { INVALID = -1, RGB, RGBA, GRAY, GREY };
// Returns the number of bytes per pixel component for each format
constexpr int pxfmt_components(const pxfmt fmt) {
    switch (fmt) {
    case pxfmt::GRAY:
    case pxfmt::GREY:
        return 1;
    case pxfmt::RGB:
        return 3;
    case pxfmt::RGBA:
        return 4;
    default:
        break;
    }
    return -1;
}
// Calculate the number of bytes of a given image size and format
// Returns 0 on error
inline unsigned px_bytes(const int w, const int h, const pxfmt fmt) {
    // Maximum dimension for width and height of an image
    constexpr int MAX_DIM = 10000;
    const auto comp = pxfmt_components(fmt);
    if (w > MAX_DIM || h > MAX_DIM || comp < 1) {
        return 0;
    }
    return static_cast<unsigned>(w * h * comp);
}

class pixels {
  public:
    // Default construct - empty buffer and values
    pixels() = default;
    // Format, width, height - allocates buffer
    pixels(const pxfmt fmt, const int w, const int h)
        : format_{fmt}
        , width_{w}
        , height_{h}
        , buf_(px_bytes(w, h, fmt), 0)
        , is_valid_{verify_buf()} {}
    // Moves buffer and verifys
    pixels(utils::bytes &&p, const pxfmt fmt, const int w, const int h)
        : format_{fmt}
        , width_{w}
        , height_{h}
        , buf_{p}
        , is_valid_{verify_buf()} {}
    // Copys buffer and verifys
    pixels(const utils::bytes &p, const pxfmt fmt, const int w, const int h)
        : format_{fmt}
        , width_{w}
        , height_{h}
        , buf_{p}
        , is_valid_{verify_buf()} {}
    // Simple getter functions
    pxfmt format() const { return format_; }
    int width() const { return width_; }
    int height() const { return height_; }
    utils::bytes const &buf() const { return buf_; }
    bool is_valid() const { return is_valid_; }
    // Public functions
    void clear();
    void convert_to(const pxfmt fmt);

  private:
    // Internal variables
    pxfmt format_{pxfmt::INVALID};
    int width_{0};
    int height_{0};
    utils::bytes buf_{};
    bool is_valid_{false};
    // Internal functions
    bool verify_buf();
    pxfmt rgb_to_grey();
    pxfmt rgba_to_grey();
};

// Resets the class back to empty/clean state
inline void pixels::clear() {
    format_ = pxfmt::INVALID;
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
    if (buf_.size() != px_bytes(width_, height_, format_)) {
        clear();
        return false;
    }
    return true;
}

// Pixel conversion delgation
inline void pixels::convert_to(const pxfmt fmt) {
    // No conversion necessary
    if (fmt == format_) {
        return;
    }
    switch (fmt) {
    case pxfmt::GRAY:
    case pxfmt::GREY:
        switch (format_) {
        case pxfmt::RGB:
            format_ = rgb_to_grey();
            break;
        case pxfmt::RGBA:
            format_ = rgba_to_grey();
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    verify_buf();
}

// Pixel conversion -> RGB to GREYSCALE
inline pxfmt pixels::rgb_to_grey() {
    // Since this is encapsulated we are trusting the size of the buffer
    constexpr auto dst_fmt = pxfmt::GREY;
    const auto dst_sz = px_bytes(width_, height_, dst_fmt);
    // RGB to Greyscale algorithm
    for (unsigned i = 0; i < dst_sz; ++i) {
        const auto offs = i * 3;
        const auto r = static_cast<double>(buf_[offs]) * 0.2126;
        const auto g = static_cast<double>(buf_[offs+1]) * 0.7152;
        const auto b = static_cast<double>(buf_[offs+2]) * 0.0722;
        buf_[i] = static_cast<uint8_t>(r + g + b);
    }
    // Trim the rest of the vector
    buf_.resize(dst_sz);
    return dst_fmt;
}

// Pixel conversion -> RGBA to GREYSCALE
inline pxfmt pixels::rgba_to_grey() {
    // Since this is encapsulated we are trusting the size of the buffer
    constexpr auto dst_fmt = pxfmt::GREY;
    const auto dst_sz = px_bytes(width_, height_, dst_fmt);
    // RGBA to Greyscale algorithm
    for (unsigned i = 0; i < dst_sz; ++i) {
        const auto offs = i * 4;
        const auto r = static_cast<double>(buf_[offs]) * 0.2126;
        const auto g = static_cast<double>(buf_[offs+1]) * 0.7152;
        const auto b = static_cast<double>(buf_[offs+2]) * 0.0722;
        const auto lum = static_cast<double>(buf_[offs+3]) / 255.0;
        buf_[i] = static_cast<uint8_t>((r + g + b) * lum);
    }
    // Trim the rest of the vector
    buf_.resize(dst_sz);
    return dst_fmt;
}

} // namespace image

#endif
