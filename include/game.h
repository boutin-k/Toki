#ifndef GAME_H
#define GAME_H

#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

#include "tklevel.h"
#include "tkimage.h"

class Game {
  std::vector<std::string> levelList {
//    { "maps/ForestFalls/TestShoebox.tokilevel" },
    { "maps/ForestFalls/01_ForestFalls.tokilevel" },
    { "maps/ForestFalls/02_ForestFalls.tokilevel" },
    { "maps/ForestFalls/03_ForestFalls.tokilevel" },
    { "maps/ForestFalls/charlie1.tokilevel" },
    { "maps/ForestFalls/charlie2.tokilevel" },
    { "maps/ForestFalls/charlie3.tokilevel" },
    { "maps/ForestFalls/charlie4.tokilevel" }
  };

 public:
  Game();
  virtual ~Game();
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

  uint32_t levelIndex{0U};
};

#endif // GAME_H
