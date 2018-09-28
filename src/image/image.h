/*
  image.h -- Generic image reading
*/
#ifndef IMAGE_H
#define IMAGE_H

#include "image/helpers.h"
#include "image/jpeg.h"
#include "image/tiff.h"
#include "utils/system.h"
#include <cstdint>

namespace image {

class load_image {
  public:
    // Constructors
    explicit load_image(const char *filename)
        : type_{get_image_type(filename)} {
        switch (type_) {
        case TYPE_JPEG:
            is_image_ = jpeg_.load(filename);
            break;
        case TYPE_TIFF:
            is_image_ = tiff_.load(filename);
            break;
        case TYPE_UNKNOWN:
        default:
            is_image_ = false;
            break;
        }
    }
    ~load_image() {}
    // Get rid of all other constructors
    load_image() = delete;
    load_image(load_image &&) = delete;
    load_image(load_image &) = delete;
    load_image &operator=(const load_image &) = delete;
    load_image &operator=(load_image &&) = delete;
    // Member functions
    bool is_image() const { return is_image_; }
    int width() const {
        switch (type_) {
        case TYPE_TIFF:
            return static_cast<int>(tiff_.width());
        case TYPE_JPEG:
            return jpeg_.width();
        case TYPE_UNKNOWN:
        default:
            break;
        }
        return 0;
    }
    int height() const {
        switch (type_) {
        case TYPE_TIFF:
            return static_cast<int>(tiff_.height());
        case TYPE_JPEG:
            return jpeg_.height();
        case TYPE_UNKNOWN:
        default:
            break;
        }
        return 0;
    }
    utils::bytearr_t get_pixels(const int fmt) const {
        switch (type_) {
        case TYPE_TIFF:
            return tiff_.get_pixels();
        case TYPE_JPEG:
            return jpeg_.get_pixels(fmt);
        case TYPE_UNKNOWN:
        default:
            break;
        }
        return utils::bytearr_t{};
    };

  private:
    bool is_image_{false};
    int type_{TYPE_UNKNOWN};
    image::tiff tiff_{};
    image::jpeg jpeg_{};

};
} // namespace image

#endif
