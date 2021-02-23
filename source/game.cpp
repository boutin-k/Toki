#include "game.h"
#include "forest.h"
#include "tools/tkpoint.h"

#include "SFML/Window/Event.hpp"
#include "SFML/Graphics/RenderTexture.hpp"
#include "SFML/Graphics/Texture.hpp"

#include <iostream>
#include <cmath>
#include <array>

#define FRAMERATE 30

static const sf::Time sTimePerFrame = sf::seconds(1.f / FRAMERATE);

/**
 * @brief Game::Game
 */
Game::Game(void)
    : mWindow(sf::VideoMode(960, 600, 32), "TokiTori", sf::Style::Titlebar | sf::Style::Close),
      mBackgroundImage{{"Media/bg01.png"}, {"Media/forest_bg_light.png"}, {"Media/bg02.png"}, {}} {
  // bg_light
  mBackgroundImage[1].setPosition(384.f, 0.f);
  mBackgroundImage[1].setScale(4.4f, 4.4f);
  // bg_light
  mBackgroundImage[3].setTexture(*mBackgroundImage[1].getTexture());
  mBackgroundImage[3].setPosition(95.f, 0.f);
  mBackgroundImage[3].setScale(6.2f, 6.2f);

  mWindow.setFramerateLimit(FRAMERATE);

  mLevel = new Forest(mWindow.getSize());
  mLevel->createLevel("01_ForestFalls.tokilevel");

  mPlayer.setLevel(mLevel);
}

Game::~Game(void) {
  if (mLevel != nullptr) {
    delete mLevel;
    mLevel = nullptr;
  }
}

/**
 * @brief Game::run
 */
void Game::run(void) {
  sf::Clock clock;
  sf::Time timeSinceLastUpdate = sf::Time::Zero;

  while (mWindow.isOpen()) {
    processEvents();
    timeSinceLastUpdate += clock.restart();

    while (timeSinceLastUpdate > sTimePerFrame) {
      timeSinceLastUpdate -= sTimePerFrame;
      update(sTimePerFrame);
    }
    render();
  }
}

/**
 * @brief Game::processEvents
 */
void Game::processEvents(void) {
  sf::Event event;
  while (mWindow.pollEvent(event)) {
    switch (event.type) {
      case sf::Event::KeyPressed:
        handlePlayerInput(event.key.code, true);
        break;
      case sf::Event::KeyReleased:
        handlePlayerInput(event.key.code, false);
        break;
      case sf::Event::Closed:
        mWindow.close();
        break;
      case sf::Event::MouseButtonPressed:
        if (!mPlayer.isVisible()) mPlayer.setVisible();
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
  switch (key) {
    case sf::Keyboard::Escape:
      mIsEscapePress = true;
      break;
    case sf::Keyboard::Up:
      mIsMovingUp = isPressed;
      break;
    case sf::Keyboard::Down:
      mIsMovingDown = isPressed;
      break;
    case sf::Keyboard::Left:
      mIsMovingLeft = isPressed;
      break;
    case sf::Keyboard::Right:
      mIsMovingRight = isPressed;
      break;
    default:
      break;
  }
}

/**
 * @brief Game::update
 * @param deltaTime
 */
void Game::update(const sf::Time& deltaTime) {
  // If Toki Tori has not appear : do nothing
  sf::Vector2f _movement(0.f, 0.f);
  if (mIsMovingRight)
    _movement.x -= 130;
  else if (mIsMovingLeft)
    _movement.x += 130;
  else if (mIsMovingUp)
    _movement.y += 130;
  else if (mIsMovingDown)
    _movement.y -= 130;

  if (mPlayer.getState() == TkPlayer::anim::finish) mWindow.close();

  // clang-format off
  enum tk::gesture movement =
      (mIsEscapePress) ? tk::gesture::back  :
      (mIsMovingLeft)  ? tk::gesture::left  :
      (mIsMovingRight) ? tk::gesture::right :
      (mIsMovingUp)    ? tk::gesture::up    :
      (mIsMovingDown)  ? tk::gesture::down  :
                         tk::gesture::none;
  // clang-format on
  mPlayer.setMovement(movement);
  mPlayer.move(deltaTime);
  mLevel->render();
}

/**
 * @brief Game::render
 */
void Game::render() {
  mWindow.clear(sf::Color(255, 255, 255, 255));
  // If Toki Tori has not appear : do nothing
  mWindow.draw(mBackgroundImage[0]);
  mWindow.draw(mBackgroundImage[1]);
  mWindow.draw(mBackgroundImage[2]);
  mWindow.draw(mBackgroundImage[3]);
  mWindow.draw(mLevel->getSprite());
  if (mPlayer.isVisible())
    mWindow.draw(mPlayer.drawableSprite());
  mWindow.display();
}
