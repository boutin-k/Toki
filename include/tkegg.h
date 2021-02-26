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

  inline anim getState() { return _animState; }

  inline bool isReleased() {
    return (_animState == escape || _animState == free);
  }

  inline void release() {
    if (_animState != anim::escape && _animState != anim::free)
      _animState = anim::escape;
    _counter = 0UL;
  }


  sf::Vector2f move(TkLevel& level,
                    const enum tk::gesture& gesture = tk::gesture::none) override;

 private:
  enum anim _animState = anim::wriggle;
  enum anim _lastAnimState = anim::idle;

  TkAnimation _eggAnim;
  std::unordered_map<TkEgg::anim, animMachine> _animMap;

  ulong _counter;
};

#endif // TKEGG_H
