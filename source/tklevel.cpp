#include "tklevel.h"
#include "pugixml.hpp"

#include <unordered_map>
#include <fstream>
#include <string>

/**
 * @brief Constructor
 * @param windowSize
 */
TkLevel::TkLevel(const sf::Vector2u& windowSize)
    : view(static_cast<sf::Vector2f>(windowSize / 2U), static_cast<sf::Vector2f>(windowSize)),
      windowSize(windowSize), action(windowSize),
      teleportGhostAnim(teleportGhostPath, {0, 0, 48, 48}),
      teleportGhostNoAnim(teleportGhostNoCanDoPath, {0, 0, 48, 48}) {
  if (!eggSnd.loadFromFile("audio/sfx/sfx [12].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [12].wav'"
              << std::endl;
  if (!bridgeBuildSound.loadFromFile("audio/sfx/sfx [4].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [4].wav'"
              << std::endl;
  if (!actionFailSound.loadFromFile("audio/sfx/sfx [6].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [6].wav'"
              << std::endl;
  if (!teleportSound.loadFromFile("audio/sfx/sfx [17].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [17].wav'"
              << std::endl;
  teleportSound.setLoop(true);
}

/**
 * @brief Destructor
 */
TkLevel::~TkLevel(){
  if (data.levelMap != nullptr) {
    delete[] data.levelMap;
  }

  eggList.clear();
}

/**
 * @brief TkLevel::createLevel
 * @param levelName
 */
void TkLevel::createLevel(const std::string& levelName) {
  // Open the level file
  std::ifstream file(levelName, std::ios::out | std::ios::binary);
  if (!file) throw "Cannot open file " + levelName + "\n";

  file.read((char*)&data, 16);
  // Check the validity of the opened file
  if (data.magicNumber != tkMagicNumber) {
    throw "Opened file is not of type tokilevel: " + levelName + "\n";
  }
  if (data.version != tkVersion) {
    throw "Opened file version is v" + std::to_string(data.version) +
        ", expected v" + std::to_string(tkVersion) + "\n";
  }
  // Check the validity of the map
  if (data.levelWidth < tkGridMinWidth || data.levelHeight < tkGridMinHeight ||
      data.levelWidth > tkGridMaxWidth || data.levelHeight > tkGridMaxHeight) {
    throw std::string("Level '") + levelName + "' size " +
        std::to_string(data.levelWidth) + "x" + std::to_string(data.levelHeight) +
        " is not in the range ["+
        std::to_string(tkGridMinWidth)+"..."+std::to_string(tkGridMaxWidth)+"x"+
        std::to_string(tkGridMinHeight)+"..."+std::to_string(tkGridMaxHeight)+"]\n";
  }

  file.read((char*)&data.tilePngFile, 516);
  // Check the validity of configuration files
  if (!eggTexture.loadFromFile(data.eggsPngFile)) {
    throw std::string("Cannot open egg sprite file: ") +
        data.eggsPngFile + "\n";
  }
  if (!levelTiles.loadFromFile(data.tilePngFile)) {
    throw std::string("Cannot open tiles file: ") +
        data.tilePngFile + "\n";
  }
  pugi::xml_parse_result result;
  pugi::xml_document domShoebox;
  result = domShoebox.load_file(data.backXmlFile);
  if (result.status != pugi::status_ok) {
    throw std::string("Cannot open shoebox file '") +
        data.backXmlFile + "': " + result.description() + "\n";
  }
  pugi::xml_document domMusic;
  result = domMusic.load_file(data.musicFile);
  if (result.status != pugi::status_ok) {
    throw std::string("Cannot open music file '") +
        data.musicFile + "': " + result.description() + "\n";
  }

  if (!mapRender.create(data.levelWidth << 5, data.levelHeight << 5))
    throw "Cannot create map renderTexture";
  mapRender.clear(sf::Color::Transparent);

  if (!foregroundRender.create(data.levelWidth << 5, data.levelHeight << 5))
    throw "Cannot create foreground renderTexture";
  foregroundRender.clear(sf::Color::Transparent);

  // Load the level map
  data.levelMap = new uint32_t[data.getLevelSize()];
  file.read((char*)data.levelMap, sizeof(uint32_t) * data.getLevelSize());
  file.close();

  music.init(domMusic);
  shoebox.init(domShoebox, {data.levelWidth*16.f, data.levelHeight*16.f});

  buildBoard();
  resetPosition();
  action.update(data.action);

  music.play();
}

/**
 * @brief TkLevel::move
 * @param offset
 * @return
 */
void TkLevel::move(const sf::Vector2f& offset) {
  const sf::Vector2u minLimit = windowSize / 2U;
  const sf::Vector2u maxLimit =
      sf::Vector2u{data.levelWidth << 5, data.levelHeight << 5} - minLimit;

  if (offset.x != 0.f || offset.y != 0.f) {
    sf::Vector2f center = view.getCenter() + offset;

    if (center.x < minLimit.x) {
      center.x = minLimit.x;
    } else if (center.x > maxLimit.x) {
      center.x = maxLimit.x;
    }

    if (center.y < minLimit.y) {
      center.y = minLimit.y;
    } else if (center.y > maxLimit.y) {
      center.y = maxLimit.y;
    }

    if (player.syncPosition()) {
      center.x = (int)(center.x + 0.5f);
      center.y = (int)(center.y + 0.5f);
    }

    // Move the shoebox
    shoebox.move(center - view.getCenter());
    view.setCenter(center);
  }
}

/**
 * @brief TkLevel::update
 * @param gesture
 */
void TkLevel::update(const enum tk::gesture& gesture) {
  static const uint32_t margin{16U};

  sf::Vector2f offset = player.move(*this, gesture);
  sf::Vector2f playerPos = player.entity->getPosition();
  sf::Vector2f viewCenter = view.getCenter();

  if ((offset.x > 0.f && playerPos.x < viewCenter.x - margin) ||
      (offset.x < 0.f && playerPos.x > viewCenter.x + margin))
    offset.x = 0.f;
  if ((offset.y > 0.f && playerPos.y < viewCenter.y - margin) ||
      (offset.y < 0.f && playerPos.y > viewCenter.y + margin))
    offset.y = 0.f;

  if (offset.x != 0.f || offset.y != 0.f) move(offset);

  // Update the eggs
  for (auto& egg : eggList) egg->move(*this);

  // Update music
  finish = !music.update(player.getState());
}

/**
 * @brief TkLevel::eggChecker
 * @param position
 */
void TkLevel::eggChecker(const sf::FloatRect& position) {
  for (auto it = eggList.begin(); it != eggList.end();) {
    if ((*it)->getState() == TkEgg::anim::free) {
      delete (*it).release();
      it = eggList.erase(it);
    } else {
      sf::FloatRect rect =
          (*it)->entity->getTransform().transformRect({8, 4, 48, 60});
      if (rect.intersects(position)) {
        if (!(*it)->isReleased()) {
          (*it)->release();
          eggSnd.stop();
          eggSnd.play();
        }
      }
      ++it;
    }
  }

  if (eggList.empty()) {
    player.setState(TkPlayer::anim::levelComplete);
  }
}

/**
 * @brief TkLevel::handleMouseEvent
 * @param event
 */
void TkLevel::handleMouseEvent(const sf::Event& event) {

  switch (event.type) {
    case sf::Event::MouseButtonPressed: {
      if (isIdle()) return start();

      auto item = action.handleMouseButtonEvent(event.mouseButton);
      if (item == TkAction::TkItem::bridge) buildBridge();
      if (item == TkAction::TkItem::teleport) {
        teleportInProgress = !teleportInProgress;
        teleportInProgress ? buildTeleport() : clearTeleport();
      } else if (teleportInProgress) {
        //clang-format off
        static const std::unordered_map<teleportGate, tk::gesture> map{
            { teleportGate::left,   tk::gesture::left   },
            { teleportGate::right,  tk::gesture::right  },
            { teleportGate::up,     tk::gesture::up     },
            { teleportGate::down,   tk::gesture::down   }
        };
        //clang-format on

        sf::Vector2f delta(view.getCenter() - view.getSize() / 2.f);
        for (uint32_t index = 0U; index < teleportGate::size; ++index) {
          if (teleportGhost[index].enabled &&
              teleportGhost[index].rect.contains(
                  event.mouseButton.x + delta.x,
                  event.mouseButton.y + delta.y)) {
            player.updateTeleportState(TkPlayer::teleportBeamOut,
                                       map.at((teleportGate)index));
            action.decrease(TkAction::TkItem::teleport);
            clearTeleport();
            currentGate = (TkLevel::teleportGate)index;
            break;
          }
        }
      }
      break;
    }
    case sf::Event::MouseMoved: {
      action.handleMouseMovedEvent(event.mouseMove);

      if (teleportInProgress) {
        //clang-format off
        static const std::unordered_map<teleportGate, TkPlayer::anim> map{
            { teleportGate::left,   TkPlayer::teleportLookLeft   },
            { teleportGate::right,  TkPlayer::teleportLookRight  },
            { teleportGate::up,     TkPlayer::teleportLookUp     },
            { teleportGate::down,   TkPlayer::teleportLookDown   }
        };
        //clang-format on

        sf::Vector2f delta(view.getCenter() - view.getSize() / 2.f);
        for (uint32_t index = 0U; index < teleportGate::size; ++index) {
          if (teleportGhost[index].enabled &&
              teleportGhost[index].rect.contains(event.mouseMove.x + delta.x,
                                                 event.mouseMove.y + delta.y)) {
            player.updateTeleportState(map.at((teleportGate)index));
            break;
          }
        }
      }
      break;
    }
    default:
      break;
  }
}

/**
 * @brief TkLevel::buildBridge
 */
void TkLevel::buildBridge() {
  // clang-format off
  tk::gesture gesture;
  switch (player.getStandState()) {
    case TkPlayer::anim::standLeft:  gesture = tk::gesture::left;  break;
    case TkPlayer::anim::standRight: gesture = tk::gesture::right; break;
    default:                         gesture = tk::gesture::none;  break;
  }
  // clang-format on

  if (bridgeBuilderHandler(player.getPosition(), gesture)) {
    action.decrease(TkAction::TkItem::bridge);
  }
}

/**
 * @brief TkLevel::buildTeleport
 */
void TkLevel::buildTeleport() {
  if (player.actionAuthorized() &&
      action.authorized(TkAction::TkItem::teleport)) {
    teleportSound.stop();
    teleportSound.play();
    player.activeTeleport(true);
    teleportHandler(player.getPosition());
  } else {
    // Play fail sound
    actionFailSound.stop();
    actionFailSound.play();
  }
}

/**
 * @brief TkLevel::clearTeleport
 */
void TkLevel::clearTeleport() {
  // clang-format off
  teleportGhost[teleportGate::up].anim     =
  teleportGhost[teleportGate::down].anim  =
  teleportGhost[teleportGate::left].anim    =
  teleportGhost[teleportGate::right].anim   = nullptr;

  teleportGhost[teleportGate::up].enabled    =
  teleportGhost[teleportGate::down].enabled =
  teleportGhost[teleportGate::left].enabled   =
  teleportGhost[teleportGate::right].enabled  = false;
  // clang-format on

  teleportSound.stop();
  player.activeTeleport(false);
  teleportInProgress = false;
}
