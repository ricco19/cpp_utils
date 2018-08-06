#ifndef TIFF_H
#define TIFF_H

#include "utils/system.h"
#include <fstream>
#include <iostream>
#include <vector>

namespace image {

using utils::bytearr_t;
using utils::read_int;

enum : uint16_t {
    // A general indication of the kind of data contained in this subfile.
    TAG_NEW_SUBFILETYPE = 0x00FE,
    // A general indication of the kind of data contained in this subfile.
    TAG_SUBFILETYPE = 0x00FF,
    // The number of columns in the image, i.e., the number of pixels per row.
    TAG_WIDTH = 0x0100,
    // The number of rows of pixels in the image.
    TAG_HEIGHT = 0x0101,
    // Number of bits per component.
    TAG_BITS_PER_SAMPLE = 0x0102,
    // Compression scheme used on the image data.
    TAG_COMPRESSION = 0x0103,
    // The color space of the image data.
    TAG_COLORSPACE = 0x0106,
    // For black and white TIFF files that represent shades of gray, the
    // technique used to convert from gray to black and white pixels.
    TAG_THRESHHOLDING = 0x0107,
    // The width of the dithering or halftoning matrix used to create a dithered
    // or halftoned bilevel file.
    TAG_CELL_WIDTH = 0x0108,
    // The length of the dithering or halftoning matrix used to create a
    // dithered or halftoned bilevel file.
    TAG_CELL_HEIGHT = 0x0109,
    // The logical order of bits within a byte.
    TAG_FILL_ORDER = 0x010A,
    // A string that describes the subject of the image.
    TAG_IMAGE_DESC = 0x010E,
    // The scanner manufacturer.
    TAG_MAKE = 0x010F,
    // The scanner model name or number.
    TAG_MODEL = 0x0110,
    // For each strip, the byte offset of that strip.
    TAG_PIXEL_OFFSET = 0x0111,
    // The orientation of the image with respect to the rows and columns.
    TAG_ORIENTATION = 0x0112,
    // The number of components per pixel.
    TAG_SAMPLES_PER_PIXEL = 0x0115,
    // The number of rows per strip.
    TAG_ROWS_PER_STRIP = 0x0116,
    // For each strip, the number of bytes in the strip after compression.
    TAG_STRIP_BYTE_COUNT = 0x0117,
    // The minimum component value used.
    TAG_MIN_SAMPLE_VAL = 0x0118,
    // The maximum component value used.
    TAG_MAX_SAMPLE_VAL = 0x0119,
    // The number of pixels per ResolutionUnit in the ImageWidth direction.
    TAG_X_RES = 0x011A,
    // The number of pixels per ResolutionUnit in the ImageLength direction.
    TAG_Y_RES = 0x011B,
    // How the components of each pixel are stored.
    TAG_PLANAR_CFG = 0x011C,
    // For each string of contiguous unused bytes in a TIFF file, the byte
    // offset of the string.
    TAG_FREE_OFFSET = 0x0120,
    // For each string of contiguous unused bytes in a TIFF file, the number of
    // bytes in the string.
    TAG_FREE_BYTE_CNT = 0x0121,
    // The precision of the information contained in the GrayResponseCurve.
    TAG_GRAY_RESP_UNIT = 0x0122,
    // For grayscale data, the optical density of each possible pixel value.
    TAG_GRAY_RESP_CURVE = 0x0123,
    // The unit of measurement for XResolution and YResolution.
    TAG_RES_UNIT = 0x0128,
    // Name and version number of the software used to create the image.
    TAG_SOFTWARE = 0x0131,
    // Date and time of image creation.
    TAG_DATETIME = 0x0132,
    // Person who created the image.
    TAG_ARTIST = 0x013B,
    // The computer or operating system in use at the time of image creation.
    TAG_HOST = 0x013C,
    // A color map for palette color images
    TAG_COLOR_MAP = 0x0140,
    // Description of extra components
    TAG_EXTRA_SAMPLES = 0x0152,
    // Copyright notice.
    TAG_COPYRIGHT = 0x8298
};
enum : uint16_t {
    TIFF_DATATYPE_BYTE = 1,
    TIFF_DATATYPE_ASCII = 2,
    TIFF_DATATYPE_SHORT = 3,
    TIFF_DATATYPE_LONG = 4,
    TIFF_DATATYPE_RATIONAL = 5
};

// Class that holds a single IFD entry
class ifd_entry {
  public:
    ifd_entry(const bytearr_t &buf, const uint32_t offs, const bool is_be) {
        tag_ = read_int<uint16_t>(buf, offs, is_be);
        type_ = read_int<uint16_t>(buf, offs + 2, is_be);
        count_ = read_int<uint32_t>(buf, offs + 4, is_be);
        size_ = get_val_size();
        if (is_val_offset()) {
            offset_ = read_int<uint32_t>(buf, offs + 8, is_be);
        } else {
            offset_ = offs + 8;
        }
    }
    constexpr std::string_view tag_name() const;
    constexpr std::string_view type_name() const;
    uint16_t tag() const { return tag_; }
    uint16_t type() const { return type_; }
    uint32_t count() const { return count_; }
    uint32_t size() const { return size_; }
    uint32_t offset() const { return offset_; }

