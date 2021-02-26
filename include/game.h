#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Keyboard.hpp"

#include "tklevel.h"
#include "tkimage.h"

class Game {
 public:
  Game();
  ~Game();
  void run();

 private:
  void processEvents();
  void update();
  void render();
  void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);

 private:
  sf::RenderWindow _window;

  TkImage _backgroundImage[4];
  TkLevel* _level{nullptr};

  bool _movingUp{false};
  bool _movingDown{false};
  bool _movingLeft{false};
  bool _movingRight{false};
  bool _escape{false};

  TkImage _cursor;
  TkImage _score;
};


#endif // GAME_H
