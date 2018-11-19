/*
  image.h -- Generic image reading
  TODO: Simplfy classes and add load_xxx functions that return pixels
*/
#ifndef IMAGE_H
#define IMAGE_H

#include "utils/pixels.h"
#include "utils/system.h"
#include <cstdint>
#include <iostream>
#include <tiffio.h>
#include <turbojpeg.h>
#include <type_traits>
#include <vector>

namespace utils {

// JPEG loader class
class jpeg {
  public:
    // No default/copy/move constructors and assignments
    jpeg() = delete;
    jpeg(const jpeg &) = delete;
    jpeg(jpeg &&) = delete;
    jpeg &operator=(const jpeg &) = delete;
    jpeg &operator=(jpeg &&) = delete;
    // Constructor -> filename
    explicit jpeg(const char *filename);
    // We need a custom destructor to destroy our JPEG handles
    ~jpeg() {
        if (hand_ != nullptr) {
            tjDestroy(hand_);
        }
    }
    // Simple getter functions
    [[nodiscard]] bool is_jpeg() const noexcept { return is_jpeg_; }
    [[nodiscard]] int width() const noexcept { return width_; }
    [[nodiscard]] int height() const noexcept { return height_; }
    [[nodiscard]] int subsamp() const noexcept { return subsamp_; }
    [[nodiscard]] int colorspace() const noexcept { return colorspace_; }

  private:
    bool is_jpeg_{false};
    tjhandle hand_{nullptr};
    int width_{};
    int height_{};
    int subsamp_{};
    int colorspace_{};
    bytes_t file_buf_{};
};

// Our only constructor -> filename to open
inline jpeg::jpeg(const char *filename) {
    // Read entire file into memory
    file_buf_ = file_binread(filename);
    if (file_buf_.empty()) {
        return;
    }
    // Read the JPEG header
    hand_ = tjInitDecompress();
    if (tjDecompressHeader3(hand_, file_buf_.data(), file_buf_.size(),
                            &width_, &height_, &subsamp_,
                            &colorspace_) == -1) {
        return;
    }
    is_jpeg_ = true;
}

class image {
  public:
    // Default constructor, everything empty
    image() = default;
    // Construct with a given filename
    // Public member pixel class
    pixel_data pixels{};
    // Simple getter functions
    [[nodiscard]] int width() const noexcept { return pixels.width(); }
    [[nodiscard]] int height() const noexcept { return pixels.height(); }

  private:
};

} // namespace utils

/*
namespace image {

// Internal namespace for invidual image librarys
namespace internal {
class jpeg {
  public:
    jpeg() = default;
    jpeg(const char *filename) { is_jpeg_ = read_header(filename); }
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
    bool open(const char *filename) {
        clear();
        is_jpeg_ = read_header(filename);
        return is_jpeg_;
    }
    bool is_jpeg() const { return is_jpeg_; }
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }
    void clear() {
        if (hand_ != nullptr) {
            tjDestroy(hand_);
        }
        file_buf_.clear();
        is_jpeg_ = false;
        width_ = 0;
        height_ = 0;
        subsamp_ = 0;
        colorspace_ = 0;
    }
    pixels get_pixels(const int fmt = PIXFMT_UNKNOWN) {
        // Make sure we actually have a jpeg
        if (!is_jpeg_ || hand_ == nullptr) {
            return pixels{};
        }
        // Get pixel formats
        const int jpeg_pix_fmt{get_jpeg_pixel_fmt(fmt)};
        const int pix_fmt{get_pixel_fmt(jpeg_pix_fmt)};
        // Initalize pixel buffer
        pixels p{pix_fmt, width_, height_};
        if (!p.is_valid()) {
            return p;
        }
        // Decompress image into pixels
        const int pitch = width_ * pixfmt_components(pix_fmt);
        const int err = tjDecompress2(hand_, &file_buf_[0], file_buf_.size(),
                                      &p.buf[0], width_, pitch, height_,
                                      jpeg_pix_fmt, TJFLAG_NOREALLOC);
        if (err != 0) {
            p.clear();
        }
        return p;
    }

  private:
    tjhandle hand_{nullptr};
    utils::bytearr_t file_buf_{};
    bool is_jpeg_{false};
    int width_{};
    int height_{};
    int subsamp_{};
    int colorspace_{};
    // Opens a file and reads just the header
    inline bool read_header(const char *filename) {
        // Load entire file into memory
        file_buf_ = utils::file_binread(filename);
        if (file_buf_.size() < 10) {
            return false;
        }
        // Create a decompress handle and fetch information
        hand_ = tjInitDecompress();
        if (tjDecompressHeader3(hand_, &file_buf_[0], file_buf_.size(), &width_,
                                &height_, &subsamp_, &colorspace_) == -1) {
            return false;
        }
        return true;
    }
    // Translates our PIXFMT to jpegturbos pixel format
    inline int get_jpeg_pixel_fmt(const int fmt) {
        switch (fmt) {
        case PIXFMT_GREY:
            return TJPF_GRAY;
        case PIXFMT_RGBA:
            return TJPF_RGBA;
        // If no format is specified, we default to RGB or GREY depending on the
        // images colorspace
        case PIXFMT_RGB:
        case PIXFMT_UNKNOWN:
        default:
            break;
        }
        return TJPF_RGB;
    }
    // Translates jpegturbos pixel format to our PIXFMT
    inline int get_pixel_fmt(const int fmt) {
        switch (fmt) {
        case TJPF_GRAY:
            return PIXFMT_GREY;
        case TJPF_RGBA:
            return PIXFMT_RGBA;
        case TJPF_RGB:
            return PIXFMT_RGB;
        default:
            break;
        }
        return PIXFMT_UNKNOWN;
    }
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
    image::pixels pixels_{};
};
} // namespace image
*/
#endif
