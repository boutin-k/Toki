#include "tkanimatedimage.h"

#include "SFML/Graphics/Rect.hpp"

//template <typename T>
//void centerOrigin(T& drawable)
//{
//  sf::FloatRect bound = drawable.getLocalBounds();
//  drawable.setOrigin(bound.width/2, bound.height/2);
//}

///**
// * @brief TkAnimatedImage::TkAnimatedImage
// */
//TkAnimatedImage::TkAnimatedImage() {}

/**
 * @brief TkAnimatedImage::TkAnimatedImage
 * @param filename
 * @param rectangle
 * @param nbSprite
 */
TkAnimatedImage::TkAnimatedImage(const std::string& filename,
                                 const sf::IntRect& rectangle)
    : TkImage(filename), mCurrentRect(rectangle) {
  mSpritePerLine = static_cast<unsigned int>(getLocalBounds().width) / rectangle.width;

  setTextureRect(rectangle);
  setOrigin((rectangle.width>>1) + rectangle.left, (rectangle.height>>1) + rectangle.top);
}

///**
// * @brief TkAnimatedImage::initialize
// * @param filename
// * @param rectangle
// * @param nbSprite
// */
//void TkAnimatedImage::initialize(const std::string& filename,
//                                 const sf::IntRect& rectangle,
//                                 const unsigned int nbSprite) {
//  loadFromFile(filename);
//  mCurrentRect = rectangle;
//  mNumberOfSprite = nbSprite;
//  mNumberOfSpritePerLine = static_cast<unsigned int>(getLocalBounds().width) / rectangle.width;

//  setTextureRect(rectangle);
//  setOrigin((rectangle.width>>1) + rectangle.left, (rectangle.height>>1) + rectangle.top);
//}

///**
// * @brief TkAnimatedImage::resetSprite
// * @param position
// */
//void TkAnimatedImage::resetSprite(uint position) {
//  mCurrentSprite = position;
//}

/**
 * @brief TkAnimatedImage::nextSprite
 * @return The position of the current sprite
 */
uint TkAnimatedImage::nextSprite(void) {
  // Loop
  if (mCurrentSprite > mLastSprite) {
    mCurrentSprite = mFirstSprite;
  }

  // left, top, width, height
  sf::IntRect rectangle(
      mCurrentRect.width * (mCurrentSprite % mSpritePerLine),
      mCurrentRect.width * (mCurrentSprite / mSpritePerLine),
      mCurrentRect.width, mCurrentRect.height);
  setTextureRect(rectangle);

  return(mCurrentSprite++);
}
