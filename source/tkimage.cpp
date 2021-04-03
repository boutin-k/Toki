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
TkImage::TkImage(const sf::Texture& t) : sf::Sprite(), texture(t) {
  setTexture(texture);
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

void TkImage::setTkOrigin(const TkOrigin& origin) {
  float x = 0.f;
  float y = 0.f;

  // Horizontality
  switch (origin) {
    case topRight:
    case centerRight:
    case bottomRight:
      x = getLocalBounds().width;
      break;
    case center:
    case topCenter:
    case bottomCenter:
      x = getLocalBounds().width / 2.f;
      break;
    default:
      break;
  }

  // Verticality
  switch (origin) {
    case bottomRight:
    case bottomLeft:
    case bottomCenter:
      y = getLocalBounds().height;
      break;
    case centerRight:
    case centerLeft:
    case center:
      y = getLocalBounds().height / 2.f;
      break;
    default:
      break;
  }

  setOrigin(x, y);
}
