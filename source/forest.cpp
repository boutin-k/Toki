#include "forest.h"
#include "tkanimation.h"

#include <cmath>

/**
 * @brief Forest::Forest
 * @param windowSize
 */
Forest::Forest(const sf::Vector2u& windowSize)
    : TkLevel(windowSize), bridgeBuilder(bridgeBuilderPath, {0, 0, 64, 32}) {
  bridgeBuilder.setOrigin(0, 0);
  bridgeBuilder.resetSprite(0, 3);
}

Forest::~Forest(void) {}

/**
 * @brief Forest::buildBoard
 */
void Forest::buildBoard() {
  // Build the board
  uint32_t size = data.getLevelSize();
  for (uint32_t i = 0U; i < size; ++i) {
    // Find toki start position
    if ((data.levelMap[i] & 0xFF) == 0xFD) {
      startPosition.x = (((i + 1) % data.levelWidth) << 5);
      startPosition.y = (((i + 2) / data.levelWidth) << 5);
    }

    // Populate the egg list
    else if ((data.levelMap[i]&0xFF) == 0xFE) {
      eggList.push_back(new TkEgg(eggTexture,
                                   ((i+1) % data.levelWidth) << 5,
                                   ((i+2) / data.levelWidth) << 5));
      data.levelMap[i] +=1;
    }

    if ((data.levelMap[i]&0xFF00)) {
      levelTiles.setTextureRect(sprites[(data.levelMap[i]&0xFF00)>>8]);
      levelTiles.setPosition((i % data.levelWidth) << 5,
                              (i / data.levelWidth) << 5);
      mapRender.draw(levelTiles);
    }

    // Populate the map
    if ((data.levelMap[i]&0xFF) != 0xFF) {
      levelTiles.setTextureRect(sprites[(data.levelMap[i]&0xFF)]);
      levelTiles.setPosition((i % data.levelWidth) << 5,
                              (i / data.levelWidth) << 5);
      mapRender.draw(levelTiles);
    }

    // Populate the map
    if ((data.levelMap[i]&0xFF0000)) {
      levelTiles.setTextureRect(sprites[(data.levelMap[i]&0xFF0000)>>16]);
      levelTiles.setPosition((i % data.levelWidth) << 5,
                              (i / data.levelWidth) << 5);
      foregroundRender.draw(levelTiles);
    }
    data.levelMap[i] &= 0xFF;
  }

  mapRender.display();
  mapSprite.setTexture(mapRender.getTexture());

  foregroundRender.display();
  foregroundSprite.setTexture(foregroundRender.getTexture());

  if (!levelRender.create(data.levelWidth << 5, data.levelHeight << 5))
    throw "Can't create map forest renderTexture";
  levelSprite.setTexture(levelRender.getTexture());
}

/**
 * @brief render Render all the forest map textures in a sprite
 * @return The final sprite
 */
const sf::Drawable& Forest::render() {
  levelRender.clear(sf::Color::Transparent);
  levelRender.draw(mapSprite);
  for (auto egg : eggList)
    levelRender.draw(*egg->drawableSprite());
  levelRender.draw(*player.drawableSprite());
  levelRender.draw(foregroundSprite);
  levelRender.display();

  return levelSprite;
}

/**
 * @brief Forest::isMovable
 * @param origin The position of the player on the forest map
 * @param gesture The user keyboard action
 * @return The authorized movement related to the gesture and the map
 */
