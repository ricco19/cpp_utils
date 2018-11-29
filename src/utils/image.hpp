/*
  image.h -> Generic image reading
*/
#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "utils/pixels.hpp"
#include "utils/system.hpp"
#include "utils/image_wrappers/jpeg.hpp"

namespace utils {

// Generic image loader
class Image_Load {
  public:
    // Default constructor, everything empty
    Image_Load() = default;
    // Construct with a given filename
    // Public member pixel class
    utils::Pixels pixels{};
    // Simple getter functions
    int width() const noexcept { return pixels.width(); }
    int height() const noexcept { return pixels.height(); }

  private:
}; // Image_Load

} // namespace utils


#endif
