#include "tksound.h"

/**
 * @brief TkSound::TkSound
 */
TkSound::TkSound() : sf::Sound() {}

/**
 * @brief TkSound::TkSound
 * @param buffer
 */
TkSound::TkSound(const sf::SoundBuffer& sb) : sf::Sound(sb) {}

/**
 * @brief TkSound::TkSound
 * @param filepath
 */
TkSound::TkSound(const std::string& filename) { loadFromFile(filename); }

TkSound::~TkSound() { stop(); }

/**
 * @brief TkSound::loadFromFile
 * @param filepath
 * @return
 */
bool TkSound::loadFromFile(const std::string& filename) {
  if (!buffer.loadFromFile(filename))
    return false;

  setBuffer(buffer);
  return true;
}