tk::action Forest::isMovable(const sf::Vector2f& origin, tk::gesture gesture, bool recursiveLocked) const {
  uint32_t position =
      ((uint32_t)std::rint(origin.x) / 32 - 1) +
      (((uint32_t)std::rint(origin.y) / 32 - 1) * data.levelWidth);

  switch (gesture) {
    case tk::gesture::left:
    case tk::gesture::right: {

      // Check head collision
      position += (gesture == tk::gesture::left) ? -1 : 2;
      if (data.levelMap[position] < 0x3A)
        return tk::action::none;

      // Check staircase up
      position += data.levelWidth;
      if (data.levelMap[position] < 0x3A) {
        position -= (data.levelWidth << 1);
        if (data.levelMap[position] < 0x3A)
          return tk::action::none;
        return tk::action::jumpUp;
      }

      // Check run
      position += data.levelWidth + (gesture == tk::gesture::left ? 1 : -1);
      if (data.levelMap[position] < 0x40) return tk::action::run;

      if (recursiveLocked == false) {
        sf::Vector2f tmp{(gesture == tk::gesture::left ? -32.f : 32.f), 0};
        if (isMovable(origin + tmp, gesture, true) == tk::action::run)
          return tk::action::run;
      }

      // Check staircase down
      position += data.levelWidth;
      if (data.levelMap[position] < 0x40 ||
          data.levelMap[position+(gesture == tk::gesture::left ? -1:1)] < 0x40)
        return tk::action::jumpDown;
      return tk::action::fall;
    }

    case tk::gesture::up: {
      // Check ladder
      position += data.levelWidth;
      if (data.levelMap[position] == 0x3A ||
          data.levelMap[position] == 0x3C ||
          data.levelMap[position] == 0x3E) {
        position -= (data.levelWidth << 1);
        if (data.levelMap[position] < 0x3A ||
            data.levelMap[position+1] < 0x3A)
          return tk::action::none;
        return tk::action::ladderUp;
      }
      break;
    }

    case tk::gesture::down: {
      // Check ladder
      position += (data.levelWidth << 1);
      if (data.levelMap[position] == 0x3A ||
          data.levelMap[position] == 0x3C ||
          data.levelMap[position] == 0x3E)
        return tk::action::ladderDown;
      if (data.levelMap[position] == 0xFF &&
          data.levelMap[position+1] == 0xFF)
        return tk::action::fall;
      return tk::action::none;
    }

    case tk::gesture::none: {
      // Check empty
      position += (data.levelWidth << 1);
      if (data.levelMap[position] == 0xFF &&
          data.levelMap[position+1] == 0xFF)
        return tk::action::fall;
    }
    default:
      break;
  }

  return tk::action::none;
}

void Forest::updateGesture(const enum tk::gesture& gesture) {
  TkLevel::updateGesture(gesture);

  if (bridgeBuilderPtr != nullptr) {
    if (bridgeBuilder.nextSprite() == bridgeBuilder.getLastSprite()) {
      levelTiles.setTextureRect(sprites[0x36]);
      levelTiles.setPosition(bridgeBuilder.getPosition());
      mapRender.draw(levelTiles);
      levelTiles.setPosition(bridgeBuilder.getPosition() +
                             sf::Vector2f{32.f, 0.f});
      mapRender.draw(levelTiles);

      // Free the bridge builder pointer
      bridgeBuilderPtr = nullptr;
    } else {
      // Until animation sprite is not the last
      mapRender.draw(bridgeBuilder);
    }
  }
}

/**
 * @brief Forest::bridgeBuilderHandler
 * @param origin
 * @param anim
 * @param position
 */
bool Forest::bridgeBuilderHandler(const sf::Vector2f& origin,
                                  const tk::gesture& gesture) {
  uint32_t position =
      ((uint32_t)std::rint(origin.x) / 32) +
      (((uint32_t)std::rint(origin.y) / 32 + 1) * data.levelWidth);

  if (gesture == tk::gesture::left || gesture == tk::gesture::right) {
    for (int i = 0; i < 2; ++i) {
      int32_t next = (gesture == tk::gesture::left ? -1 : 1);

      position -= (gesture == tk::gesture::left ? 1 : 0);
      if (data.levelMap[position] == 0xFF &&
          data.levelMap[position + next] == 0xFF) {

        if (bridgeBuilderPtr == nullptr) {
          data.levelMap[position] = data.levelMap[position + next] = 0x36;
          position -= (gesture == tk::gesture::left ? 1 : 0);

          bridgeBuilderPtr = &bridgeBuilder;
          bridgeBuilder.setPosition((position % data.levelWidth) << 5,
                                    (position / data.levelWidth) << 5);

          return true;
        }
        return false;
      }
      position += (gesture == tk::gesture::right ? 1 : 0);
    }
  }

  return false;
}