  private:
    uint16_t tag_ = 0;
    uint16_t type_ = 0;
    uint32_t count_ = 0;
    uint32_t size_ = 0;
    uint32_t offset_ = 0;
    inline bool is_val_offset() const {
        switch (type_) {
        case TIFF_DATATYPE_BYTE:
        case TIFF_DATATYPE_ASCII:
            if (count_ > 4) {
                return true;
            }
            break;
        case TIFF_DATATYPE_SHORT:
            if (count_ > 2) {
                return true;
            }
            break;
        case TIFF_DATATYPE_LONG:
            if (count_ > 1) {
                return true;
            }
            break;
        case TIFF_DATATYPE_RATIONAL:
            return true;
        default:
            break;
        }
        return false;
    }
    inline uint32_t get_val_size() const {
        switch (type_) {
        case TIFF_DATATYPE_BYTE:
        case TIFF_DATATYPE_ASCII:
            return count_;
        case TIFF_DATATYPE_SHORT:
            return count_ * 2;
        case TIFF_DATATYPE_LONG:
            return count_ * 4;
        case TIFF_DATATYPE_RATIONAL:
            return count_ * 8;
        default:
            break;
        }
        return 0;
    }
};
// TIFF reading
class tiff_read {
  public:
    tiff_read(const char *filename)
        : buf_{utils::file_binread(filename)}
        , is_tiff_{read_header()} {
        read_ifd();
    }
    bool is_tiff() const { return is_tiff_; }
    void print_ifd() const;

