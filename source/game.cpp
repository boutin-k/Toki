#include "game.h"
#include "forest.h"

#include "SFML/Window/Event.hpp"

#include <string>
#include <iostream>

#define FRAMERATE 30

static const sf::Time sTimePerFrame = sf::seconds(1.f / FRAMERATE);

/**
 * @brief Game::Game
 */
Game::Game(void)
    : _window(sf::VideoMode(960, 600, 32), "TokiTori",
              sf::Style::Titlebar | sf::Style::Close),
      _backgroundImage{{"Media/bg01.png"},
                       {"Media/forest_bg_light.png"},
                       {"Media/bg02.png"},
                       {}} {
  if (!_font.loadFromFile("Media/Z003.ttf")) {
    std::cerr << "Could not load font Z003.ttf\n";
  }  // TODO
  _text.setFont(_font);
  _text.setFillColor(sf::Color::White);
  _text.setStyle(sf::Text::Bold);
  _text.setCharacterSize(22);
  _text.setPosition(928.f, 69.f);

  // Manage custom cursor
  if (_cursor.loadFromFile("Media/pathcursor.png")) {
    // Hide system cursor
    _window.setMouseCursorVisible(false);
    _cursor.setOrigin(_cursor.getLocalBounds().width / 2.f,
                      _cursor.getLocalBounds().height / 2.f);
  }
  _window.setFramerateLimit(FRAMERATE);

  if (_score.loadFromFile("Media/score.png")) {
    _score.setPosition(850.f, 20.f);
  }

  // bg_light
  _backgroundImage[1].setPosition(384.f, 0.f);
  _backgroundImage[1].setScale(4.4f, 4.4f);
  // bg_light
  _backgroundImage[3].setTexture(*_backgroundImage[1].getTexture());
  _backgroundImage[3].setPosition(95.f, 0.f);
  _backgroundImage[3].setScale(6.2f, 6.2f);


  try {
    _level = new Forest(_window.getSize());
    _level->createLevel("01_ForestFalls.tokilevel");
  } catch (char const* e) {
    std::cerr << e << std::endl;
  }
}

Game::~Game(void) {
  if (_level != nullptr) {
    delete _level;
    _level = nullptr;
  }
}

/**
 * @brief Game::run
 */
void Game::run(void) {
  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;

  while (_window.isOpen()) {
    processEvents();
    timeSinceLastUpdate += clock.restart();

    while (timeSinceLastUpdate > sTimePerFrame) {
      timeSinceLastUpdate -= sTimePerFrame;
      update();
    }
    // Set cursor position
    _cursor.setPosition(static_cast<sf::Vector2f>(sf::Mouse::getPosition(_window)));

    render();
  }
}

/**
 * @brief Game::processEvents
 */
void Game::processEvents(void) {
  sf::Event event;
  while (_window.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::KeyPressed:
        handlePlayerInput(event.key.code, true);
        break;
      case sf::Event::KeyReleased:
        handlePlayerInput(event.key.code, false);
        break;
      case sf::Event::Closed:
        _window.close();
        break;
      case sf::Event::MouseButtonPressed:
        if (_level->isIdle()) _level->start();
        break;
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
    case sf::Keyboard::Escape: _escape      = true;      break;
    case sf::Keyboard::Up:     _movingUp    = isPressed; break;
    case sf::Keyboard::Down:   _movingDown  = isPressed; break;
    case sf::Keyboard::Left:   _movingLeft  = isPressed; break;
    case sf::Keyboard::Right:  _movingRight = isPressed; break;
    default: break;
  }
  // clang-format on
}

/**
 * @brief Game::update
 * @param deltaTime
 */
void Game::update() {
//  if (mPlayer.getState() == TkPlayer::anim::finish) mWindow.close();

  // clang-format off
  enum tk::gesture gesture =
      (_escape)       ? tk::gesture::back  :
      (_movingLeft)   ? tk::gesture::left  :
      (_movingRight)  ? tk::gesture::right :
      (_movingUp)     ? tk::gesture::up    :
      (_movingDown)   ? tk::gesture::down  :
                        tk::gesture::none;
  // clang-format on
  _level->updateGesture(gesture);
  _level->render();
  _text.setString(std::to_string(_level->getEggNumber()));
  _text.setOrigin(_text.getGlobalBounds().width / 2,
                  _text.getGlobalBounds().height / 2);
}

/**
 * @brief Game::render
 */
void Game::render() {
  _window.clear(sf::Color(255, 255, 255, 255));
  // If Toki Tori has not appear : do nothing
  _window.draw(_backgroundImage[0]);
  _window.draw(_backgroundImage[1]);
  _window.draw(_backgroundImage[2]);
  _window.draw(_backgroundImage[3]);

  _window.draw(_level->getSprite());
  _window.draw(_score);
  _window.draw(_cursor);
  _window.draw(_text);
  _window.display();
}
