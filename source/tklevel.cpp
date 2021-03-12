#include "tklevel.h"
#include "pugixml.hpp"

#include <fstream>
#include <string>

static constexpr auto tkDefaultVolume = 25.f;

/**
 * @brief Constructor
 * @param windowSize
 */
TkLevel::TkLevel(const sf::Vector2u& windowSize)
    :windowSize(windowSize) {
  if (!eggSnd.loadFromFile("audio/sfx/sfx [12].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [12].wav'"
              << std::endl;
  if (!bridgeSoundWin.loadFromFile("audio/sfx/sfx [4].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [4].wav'"
              << std::endl;
  if (!bridgeSoundFailed.loadFromFile("audio/sfx/sfx [6].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [6].wav'"
              << std::endl;
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
  static const sf::Vector2u limits =
      sf::Vector2u{data.levelWidth << 5, data.levelHeight << 5} -
      windowSize;

  if (offset.x != 0.f || offset.y != 0.f) {
    levelSprite.move(-offset);

    sf::Vector2f finalPos = levelSprite.getPosition();
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
 * @brief TkLevel::updateGesture
 * @param gesture
 */
void TkLevel::updateGesture(const enum tk::gesture& gesture) {
  static const uint32_t margin{16U};
  static const uint32_t centerBox[4]{
      (windowSize.x>>1) - margin, (windowSize.x>>1) + margin, // left, right
      (windowSize.y>>1) - margin, (windowSize.y>>1) + margin  // top, bottom
  };

  sf::Vector2f offset = player.move(*this, gesture);
  sf::Vector2f relativePos =
      player.entity->getPosition() + levelSprite.getPosition();

  if ((offset.x > 0.f && relativePos.x < centerBox[0]) ||
      (offset.x < 0.f && relativePos.x > centerBox[1]))
    offset.x = 0.f;
  if ((offset.y > 0.f && relativePos.y < centerBox[2]) ||
      (offset.y < 0.f && relativePos.y > centerBox[3]))
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

void TkLevel::click() {
  // clang-format off
  tk::gesture gesture;
  switch (player.getStandState()) {
    case TkPlayer::anim::standLeft:  gesture = tk::left;  break;
    case TkPlayer::anim::standRight: gesture = tk::right; break;
    default:                         gesture = tk::none;  break;
  }
  // clang-format on

  if (bridgeBuilderHandler(player.getPosition(), gesture)) {
    bridgeSoundWin.stop();
    bridgeSoundWin.play();
  } else {
    bridgeSoundFailed.stop();
    bridgeSoundFailed.play();
  }
}