  private:
    bytearr_t buf_{};
    bool is_big_endian_ = false;
    bool is_tiff_ = false;
    uint32_t ifd_offset_ = 0;
    std::vector<ifd_entry> ifd_;
    bool read_header();
    void read_ifd();
};

bool tiff_read::read_header() {
    // Don't even attempt tiny files
    if (buf_.size() <= 12) {
        buf_.clear();
        return false;
    }
    // Get the byte order
    switch (read_int<uint32_t>(buf_, 0)) {
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

void tiff_read::read_ifd() {
    ifd_offset_ = read_int<uint32_t>(buf_, 4, is_big_endian_);
    if (ifd_offset_ >= buf_.size()) {
        is_tiff_ = false;
        std::cerr << "Cannot read TIFF file.\n -> Invalid IFD offset!\n";
        return;
    }
    const auto count = read_int<uint16_t>(buf_, ifd_offset_, is_big_endian_);
    const auto sz = count * 12;
    if (static_cast<size_t>(count) * 12 > buf_.size()) {
        is_tiff_ = false;
        std::cerr << "Cannot read TIFF file.\n -> Invalid IFD!\n";
        return;
    }
    uint32_t offs = ifd_offset_ + 2;
    ifd.reserve(count);
    for (uint16_t offs = ifd_offset_ + 2; offs < count * 12; ++i) {
        ifd.entry.emplace_back(ifd_entry{buf_, offs, is_big_endian_});
        offs += 12;
    }
}

static inline double get_rational(const bytearr_t &buf, const uint32_t offs,
                                  const bool is_be) {
    constexpr auto sz = static_cast<uint32_t>(sizeof(uint32_t));
    const auto num = read_int<uint32_t>(buf, offs, is_be);
    const auto den = read_int<uint32_t>(buf, offs + sz, is_be);
    return static_cast<double>(num) / static_cast<double>(den);
}

inline constexpr std::string_view ifd_entry::type_name() const {
    switch (type_) {
    case TIFF_DATATYPE_BYTE:
        return std::string_view{"Byte"};
    case TIFF_DATATYPE_ASCII:
        return std::string_view{"Ascii"};
    case TIFF_DATATYPE_SHORT:
        return std::string_view{"Short"};
    case TIFF_DATATYPE_LONG:
        return std::string_view{"Long"};
    case TIFF_DATATYPE_RATIONAL:
        return std::string_view{"Rational"};
    default:
        break;
    }
    return std::string_view{"Unknown"};
}

inline constexpr std::string_view ifd_entry::tag_name() const {
    switch (tag_) {
    case TAG_NEW_SUBFILETYPE:
        return std::string_view{"NewSubFileType"};
    case TAG_SUBFILETYPE:
        return std::string_view{"SubFileType"};
    case TAG_WIDTH:
        return std::string_view{"ImageWidth"};
    case TAG_HEIGHT:
        return std::string_view{"ImageHeight"};
    case TAG_BITS_PER_SAMPLE:
        return std::string_view{"BitsPerSample"};
    case TAG_COMPRESSION:
        return std::string_view{"Compression"};
    case TAG_COLORSPACE:
        return std::string_view{"PhotometricInterpretation"};
    case TAG_THRESHHOLDING:
        return std::string_view{"Threshholding"};
    case TAG_CELL_WIDTH:
        return std::string_view{"CellWidth"};
    case TAG_CELL_HEIGHT:
        return std::string_view{"CellHeight"};
    case TAG_FILL_ORDER:
        return std::string_view{"FillOrder"};
    case TAG_IMAGE_DESC:
        return std::string_view{"ImageDescription"};
    case TAG_MAKE:
        return std::string_view{"Make"};
    case TAG_MODEL:
        return std::string_view{"Model"};
    case TAG_PIXEL_OFFSET:
        return std::string_view{"StripOffsets"};
    case TAG_ORIENTATION:
        return std::string_view{"Orientation"};
    case TAG_SAMPLES_PER_PIXEL:
        return std::string_view{"SamplesPerPixel"};
    case TAG_ROWS_PER_STRIP:
        return std::string_view{"RowsPerStrip"};
    case TAG_STRIP_BYTE_COUNT:
        return std::string_view{"StripByteCount"};
    case TAG_MIN_SAMPLE_VAL:
        return std::string_view{"MinSampleValue"};
    case TAG_MAX_SAMPLE_VAL:
        return std::string_view{"MaxSampleValue"};
    case TAG_X_RES:
        return std::string_view{"XResolution"};
    case TAG_Y_RES:
        return std::string_view{"YResolution"};
    case TAG_PLANAR_CFG:
        return std::string_view{"PlanarConfiguration"};
    case TAG_FREE_OFFSET:
        return std::string_view{"FreeOffsets"};
    case TAG_FREE_BYTE_CNT:
        return std::string_view{"FreeByteCounts"};
    case TAG_GRAY_RESP_UNIT:
        return std::string_view{"GrayResponseUnit"};
    case TAG_GRAY_RESP_CURVE:
        return std::string_view{"GrayResponseCurve"};
    case TAG_RES_UNIT:
        return std::string_view{"ResolutionUnit"};
    case TAG_SOFTWARE:
        return std::string_view{"Software"};
    case TAG_DATETIME:
        return std::string_view{"DateTime"};
    case TAG_ARTIST:
        return std::string_view{"Artist"};
    case TAG_HOST:
        return std::string_view{"HostComputer"};
    case TAG_COLOR_MAP:
        return std::string_view{"ColorMap"};
    case TAG_EXTRA_SAMPLES:
        return std::string_view{"ExtraSamples"};
    case TAG_COPYRIGHT:
        return std::string_view{"Copyright"};
    default:
        break;
    }
    return std::string_view{"Unknown"};
}

void tiff_read::print_ifd() const {
    std::cout << "IFD Offset: " << ifd.offset << '\n';
    std::cout << "IFD Entries: " << ifd.num_entries << '\n';
    for (const auto ent : ifd.entry) {
        std::cout << "  [" << ent.tag_name()
                  << "]\n    Type: " << ent.type_name()
                  << ", Length: " << ent.count() << ", " << ent.size()
                  << " bytes.\n";
        if (ent.type() == TIFF_DATATYPE_ASCII) {
            std::string str(ent.size(), 0);
            memcpy(&str[0], &buf_[ent.offset()], ent.size());
            std::cout << "    " << str << '\n';
            return;
        }
        std::vector<uint32_t> val;
        val.reserve(ent.count());

        uint32_t sz = 0;
        switch (ent.type()) {
        case TIFF_DATATYPE_BYTE:
            sz = 1;
            break;
        case TIFF_DATATYPE_SHORT:
            sz = 2;
            break;
        case TIFF_DATATYPE_LONG:
            sz = 4;
            break;
        case TIFF_DATATYPE_RATIONAL:
            sz = 8;
            break;
        default:
            break;
        }

        for (unsigned i = 0; i < ent.count(); ++i) {
            val.emplace_back(read_int<uint16_t>(buf_, i * 2, is_big_endian_));
        }
    }
}

} // namespace image

#endif
