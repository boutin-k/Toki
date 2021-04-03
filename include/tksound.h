#ifndef TKSOUND_H
#define TKSOUND_H

#include <string>
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"

class TkSound : public sf::Sound
{
 public:
  TkSound();
  TkSound(const std::string& filename);
  TkSound(const sf::SoundBuffer& sb);
  virtual ~TkSound();

  bool loadFromFile(const std::string& filename);

  sf::SoundBuffer buffer;
};

#endif // TKSOUND_H
