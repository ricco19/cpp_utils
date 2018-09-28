/*
  jpeg.h -- Simple wrapper for jpegturbo
*/
#ifndef JPEG_H
#define JPEG_H

#include "image/helpers.h"
#include "utils/system.h"
#include <cstdint>
#include <iostream>
#include <turbojpeg.h>
#include <vector>

namespace image {
class jpeg {
  public:
    jpeg() = default;
    jpeg(const char *filename) {
        load(filename);
    }
    ~jpeg() {
        if (hand_ != nullptr) {
            tjDestroy(hand_);
        }
    }
    // Get rid of all other constructors
    jpeg(jpeg &&) = delete;
    jpeg(jpeg &) = delete;
    jpeg &operator=(const jpeg &) = delete;
    jpeg &operator=(jpeg &&) = delete;
    // Member functions
    bool load(const char *filename) {
        if (hand_ != nullptr) {
            tjDestroy(hand_);
            width_ = 0;
            height_ = 0;
            subsamp_ = 0;
            colorspace_ = 0;
        }
        comp_img_ = utils::file_binread(filename);
        if (comp_img_.size() < 10) {
            is_jpeg_ = false;
            return false;
        }

        hand_ = tjInitDecompress();
        if (tjDecompressHeader3(hand_, &comp_img_[0], comp_img_.size(), &width_,
                                &height_, &subsamp_, &colorspace_) == -1) {
            is_jpeg_ = false;
            return false;
        }
        is_jpeg_ = true;
        return true;
    }
    bool is_jpeg() const { return is_jpeg_; }
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }
    utils::bytearr_t get_pixels(const int fmt) const {

        if ((hand_ == nullptr) || (!is_jpeg_)) {
            std::cout << "bye?\n";
            return utils::bytearr_t{};
        }

        const int num_comp = image::num_components(fmt);
        if (num_comp < 1) {
            return utils::bytearr_t{};
        }

        const auto pitch = num_comp * width_;
        const auto sz = pitch * height_;
        if (sz < 1) {
            return utils::bytearr_t{};
        }

        int jpeg_fmt = -1;
        switch (fmt) {
        case FORMAT_GREY:
            jpeg_fmt = TJPF_GRAY;
            break;
        case FORMAT_RGB:
            jpeg_fmt = TJPF_RGB;
            break;
        case FORMAT_RGBA:
            jpeg_fmt = TJPF_RGBA;
            break;
        default:
            return utils::bytearr_t{};
        }

        utils::bytearr_t pixels(sz, 0);
        if (tjDecompress2(hand_, &comp_img_[0], comp_img_.size(), &pixels[0],
                          width_, pitch, height_, jpeg_fmt,
                          TJFLAG_NOREALLOC | TJFLAG_FASTDCT) == -1) {
            pixels.clear();
        }
        return pixels;
    }

  private:
    tjhandle hand_{nullptr};
    utils::bytearr_t comp_img_{};
    bool is_jpeg_{false};
    int width_{};
    int height_{};
    int subsamp_{};
    int colorspace_{};
};
} // namespace image

#endif
