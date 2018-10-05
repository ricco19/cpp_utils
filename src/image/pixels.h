/*
  helpers.h -- Image constants and generic helper functions
*/
#ifndef HELPERS_H
#define HELPERS_H

#include "utils/system.h"
#include <cstring>

namespace image {

enum : int { PIXFMT_UNKNOWN = -1, PIXFMT_RGB, PIXFMT_RGBA, PIXFMT_GREY };
static inline constexpr int num_components(const int fmt) {
    switch (fmt) {
    case PIXFMT_GREY:
        return 1;
    case PIXFMT_RGB:
        return 3;
    case PIXFMT_RGBA:
        return 4;
    default:
        break;
    }
    return -1;
}

class pixels_t {
  public:
    // Constructors
    pixels_t() = default;
    pixels_t(const utils::bytearr_t &p, const int fmt, const int w, const int h)
        : buf_{p}
        , format_{fmt}
        , width_{w}
        , height_{h}
        , bytes_per_pixel_{num_components(fmt)}
        , is_valid_{verify_data()} {}
    pixels_t(utils::bytearr_t &&p, const int fmt, const int w, const int h)
        : buf_{p}
        , format_{fmt}
        , width_{w}
        , height_{h}
        , bytes_per_pixel_{num_components(fmt)}
        , is_valid_{verify_data()} {}
    // Member functions
    int convert_to(const int dst_fmt);
    int width() const { return width_; }
    int height() const { return height_; }
    int bytes_per_pixel() const { return bytes_per_pixel_; }
    bool is_valid() const { return is_valid_; }
    void clear() {
        buf_.clear();
        format_ = PIXFMT_UNKNOWN;
        width_ = 0;
        height_ = 0;
        bytes_per_pixel_ = 0;
    }

  private:
    utils::bytearr_t buf_{};
    int format_{PIXFMT_UNKNOWN};
    int width_{};
    int height_{};
    int bytes_per_pixel_{};
    bool is_valid_{false};
    bool verify_data() {
        const auto expected_sz =
            static_cast<unsigned>(width_ * height_ * bytes_per_pixel_);
        if (buf_.size() != expected_sz) {
            clear();
            return false;
        }
        return true;
    }
    // Pixel conversion functions
    inline int pc_rgb_to_grey();
};

inline int pixels_t::pc_rgb_to_grey() {
    // Verify the current size of the buffer
    const auto src_sz = static_cast<unsigned>(width_ * height_ * 3);
    if (buf_.size() != src_sz) {
        return -1;
    }
    const int dst_sz = width_ * height_;
    // Average out every 3 pixels
    for (int i = 0; i < dst_sz; ++i) {
        const int pos = i * 3;
        buf_[i] = (buf_[pos] + buf_[pos+1] + buf_[pos+2]) / 3;
    }
    // Trim the rest of the vector
    buf_.resize(dst_sz);
    // Update the image variables
    format_ = PIXFMT_GREY;
    bytes_per_pixel_ = 1;
    return format_;
}

int pixels_t::convert_to(const int fmt){
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



} // namespace image

#endif
