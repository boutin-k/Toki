#include "game.h"
#include "forest.h"

#include "SFML/Window/Event.hpp"

#define FRAMERATE 30

static const sf::Time sTimePerFrame = sf::seconds(1.f / FRAMERATE);

/**
 * @brief Game::Game
 */
Game::Game(void)
    : window(sf::VideoMode(/*960, 600*/880, 660, 32), "Toki Tori",
              sf::Style::Titlebar | sf::Style::Close) {
  if (!font.loadFromFile("fonts/Z003.ttf")) {
    std::cerr << "Could not load font Z003.ttf\n";
  }

//  window.create(sf::VideoMode::getFullscreenModes()[0], "Toki Tori", sf::Style::Fullscreen);
  text.setFont(font);
  text.setFillColor(sf::Color::White);
  text.setStyle(sf::Text::Bold);
  text.setCharacterSize(22);
  text.setPosition(window.getSize().x-32.f, 69.f);

  // Manage custom cursor
  if (cursor.loadFromFile("textures/cursor/pathcursor.png")) {
    // Hide system cursor
    window.setMouseCursorVisible(false);
    cursor.setOrigin(cursor.getLocalBounds().width / 2.f,
                      cursor.getLocalBounds().height / 2.f);
  }
  window.setFramerateLimit(FRAMERATE);
//  _window.getDefaultView();

  if (score.loadFromFile("textures/backgrounds/score.png")) {
    score.setPosition(window.getSize().x-110.f, 20.f);
  }

  try {
    level = new Forest(window.getSize());
    level->createLevel(levelList[levelIndex]);
  } catch (const std::string& e) {
    std::cerr << e << std::endl;
  }
}

Game::~Game(void) {
  if (level != nullptr) {
    delete level;
    level = nullptr;
  }
}

/**
 * @brief Game::run
 */
void Game::run(void) {
  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;

  while (window.isOpen()) {
    if (level->finish) {
      delete level;
      // Restore the default viewport
      window.setView(window.getDefaultView());
      // Launch the new level
      levelIndex = ++levelIndex % levelList.size();
      level = new Forest(window.getSize());
      level->createLevel(levelList[levelIndex]);

      //    _window.close();
    }

    processEvents();
    timeSinceLastUpdate += clock.restart();

    while (timeSinceLastUpdate > sTimePerFrame) {
      timeSinceLastUpdate -= sTimePerFrame;
      update();
    }
    // Set cursor position
    cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));
    render();
  }
}

/**
 * @brief Game::processEvents
 */
void Game::processEvents(void) {
  sf::Event event;
  while (window.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::KeyPressed:
        handlePlayerInput(event.key.code, true);
        break;
      case sf::Event::KeyReleased:
        handlePlayerInput(event.key.code, false);
        break;
      case sf::Event::Closed:
        window.close();
        break;
      case sf::Event::MouseButtonPressed:
      case sf::Event::MouseButtonReleased:
      case sf::Event::MouseMoved:
      case sf::Event::MouseEntered:
      case sf::Event::MouseLeft: {
        level->handleMouseEvent(event);
        break;
      }
      default:
        break;
    }
  }
}

/**
 * @brief Game::handlePlayerInput
 * @param key
 * @param isPressed
 */
void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed) {
  // clang-format off
  switch (key) {
    case sf::Keyboard::Escape: escape      = true;      break;
    case sf::Keyboard::Up:     movingUp    = isPressed; break;
    case sf::Keyboard::Down:   movingDown  = isPressed; break;
    case sf::Keyboard::Left:   movingLeft  = isPressed; break;
    case sf::Keyboard::Right:  movingRight = isPressed; break;
    default: break;
  }
  // clang-format on
}

/**
 * @brief Game::update
 * @param deltaTime
 */
void Game::update() {
  // clang-format off
  enum tk::gesture gesture =
      (movingLeft)   ? tk::gesture::left  :
      (movingRight)  ? tk::gesture::right :
      (movingUp)     ? tk::gesture::up    :
      (movingDown)   ? tk::gesture::down  :
                       tk::gesture::none;
  // clang-format on
  level->update(gesture);
  text.setString(std::to_string(level->getEggNumber()));
  text.setOrigin(text.getGlobalBounds().width / 2,
                  text.getGlobalBounds().height / 2);
}

/**
 * @brief Game::render
 */
void Game::render() {
  window.clear(sf::Color(255, 255, 255, 255));
  level->render(window);
  window.draw(score);
  window.draw(cursor);
  window.draw(text);
  window.display();
}
