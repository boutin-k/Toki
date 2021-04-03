#ifndef TKEGG_H
#define TKEGG_H

#include "tkanimation.h"
#include "tkgesture.h"
#include "tkentity.h"

#include <unordered_map>

class TkEgg : public TkEntity
{
 public:
  enum anim : uint32_t { idle, wriggle, escape, free };

  TkEgg(const sf::Texture& fileName, float positionX = 0.f, float positionY = 0.f);
  virtual ~TkEgg();

  inline anim getState() const { return animState; }
  inline bool isReleased() const { return (animState == escape || animState == free); }

  inline void release() {
    if (animState != anim::escape && animState != anim::free)
      animState = anim::escape;
    counter = 0UL;
  }

  sf::Vector2f move(TkLevel& level,
                    const enum tk::gesture& gesture = tk::gesture::none) override;

 private:
  enum anim animState     = anim::wriggle;
  enum anim lastAnimState = anim::idle;

  TkAnimation                                  eggAnim;
  std::unordered_map<TkEgg::anim, animMachine> animMap;

  unsigned long counter;
};

#endif // TKEGG_H
