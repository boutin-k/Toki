#ifndef TKENTITY_H
#define TKENTITY_H

#include "tkgesture.h"
#include "tkanimation.h"

#include "SFML/Audio/Music.hpp"

#include <vector>
#include <functional>
#include <unordered_map>

class TkLevel;

class TkEntity
{
 public:
  TkEntity();
  virtual ~TkEntity();

  inline virtual const sf::Drawable* drawableSprite() const { return _entity; }

  inline void setPosition(float positionX, float positionY) {
    setPosition(sf::Vector2f(positionX, positionY));
  }

  inline virtual void setPosition(const sf::Vector2f& position) {
    _position = position;
    if (nullptr != _entity) _entity->setPosition(position);
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
    std::unordered_map<uint32_t, sf::Music*> sound;
  };

  sf::Vector2f _position{0.f, 0.f};

 public:
  TkAnimation* _entity{nullptr};
};

#endif // TKENTITY_H
