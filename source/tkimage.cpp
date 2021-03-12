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
TkImage::TkImage(const sf::Texture& t) : sf::Sprite(t) {
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
  if (!texture.loadFromFile(filename))
    return false;

  texture.setSmooth(true);
  setTexture(texture);
  return true;
}
