/*
  jpeg.h -> Simple C++ wrapper for turbojpeg
*/
#ifndef JPEG_HPP
#define JPEG_HPP

#include "utils/pixels.hpp"
#include "utils/system.hpp"
#include <turbojpeg.h>
#include <string_view>

namespace utils {
// JPEG Reader class
class JPEG_Read {
  public:
    // No default/copy/move constructors and assignments
    JPEG_Read() = delete;
    JPEG_Read(const JPEG_Read &) = delete;
    JPEG_Read(JPEG_Read &&) = delete;
    JPEG_Read &operator=(const JPEG_Read &) = delete;
    JPEG_Read &operator=(JPEG_Read &&) = delete;

    // Our only constructor -> filename to open
    explicit JPEG_Read(const char *filename) {
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
    ~JPEG_Read() {
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
    utils::Pixels get_pixels() const {
        return get_pixels(get_best_format());
    }
    utils::Pixels get_pixels(utils::Pixel_Format fmt) const {
        // Create empty pixel buffer
        utils::Pixels p{fmt, width_, height_};
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
    int pfmt_to_jfmt(const utils::Pixel_Format fmt) const noexcept {
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
        return -1;
    }
}; // JPEG_Read

// JPEG Writer class
class JPEG_Write {}; // JPEG_Write

} // namespace utils

#endif
