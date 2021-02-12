#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Keyboard.hpp"

#include "tkplayer.h"
#include "tkimage.h"

class Game {
 public:
  Game(void);
  void run(void);

 private:
  void processEvents();
  void update(const sf::Time&);
  void render();
  void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

 private:
  sf::RenderWindow mWindow;

  TkImage mBackgroundImage[7];
  TkImage mBackgroundGrid;
  TkPlayer mPlayer;

  bool mIsMovingUp = false;
  bool mIsMovingDown = false;
  bool mIsMovingLeft = false;
  bool mIsMovingRight = false;
  bool mIsEscapePress = false;
};


#endif // GAME_H
