/*
  tiff.h -- Simple wrapper for LibTIFF c library
*/
#ifndef TIFF_H
#define TIFF_H

#include <cstdint>
#include <iostream>
#include <tiffio.h>
#include <vector>

namespace image {
class tiff_open {
  public:
    explicit tiff_open(const char *filename) {
        tif_ = TIFFOpen(filename, "r");
        if (tif_ != nullptr) {
            TIFFGetField(tif_, TIFFTAG_IMAGEWIDTH, &width_);         // NOLINT
            TIFFGetField(tif_, TIFFTAG_IMAGELENGTH, &height_);       // NOLINT
            TIFFGetField(tif_, TIFFTAG_SAMPLESPERPIXEL, &bytes_pp_); // NOLINT
            TIFFGetField(tif_, TIFFTAG_BITSPERSAMPLE, &bits_pp_);    // NOLINT
            is_tiff_ = true;
        }
    }
    ~tiff_open() {
        if (tif_ != nullptr) {
            TIFFClose(tif_);
        }
    }
    // Get rid of all other constructors
    tiff_open() = delete;
    tiff_open(tiff_open &&) = delete;
    tiff_open(tiff_open &) = delete;
    tiff_open &operator=(const tiff_open &) = delete;
    tiff_open &operator=(tiff_open &&) = delete;
    bool is_tiff() const { return is_tiff_; }
    uint32_t width() const { return width_; }
    uint32_t height() const { return height_; }
    uint16_t bytes_per_pixel() const { return bytes_pp_; }
    uint16_t bits_per_pixel() const { return bits_pp_; }
    std::vector<uint8_t> get_pixels() const {
        if (tif_ == nullptr) {
            return std::vector<uint8_t>();
        }
        const auto row_sz = static_cast<size_t>(TIFFRasterScanlineSize(tif_));
        std::vector<uint8_t> pixels(row_sz * height_, 0);
        for (unsigned row = 0; row < height_; ++row) {
            const auto offs = row * row_sz;
            if (TIFFReadScanline(tif_, &pixels[offs], row) == -1) {
                std::cerr << "Error reading TIFF scanline!\n";
                return std::vector<uint8_t>();
            }
        }
        return pixels;
    }

  private:
    TIFF *tif_{nullptr};
    bool is_tiff_{false};
    uint32_t width_{};
    uint32_t height_{};
    uint16_t bytes_pp_{};
    uint16_t bits_pp_{};
};
} // namespace image

#endif
