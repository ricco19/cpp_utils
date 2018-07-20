#ifndef TIFF_H
#define TIFF_H

#include "utils/system.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace image {

static constexpr unsigned MAX_TIFF_SIZE = 10485760;

struct tiff_ifd {
    tiff_ifd(const int tag)
        : id{tag} {}
    int id = 0;
    int type = 0;
    int length = 0;
    int value = 0;
};

class tiff_open {
  public:
    tiff_open(const char *filename)
        : buf_{utils::file_binread(filename)}
        , is_tiff_{read_header()} {}
    bool is_tiff() const { return is_tiff_; }

  private:
    std::vector<uint8_t> buf_{};
    bool is_tiff_ = false;
    bool is_big_endian_ = false;
    int ifd_offset_ = 0;
    bool read_header();

};

bool tiff_open::read_header() {
    // Don't even attempt tiny files
    if (buf_.size() <= 12) {
        return false;
    }
    // int pointer at offset 0 of buffer
    // 4 bytes, TIFF header
    switch (reinterpret_cast<const int *>(&buf_[0])[0]) {
    case 0x002A4949:
        is_big_endian_ = false;
        break;
    case 0x2A004D4D:
        is_big_endian_ = true;
        break;
    default:
        buf_.clear();
        std::cerr << "Cannot read TIFF file.\n -> Invalid header!\n";
        return false;
    }
    // int pointer at offset 4 of buffer
    // 4 bytes, offset
    ifd_offset_ = reinterpret_cast<const int *>(&buf_[4])[0];
    if (ifd_offset_ < 8) {
        std::cerr << "Cannot read TIFF file.\n -> Invalid IFD offset!\n";
        return false;
    }
    std::cout << "ifd offset = " << ifd_offset_ << '\n';
    return true;
}

// struct subfiletype {
//     const int id = 0x00FE;
//     int32_t val = 0;
// };

// class tiff_ifd {
//   public:
//     // Constructors
//     tiff_ifd() = delete;
//     tiff_ifd(const std::string &data)
//         : offset{0}
//         , buf{data} {}

//   private:
//     // A general indication of the kind of data contained in this subfile.
//     static constexpr int TAG_NEW_SUBFILETYPE = 0x00FE;
//     // A general indication of the kind of data contained in this subfile.
//     static constexpr int TAG_SUBFILETYPE = 0x00FF;
//     // The number of columns in the image, i.e., the number of pixels per
//     row. static constexpr int TAG_WIDTH = 0x0100;
//     // The number of rows of pixels in the image.
//     static constexpr int TAG_HEIGHT = 0x0101;
//     // Number of bits per component.
//     static constexpr int TAG_BITS_PER_SAMPLE = 0x0102;
//     // Compression scheme used on the image data.
//     static constexpr int TAG_COMPRESSION = 0x0103;
//     // The color space of the image data.
//     static constexpr int TAG_COLORSPACE = 0x0106;
//     // For black and white TIFF files that represent shades of gray, the
//     // technique used to convert from gray to black and white pixels.
//     static constexpr int TAG_THRESHHOLDING = 0x0107;
//     // The width of the dithering or halftoning matrix used to create a
//     // dithered or halftoned bilevel file.
//     static constexpr int TAG_CELL_WIDTH = 0x0108;
//     // CellLength // The length of the dithering or halftoning matrix used
//     // to create a dithered or halftoned bilevel file.
//     static constexpr int TAG_CELL_HEIGHT = 0x0109;
//     // The logical order of bits within a byte.
//     static constexpr int TAG_FILL_ORDER = 0x010A;
//     // A string that describes the subject of the image.
//     static constexpr int TAG_IMAGE_DESC = 0x010E;
//     // The scanner manufacturer.
//     static constexpr int TAG_MAKE = 0x010F;
//     // The scanner model name or number.
//     static constexpr int TAG_MODEL = 0x0110;
//     // For each strip, the byte offset of that strip.
//     static constexpr int TAG_PIXEL_OFFSET = 0x0111;
//     // The orientation of the image with respect to the rows and columns.
//     static constexpr int TAG_ORIENTATION = 0x0112;
//     // The number of components per pixel.
//     static constexpr int TAG_SAMPLES_PER_PIXEL = 0x0115;
//     // The number of rows per strip.
//     static constexpr int TAG_ROWS_PER_STRIP = 0x0116;
//     // For each strip, the number of bytes in the strip after compression.
//     static constexpr int TAG_STRIP_BYTE_COUNT = 0x0117;
//     // The minimum component value used.
//     static constexpr int TAG_MIN_SAMPLE_VAL = 0x0118;
//     // The maximum component value used.
//     static constexpr int TAG_MAX_SAMPLE_VAL = 0x0119;
//     // The number of pixels per ResolutionUnit in the ImageWidth direction.
//     static constexpr int TAG_X_RES = 0x011A;
//     // The number of pixels per ResolutionUnit in the ImageLength direction.
//     static constexpr int TAG_Y_RES = 0x011B;
//     // How the components of each pixel are stored.
//     static constexpr int TAG_PLANAR_CFG = 0x011C;
//     // For each string of contiguous unused bytes in a TIFF file, the byte
//     // offset of the string.
//     static constexpr int TAG_FREE_OFFSET = 0x0120;
//     // For each string of contiguous unused bytes in a TIFF file, the number
//     // of bytes in the string.
//     static constexpr int TAG_FREE_BYTE_CNT = 0x0121;
//     // The precision of the information contained in the GrayResponseCurve.
//     static constexpr int TAG_GRAY_RESP_UNIT = 0x0122;
//     // For grayscale data, the optical density of each possible pixel value.
//     static constexpr int TAG_GRAY_RESP_CURVE = 0x0123;
//     // The unit of measurement for XResolution and YResolution.
//     static constexpr int TAG_RES_UNIT = 0x0128;
//     // Name and version number of the software used to create the image.
//     static constexpr int TAG_SOFTWARE = 0x0131;
//     // Date and time of image creation.
//     static constexpr int TAG_DATETIME = 0x0132;
//     // Person who created the image.
//     static constexpr int TAG_ARTIST = 0x013B;
//     // The computer or operating system in use at the time of image creation.
//     static constexpr int TAG_HOST = 0x013C;
//     // A color map for palette color images
//     static constexpr int TAG_COLOR_MAP = 0x0140;
//     // Description of extra components
//     static constexpr int TAG_EXTRA_SAMPLES = 0x0152;
//     // Copyright notice.
//     static constexpr int TAG_COPYRIGHT = 0x8298;

//     const int offset{};
//     const std::string buf{};
// };

} // namespace image

#endif
