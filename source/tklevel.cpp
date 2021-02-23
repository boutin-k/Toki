#include "tklevel.h"

/**
 * @brief Constructor
 * @param windowSize
 */
TkLevel::TkLevel(const sf::Vector2u& windowSize)
    :_windowSize(windowSize) {}

/**
 * @brief Destructor
 */
TkLevel::~TkLevel(){
  if (_data.levelMap != nullptr) {
    delete[] _data.levelMap;
  }
}

/**
 * @brief TkLevel::move
 * @param offset
 * @return
 */
sf::Vector2f TkLevel::move(const sf::Vector2f& offset) {
  if (offset.x != 0.f || offset.y != 0.f) {
    sf::Vector2f startPos = levelSprite.getPosition();

    levelSprite.move(-offset);

    sf::Vector2f finalPos = levelSprite.getPosition();
    sf::Vector2u limits =
        sf::Vector2u{_data.levelWidth << 5, _data.levelHeight << 5} - _windowSize;

    if (finalPos.x > 0.f) {
      finalPos.x = 0.f;
    } else if (std::abs(finalPos.x) > limits.x) {
      finalPos.x = static_cast<int>(-limits.x);
    }

    if (finalPos.y > 0.f) {
      finalPos.y = 0.f;
    } else if (std::abs(finalPos.y) > limits.y) {
      finalPos.y = static_cast<int>(-limits.y);
    }

    levelSprite.setPosition(finalPos);
    return offset - (startPos - finalPos);
  }
  return offset;
}
