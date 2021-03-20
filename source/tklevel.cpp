#include "tklevel.h"
#include "pugixml.hpp"

#include <unordered_map>
#include <fstream>
#include <string>

static constexpr auto tkDefaultVolume = 25.f;

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

  for (auto it = eggList.begin(); it != eggList.end(); it++) delete *it;
  eggList.clear();

  for (auto it = shoeboxTextureMap.begin(); it != shoeboxTextureMap.end(); it++)
    delete &(*it->second);

  if (musicList.size()) {
    for (auto& music : musicList) {
      (*music.get())->stop();
    }
    musicList.clear();
  }
  if (finishMusic.getStatus() != sf::SoundSource::Stopped) {
    finishMusic.stop();
  }
  shoeboxTextureMap.clear();
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
    throw "Cannot create map renderTexture";
  foregroundRender.clear(sf::Color::Transparent);

  // Load the level map
  data.levelMap = new uint32_t[data.getLevelSize()];
  file.read((char*)data.levelMap, sizeof(uint32_t) * data.getLevelSize());
  file.close();

  initMusic(domMusic);
  initShoebox(domShoebox);

  buildBoard();
  resetPosition();
  action.update(data.action);

  (*musicList[musicCounter].get())->play();
}

/**
 * @brief TkLevel::initMusic
 * @param domDocument
 */
void TkLevel::initMusic(const pugi::xml_document& domDocument) {
  pugi::xml_node audioNode = domDocument.child("audio");
  std::string path = audioNode.attribute("root").value();

  for (auto item : audioNode.children("music")) {
    std::string filename = item.attribute("filename").value();
    sf::Music* music = new sf::Music;
    if (!music->openFromFile(path + filename)) {
      std::cerr << "Cannot open music file: " + path + filename + "\n";
      delete music;
    } else {
      music->setVolume(tkDefaultVolume);
      musicList.push_back(std::make_unique<sf::Music*>(music));
    }
  }

  if (!finishMusic.openFromFile("audio/jingles/LevelComplete.ogg")) {
    std::cerr << "Cannot open music file: audio/jingles/LevelComplete.ogg\n";
  } else {
    finishMusic.setVolume(tkDefaultVolume);
  }
  if (!diesMusic.openFromFile("audio/jingles/TokiToriDies.ogg")) {
    std::cerr << "Cannot open music file: audio/jingles/TokiToriDies.ogg\n";
  } else {
    diesMusic.setVolume(tkDefaultVolume);
  }
}

/**
 * @brief TkLevel::initShoebox
 * @param domDocument
 */
void TkLevel::initShoebox(const pugi::xml_document& domDocument) {
  pugi::xml_node backgroundNode =
      domDocument.child("shoebox").child("background");
  for (auto item : backgroundNode.children("plane")) {
    std::string path = item.attribute("texture").value();

    sf::Texture* texture;
    if (shoeboxTextureMap.find(path) != shoeboxTextureMap.end()) {
      texture = shoeboxTextureMap.at(path);
    } else {
      texture = new sf::Texture;
      texture->loadFromFile(path);
      texture->setSmooth(true);
    }

    sf::Sprite sprite(*texture);
    sprite.setOrigin(texture->getSize().x / 2, texture->getSize().y / 2);
    sprite.setPosition(item.attribute("x").as_int(0),
                       item.attribute("y").as_int(0));
    sprite.setScale(item.attribute("width").as_float(1.f) / texture->getSize().x,
                    item.attribute("height").as_float(1.f) / texture->getSize().y);
    sprite.setRotation(-item.attribute("rotation").as_int(0));

    shoeboxSpriteList.push_back(
        std::pair<int32_t, sf::Sprite>(item.attribute("z").as_int(), sprite));
  }

  // Sort the list
  shoeboxSpriteList.sort(
      [](const std::pair<int, sf::Sprite>& a,
         const std::pair<int, sf::Sprite>& b) { return a.first < b.first; });
  for (auto& item : shoeboxSpriteList) {
    item.second.move(480, 400);
    mapRender.draw(item.second);
  }
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
  for (auto egg : eggList) egg->move(*this);

  updateMusic();
}

/**
 * @brief TkLevel::updateMusic
 */
void TkLevel::updateMusic() {
  if (player.getState() == TkPlayer::anim::finish) {
    isLevelFinish = (finishMusic.getStatus() == sf::SoundSource::Stopped);
    return;
  }

  if (player.getState() == TkPlayer::anim::levelComplete) {
    if (musicList.size()) {
      (*musicList[musicCounter].get())->stop();
      musicList.clear();

      finishMusic.stop();
      finishMusic.play();
    }
    return;
  }

  if ((*musicList[musicCounter].get())->getStatus() == sf::SoundSource::Stopped) {
    musicCounter = (musicCounter + 1) % musicList.size();
    (*musicList[musicCounter].get())->stop();
    (*musicList[musicCounter].get())->play();
  }
}

/**
 * @brief TkLevel::eggChecker
 * @param position
 */
void TkLevel::eggChecker(const sf::FloatRect& position) {
  for (auto it = eggList.begin(); it != eggList.end();) {
    if ((*it)->getState() == TkEgg::anim::free) {
      delete (*it);
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
      if (item == TkAction::bridge) buildBridge();
      if (item == TkAction::teleport) {
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
//            player.updateTeleportState(TkPlayer::teleportBeamOut,
//                                       teleportGhost[index].pos);
            action.decrease(TkAction::teleport);
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

//            // clang-format off
//            TkPlayer::anim anim = (index == teleportGate::left)  ? TkPlayer::teleportLookLeft  :
//                                  (index == teleportGate::right) ? TkPlayer::teleportLookRight :
//                                  (index == teleportGate::up)    ? TkPlayer::teleportLookUp    :
//                                  (index == teleportGate::down)  ? TkPlayer::teleportLookDown  :
//                                                                   TkPlayer::teleportLookFront;
//            // clang-format on
//            player.updateTeleportState(anim);
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
    action.decrease(TkAction::bridge);
  }
}

/**
 * @brief TkLevel::buildTeleport
 */
void TkLevel::buildTeleport() {
  if (player.actionAuthorized() &&
      action.authorized(TkAction::teleport)) {
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
