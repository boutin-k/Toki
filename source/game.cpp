#include "game.h"
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <cmath>

#define FRAMERATE 30

static const sf::Time sTimePerFrame = sf::seconds(1.f / FRAMERATE);

/**
 * @brief Game::Game
 */
Game::Game(void)
    : mWindow(sf::VideoMode(1024, 640, 32), "TokiTori"),
      mBackgroundImage{{"Media/forest_bg01.png"},
                       {"Media/forest_bg02.png"},
                       {"Media/forest_bg_light.png"},
                       {"Media/forest_bg_floor3.png"},
                       {"Media/forest_bg03.png"},
                       {"Media/forest_bg_floor2.png"},
                       {"Media/forest_bg_light.png"}},
      mBackgroundGrid("Media/background1.png") {
  mBackgroundImage[0].setScale(2.f, 2.f);
  mBackgroundImage[1].setPosition(360.f, -50.f);
  mBackgroundImage[1].setScale(2.2f, 2.2f);
  mBackgroundImage[2].setPosition(384.f, 0.f);
  mBackgroundImage[2].setScale(4.4f, 4.4f);
  mBackgroundImage[3].setPosition(400.f, 130.f);
  mBackgroundImage[3].setScale(1.8f, 1.8f);
  mBackgroundImage[4].setPosition(-460.f, -185.f);
  mBackgroundImage[4].setScale(1.5f, 1.5f);
  mBackgroundImage[5].setPosition(660.f, -10.f);
  mBackgroundImage[5].setScale(1.2f, 1.2f);
  mBackgroundImage[6].setPosition(100.f, 0.f);
  mBackgroundImage[6].setScale(6.2f, 6.2f);

  mWindow.setFramerateLimit(FRAMERATE);

  mPlayer.setPosition(131.f, 89.f);
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
  //  if (mPlayer.isVisible()) {
  //    sf::Vector2f movement(0.f, 0.f);
  //        if (mIsMovingRight)
  //          movement.x += sPlayerSpeed;
  //        else if (mIsMovingLeft)
  //          movement.x -= sPlayerSpeed;
  //        else if (mIsMovingUp)
  //          movement.y -= sPlayerSpeed;
  //        else if (mIsMovingDown)
  //          movement.y += sPlayerSpeed;
  //    mPlayer.move(movement * deltaTime.asSeconds());
  //  }

  if (mPlayer.getState() == TkPlayer::state::finish) mWindow.close();

  // clang-format off
  enum TkPlayer::movement movement =
      (mIsEscapePress) ? TkPlayer::movement::back  :
      (mIsMovingLeft)  ? TkPlayer::movement::left  :
      (mIsMovingRight) ? TkPlayer::movement::right :
      (mIsMovingUp)    ? TkPlayer::movement::up    :
      (mIsMovingDown)  ? TkPlayer::movement::down  :
                         TkPlayer::movement::none;
  // clang-format on
  mPlayer.setMovement(movement);
  mPlayer.move(deltaTime);
}

/**
 * @brief Game::render
 */
void Game::render() {
  mWindow.clear(sf::Color(255, 255, 255, 255));
  // If Toki Tori has not appear : do nothing
  mWindow.draw(mBackgroundGrid);
  mWindow.draw(mBackgroundImage[0]);
  mWindow.draw(mBackgroundImage[1]);
  mWindow.draw(mBackgroundImage[2]);
  mWindow.draw(mBackgroundImage[3]);
  mWindow.draw(mBackgroundImage[4]);
  mWindow.draw(mBackgroundImage[5]);
  mWindow.draw(mBackgroundImage[6]);
  if (mPlayer.isVisible())
    mWindow.draw(mPlayer.drawableSprite());
  mWindow.display();
}
