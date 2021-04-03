#include "tkanimation.h"
#include "SFML/Graphics/Rect.hpp"

/**
 * @brief TkAnimation::TkAnimation
 */
TkAnimation::TkAnimation() : TkImage() {}

/**
 * @brief TkAnimatedImage::TkAnimatedImage
 * @param filename
 * @param rectangle
 * @param nbSprite
 */
TkAnimation::TkAnimation(const std::string& filename, const sf::IntRect& rectangle)
    : TkImage(filename), currentRect(rectangle) {
  initialize();
}

/**
 * @brief TkAnimation::TkAnimation
 * @param texture
 * @param rectangle
 */
TkAnimation::TkAnimation(const sf::Texture& texture, const sf::IntRect& rectangle)
    : TkImage(texture), currentRect(rectangle) {
  initialize();
}

/**
 * @brief TkAnimation::loadFromFile
 * @param filepath
 * @param rectangle
 * @return
 */
bool TkAnimation::loadFromFile(const std::string& filepath,
                               const sf::IntRect& rectangle) {
  if (TkImage::loadFromFile(filepath)) {
    currentRect = rectangle;
    initialize();
    return true;
  }
  return false;
}

/**
 * @brief TkAnimation::initialize
 */
void TkAnimation::initialize() {
  spritePerLine = static_cast<unsigned int>(getLocalBounds().width) / currentRect.width;

  setTextureRect(currentRect);
    setOrigin(currentRect.width>>1, currentRect.height>>1);
  //  setOrigin((currentRect.width>>1) + currentRect.left,
  //  (currentRect.height>>1) + currentRect.top);
}

/**
 * @brief TkAnimatedImage::nextSprite
 * @return The position of the current sprite
 */
uint32_t TkAnimation::nextSprite(void) {
  // Loop
  if ((uint32_t)currentSprite > lastSprite) {
    currentSprite = firstSprite;
  }

  // left, top, width, height
  sf::IntRect rectangle(
      currentRect.width * ((uint32_t)currentSprite % spritePerLine),
      currentRect.height * ((uint32_t)currentSprite / spritePerLine),
      currentRect.width, currentRect.height);
  setTextureRect(rectangle);

  uint32_t retVal = currentSprite;

  if (reverse && reverseInProgress) {
    animationFinished = ((currentSprite -= speed) < firstSprite);
    if (animationFinished) {
      reverseInProgress = false;
      currentSprite += speed * 2;
    }
  } else {
    if (reverse && currentSprite + speed > lastSprite) {
      reverseInProgress = true;
      currentSprite -= speed;
    } else {
      animationFinished = ((currentSprite += speed) > lastSprite);
    }
  }

  return (retVal);
}

/**
 * @brief TkAnimation::setTkOrigin
 * @param origin
 */
void TkAnimation::setTkOrigin(const TkOrigin& origin) {
  float x = 0.f;
  float y = 0.f;

  // Horizontality
  switch (origin) {
    case topRight:
    case centerRight:
    case bottomRight:
      x = currentRect.width;
      break;
    case center:
    case topCenter:
    case bottomCenter:
      x = currentRect.width >> 1;
      break;
    default:
      break;
  }

  // Verticality
  switch (origin) {
    case bottomRight:
    case bottomLeft:
    case bottomCenter:
      y = currentRect.height;
      break;
    case centerRight:
    case centerLeft:
    case center:
      y = currentRect.height >> 1;
      break;
    default:
      break;
  }

  setOrigin(x, y);
}
