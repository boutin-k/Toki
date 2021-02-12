#include "tkimage.h"

#include <stdexcept>

/**
 * @brief TkImage::TkImage
 */
TkImage::TkImage() {}

/**
 * @brief TkImage::TkImage
 * @param filename
 */
TkImage::TkImage(const std::string& filename) {
  //
  loadFromFile(filename);
}

/**
 * @brief TkImage::loadFromFile
 * @param filename
 */
void TkImage::loadFromFile(const std::string &filename) {
  if (!mTexture.loadFromFile(filename)) {
    // Handle loading error
    throw std::runtime_error("Could not load image file '" + filename + "'");
  }
  mTexture.setSmooth(true);
  setTexture(mTexture);
}
