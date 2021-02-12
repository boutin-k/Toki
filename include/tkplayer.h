#ifndef TKPLAYER_H
#define TKPLAYER_H

#include "tkanimatedimage.h"
#include "SFML/System/Clock.hpp"
#include "SFML/Audio/Music.hpp"

#include <unordered_map>
#include <functional>
#include <vector>

class TkPlayer {
  struct stateMachine {
    uint startSprite;
    uint endSprite;

    TkAnimatedImage* image;
    std::function<void(uint)> func;
    bool transitionOnLast;
    std::vector<std::pair<uint, uint>> transition;
    std::unordered_map<uint, sf::Music*> sound;
  };

 public:
  enum state : uint {
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
    dizzy,
    climb,
    levelComplete,
    finish,
    EOL_State
  };

  enum movement : uint {
    none  = 0U,
    left  = 1U,
    right = 2U,
    up    = 4U,
    down  = 8U,
    back  = 16U
  };

  TkPlayer(void);

  inline bool isVisible(void) { return animationState != state::EOL_State; }
  inline void setVisible(void) {
    if (animationState == state::EOL_State) {
      animationState = state::appear;
    }
  }

  void setPosition(float positionX, float positionY);
  void setPosition(const sf::Vector2f& position);
  void move(const sf::Time& deltaTime);
  void setMovement(const enum movement& movement);

  const sf::Drawable& drawableSprite(void);
  state getState(void) { return animationState; }

 private:
  enum state animationState = state::EOL_State;
  enum movement movementState = movement::none;

  TkAnimatedImage* mCurrentImage{nullptr};

  TkAnimatedImage mAppear;
  TkAnimatedImage mRun;
  TkAnimatedImage mStand;
  TkAnimatedImage mTurn;
  TkAnimatedImage mClimb;
  TkAnimatedImage mDizzy;

  sf::Music mAppearSound;
  sf::Music mTurnSound;
  sf::Music mFootLeftSnd;
  sf::Music mFootRightSnd;

  std::unordered_map<TkPlayer::state, stateMachine> mStateMachine;

  sf::Vector2f mPosition{0.f, 0.f};

  uint mDizzyCounter{0U};
};

#endif // TKPLAYER_H
