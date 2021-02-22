#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Window/Keyboard.hpp"

#include "tkplayer.h"
#include "tklevel.h"
#include "tkimage.h"

class Game {
 public:
  Game(void);
  ~Game(void);
  void run(void);

 private:
  void processEvents();
  void update(const sf::Time&);
  void render();
  void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

 private:
  sf::RenderWindow mWindow;

  TkImage mBackgroundImage[4];
  TkPlayer mPlayer;
  TkLevel *mLevel{nullptr};

  bool mIsMovingUp = false;
  bool mIsMovingDown = false;
  bool mIsMovingLeft = false;
  bool mIsMovingRight = false;
  bool mIsEscapePress = false;


  sf::RenderTexture renderTexture;
};


#endif // GAME_H
