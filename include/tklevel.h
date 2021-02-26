#ifndef TKLEVEL_H
#define TKLEVEL_H

#include "tkgesture.h"
#include "tkplayer.h"
#include "tkegg.h"

#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"

#include <vector>
#include <iostream>

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
  virtual tk::action isMovable(const sf::Vector2f& origin, tk::gesture) const = 0;
  virtual void move(const sf::Vector2f& offset);
  virtual const sf::Drawable& render() = 0;

  void eggChecker(const sf::FloatRect&);


  inline bool isIdle() { return (_player.getState()==TkPlayer::anim::idle); }
  inline void start() { _player.setVisible(); }

  inline sf::Sprite& getSprite() {
    return levelSprite;
  }

  inline sf::Vector2f getStartPosition() {
    return _data.getStartPosition();
  }

  inline void resetPosition() {
    _player.setPosition(_data.getStartPosition());

    levelSprite.setPosition(0.f, 0.f);
    sf::Vector2f winCenter{_windowSize.x / 2.f, _windowSize.y / 2.f};
    move(_data.getStartPosition() - winCenter);
  }

  inline void updateGesture(const enum tk::gesture& gesture) {
    sf::Vector2u windowCenter{_windowSize.x>>1, _windowSize.y>>1};
    sf::Vector2f vec = _player.move(*this, gesture);
    sf::Vector2f relativePos =
        _player._entity->getPosition() + levelSprite.getPosition() - vec;

    if ((relativePos.x >= windowCenter.x -16.f && relativePos.x <= windowCenter.x + 16.f) ||
        (relativePos.y >= windowCenter.y -16.f && relativePos.y <= windowCenter.y + 16.f))
      move(vec);

    // Update the eggs
    for (auto egg : _eggList) egg->move(*this);
  }

 protected:
  levelData _data;
  sf::Vector2u _windowSize;

  sf::RenderTexture mapRender;
  sf::Sprite mapSprite;

//  sf::RenderTexture eggRender;
//  sf::Texture eggTexture;
//  sf::Sprite eggSprite;

  sf::RenderTexture levelRender;
  sf::Sprite levelSprite;

  TkPlayer _player;
  std::vector<TkEgg*> _eggList;
  sf::Music _escapeEggSnd;
};

#endif // TKLEVEL_H
