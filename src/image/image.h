/*
  image.h -- Generic image reading
  TODO: Simplfy classes and add load_xxx functions that return pixels
*/
#ifndef IMAGE_H
#define IMAGE_H

#include "image/pixels.h"
#include "utils/system.h"
#include <cstdint>
#include <iostream>
#include <tiffio.h>
#include <turbojpeg.h>
#include <vector>

namespace image {

// Internal namespace for invidual image librarys
namespace internal {
class jpeg {
  public:
    jpeg() = default;
    jpeg(const char *filename) { load(filename); }
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
        case PIXFMT_GREY:
            jpeg_fmt = TJPF_GRAY;
            break;
        case PIXFMT_RGB:
            jpeg_fmt = TJPF_RGB;
            break;
        case PIXFMT_RGBA:
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

class tiff {
  public:
    tiff() = default;
    tiff(const char *filename) { is_tiff_ = load(filename); }
    ~tiff() {
        if (hand_ != nullptr) {
            TIFFClose(hand_);
        }
    }
    // Get rid of all other constructors
    tiff(tiff &&) = delete;
    tiff(tiff &) = delete;
    tiff &operator=(const tiff &) = delete;
    tiff &operator=(tiff &&) = delete;
    // Member functions
    bool load(const char *filename) {
        if (hand_ != nullptr) {
            TIFFClose(hand_);
            width_ = 0;
            height_ = 0;
            bytes_pp_ = 0;
            bits_pp_ = 0;
        }
        hand_ = TIFFOpen(filename, "r");
        if (hand_ == nullptr) {
            return false;
        }
        TIFFGetField(hand_, TIFFTAG_IMAGEWIDTH, &width_);         // NOLINT
        TIFFGetField(hand_, TIFFTAG_IMAGELENGTH, &height_);       // NOLINT
        TIFFGetField(hand_, TIFFTAG_SAMPLESPERPIXEL, &bytes_pp_); // NOLINT
        TIFFGetField(hand_, TIFFTAG_BITSPERSAMPLE, &bits_pp_);    // NOLINT
        return true;
    }
    bool is_tiff() const { return is_tiff_; }
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }
    std::vector<uint8_t> get_pixels() const {
        if (hand_ == nullptr) {
            return std::vector<uint8_t>();
        }
        const auto row_sz = static_cast<size_t>(TIFFRasterScanlineSize(hand_));
        std::vector<uint8_t> pixels(row_sz * height_, 0);
        for (unsigned row = 0; row < height_; ++row) {
            const auto offs = row * row_sz;
            if (TIFFReadScanline(hand_, &pixels[offs], row) == -1) {
                std::cerr << "Error reading TIFF scanline!\n";
                return std::vector<uint8_t>();
            }
        }
        return pixels;
    }

  private:
    TIFF *hand_{nullptr};
    bool is_tiff_{false};
    uint32_t width_{};
    uint32_t height_{};
    uint16_t bytes_pp_{};
    uint16_t bits_pp_{};
};

inline pixels_t load_jpeg(const char *filename) {
    // Load the file into memory
    const auto buf = utils::file_binread(filename);
    if (buf.size() <= 10) {
        return pixels_t{};
    }
    // Initialze decompress handle
    auto handle = tjInitDecompress();
    if (handle == nullptr) {
        return pixels_t{};
    }
    int width{};
    int height{};
    int subsamp{};
    int colorspace{};
    const int err = tjDecompressHeader3(handle, &buf[0], buf.size(), &width,
                                         &height, &subsamp, &colorspace);
    if (err < 0) {
        tjDestroy(handle);
        return pixels_t{};
    }

    return pixels_t{};
}

} // namespace internal

class load_image {
  public:
    // Constructors
    explicit load_image(const char *filename) {
        // Try to load the image based on file extension
        const int ext = utils::get_file_ext(filename);
        // JPEG
        if (ext == utils::EXT_JPEG) {
            const auto jpeg = internal::jpeg{filename};
            if (!jpeg.is_jpeg()) {
                return;
            }
        }
        // TIFF
        if (ext == utils::EXT_TIFF) {
            return;
        }
        // Failed to load the image
        // TODO: If above fails, try to determine image type with header
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

  private:
    bool is_image_{false};
    image::pixels_t pixels_{};
};
} // namespace image

#endif
