#include "tkmusic.h"
#include "pugixml.hpp"

#include <memory>
#include <iostream>

static constexpr auto tkDefaultVolume = 25.f;

/**
 * @brief TkMusic::TkMusic
 */
TkMusic::TkMusic(){}

/**
 * @brief TkMusic::~TkMusic
 */
TkMusic::~TkMusic() {
  if (musicList.size()) {
    for (auto& music : musicList) music->stop();
    musicList.clear();
  }
  if (finishMusic.getStatus() != sf::SoundSource::Stopped) {
    finishMusic.stop();
  }
  if (diesMusic.getStatus() != sf::SoundSource::Stopped) {
    diesMusic.stop();
  }
}

/**
 * @brief TkMusic::init
 * @param domDocument
 */
void TkMusic::init(const pugi::xml_document& domDocument) {
  pugi::xml_node audioNode = domDocument.child("audio");
  std::string path = audioNode.attribute("root").value();

  for (auto item : audioNode.children("music")) {
    std::string filename = item.attribute("filename").value();
    auto music = std::make_unique<sf::Music>();
    if (!music->openFromFile(path + filename)) {
      std::cerr << "Cannot open music file: " + path + filename + "\n";
      delete music.release();
    } else {
      music->setVolume(tkDefaultVolume);
      musicList.push_back(std::move(music));
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
 * @brief TkMusic::update
 * @param anim
 * @return
 */
bool TkMusic::update(const TkPlayer::anim& anim) {
  switch (anim) {
    case TkPlayer::anim::finish:
      return !(finishMusic.getStatus() == sf::SoundSource::Stopped);
      break;
    case TkPlayer::anim::levelComplete:
      playLevelCompleteJingle();
      break;
    default: {
      if (musicList[musicCounter]->getStatus() == sf::SoundSource::Stopped) {
        musicCounter = ++musicCounter % musicList.size();
        play();
      }
      break;
    }
  }
  return true;
}

/**
 * @brief TkMusic::playLevelCompleteJingle
 */
void TkMusic::playLevelCompleteJingle() {
  if (finishMusic.getStatus() != sf::SoundSource::Playing) {
    stop();
    finishMusic.stop();
    finishMusic.play();
  }
}
