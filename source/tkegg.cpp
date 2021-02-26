#include "tkegg.h"

// Array of escape Y movement
static constexpr std::array<float, 12UL> _eggEscapeAnim =
    { -32.f, -10.f, -5.f, -3.f,
        0.f,  10.f, 25.f, 25.f,
       32.f,  32.f, 50.f, 64.f };

/**
 * @brief TkEgg constructor with egg coordinates
 * @param positionX X position of the egg on the map
 * @param positionY Y position of the egg on the map
 */
TkEgg::TkEgg(const sf::Texture& texture, float positionX, float positionY)
    : _eggAnim(texture/*"Media/forest_egg.png"*/, { 0, 0, 64, 64 })
{
  // clang-format off
  _animMap = {
    { TkEgg::anim::idle,    { 0U, 10U, &_eggAnim,
      [this](uint32_t spriteIndex){
        if (spriteIndex == _eggAnim.getLastSprite() && !--_counter)
          _animState = TkEgg::anim::wriggle;
        return sf::Vector2f{};
      },
      true, { }, { } } },
    { TkEgg::anim::wriggle, { 0U, 39U, &_eggAnim,
      [this](uint32_t spriteIndex){
        if (spriteIndex == _eggAnim.getLastSprite())
          _counter = rand() % 50 + 20;
        return sf::Vector2f{};
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::idle} },
      { } } },
    { TkEgg::anim::escape,  { 40U, 40U, &_eggAnim,
      [this](uint32_t spriteIndex){
        if (spriteIndex == _eggAnim.getLastSprite()) {
          _entity->move(0.f, _eggEscapeAnim[_counter++]);
          _entity->rotate(10.f);

          if (_counter == _eggEscapeAnim.size())
            _animState = TkEgg::anim::free;
        }
        return sf::Vector2f{};
      },
      true, { }, { } } },
  };
  // clang-format on
  _entity = &_eggAnim;
  setPosition(positionX, positionY);
}

///**
// * @brief TkEgg constructor with egg coordinates
// * @param positionX X position of the egg on the map
// * @param positionY Y position of the egg on the map
// */
//TkEgg::TkEgg(const std::string& fileName, float positionX, float positionY) : TkEgg(fileName) {
//  setPosition(positionX, positionY);
//}

/**
 * @brief TkEgg destructor
 */
TkEgg::~TkEgg() {}

/**
 * @brief TkPlayer::move
 * @param level The map in which the egg involved
 * @param gesture The user action
 * @return The displacement coordinates. Returns always {0.f,0.f}
 */
sf::Vector2f TkEgg::move(TkLevel& /*level*/,
                         const enum tk::gesture& gesture) {
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
    if (sm.func) offset += sm.func(currentId);

    if (!sm.transitionOnLast || currentId == sm.spriteEnd) {
      for (auto pair : sm.transition) {
        if ((pair.first & gesture) == pair.first) {
          _animState = static_cast<anim>(pair.second);
          break;
        }
      }
    }
  }
  return offset;
}
