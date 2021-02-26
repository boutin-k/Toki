#include "tklevel.h"

/**
 * @brief Constructor
 * @param windowSize
 */
TkLevel::TkLevel(const sf::Vector2u& windowSize)
    :_windowSize(windowSize) {
    if (!_escapeEggSnd.openFromFile("Media/sfx [12].wav"))
      std::cerr << "Error. Can't load sound file Media/sfx [12].wav" << std::endl;
}

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
void TkLevel::move(const sf::Vector2f& offset) {
  if (offset.x != 0.f || offset.y != 0.f) {

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
  }
}

/**
 * @brief TkLevel::eggChecker
 * @param position
 */
void TkLevel::eggChecker(const sf::FloatRect& position) {
  for (auto it = _eggList.begin(); it != _eggList.end();) {
    if ((*it)->getState() == TkEgg::anim::free) {
      delete (*it);
      it = _eggList.erase(it);
    } else {
      sf::FloatRect rect =
          (*it)->_entity->getTransform().transformRect({8, 4, 48, 60});
      if (rect.intersects(position)) {
        if (!(*it)->isReleased()) {
          (*it)->release();
          _escapeEggSnd.stop();
          _escapeEggSnd.play();
        }
      }
      ++it;
    }
  }
}
