#ifndef TKPLAYER_H
#define TKPLAYER_H

#include "tkanimation.h"
#include "tkgesture.h"
#include "tklevel.h"

#include "SFML/System/Clock.hpp"
#include "SFML/Audio/Music.hpp"

#include <unordered_map>
#include <functional>
#include <vector>

class TkPlayer {

  struct animMachine {
    uint spriteBegin;
    uint spriteEnd;

    TkAnimation* image;
    std::function<sf::Vector2f(uint)> func;
    bool transitionOnLast;
    std::vector<std::pair<tk::gesture, uint>> transition;
    std::unordered_map<uint, sf::Music*> sound;
  };

 public:
  enum anim : uint {
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

  void setLevel(TkLevel* level) {mCurrentWorld = level;}
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

  TkAnimation* mCurrentImage{nullptr};

  TkAnimation mAppear;
  TkAnimation mRun;
  TkAnimation mStand;
  TkAnimation mTurn;
  TkAnimation mJump;
  TkAnimation mFall01;
  TkAnimation mFall02;
  TkAnimation mClimb;
  TkAnimation mDizzy;

  sf::Music mAppearSound;
  sf::Music mTurnSound;
  sf::Music mJumpSound;
  sf::Music mFootLeftSnd;
  sf::Music mFootRightSnd;

  TkLevel* mCurrentWorld{nullptr};

  std::unordered_map<TkPlayer::anim, animMachine> mAnimation;

  sf::Vector2f mPosition{0.f, 0.f};

  uint mDizzyCounter{0U};
};

#endif // TKPLAYER_H
