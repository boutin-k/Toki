#ifndef TKPLAYER_H
#define TKPLAYER_H

#include "tkanimation.h"
#include "tkgesture.h"
#include "tkentity.h"

#include <unordered_map>

class TkLevel;

class TkPlayer : public TkEntity {

 public:
  // clang-format off
  enum anim : uint32_t {
    none, idle1, idle2, idle3, idle4, idle5, idle6, idle7, idle8, appear, standFront, standLeft, standRight, standBack,
    quarterFrontLeft, quarterFrontRight, quarterBackLeft, quarterBackRight, quarterLeftFront, quarterRightFront,
    turnLeft, turnRight, runLeft01, runLeft02, runRight01, runRight02, jumpUpLeft, jumpUpRight, jumpDownLeft, jumpDownRight,
    teleportLookLeft, teleportLookRight, teleportLookUp, teleportLookDown, teleportLookFront, teleportBeamIn, teleportBeamOut,
    createCrateLeft, createCrateRight,  moveBlocLeft, moveBlocRight, fallFront, fallLeft, fallRight, landFront,
    dizzy, climbUp, climbDown, levelComplete, finish };
  // clang-format on

  TkPlayer();

  inline anim getState() const { return animState; }
  inline void setState(enum TkPlayer::anim anim) { animState = anim; }
  inline void setVisible() { if (animState == anim::none) animState = anim::appear; }
  inline anim getStandState() const { return lastStand; }


  inline bool actionAuthorized() {
    switch (animState) {
      case anim::none:
      case anim::fallFront:
      case anim::fallLeft:
      case anim::fallRight:
      case anim::levelComplete:
      case anim::finish:
        return false;
      default:
        return true;
    }
  }

  inline void activeTeleport(bool active) {
    teleportActivated = active;
    if (active) animState = teleportLookFront;
  }
  inline void updateTeleportState(TkPlayer::anim anim, const enum tk::gesture& gesture = tk::gesture::none) {
    animState = anim;
    teleportGesture = gesture;
  }

  sf::Vector2f move(TkLevel& level, const enum tk::gesture& gesture = tk::gesture::none) override;

 private:
  void updateIdleState(bool reset = false);
  void idleTransitionHandler();
  void dizzyTransitionHandler();
  void transitionHandler(const TkLevel& level, const enum tk::gesture& gesture, const TkPlayer::anim& currentAnimState);

  enum anim animState { anim::none };
  enum anim idleState { anim::none };
  enum anim lastStand { anim::standFront };

  TkAnimation mAppear;
  TkAnimation mRun;
  TkAnimation mStand;
  TkAnimation mTurn;
  TkAnimation mJump;
  TkAnimation mFall;
  TkAnimation mClimb;
  TkAnimation mDizzy;
  TkAnimation mIdleA;
  TkAnimation mIdleB;
  TkAnimation mIdleC;
  TkAnimation mIdleD;

  bool teleportActivated{false};
  tk::gesture teleportGesture;
  TkAnimation mTeleportLookLeft;
  TkAnimation mTeleportLookRight;
  TkAnimation mTeleportLookUp;
  TkAnimation mTeleportLookDown;
  TkAnimation mTeleportLookFront;
  TkAnimation mTeleportBeamIn;
  TkAnimation mTeleportBeamOut;

  TkSound mAppearSnd;
  TkSound mTurnSnd;
  TkSound mJumpSnd;
  TkSound mFootLeftSnd;
  TkSound mFootRightSnd;
  TkSound mFallSnd;

  std::unordered_map<TkPlayer::anim, animMachine> animMap;

  uint32_t idleRange{0U};
  uint32_t idleDelay{0U};
  uint32_t idleCounter{0U};
  uint32_t dizzyCounter{0U};
};

#endif // TKPLAYER_H
