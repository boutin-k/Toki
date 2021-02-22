#ifndef TKIMAGE_H
#define TKIMAGE_H

#include <string>

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"

class TkImage : public sf::Sprite
{
 public:
  TkImage(void);
  TkImage(const std::string& filepath);
  TkImage(const sf::Texture& texture);

  void loadFromFile(const std::string& filepath);
//  inline const sf::Vector2f getGlobalOrigin() { return getPosition() + getOrigin(); }

// private:
  sf::Texture mTexture;
};

#endif // TKIMAGE_H
