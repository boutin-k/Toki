#ifndef TKANIMATION_H
#define TKANIMATION_H

#include "tkimage.h"
#include <string>

class TkAnimation: public TkImage
{
 public:
  TkAnimation(const std::string& filename, const sf::IntRect& rectangle);
  TkAnimation(const sf::Texture& texture, const sf::IntRect& rectangle);

  inline uint getCurrentSprite(void) { return _currentSprite; }
  inline uint getFirstSprite(void) { return _firstSprite; }
  inline uint getLastSprite(void) { return _lastSprite; }

  inline void resetSprite(uint first, uint last) {
    _lastSprite = last;
    _currentSprite = _firstSprite = first;
  }
  uint nextSprite(void);

 private:
  uint _spritePerLine{0U};
  uint _currentSprite{0U};
  uint _firstSprite{0U};
  uint _lastSprite{0U};

  sf::IntRect _currentRect{0,0,0,0};
};

#endif // TKANIMATION_H
