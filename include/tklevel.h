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
  struct levelData {
    uint32_t tokiStartX = 0U;
    uint32_t tokiStartY = 0U;
    uint32_t levelWidth = 0U;
    uint32_t levelHeight = 0U;
    uint8_t* levelMap = nullptr;

    uint32_t getLevelSize() {
      return levelWidth * levelHeight;
    }

    sf::Vector2f getStartPosition() {
      return sf::Vector2f{(float)tokiStartX, (float)tokiStartY};
    }
  };

 public:
  TkLevel(const sf::Vector2u& windowSize);
  virtual ~TkLevel();

  virtual void createLevel(const std::string&) = 0;
  virtual tk::action isMovable(const sf::Vector2f& origin, tk::gesture) = 0;
  virtual sf::Vector2f move(const sf::Vector2f& offset);
  virtual void render() = 0;
  virtual void eggChecker(const sf::FloatRect&) = 0;


  inline sf::Sprite& getSprite() {
    return levelSprite;
  }

  inline sf::Vector2f getStartPosition() {
    return _data.getStartPosition();
  }

  inline sf::Vector2f resetPosition() {
    levelSprite.setPosition(0.f, 0.f);
    sf::Vector2f winCenter{_windowSize.x / 2.f, _windowSize.y / 2.f};
    return (move(_data.getStartPosition()-winCenter)+winCenter);
  }

 protected:
  levelData _data;
  sf::Vector2u _windowSize;

  sf::RenderTexture mapRender;
  sf::Texture mapTexture;
  sf::Sprite mapSprite;

  sf::RenderTexture eggRender;
  sf::Texture eggTexture;
  sf::Sprite eggSprite;

  sf::RenderTexture levelRender;
  sf::Sprite levelSprite;
};

#endif // TKLEVEL_H
