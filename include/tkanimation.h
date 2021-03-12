#ifndef TKANIMATION_H
#define TKANIMATION_H

#include "tkimage.h"
#include <string>

class TkAnimation: public TkImage
{
 public:
  TkAnimation(const std::string& filename, const sf::IntRect& rectangle);
  TkAnimation(const sf::Texture& texture, const sf::IntRect& rectangle);

  inline uint getCurrentSprite(void) const { return currentSprite; }
  inline uint getFirstSprite(void) const { return firstSprite; }
  inline uint getLastSprite(void) const { return lastSprite; }
  inline bool isAnimationFinished() { return animationFinished; }
  inline void setSpeed(float s) { speed = s; }

  inline void setReverseMode(bool r) { reverse = r; }
  inline bool reverseMode() { return reverse; }

  inline void resetSprite(uint first, uint last) {
    currentSprite = firstSprite = first;
    lastSprite = last;
  }

  uint nextSprite(void);

 private:
  void initialize();

  uint spritePerLine{0U};
  uint firstSprite{0U};
  uint lastSprite{0U};

  float currentSprite{0.f};
  float speed{1.f};
  bool animationFinished{false};
  bool reverse{false};
  bool reverseInProgress{false};

  sf::IntRect currentRect{0,0,0,0};
};

#endif // TKANIMATION_H
