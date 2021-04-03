#include "forest.h"
#include "tkanimation.h"

#include <cmath>

/**
 * @brief Forest::Forest
 * @param windowSize
 */
Forest::Forest(const sf::Vector2u& windowSize)
    : TkLevel(windowSize),
      bridgeBuildAnim(bridgeBuilderPath, {0, 0, 64, 32}),
      bridgeBuildNoAnim(bridgeBuilderNoCanDoPath, {0, 0, 64, 32}) {
  bridgeBuildAnim.setOrigin(0, 0);
  bridgeBuildAnim.resetSprite(0, 4);

  bridgeBuildNoAnim.setOrigin(0, 0);
  bridgeBuildNoAnim.resetSprite(0, 4);

  teleportGhostAnim.resetSprite(0, 4);
  teleportGhostNoAnim.resetSprite(0, 4);
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
      eggList.push_back(std::make_unique<TkEgg>(eggTexture,
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
}

/**
 * @brief render Render all the forest map textures in a sprite
 */
void Forest::render(sf::RenderWindow& window) {
  // push
  window.setView(view);
  {
    window.clear(sf::Color(255, 255, 255, 255));
    for (const auto& item : shoebox.getBackList()) window.draw(*item.second.get());

    window.draw(mapSprite);
    if (bridgeBuildAnimPtr) window.draw(bridgeBuildAnim);
    for (auto& egg : eggList) window.draw(*egg->drawableSprite());
    window.draw(*player.drawableSprite());
    window.draw(foregroundSprite);
    if (bridgeBuildNoAnimPtr) window.draw(bridgeBuildNoAnim);

    if (teleportInProgress == true) {
      for (uint32_t index = 0U; index < teleportGate::size; ++index) {
        if (teleportGhost[index].anim != nullptr) {
          teleportGhost[index].anim->setPosition(teleportGhost[index].pos);
          window.draw(*teleportGhost[index].anim);
        }
      }
    }
    for (const auto& item : shoebox.getFrontList()) window.draw(*item.second.get());
  }
  // pop
  window.setView(window.getDefaultView());

  if (action.isSelected()) window.draw(action.selectedImage);
  if (action.isHovered()) window.draw(action.hoveredImage);
  for (int i = 0; i < TkAction::size; ++i) {
    window.draw(action.list[i].button);
    if (action.list[i].enabled && !action.list[i].infinite) {
      sf::Vector2f position = action.list[i].button.getPosition();

      action.counterImage.setPosition(position.x, position.y + 41.5f);
      window.draw(action.counterImage);

      action.counterText.setString(std::to_string(action.list[i].counter));
      action.counterText.setOrigin(6.f, 16.f);
      action.counterText.setPosition(action.counterImage.getPosition());
      window.draw(action.counterText);
    }
  }
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

/**
 * @brief Forest::update
 * @param gesture
 */
void Forest::update(const enum tk::gesture& gesture) {
  TkLevel::update(gesture);

  // BRIDGE
  if (bridgeBuildAnimPtr != nullptr) {
    if (bridgeBuildAnim.nextSprite() == bridgeBuildAnim.getLastSprite()) {
      levelTiles.setTextureRect(sprites[0x36]);
      levelTiles.setPosition(bridgeBuildAnim.getPosition());
      mapRender.draw(levelTiles);
      levelTiles.setPosition(bridgeBuildAnim.getPosition() +
                             sf::Vector2f{32.f, 0.f});
      mapRender.draw(levelTiles);

      // Free the bridge builder pointer
      bridgeBuildAnimPtr = nullptr;
    }
  }
  if (bridgeBuildNoAnimPtr != nullptr) {
    if (bridgeBuildNoAnim.nextSprite() == bridgeBuildNoAnim.getLastSprite()) {
      bridgeBuildNoAnimPtr = nullptr;
    }
  }

  // TELEPORT
  if (teleportInProgress == true) {
    if (teleportGhost[teleportGate::up].anim != nullptr   ||
        teleportGhost[teleportGate::down].anim != nullptr ||
        teleportGhost[teleportGate::left].anim != nullptr ||
        teleportGhost[teleportGate::right].anim != nullptr) {
      teleportGhostAnim.nextSprite();
      teleportGhostNoAnim.nextSprite();
    }
  }
}

/**
 * @brief Forest::bridgeBuilderHandler
 * @param origin
 * @param gesture
 */
bool Forest::bridgeBuilderHandler(const sf::Vector2f& origin,
                                  const tk::gesture& gesture) {
  uint32_t position =
      ((uint32_t)std::rint(origin.x) / 32) +
      (((uint32_t)std::rint(origin.y) / 32 + 1) * data.levelWidth);

  if (player.actionAuthorized() &&
      (gesture == tk::gesture::left || gesture == tk::gesture::right)) {
    for (int i = 0; i < 2; ++i) {
      int32_t next = (gesture == tk::gesture::left ? -1 : 1);

      position -= (gesture == tk::gesture::left ? 1 : 0);
      if (action.authorized(TkAction::bridge)    &&
          data.levelMap[position]        == 0xFF &&
          data.levelMap[position + next] == 0xFF) {

        if (bridgeBuildAnimPtr == nullptr) {
          data.levelMap[position] = data.levelMap[position + next] = 0x36;
          position -= (gesture == tk::gesture::left ? 1 : 0);

          bridgeBuildAnimPtr = &bridgeBuildAnim;
          bridgeBuildAnim.setPosition((position % data.levelWidth) << 5,
                                 (position / data.levelWidth) << 5);

          // Play win sound
          bridgeBuildSound.stop();
          bridgeBuildSound.play();
          return true;
        }
        return false;
      }
      position += (gesture == tk::gesture::right ? 1 : 0);
    }
  }

  if (bridgeBuildNoAnimPtr == nullptr) {
    position -= 1;
    bridgeBuildNoAnimPtr = &bridgeBuildNoAnim;
    bridgeBuildNoAnim.setPosition((position % data.levelWidth) << 5,
                             (position / data.levelWidth) << 5);
    // Play fail sound
    actionFailSound.stop();
    actionFailSound.play();
  }
  return false;
}

/**
 * @brief Forest::teleportHandler
 * @param origin
 */
void Forest::teleportHandler(const sf::Vector2f& origin) {
  uint32_t position = ((uint32_t)std::rint(origin.x) / 32) +
                      (((uint32_t)std::rint(origin.y) / 32) * data.levelWidth);

  // clang-format off
  struct handler { uint32_t position; bool enabled; };
  handler gate[teleportGate::size] = {
      {position - (data.levelWidth << 2), (origin.y >= 160)},
      {position + (data.levelWidth << 2), (origin.y <= (data.levelHeight << 5) - 192)},
      {position - 4,                      (origin.x >= 192)},
      {position + 4,                      (origin.x <= (data.levelWidth  << 5) - 192)}};

  for (uint32_t index = 0U; index < teleportGate::size; ++index) {
    if (gate[index].enabled) {
      uint32_t pos = gate[index].position;
      if (data.levelMap[pos]                       > 0x39 &&
          data.levelMap[pos - 1]                   > 0x39 &&
          data.levelMap[pos - data.levelWidth]     > 0x39 &&
          data.levelMap[pos - data.levelWidth - 1] > 0x39) {
        teleportGhost[index].anim = &teleportGhostAnim;
        teleportGhost[index].enabled = true;
      } else {
        teleportGhost[index].anim = &teleportGhostNoAnim;
        teleportGhost[index].enabled = false;
      }
      teleportGhost[index].pos.x = ((pos % data.levelWidth) << 5);
      teleportGhost[index].pos.y = ((pos / data.levelWidth) << 5);
      teleportGhost[index].rect = { teleportGhost[index].pos.x - 32.f,
                                    teleportGhost[index].pos.y - 32.f, 64.f, 64.f };
    } else {
      teleportGhost[index].anim = nullptr;
    }
  }
  // clang-format on
}
