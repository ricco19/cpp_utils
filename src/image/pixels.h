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

  private:
    // Internal variables
    pxfmt format_{pxfmt::INVALID};
    int width_{0};
    int height_{0};
    utils::bytes buf_{};
    bool is_valid_{false};

    // Internal functions
    void clear();
    bool verify_buf();
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

/*
inline int pixels::convert_to(const int fmt) {
    // No conversion necessary
    if (fmt == format_) {
        return format_;
    }
    switch (fmt) {
    case PIXFMT_GREY:
        switch (format_) {
        case PIXFMT_RGB:
            return pc_rgb_to_grey();
        default:
            break;
        }
        break;
    default:
        break;
    }
    // No conversion was done
    return format_;
}

inline int pixels::pc_rgb_to_grey() {
    // Verify the current size of the buffer
    const auto src_sz = static_cast<unsigned>(width_ * height_ * 3);
    if (buf.size() != src_sz) {
        return -1;
    }
    const int dst_sz = width_ * height_;
    // Average out every 3 pixels
    for (int i = 0; i < dst_sz; ++i) {
        const int pos = i * 3;
        buf[i] = (buf[pos] + buf[pos + 1] + buf[pos + 2]) / 3;
    }
    // Trim the rest of the vector
    buf.resize(dst_sz);
    // Update the image variables
    format_ = PIXFMT_GREY;
    bytes_per_pixel_ = 1;
    return format_;
}
*/
} // namespace image

#endif
