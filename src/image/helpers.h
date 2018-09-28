/*
  helpers.h -- Image constants and generic helper functions
*/
#ifndef HELPERS_H
#define HELPERS_H

#include "utils/system.h"
#include <cstring>

namespace image {

enum : int { TYPE_UNKNOWN = -1, TYPE_JPEG, TYPE_TIFF };
static inline int get_image_type(const char *filename) {
    const auto ext = strrchr(filename, '.');
    if (ext == nullptr) {
        return TYPE_UNKNOWN;
    }
    // JPEG
    if ((strncasecmp(ext, ".jpg", 4) == 0) ||
        (strncasecmp(ext, ".jpeg", 5) == 0)) {
        return TYPE_JPEG;
    }
    // TIFF
    if ((strncasecmp(ext, ".tif", 4) == 0) ||
        (strncasecmp(ext, ".tiff", 5) == 0)) {
        return TYPE_TIFF;
    }
    return TYPE_UNKNOWN;
}

enum : int { FORMAT_UNKNOWN = -1, FORMAT_RGB, FORMAT_RGBA, FORMAT_GREY };
static inline constexpr int num_components(const int fmt) {
    switch (fmt) {
    case FORMAT_GREY:
        return 1;
    case FORMAT_RGB:
        return 3;
    case FORMAT_RGBA:
        return 4;
    default:
        break;
    }
    return -1;
}

class pixels_t {
  public:
    // Constructors
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
    int convert_to(const int dst_fmt) const;
    int width() const { return width_; }
    int height() const { return height_; }
    int bytes_per_pixel() const { return bytes_per_pixel_; }
    bool is_valid() const { return is_valid_; }

  private:
    utils::bytearr_t buf_{};
    int format_{FORMAT_UNKNOWN};
    int width_{};
    int height_{};
    int bytes_per_pixel_{};
    bool is_valid_{false};
    bool verify_data() {
        const auto expected_sz =
            static_cast<unsigned>(width_ * height_ * bytes_per_pixel_);
        if (buf_.size() != expected_sz) {
            buf_.clear();
            format_ = FORMAT_UNKNOWN;
            width_ = 0;
            height_ = 0;
            bytes_per_pixel_ = 0;
            return false;
        }
        return true;
    }
};

namespace internal {}

int pixels_t::convert_to(const int fmt) const {
    // No conversion necessary
    if (fmt == format_) {
        return fmt;
    }
    return 0;
}

} // namespace image

#endif
