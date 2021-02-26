#ifndef TKPLAYER_H
#define TKPLAYER_H

#include "tkanimation.h"
#include "tkgesture.h"
#include "tkentity.h"

#include "SFML/Audio/Music.hpp"

#include <unordered_map>

class TkLevel;

class TkPlayer : public TkEntity {

 public:
  // clang-format off
  enum anim : uint32_t {
    idle, appear, standFront, standLeft, standRight, standBack, quarterLeft, quarterRight, turnLeft, turnRight, runLeft, runRight,
    jumpUpLeft, jumpUpRight, jumpDownLeft, jumpDownRight, fallFront, fallLeft, fallRight, dizzy, climbUp, climbDown, levelComplete,
    finish };
  // clang-format on

  TkPlayer();

  inline anim getState() { return _animState; }
  inline void setVisible() {
    if (_animState == anim::idle) _animState = anim::appear;
  }

  void setPosition(float positionX, float positionY);
  void setPosition(const sf::Vector2f& position);
  sf::Vector2f move(TkLevel& level, const enum tk::gesture& gesture = tk::gesture::none);

 private:
  void gestureHandler(const TkLevel& level);

 private:
  enum anim _animState = anim::idle;

  TkAnimation mAppear;
  TkAnimation mRun;
  TkAnimation mStand;
  TkAnimation mTurn;
  TkAnimation mJump;
  TkAnimation mFall01;
  TkAnimation mFall02;
  TkAnimation mClimb;
  TkAnimation mDizzy;

  sf::Music mAppearSnd;
  sf::Music mTurnSnd;
  sf::Music mJumpSnd;
  sf::Music mFootLeftSnd;
  sf::Music mFootRightSnd;
  sf::Music mFallSnd;

  std::unordered_map<TkPlayer::anim, animMachine> _animMap;

  uint32_t mDizzyCounter{0U};
};

#endif // TKPLAYER_H
