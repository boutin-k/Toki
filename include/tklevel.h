#ifndef TKLEVEL_H
#define TKLEVEL_H

#include "tkgesture.h"
#include "tkplayer.h"
#include "tkegg.h"
#include "tksound.h"
#include "tkaction.h"
#include "tkshoebox.h"
#include "tkmusic.h"

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/Sprite.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/Window/Event.hpp"
#include "SFML/Audio/Music.hpp"

#include <memory>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <list>

namespace pugi {
  class xml_document;
}

class TkLevel
{
  static constexpr uint32_t tkMagicNumber{0x7F23F5AD};
  static constexpr uint32_t tkGridMinWidth{30U};
  static constexpr uint32_t tkGridMaxWidth{100U};
  static constexpr uint32_t tkGridMinHeight{20U};
  static constexpr uint32_t tkGridMaxHeight{100U};
  static constexpr uint32_t tkVersion{3U};

  struct levelData {
    uint32_t magicNumber           = {0};
    uint32_t version               = {0};
    uint32_t levelWidth            = {0};
    uint32_t levelHeight           = {0};
    char tilePngFile[128]          = {0};
    char backXmlFile[128]          = {0};
    char eggsPngFile[128]          = {0};
    char musicFile[128]            = {0};
    uint8_t action[TkAction::size] = {0, 0, 0, 0};
    uint32_t* levelMap             = {nullptr};

    uint32_t getLevelSize() {
      return levelWidth * levelHeight;
    }
  };

 public:
  TkLevel(const sf::Vector2u& windowSize);
  virtual ~TkLevel();

  void createLevel(const std::string& filename);

  virtual void buildBoard() = 0;
  virtual tk::action isMovable(const sf::Vector2f& origin, tk::gesture, bool recursiveLocked = false) const = 0;
  virtual void move(const sf::Vector2f& offset);
  virtual void update(const enum tk::gesture& gesture);
  virtual void render(sf::RenderWindow& window) = 0;

  void eggChecker(const sf::FloatRect&);
  inline uint32_t getEggNumber() const { return eggList.size(); }

  inline bool isIdle() const { return (player.getState()==TkPlayer::anim::none); }
  inline void start() { player.setVisible(); }

  inline sf::Vector2f getStartPosition() const { return startPosition; }

  inline void resetPosition() {
    player.setPosition(startPosition);

    sf::Vector2f winCenter{windowSize.x / 2.f, windowSize.y / 2.f};
    move(startPosition - winCenter);
  }

  void handleMouseEvent(const sf::Event& event);

  void buildBridge();
  virtual bool bridgeBuilderHandler(const sf::Vector2f& origin,
                                    const tk::gesture& gesture) = 0;
  void buildTeleport();
  void clearTeleport();
  virtual void teleportHandler(const sf::Vector2f& origin) = 0;

  bool finish{false};

 protected:
  levelData           data;

  sf::View            view;
  sf::Vector2u        windowSize;
  sf::Vector2f        startPosition;

  sf::RenderTexture   mapRender;
  sf::Sprite          mapSprite;
  sf::RenderTexture   foregroundRender;
  sf::Sprite          foregroundSprite;

  TkImage             levelTiles;

  TkPlayer            player;

  sf::Texture         eggTexture;
  std::vector<std::unique_ptr<TkEgg>> eggList;
  TkSound             eggSnd;

  TkSound             bridgeBuildSound;
  TkSound             actionFailSound;
  TkMusic             music;
  TkShoebox           shoebox;
  TkAction            action;

  static constexpr auto teleportGhostPath =
      "textures/sprites/toki/teleportghost.png";
  static constexpr auto teleportGhostNoCanDoPath =
      "textures/sprites/toki/teleportghost_nocando.png";

  enum teleportGate : uint32_t { up, down, left, right, size };
  struct teleportData {
    bool enabled{false};
    sf::Vector2f pos;
    sf::FloatRect rect;
    TkAnimation* anim{nullptr};
  };

  bool teleportInProgress{false};
  TkAnimation teleportGhostAnim;
  TkAnimation teleportGhostNoAnim;
  teleportData teleportGhost[teleportGate::size];
  teleportGate currentGate{teleportGate::size};
  TkSound teleportSound;

};

#endif // TKLEVEL_H
