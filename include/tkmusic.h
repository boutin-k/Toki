#ifndef TKMUSIC_H
#define TKMUSIC_H

#include "tkplayer.h"

#include <memory>
#include "SFML/Audio/Music.hpp"

namespace pugi {
  class xml_document;
}

class TkMusic
{
 public:
  TkMusic();
  virtual ~TkMusic();

  void init(const pugi::xml_document& domDocument);
  bool update(const TkPlayer::anim& anim);

  inline void play() {stop();musicList[musicCounter]->play();}
  inline void pause() {musicList[musicCounter]->pause();}
  inline void stop() {musicList[musicCounter]->stop();}


 private:
  void playLevelCompleteJingle();

  std::vector<std::unique_ptr<sf::Music>> musicList;
  uint32_t            musicCounter{0};
  sf::Music           finishMusic;
  sf::Music           diesMusic;
};

#endif // TKMUSIC_H
