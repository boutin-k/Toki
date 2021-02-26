#include "forest.h"
#include "tkanimation.h"

#include <fstream>

/**
 * @brief Forest::Forest
 * @param windowSize
 */
Forest::Forest(const sf::Vector2u& windowSize)
    : TkLevel(windowSize) {
}

Forest::~Forest(void) {
  for (auto egg : _eggList) delete egg;
  _eggList.clear();
}

/**
 * @brief Forest::createLevel
 * @param levelName
 */
void Forest::createLevel(const std::string& levelName) {
  // Open the level file
  std::ifstream file(levelName, std::ios::out | std::ios::binary);
  if (!file) throw "Cannot open file " + levelName + "\n";

  file.read((char*)&_data, sizeof(uint32_t) << 2);
  _data.levelMap = new uint8_t[_data.getLevelSize()];
  file.read((char*)_data.levelMap, sizeof(uint8_t)*_data.getLevelSize());
  file.close();

//  std::fstream file;
//  uint posX = 160U;
//  uint posY = 416U;
//  file.open("01_ForestFalls.tokilevel", std::ios::app | std::ios::binary);
//  file.write(reinterpret_cast<char*>(&posX), sizeof(uint));
//  file.write(reinterpret_cast<char*>(&posY), sizeof(uint));
//  file.write(reinterpret_cast<char*>(&data.width), sizeof(uint));
//  file.write(reinterpret_cast<char*>(&data.height), sizeof(uint));
//  file.write(reinterpret_cast<char*>(data.board.data()), sizeof(unsigned char) * 880);

  if (!mapRender.create(_data.levelWidth<<5, _data.levelHeight<<5))
    throw "Can't create map forest renderTexture";
  mapRender.clear(sf::Color::Transparent);

  TkImage forestTiles("Media/forest_tiles.png");

  // Build the board
  uint32_t size = _data.getLevelSize();
  for (uint32_t i = 0U; i < size; ++i) {

    // Populate the egg list
    if (_data.levelMap[i] == 0xFE) {
      _eggList.push_back(new TkEgg((i % _data.levelWidth) << 5,
                                   (i / _data.levelWidth) << 5));
      continue;
    }

    // Populate the map
    if (_data.levelMap[i] != 0xFF) {
      forestTiles.setTextureRect(_sprites[_data.levelMap[i]]);
      forestTiles.setPosition((i % _data.levelWidth) << 5,
                              (i / _data.levelWidth) << 5);
      mapRender.draw(forestTiles);
    }
  }

  mapRender.display();
  mapSprite.setTexture(mapRender.getTexture());

  if (!levelRender.create(_data.levelWidth << 5, _data.levelHeight << 5))
    throw "Can't create map forest renderTexture";
  levelSprite.setTexture(levelRender.getTexture());

  resetPosition();
}

/**
 * @brief Forest::render
 */
const sf::Drawable& Forest::render() {
  levelRender.clear(sf::Color::Transparent);
  levelRender.draw(mapSprite);
  for (auto egg : _eggList)
    levelRender.draw(*egg->drawableSprite());
  levelRender.draw(*_player.drawableSprite());
  levelRender.display();

  return levelSprite;
}

/**
 * @brief Forest::isMovable
 * @param origin
 * @param gesture
 * @return
 */
tk::action Forest::isMovable(const sf::Vector2f& origin, tk::gesture gesture) const {
  uint32_t position = ((origin.x / 32) - 1) + (((origin.y / 32) - 1) * _data.levelWidth);

  if (gesture == tk::gesture::left || gesture == tk::gesture::right) {
    // Check head collision
    position += (gesture == tk::gesture::left) ? -1 : 2;
    if (_data.levelMap[position] < 0x30 || _data.levelMap[position] == 0x36)
      return tk::action::none;
    // Check staircase up
    position += _data.levelWidth;
    if (_data.levelMap[position] < 0x30 || _data.levelMap[position] == 0x36) {
      position -= (_data.levelWidth << 1);
      if (_data.levelMap[position] < 0x30 || _data.levelMap[position] == 0x36)
        return tk::action::none;
      return tk::action::jumpUp;
    }
    // Check run
    position += _data.levelWidth + (gesture == tk::gesture::left ? 1 : -1);
    if (_data.levelMap[position] <= 0x36)
      return tk::action::run;
    // Check staircase down
    position += _data.levelWidth;
    if (_data.levelMap[position] <= 0x36)
      return tk::action::jumpDown;
    return tk::action::fall;
  }

  if (gesture == tk::gesture::up) {
    // Check ladder
    position += _data.levelWidth;
    if (_data.levelMap[position] >= 0x30 && _data.levelMap[position] < 0x36) {
      position -= (_data.levelWidth << 1);
      if (_data.levelMap[position] < 0x30 || _data.levelMap[position] == 0x36)
        return tk::action::none;
      return tk::action::ladderUp;
    }
  }

  if (gesture == tk::gesture::down) {
    // Check ladder
    position += (_data.levelWidth << 1);
    if (_data.levelMap[position] >= 0x30 && _data.levelMap[position] < 0x36)
      return tk::action::ladderDown;
    if (_data.levelMap[position] == 0xFF) return tk::action::fall;
    return tk::action::none;
  }

  return tk::action::none;
}