#include "forest.h"
#include <iostream>
/**
 * @brief Forest::Forest
 * @param windowSize
 */
Forest::Forest(const sf::Vector2u& windowSize)
    : TkLevel(), _windowSize(windowSize) {}

/**
 * @brief Forest::createLevel
 */
void Forest::createLevel(void) {
  // clang-format off
  data.width = 40U;
  data.height = 22U;
  data.board = {
      //01
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF,
      //02
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF,
      //03
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF,
      //04
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF,
      //05
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF,
      //06
      0x01, 0x02, 0x01, 0x02, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x01, 0x02, 0x01,
      0x02, 0x01, 0x02, 0x01,
      //07
      0x04, 0x05, 0x06, 0x07, 0x13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x29, 0x0A, 0x2C, 0x04,
      0x05, 0x06, 0x07, 0x08,
      //08
      0x14, 0x15, 0x16, 0x17, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20, 0x14,
      0x15, 0x16, 0x17, 0x18,
      //09
      0x04, 0x05, 0x06, 0x07, 0x11, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
      0x03, 0x36, 0x36, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x30, 0x31, 0x00,
      0x02, 0x01, 0x02, 0x01, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x29, 0x0A,
      0x2C, 0x04, 0x05, 0x06,
      //10
      0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x32, 0x33, 0x10,
      0x04, 0x05, 0x06, 0x07, 0x13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0x20, 0x14, 0x15, 0x16,
      //11
      0x14, 0x15, 0x16, 0x17, 0x2D, 0x1A, 0x1B, 0x2E, 0x14, 0x15, 0x16, 0x17,
      0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01, 0x02, 0x01, 0x12,
      0x14, 0x15, 0x16, 0x17, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0x24, 0x04, 0x05, 0x06,
      //12
      0x04, 0x05, 0x06, 0x2B, 0x26, 0xFF, 0xFF, 0x29, 0x0A, 0x0B, 0x2C, 0x04,
      0x11, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x12, 0x04, 0x05, 0x06, 0x07,
      0x08, 0x2B, 0x0E, 0x0F, 0x26, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0x20, 0x14, 0x15, 0x16,
      //13
      0x14, 0x15, 0x16, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x27, 0x1A,
      0x1B, 0x2E, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x14, 0x15, 0x2D, 0x1A,
      0x1B, 0x22, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
      0x12, 0x04, 0x05, 0x06,
      //14
      0x04, 0x05, 0x06, 0x13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0x29, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0A, 0x0B, 0x0C, 0x26, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x20,
      0x14, 0x15, 0x16, 0x17,
      //15
      0x14, 0x15, 0x16, 0x11, 0x03, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10,
      0x04, 0x05, 0x06, 0x07,
      //16
      0x04, 0x05, 0x06, 0x07, 0x13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x02, 0x02, 0x01, 0x02, 0x02, 0x12,
      0x14, 0x15, 0x16, 0x17,
      //17
      0x14, 0x15, 0x16, 0x17, 0x23, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
      0xFF, 0xFF, 0x00, 0x01, 0x02, 0x12, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x04, 0x05, 0x06, 0x07,
      //18
      0x04, 0x05, 0x06, 0x07, 0x13, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x01,
      0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01, 0x02, 0x01,
      0x02, 0x02, 0x12, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06,
      0x07, 0x08, 0x09, 0x04,
      //19
      0x14, 0x15, 0x16, 0x17, 0x11, 0x01, 0x02, 0x01, 0x02, 0x01, 0x12, 0x14,
      0x15, 0x16, 0x17, 0x18, 0x19, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x14,
      0x15, 0x16, 0x17, 0x18, 0x19, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x14,
      0x15, 0x16, 0x17, 0x18,
      //20
      0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x04, 0x05, 0x06, 0x07,
      //21
      0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
      0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
      0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
      0x14, 0x15, 0x16, 0x17,
      //22
      0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
      0x04, 0x05, 0x06, 0x07,
    };
  // clang-format on

  if (!texture.loadFromFile("Media/forest_gnd.png"))
    throw "Can't load map forest sprite file";

  if (!renderTexture.create(32 * data.width, 32 * data.height))
    throw "Can't create map forest renderTexture";
  renderTexture.clear(sf::Color::Transparent);

  // Build the board
  uint size = data.width * data.height;
  for (uint i = 0; i < size; ++i) {
    if (data.board[i] != 0xFF) {
      sf::Sprite sprite(texture, sprites[data.board[i]]);
      sprite.move((i % data.width) << 5, (i / data.width) << 5);
      renderTexture.draw(sprite);
    }
  }

  renderTexture.display();

  // get the target texture (where the stuff has been drawn)
  sprite.setTexture(renderTexture.getTexture());
}

/**
 * @brief Forest::isMovable
 * @param origin
 * @param gesture
 * @return
 */
tk::action Forest::isMovable(const sf::Vector2f& origin, tk::gesture gesture) {
  uint position = (((origin.x-sprite.getPosition().x) / 32) - 1)
                + ((((origin.y-sprite.getPosition().y) / 32) - 1) * data.width);

  if (gesture == tk::gesture::left || gesture == tk::gesture::right) {
    // Check head collision
    position += (gesture == tk::gesture::left) ? -1 : 2;
    if (data.board[position] < 0x30 || data.board[position] == 0x36)
      return tk::action::none;
    // Check staircase up
    position += data.width;
    if (data.board[position] < 0x30 || data.board[position] == 0x36) {
      position -= (data.width << 1);
      if (data.board[position] < 0x30 || data.board[position] == 0x36)
        return tk::action::none;
      return tk::action::jumpUp;
    }
    // Check run
    position += data.width + (gesture == tk::gesture::left ? 1 : -1);
    if (data.board[position] <= 0x36)
      return tk::action::run;
    // Check staircase down
    position += data.width;
    if (data.board[position] <= 0x36)
      return tk::action::jumpDown;
    return tk::action::fall;
  }

  if (gesture == tk::gesture::up) {
    // Check ladder
    position += data.width;
    if (data.board[position] >= 0x30 && data.board[position] < 0x36) {
      position -= (data.width << 1);
      if (data.board[position] < 0x30 || data.board[position] == 0x36)
        return tk::action::none;
      return tk::action::ladderUp;
    }
  }

  if (gesture == tk::gesture::down) {
    // Check ladder
    position += (data.width << 1);
    std::cout << position << std::endl;
    if (data.board[position] >= 0x30 && data.board[position] < 0x36)
      return tk::action::ladderDown;
    if (data.board[position] == 0xFF) return tk::action::fall;
    return tk::action::none;
  }

  return tk::action::none;
}

/**
 * @brief Forest::move
 * @param offset
 */
sf::Vector2f Forest::move(const sf::Vector2f& offset) {
  if (offset.x != 0.f || offset.y != 0.f) {
    sf::Vector2f startPos = sprite.getPosition();

    sprite.move(-offset);

    sf::Vector2f finalPos = sprite.getPosition();
    sf::Vector2u limits =
        sf::Vector2u{data.width << 5, data.height << 5} - _windowSize;

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

    sprite.setPosition(finalPos);
    return offset - (startPos - finalPos);
  }
  return offset;
}
