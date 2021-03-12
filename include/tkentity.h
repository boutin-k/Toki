#ifndef TKENTITY_H
#define TKENTITY_H

#include "tkgesture.h"
#include "tkanimation.h"
#include "tksound.h"

#include <vector>
#include <functional>
#include <unordered_map>

class TkLevel;

class TkEntity
{
 public:
  TkEntity();
  virtual ~TkEntity();

  inline virtual const sf::Drawable* drawableSprite() const { return entity; }

  inline void setPosition(float x, float y) { setPosition(sf::Vector2f(x, y)); }
  inline void setPosition(const sf::Vector2f& p) {
    position = p;
    if (nullptr != entity) entity->setPosition(position);
  }

  virtual sf::Vector2f move(TkLevel& level, const enum tk::gesture& gesture = tk::none) = 0;

 protected:
  struct animMachine {
    uint32_t spriteBegin;
    uint32_t spriteEnd;

    TkAnimation* image;
    std::function<sf::Vector2f(uint32_t)> func;
    bool transitionOnLast;
    std::vector<std::pair<tk::gesture, uint32_t>> transition;
    std::unordered_map<uint32_t, TkSound*> sound;
  };

  sf::Vector2f position{0.f, 0.f};

 public:
  TkAnimation* entity{nullptr};
};

#endif // TKENTITY_H
