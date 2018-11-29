/*
  tiff.h -> Simple C++ wrapper for libtiff
*/
#ifndef TIFF_HPP
#define TIFF_HPP

#include <tiffio.h>

namespace utils {
class TIFF_Read {};
class TIFF_Write {};
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
