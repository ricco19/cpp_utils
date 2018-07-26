#ifndef TIFF_H
#define TIFF_H

#include "utils/system.h"
#include <fstream>
#include <iostream>
#include <vector>

/*

some_type read_ifd_entry(buf, offset)

some_type
    uint16_t -> tag id
    uint16_t -> tag data type
    uint32_t -> tag length (of data type)
    ???????? -> tag values

*/
namespace image {

enum : uint16_t {
    TIFF_DATATYPE_BYTE = 1,
    TIFF_DATATYPE_ASCII = 2,
    TIFF_DATATYPE_SHORT = 3,
    TIFF_DATATYPE_LONG = 4,
    TIFF_DATATYPE_RATIONAL = 5
};

struct ifd_entry {
    uint16_t tag = 0;
    uint16_t type = 0;
    uint32_t count = 0;
    uint32_t data = 0;
    bool data_is_offset = false;
};

struct tiff_ifd {
    uint32_t offset = 0;
    uint16_t num_entries = 0;
    std::vector<ifd_entry> entry;
};

class tiff_open {
  public:
    tiff_open(const char *filename)
        : buf_{utils::file_binread(filename)}
        , is_tiff_{read_header()} {
        read_ifd();
    }
    bool is_tiff() const { return is_tiff_; }
    tiff_ifd ifd;

  private:
    utils::bytearr_t buf_{};
    bool is_big_endian_ = false;
    bool is_tiff_ = false;
    bool read_header();
    void read_ifd();
    ifd_entry read_ifd_entry(const uint32_t offset);
};

bool tiff_open::read_header() {
    // Don't even attempt tiny files
    if (buf_.size() <= 12) {
        buf_.clear();
        return false;
    }
    // Get the byte order
    switch (utils::read_int<uint32_t>(buf_, 0)) {
    case 0x002A4949:
        is_big_endian_ = false;
        return true;
    case 0x2A004D4D:
        is_big_endian_ = true;
        return true;
    default:
        break;
    }
    buf_.clear();
    std::cerr << "Cannot read TIFF file.\n -> Invalid header!\n";
    return false;
}

ifd_entry tiff_open::read_ifd_entry(const uint32_t offs) {
    using utils::read_int;
    ifd_entry ent;
    ent.tag = read_int<uint16_t>(buf_, offs, is_big_endian_);
    ent.type = read_int<uint16_t>(buf_, offs + 2, is_big_endian_);
    ent.count = read_int<uint32_t>(buf_, offs + 4, is_big_endian_);
    // Left aligned regardless of endianess
    switch (ent.type) {
    case TIFF_DATATYPE_BYTE:
        ent.data = static_cast<uint32_t>(buf_[offs + 8]);
        if (ent.count > sizeof(uint32_t) / sizeof(uint8_t)) {
            ent.data_is_offset = true;
        }
        break;
    case TIFF_DATATYPE_SHORT:
        ent.data = static_cast<uint32_t>(
            read_int<uint16_t>(buf_, offs + 8, is_big_endian_));
        if (ent.count > sizeof(uint32_t) / sizeof(uint16_t)) {
            ent.data_is_offset = true;
        }
        break;
    case TIFF_DATATYPE_LONG:
        ent.data = read_int<uint32_t>(buf_, offs + 8, is_big_endian_);
        if (ent.count > 1) {
            ent.data_is_offset = true;
        }
        break;
    case TIFF_DATATYPE_ASCII:
    case TIFF_DATATYPE_RATIONAL:
        ent.data = read_int<uint32_t>(buf_, offs + 8, is_big_endian_);
        ent.data_is_offset = true;
        break;
    default:
        ent.data = read_int<uint32_t>(buf_, offs + 8, is_big_endian_);
        break;
    }
    return ent;
}

void tiff_open::read_ifd() {
    using utils::read_int;
    ifd.offset = read_int<uint32_t>(buf_, 4, is_big_endian_);
    if (ifd.offset >= buf_.size()) {
        is_tiff_ = false;
        std::cerr << "Cannot read TIFF file.\n -> Invalid IFD offset!\n";
        return;
    }
    ifd.num_entries = read_int<uint16_t>(buf_, ifd.offset, is_big_endian_);
    if (ifd.num_entries * 12 > buf_.size()) {
        is_tiff_ = false;
        std::cerr << "Cannot read TIFF file.\n -> Invalid IFD!\n";
        return;
    }
    std::cout << "ifd offset = " << ifd.offset << '\n';
    std::cout << "ifd entries = " << ifd.num_entries << '\n';
    uint32_t offs = ifd.offset + 2;
    ifd.entry.reserve(ifd.num_entries);
    for (uint16_t i = 0; i < ifd.num_entries; ++i) {
        ifd.entry.emplace_back(read_ifd_entry(offs));
        offs += 12;
    }
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
