#include "tkanimation.h"
#include "SFML/Graphics/Rect.hpp"

/**
 * @brief TkAnimatedImage::TkAnimatedImage
 * @param filename
 * @param rectangle
 * @param nbSprite
 */
TkAnimation::TkAnimation(const std::string& filename, const sf::IntRect& rectangle)
    : TkImage(filename), _currentRect(rectangle) {
  _spritePerLine = static_cast<unsigned int>(getLocalBounds().width) / rectangle.width;

  setTextureRect(rectangle);
  setOrigin((rectangle.width>>1) + rectangle.left, (rectangle.height>>1) + rectangle.top);
}

/**
 * @brief TkAnimation::TkAnimation
 * @param texture
 * @param rectangle
 */
TkAnimation::TkAnimation(const sf::Texture& texture, const sf::IntRect& rectangle)
    : TkImage(texture), _currentRect(rectangle) {
  _spritePerLine = static_cast<unsigned int>(getLocalBounds().width) / rectangle.width;

  setTextureRect(rectangle);
  setOrigin((rectangle.width>>1) + rectangle.left, (rectangle.height>>1) + rectangle.top);
}

/**
 * @brief TkAnimatedImage::nextSprite
 * @return The position of the current sprite
 */
uint TkAnimation::nextSprite(void) {
  // Loop
  if (_currentSprite > _lastSprite) {
    _currentSprite = _firstSprite;
  }

  // left, top, width, height
  sf::IntRect rectangle(
      _currentRect.width * (_currentSprite % _spritePerLine),
      _currentRect.width * (_currentSprite / _spritePerLine),
      _currentRect.width, _currentRect.height);
  setTextureRect(rectangle);

  return(_currentSprite++);
}
