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
 * @return
 */
bool TkImage::loadFromFile(const std::string& filename) {
  if (!_texture.loadFromFile(filename))
    return false;

//  mTexture.setSmooth(true);
  setTexture(_texture);
  return true;
}
