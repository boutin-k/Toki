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
    : eggAnim(texture, { 0, 0, 64, 64 })
{
  // clang-format off
  animMap = {
    { TkEgg::anim::idle,    { 0U, 10U, &eggAnim,
      [this](uint32_t spriteIndex){
        if (spriteIndex == eggAnim.getLastSprite() && !--counter)
          animState = TkEgg::anim::wriggle;
        return sf::Vector2f{};
      },
      true, { }, { } } },
    { TkEgg::anim::wriggle, { 0U, 39U, &eggAnim,
      [this](uint32_t spriteIndex){
        if (spriteIndex == eggAnim.getLastSprite())
          counter = rand() % 50 + 20;
        return sf::Vector2f{};
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::idle} },
      { } } },
    { TkEgg::anim::escape,  { 40U, 40U, &eggAnim,
      [this](uint32_t spriteIndex){
        if (spriteIndex == eggAnim.getLastSprite()) {
          entity->move(0.f, _eggEscapeAnim[counter++]);
          entity->rotate(10.f);

          if (counter == _eggEscapeAnim.size())
            animState = TkEgg::anim::free;
        }
        return sf::Vector2f{};
      },
      true, { }, { } } },
  };
  // clang-format on
  entity = &eggAnim;
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

  auto currentState = animMap.find(animState);
  if (currentState != animMap.end()) {
    animMachine sm = currentState->second;

    if (animState != lastAnimState) {
      entity = sm.image;
      entity->resetSprite(sm.spriteBegin, sm.spriteEnd);
      entity->setPosition(position);
      // Update last animation state
      lastAnimState = animState;
    }
    uint currentId = entity->nextSprite();
    if (sm.func) offset += sm.func(currentId);

    if (!sm.transitionOnLast || currentId == sm.spriteEnd) {
      for (auto pair : sm.transition) {
        if ((pair.first & gesture) == pair.first) {
          animState = static_cast<anim>(pair.second);
          break;
        }
      }
    }
  }
  return offset;
}
