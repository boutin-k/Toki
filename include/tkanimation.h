#ifndef TKANIMATION_H
#define TKANIMATION_H

#include "tkimage.h"
#include <string>

class TkAnimation: public TkImage
{
 public:
  TkAnimation(const std::string& filename, const sf::IntRect& rectangle);
  TkAnimation(const sf::Texture& texture, const sf::IntRect& rectangle);

  inline uint getCurrentSprite(void) { return mCurrentSprite; }
  inline uint getFirstSprite(void) { return mFirstSprite; }
  inline uint getLastSprite(void) { return mLastSprite; }

  inline void resetSprite(uint first, uint last) {
    mLastSprite = last;
    mCurrentSprite = mFirstSprite = first;
  }
  uint nextSprite(void);

 private:
  uint mSpritePerLine{0U};
  uint mCurrentSprite{0U};
  uint mFirstSprite{0U};
  uint mLastSprite{0U};

  sf::IntRect mCurrentRect{0,0,0,0};

  bool mReverse = false;
};

#endif // TKANIMATION_H
