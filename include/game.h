#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

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
  sf::RenderWindow window;

  TkLevel*  level{nullptr};

  bool      movingUp{false};
  bool      movingDown{false};
  bool      movingLeft{false};
  bool      movingRight{false};
  bool      escape{false};

  TkImage   cursor;
  TkImage   score;
  sf::Text  text;
  sf::Font  font;
};

#endif // GAME_H
