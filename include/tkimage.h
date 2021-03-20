#ifndef TKIMAGE_H
#define TKIMAGE_H

#include <string>

#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"

class TkImage : public sf::Sprite
{
 public:
  enum TkOrigin { topLeft, topCenter, topRight, centerRight, bottomRight,
                  bottomCenter, bottomLeft, centerLeft, center};

  TkImage(void);
  TkImage(const std::string& filepath);
  TkImage(const sf::Texture& t);

  bool loadFromFile(const std::string& filepath);
  virtual void setTkOrigin(const TkOrigin& origin);

  sf::Texture texture;
};

#endif // TKIMAGE_H
