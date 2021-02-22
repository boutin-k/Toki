#ifndef TKLEVEL_H
#define TKLEVEL_H

#include "tkgesture.h"

#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

class TkLevel
{
 public:
  TkLevel();
  virtual ~TkLevel();
  virtual void createLevel(void) = 0;
  virtual tk::action isMovable(const sf::Vector2f& origin, tk::gesture) = 0;

  virtual sf::Vector2f move(const sf::Vector2f& offset) = 0;

  inline sf::Sprite& getSprite(void) { return sprite; }

 protected:
  sf::RenderTexture renderTexture;
  sf::Texture texture;
  sf::Sprite sprite;
};

#endif // TKLEVEL_H
