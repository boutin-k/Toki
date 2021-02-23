#ifndef TKPLAYER_H
#define TKPLAYER_H

#include "tkanimation.h"
#include "tkgesture.h"
#include "tklevel.h"

#include "SFML/System/Clock.hpp"
#include "SFML/Audio/Music.hpp"

#include <unordered_map>
#include <functional>
#include <iostream>
#include <vector>

class TkPlayer {

  struct animMachine {
    uint32_t spriteBegin;
    uint32_t spriteEnd;
    float spriteSpeed;

    TkAnimation* image;
    std::function<sf::Vector2f(uint32_t)> func;
    bool transitionOnLast;
    std::vector<std::pair<tk::gesture, uint32_t>> transition;
    std::unordered_map<uint32_t, sf::Music*> sound;
  };

 public:
  enum anim : uint32_t {
    appear,
    standFront,
    standLeft,
    standRight,
    standBack,
    quarterLeft,
    quarterRight,
    turnLeft,
    turnRight,
    runLeft,
    runRight,
    jumpUpLeft,
    jumpUpRight,
    jumpDownLeft,
    jumpDownRight,
    fallFront,
    fallLeft,
    fallRight,
    dizzy,
    climbUp,
    climbDown,
    levelComplete,
    finish,
    EOL_State
  };

  TkPlayer(void);

  inline bool isVisible(void) { return animationState != anim::EOL_State; }
  inline void setVisible(void) {
    if (animationState == anim::EOL_State) {
      animationState = anim::appear;
    }
  }

  void setLevel(TkLevel* level) {
    mLevel = level;
    setPosition(mLevel->resetPosition());
  }

  void setPosition(float positionX, float positionY);
  void setPosition(const sf::Vector2f& position);
  void move(const sf::Time& deltaTime);
  void setMovement(const enum tk::gesture& movement);

  const sf::Drawable& drawableSprite(void);
  anim getState(void) { return animationState; }


 private:
  void gestureHandler();

 private:
  enum anim animationState = anim::EOL_State;
  enum tk::gesture movementState = tk::gesture::none;

  TkAnimation* mToki{nullptr};

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

  TkLevel* mLevel{nullptr};

  std::unordered_map<TkPlayer::anim, animMachine> mAnimation;

  sf::Vector2f mPosition{0.f, 0.f};

  uint32_t mDizzyCounter{0U};
};

#endif // TKPLAYER_H
