#include "tkegg.h"
#include "tklevel.h"

#include <cstdlib>
#include <iostream>

TkEgg::TkEgg() : _eggAnim("Media/forest_egg.png", { 0, 0, 64, 64 })
{
  // clang-format off
  _animMap = {
    { TkEgg::anim::idle,    { 0U, 1U, &_eggAnim, nullptr, true, { }, { } } },
    { TkEgg::anim::wriggle, { 0U, 39U, &_eggAnim, nullptr, true, { }, { } } },
    { TkEgg::anim::escape,  { 40U, 40U, &_eggAnim, nullptr, true, { }, { } } },
  };
  // clang-format on
  _entity = &_eggAnim;
}

TkEgg::TkEgg(float positionX, float positionY) : TkEgg() {
  setPosition(positionX, positionY);
}

TkEgg::~TkEgg() {}

/**
 * @brief TkPlayer::move
 * @param level
 * @param gesture
 * @return
 */
sf::Vector2f TkEgg::move(TkLevel& /*level*/,
                         const enum tk::gesture& /*gesture*/) {
  sf::Vector2f offset(0.f, 0.f);

  auto currentState = _animMap.find(_animState);
  if (currentState != _animMap.end()) {
    animMachine sm = currentState->second;

    if (_animState != _lastAnimState) {
      _entity = sm.image;
      _entity->resetSprite(sm.spriteBegin, sm.spriteEnd);
      _entity->setPosition(_position);
      // Update last animation state
      _lastAnimState = _animState;
    }
    uint currentId = _entity->nextSprite();
    if (currentId == sm.spriteEnd) {
      switch (_animState) {
        case TkEgg::anim::idle: {
          if (--_counter <= 0) {
            _animState = TkEgg::anim::wriggle;
          }
          break;
        }
        case TkEgg::anim::wriggle: {
          _counter = rand() % 50 + 20;
          _animState = TkEgg::anim::idle;
          break;
        }
        case TkEgg::anim::escape: {
          _entity->move(0.f, _eggEscapeAnim[_counter++]);
          _entity->rotate(10);

          if (_counter == _eggEscapeAnim.size())
            _animState = TkEgg::anim::free;
          break;
        }
        default:
          break;
      }
    }

  }
  return offset;
}
