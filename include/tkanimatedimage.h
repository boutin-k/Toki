#ifndef TKANIMATEDIMAGE_H
#define TKANIMATEDIMAGE_H

#include "tkimage.h"
#include <string>

class TkAnimatedImage: public TkImage
{
 public:
  TkAnimatedImage(const std::string& filename, const sf::IntRect& rectangle);
  //                  const unsigned int nbSprite);

  //  TkAnimatedImage();
  //  void initialize(const std::string& filename,
  //                  const sf::IntRect& rectangle,
  //                  const unsigned int nbSprite);

  //  inline uint numberOfSprite(void) { return mNumberOfSprite; }
  inline uint getCurrentSprite(void) { return mCurrentSprite; }
  inline uint getFirstSprite(void) { return mFirstSprite; }
  inline uint getLastSprite(void) { return mLastSprite; }

  inline void resetSprite(uint first, uint last) {
    mLastSprite = last;
    mCurrentSprite = mFirstSprite = first;
  }
//  inline uint resetSprite(uint position) {}
  uint nextSprite(void);

 public:
//  static constexpr auto Idle{0xFFFFFFFF};

 private:
  uint mSpritePerLine{0U};
//  uint mNumberOfSprite{0U};
  uint mCurrentSprite{0U};
  uint mFirstSprite{0U};
  uint mLastSprite{0U};

  sf::IntRect mCurrentRect{0,0,0,0};

  bool mReverse = false;
};

#endif // TKANIMATEDIMAGE_H
