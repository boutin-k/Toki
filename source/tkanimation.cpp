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
  setOrigin((currentRect.width>>1) + currentRect.left, (currentRect.height>>1) + currentRect.top);
}

/**
 * @brief TkAnimatedImage::nextSprite
 * @return The position of the current sprite
 */
uint TkAnimation::nextSprite(void) {
  // Loop
  if ((uint)currentSprite > lastSprite) {
    currentSprite = firstSprite;
  }

  // left, top, width, height
  sf::IntRect rectangle(
      currentRect.width * ((uint)currentSprite % spritePerLine),
      currentRect.height * ((uint)currentSprite / spritePerLine),
      currentRect.width, currentRect.height);
  setTextureRect(rectangle);

  uint retVal = currentSprite;

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
