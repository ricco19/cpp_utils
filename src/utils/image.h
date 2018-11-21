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

    // Our only constructor -> filename to open
    explicit jpeg(const char *filename) {
        // Read entire file into memory
        file_buf_ = utils::file_binread(filename);
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

    // We need a custom default destructor to destroy our JPEG handles
    ~jpeg() {
        if (hand_ != nullptr) {
            tjDestroy(hand_);
        }
    }

    // Simple getters
    bool is_jpeg() const noexcept { return is_jpeg_; }
    int width() const noexcept { return width_; }
    int height() const noexcept { return height_; }
    int subsamp() const noexcept { return subsamp_; }
    int colorspace() const noexcept { return colorspace_; }

    // String view of TJSAMP enum
    std::string_view subsamp_sv() const {
        switch (subsamp_) {
        case TJSAMP_444:
            return "TJSAMP_444";
        case TJSAMP_422:
            return "TJSAMP_422";
        case TJSAMP_420:
            return "TJSAMP_420";
        case TJSAMP_GRAY:
            return "TJSAMP_GRAY";
        case TJSAMP_440:
            return "TJSAMP_440";
        case TJSAMP_411:
            return "TJSAMP_411";
        default:
            break;
        }
        return "Unknown";
    }

    // String view of TJCS enum
    std::string_view colorspace_sv() const {
        switch (colorspace_) {
        case TJCS_RGB:
            return "TJCS_RGB";
        case TJCS_YCbCr:
            return "TJCS_YCbCr";
        case TJCS_GRAY:
            return "TJCS_GRAY";
        case TJCS_CMYK:
            return "TJCS_CMYK";
        case TJCS_YCCK:
            return "TJCS_YCCK";
        default:
            break;
        }
        return "Unknown";
    }

    // Decompress to pixels
    pixel_data get_pixels() const {
        return get_pixels(get_best_format());
    }
    pixel_data get_pixels(utils::Pixel_Format fmt) const {
        // Create empty pixel buffer
        pixel_data p{fmt, width_, height_};
        if (!p.is_valid()) {
            return p;
        }
        // Decompress image
        const auto jpfmt = pfmt_to_jfmt(fmt);
        const auto pitch = static_cast<int>(pixels_pitch(width_, fmt));
        const auto err = tjDecompress2(hand_, file_buf_.data(),
                                       file_buf_.size(), p.buf.data(), width_,
                                       pitch, height_, jpfmt, TJFLAG_NOREALLOC);
        if (err != 0) {
            std::cerr << "[ERROR] Could not decompress JPEG! errcode = " << err
                      << '\n';
            p.clear();
        }
        return p;
    }

  private:
    bool is_jpeg_{false};
    tjhandle hand_{nullptr};
    int width_{};
    int height_{};
    int subsamp_{-1};
    int colorspace_{-1};
    utils::bytes_t file_buf_{};

    // Determines the best pixel format given our JPEG info
    utils::Pixel_Format get_best_format() const noexcept {
        switch (colorspace_) {
        // Decompress to RGB
        case TJCS_RGB:
        case TJCS_YCbCr:
            return utils::Pixel_Format::RGB;
        // Decompress to GRAYSCALE
        case TJCS_GRAY:
            return utils::Pixel_Format::GRAY;
        // Cuurently not supported
        case TJCS_CMYK:
        case TJCS_YCCK:
        default:
            break;
        }
        std::cerr << "[ERROR] JPEG colorspace (" << colorspace_sv()
                  << ") is not supported!\n";
        return utils::Pixel_Format::Unknown;
    }

    // Converts our pixel format to the correct JPEG pixel format enum
    int pfmt_to_jfmt(utils::Pixel_Format fmt) const noexcept {
        switch (fmt) {
        default:
        case utils::Pixel_Format::Unknown:
            break;
        case utils::Pixel_Format::RGB:
            return TJPF_RGB;
        case utils::Pixel_Format::RGBA:
            return TJPF_RGBA;
        case utils::Pixel_Format::GRAY:
            return TJPF_GRAY;
        }
        return TJPF_UNKNOWN;
    }
};


class image {
  public:
    // Default constructor, everything empty
    image() = default;
    // Construct with a given filename
    // Public member pixel class
    utils::pixel_data pixels{};
    // Simple getter functions
    int width() const noexcept { return pixels.width(); }
    int height() const noexcept { return pixels.height(); }

  private:
};

} // namespace utils

/*
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
*/
#endif
