#include "tkimage.h"

#include <stdexcept>

/**
 * @brief TkImage::TkImage
 */
TkImage::TkImage() : sf::Sprite() {}

/**
 * @brief TkImage::TkImage
 * @param texture
 */
TkImage::TkImage(const sf::Texture& texture) : sf::Sprite(texture) {
}

/**
 * @brief TkImage::TkImage
 * @param filename
 */
TkImage::TkImage(const std::string& filename) {
  // Don't do job here
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
//  mTexture.setSmooth(true);
  setTexture(mTexture);
}
