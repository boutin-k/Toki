#ifndef TKIMAGE_H
#define TKIMAGE_H

#include <string>

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"

class TkImage : public sf::Sprite
{
 public:
  TkImage(void);
  TkImage(const std::string& filepath);

  void loadFromFile(const std::string& filepath);

 private:
  sf::Texture mTexture;
};

#endif // TKIMAGE_H
