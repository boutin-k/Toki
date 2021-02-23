#include "forest.h"
#include "tkanimation.h"

#include <iostream>
#include <fstream>

/**
 * @brief Forest::Forest
 * @param windowSize
 */
Forest::Forest(const sf::Vector2u& windowSize)
    : TkLevel(windowSize) {

  if (!mEggSnd.openFromFile("Media/sfx [12].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [12].wav" << std::endl;
}

Forest::~Forest(void) {}

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

  if (!mapTexture.loadFromFile("Media/forest_tiles.png"))
    throw "Can't load map forest tiles file";
  if (!mapRender.create(_data.levelWidth<<5, _data.levelHeight<<5))
    throw "Can't create map forest renderTexture";
  mapRender.clear(sf::Color::Transparent);

  if (!eggTexture.loadFromFile("Media/forest_egg.png"))
    throw "Can't load forest_egg sprite file";
  if (!eggRender.create(_data.levelWidth<<5, _data.levelHeight<<5))
    throw "Can't create forest_egg renderTexture";

  // Build the board
  uint32_t size = _data.getLevelSize();
  for (uint32_t i = 0U; i < size; ++i) {
    // Populate the egg list
    if (_data.levelMap[i] == 0xFE) {
      TkAnimation egg(eggTexture, {0, 0, 64, 64});
      egg.resetSprite(0, 39);
      egg.move((i % _data.levelWidth) << 5, (i / _data.levelWidth) << 5);
      _eggList.push_back(std::move(egg));
      continue;
    }
    // Populate the map
    if (_data.levelMap[i] != 0xFF) {
      sf::Sprite sprite(mapTexture, sprites[_data.levelMap[i]]);
      sprite.move((i % _data.levelWidth) << 5, (i / _data.levelWidth) << 5);
      mapRender.draw(sprite);
    }
  }

  mapRender.display();
  // get the target texture (where the stuff has been drawn)
  mapSprite.setTexture(mapRender.getTexture());
  eggSprite.setTexture(eggRender.getTexture());

  if (!levelRender.create(_data.levelWidth << 5, _data.levelHeight << 5))
    throw "Can't create map forest renderTexture";
  levelSprite.setTexture(levelRender.getTexture());
}

/**
 * @brief Forest::render
 */
void Forest::render(void) {
  eggRender.clear(sf::Color::Transparent);
  for (uint32_t i = 0U; i < _eggList.size(); ++i) {
    _eggList[i].nextSprite();
    eggRender.draw(_eggList[i]);
  }
  eggRender.display();

  levelRender.clear(sf::Color::Transparent);
  levelRender.draw(mapSprite);
  levelRender.draw(eggSprite);
  levelRender.display();
}

/**
 * @brief Forest::eggChecker
 * @param position
 */
void Forest::eggChecker(const sf::FloatRect& position) {
  for (auto it = _eggList.begin(); it != _eggList.end(); ++it) {
    sf::FloatRect rect = it->getGlobalBounds();
    rect.left += levelSprite.getPosition().x;
    rect.top += levelSprite.getPosition().y;
    if (rect.intersects(position)) {
      _eggList.erase(it);
      mEggSnd.stop();
      mEggSnd.play();
      break;
    }
  }
}

/**
 * @brief Forest::isMovable
 * @param origin
 * @param gesture
 * @return
 */
tk::action Forest::isMovable(const sf::Vector2f& origin, tk::gesture gesture) {
  uint32_t position = (((origin.x-levelSprite.getPosition().x) / 32) - 1)
                    + ((((origin.y-levelSprite.getPosition().y) / 32) - 1) * _data.levelWidth);

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
    std::cout << position << std::endl;
    if (_data.levelMap[position] >= 0x30 && _data.levelMap[position] < 0x36)
      return tk::action::ladderDown;
    if (_data.levelMap[position] == 0xFF) return tk::action::fall;
    return tk::action::none;
  }

  return tk::action::none;
}
