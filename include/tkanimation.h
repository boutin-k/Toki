#ifndef TKANIMATION_H
#define TKANIMATION_H

#include "tkimage.h"
#include <string>

class TkAnimation: public TkImage
{
 public:
  TkAnimation();
  TkAnimation(const std::string& filename, const sf::IntRect& rectangle);
  TkAnimation(const sf::Texture& texture, const sf::IntRect& rectangle);

  bool loadFromFile(const std::string& filepath, const sf::IntRect& rectangle);
  virtual void setTkOrigin(const TkOrigin& origin) override;

  inline uint32_t getCurrentSprite(void) const { return currentSprite; }
  inline uint32_t getFirstSprite(void) const { return firstSprite; }
  inline uint32_t getLastSprite(void) const { return lastSprite; }
  inline bool isAnimationFinished() { return animationFinished; }
  inline void setSpeed(float s) { speed = s; }

  inline void setReverseMode(bool r) { reverse = r; }
  inline bool reverseMode() { return reverse; }

  inline void resetSprite(uint32_t first, uint32_t last) {
    currentSprite = firstSprite = first;
    lastSprite = last;
  }

  uint32_t nextSprite(void);

 private:
  void initialize();

  uint32_t spritePerLine{0U};
  uint32_t firstSprite{0U};
  uint32_t lastSprite{0U};

  float currentSprite{0.f};
  float speed{1.f};
  bool animationFinished{false};
  bool reverse{false};
  bool reverseInProgress{false};

  sf::IntRect currentRect{0,0,0,0};
};

#endif // TKANIMATION_H
