#include "tkplayer.h"
#include "tklevel.h"

#include <iostream>
#include <unordered_map>

/**
 * @brief TkPlayer::TkPlayer
 */
// clang-format off
TkPlayer::TkPlayer()
: mAppear("textures/sprites/toki/appear.png", { 0, 0, 85, 85 }),
  mRun(   "textures/sprites/toki/run.png",    { 0, 0, 64, 64 }),
  mStand( "textures/sprites/toki/stand.png",  { 0, 0, 64, 64 }),
  mTurn(  "textures/sprites/toki/turn.png",   { 0, 0, 64, 64 }),
  mJump(  "textures/sprites/toki/jump.png",   { 0, 0, 64, 64 }),
  mFall(  "textures/sprites/toki/fall.png",   { 0, 0, 75, 64 }),
  mClimb( "textures/sprites/toki/climb.png",  { 0, 0, 64, 74 }),
  mDizzy( "textures/sprites/toki/dizzy.png",  { 0, 0, 64, 64 }),
  mIdleA( "textures/sprites/toki/idle.png",   { 0, 0, 64, 64 }),
  mIdleB( mIdleA.texture,                     { 0, 0, 64, 64 }),
  mIdleC( mIdleA.texture,                     { 0, 0, 84, 64 }),
  mIdleD( mIdleA.texture,                     { 0, 0, 84, 64 }),
  mTeleportLookLeft("textures/sprites/toki/teleportlookleft.png", { 0, 0, 84, 64 }),
  mTeleportLookRight("textures/sprites/toki/teleportlookright.png", { 0, 0, 84, 64 }),
  mTeleportLookUp("textures/sprites/toki/teleportlookup.png", { 0, 0, 84, 64 }),
  mTeleportLookDown("textures/sprites/toki/teleportlookdown.png", { 0, 0, 84, 64 }),
  mTeleportLookFront("textures/sprites/toki/teleportlookfront.png", { 0, 0, 84, 64 }),
  mTeleportBeamIn("textures/sprites/toki/teleportbeamin.png", { 0, 0, 84, 64 }),
  mTeleportBeamOut("textures/sprites/toki/teleportbeamout.png", { 0, 0, 84, 64 })
{
  entity = &mAppear;

  // Force climb origin due to height value generating glitch
  mClimb.setOrigin(32.f, 32.f);

  mIdleA.setReverseMode(true);
  mIdleC.setReverseMode(true);

  if (!mAppearSnd.loadFromFile("audio/sfx/appear.wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/'" << std::endl;
  if (!mFootLeftSnd.loadFromFile("audio/sfx/sfx [1].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [1].wav'" << std::endl;
  if (!mFootRightSnd.loadFromFile("audio/sfx/sfx [2].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [2].wav'" << std::endl;
  if (!mJumpSnd.loadFromFile("audio/sfx/sfx [24].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [24].wav'" << std::endl;
  if (!mTurnSnd.loadFromFile("audio/sfx/sfx [38].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [38].wav'" << std::endl;
  if (!mFallSnd.loadFromFile("audio/sfx/sfx [0].wav"))
    std::cerr << "Error. Can't load sound file 'audio/sfx/sfx [0].wav'" << std::endl;

  animMap = {
    { TkPlayer::anim::appear, { 0U, 39U, &mAppear, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standFront} },
      { {0U, &mAppearSnd} } // Movement Sound
    } },
    { TkPlayer::anim::standFront, { 64U, 83U, &mStand, nullptr,
      false, // Transition on last sprite
      { {tk::gesture::left,  anim::quarterFrontLeft  },
        {tk::gesture::right, anim::quarterFrontRight } },
      {} // Movement Sound
    } },
    { TkPlayer::anim::quarterLeftFront,  { 0U, 2U, &mTurn,
      [this](uint32_t /*spriteIndex*/){
        if (mTurn.isAnimationFinished()) animState = idleState;
        return sf::Vector2f();
      },
      true, { }, { }
    } },
    { TkPlayer::anim::quarterRightFront, { 16U, 18U, &mTurn,
      [this](uint32_t /*spriteIndex*/){
        if (mTurn.isAnimationFinished()) animState = idleState;
        return sf::Vector2f();
      },
      true, { }, { }
    } },
    { TkPlayer::anim::quarterFrontRight, { 2U, 5U, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      {} // Movement Sound
    } },
    { TkPlayer::anim::quarterFrontLeft, { 18U, 21U, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      {} // Movement Sound
    } },
    { TkPlayer::anim::quarterBackLeft, { 9U, 11U, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      {} // Movement Sound
    } },
    { TkPlayer::anim::quarterBackRight, { 25U, 27U, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      {} // Movement Sound
    } },
    { TkPlayer::anim::standBack, { 96U, 115U, &mStand, nullptr, false, {}, {}
    } },
    { TkPlayer::anim::standLeft, { 0U, 19U, &mStand,
      [this](uint32_t spriteIndex){
        if (spriteIndex == mStand.getLastSprite()) dizzyCounter = 0U;
        return sf::Vector2f();
      },
      false, // Transition on last sprite
      { {tk::gesture::left, anim::runLeft01     },
        {tk::gesture::right, anim::turnRight    },
        {tk::gesture::up, anim::climbUp         },
        {tk::gesture::down, anim::climbDown     } },
      {} // Movement sound
    } },
    { TkPlayer::anim::standRight, { 32U, 51U, &mStand,
      [this](uint32_t spriteIndex){
        if (spriteIndex == mStand.getLastSprite()) dizzyCounter = 0U;
        return sf::Vector2f();
      },
      false, // Transition on last sprite
      { {tk::gesture::left, anim::turnLeft      },
        {tk::gesture::right, anim::runRight01   },
        {tk::gesture::up, anim::climbUp         },
        {tk::gesture::down, anim::climbDown     } },
      {} // Movement sound
    } },
    { TkPlayer::anim::turnLeft, { 6U, 11U, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {6U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::turnRight, { 0U, 5U, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {0U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::runLeft01, { 0U, 4U, &mRun,
      [](uint32_t /*spriteIndex*/){
        return sf::Vector2f(-6.4f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::left, anim::runLeft02},
        {tk::gesture::none, anim::standLeft} },
      { {2U, &mFootLeftSnd} } // Movement Sound
    } },
    { TkPlayer::anim::runLeft02, { 5U, 9U, &mRun,
      [](uint32_t /*spriteIndex*/){
        return sf::Vector2f(-6.4f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::left, anim::runLeft01},
        {tk::gesture::none, anim::standLeft} },
      { {7U, &mFootLeftSnd} } // Movement Sound
    } },
    { TkPlayer::anim::runRight01, { 10U, 14U, &mRun,
      [](uint32_t /*spriteIndex*/){
        return sf::Vector2f(6.4f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::right, anim::runRight02 },
        {tk::gesture::none, anim::standRight} },
      { {12U, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::anim::runRight02, { 15U, 19U, &mRun,
      [](uint32_t /*spriteIndex*/){
        return sf::Vector2f(6.4f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::right, anim::runRight01 },
        {tk::gesture::none, anim::standRight} },
      { {17U, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::anim::jumpUpLeft, { 0U, 5U, &mJump,
      [this](uint32_t spriteIndex){
        static std::array<sf::Vector2f, 6UL> anim =
          {{ {0.f, 0.f}, {-4.f, 0.f}, {-6.f, -18.f}, {-8.f, -11.f}, {-9.f, -7.f}, {-5.f, 4.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {0U, &mTurnSnd}, {3U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::jumpUpRight, { 8U, 13U, &mJump,
      [this](uint32_t spriteIndex){
        static std::array<sf::Vector2f, 6UL> anim =
          {{ {0.f, 0.f}, {4.f, 0.f}, {6.f, -18.f}, {8.f, -11.f}, {9.f, -7.f}, {5.f, 4.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {8U, &mTurnSnd}, {11U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::jumpDownLeft, { 16U, 22U, &mJump,
      [this](uint32_t spriteIndex){
        static std::array<sf::Vector2f, 7UL> anim =
          {{ {-1.f, 0.f}, {-11.f, -3.f}, {-7.f, 3.f}, {-7.f, 9.f}, {-4.f, 16.f}, {-2.f, 7.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {16U, &mTurnSnd}, {20U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::jumpDownRight, { 24U, 30U, &mJump,
      [this](uint32_t spriteIndex){
        static std::array<sf::Vector2f, 7UL> anim =
          {{ {1.f, 0.f}, {11.f, -3.f}, {7.f, 3.f}, {7.f, 9.f}, {4.f, 16.f}, {2.f, 7.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {24U, &mTurnSnd}, {28U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::fallFront, { 0U, 7U, &mFall,
      [](uint32_t /*spriteIndex*/){
        return sf::Vector2f(0.f, 4.f);
      },
      true, {}, // Transition on last sprite
      { {0U, &mFallSnd}, {4U, &mFallSnd} } // Movement sound
    } },
    { TkPlayer::anim::fallLeft, { 13U, 24U, &mFall,
      [this](uint32_t spriteIndex){
        static std::array<sf::Vector2f, 12UL> anim =
          {{ {-1.f, 0.f}, {-11.f, -3.f}, {-7.f, 3.f}, {-7.f, 9.f}, {-4.f, 16.f}, {-2.f, 7.f},
             { 0.f, 6.f}, {  0.f,  6.f}, { 0.f, 5.f}, { 0.f, 5.f}, { 0.f, 5.f},  { 0.f, 5.f} }};
        return anim[spriteIndex - mFall.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::fallFront} },
      { {0U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::fallRight, { 26U, 37U, &mFall,
      [this](uint32_t spriteIndex){
        static std::array<sf::Vector2f, 12UL> anim =
          {{ {1.f, 0.f}, {11.f, -3.f}, {7.f, 3.f}, {7.f, 9.f}, {4.f, 16.f}, {2.f, 7.f},
             {0.f, 6.f}, { 0.f,  6.f}, {0.f, 5.f}, {0.f, 5.f}, {0.f, 5.f},  {0.f, 5.f} }};
        return anim[spriteIndex - mFall.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::fallFront} },
      { {13U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::landFront, { 39U, 42U, &mFall, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standFront } },
      { {39U, &mFootLeftSnd}, {39U, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::anim::dizzy, { 0U, 23U, &mDizzy, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standFront} },
      {} // Movement sound
    } },
    { TkPlayer::anim::climbUp, { 0U, 7U, &mClimb,
      [](uint32_t /*spriteIndex*/){
        return sf::Vector2f(0.f, -4.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::up, anim::climbUp },
        {tk::gesture::down, anim::climbDown },
        {tk::gesture::left, anim::quarterBackLeft },
        {tk::gesture::right, anim::quarterBackRight },
        {tk::gesture::none, anim::standBack } },
      { {0U, &mFootLeftSnd}, {4U, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::anim::climbDown, { 0U, 7U, &mClimb,
      [](uint32_t /*spriteIndex*/){
        return sf::Vector2f(0.f, 4.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::down, anim::climbDown },
        {tk::gesture::up, anim::climbUp },
        {tk::gesture::left, anim::quarterBackLeft },
        {tk::gesture::right, anim::quarterBackRight },
        {tk::gesture::none, anim::standBack } },
      { {0U, &mFootLeftSnd}, {4U, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::anim::idle1, { 0U, 2U, &mIdleA,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleA.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::idle2, { 16U, 20U, &mIdleA,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleA.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::idle3, { 4U, 12U, &mIdleA,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleA.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::idle4, { 22U, 30U, &mIdleA,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleA.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::idle5, { 120U, 134U, &mIdleC,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleC.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::idle6, { 64U, 111U, &mIdleA,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleA.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::idle7, { 112U, 155U, &mIdleB,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleB.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::idle8, { 144U, 182U, &mIdleD,
      [this](uint32_t /*spriteIndex*/){
        if (mIdleD.isAnimationFinished()) {
          animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                      (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                             lastStand;
          idleState = anim::none;
        }
        return sf::Vector2f();
      },
      false, { }, { }
    } },
    { TkPlayer::anim::teleportLookLeft, { 0U, 2U, &mTeleportLookLeft, nullptr, false, {}, {}
    } },
    { TkPlayer::anim::teleportLookRight, { 0U, 2U, &mTeleportLookRight, nullptr, false, {}, {}
    } },
    { TkPlayer::anim::teleportLookUp, { 0U, 2U, &mTeleportLookUp, nullptr, false, {}, {}
    } },
    { TkPlayer::anim::teleportLookDown, { 0U, 2U, &mTeleportLookDown, nullptr, false, {}, {}
    } },
    { TkPlayer::anim::teleportLookFront, { 0U, 6U, &mTeleportLookFront, nullptr,
      true,
      { {tk::gesture::none, anim::teleportLookUp } },
      {}
    } },
    { TkPlayer::anim::teleportBeamOut, {0U, 14U, &mTeleportBeamOut, nullptr,
      true,
      { {tk::gesture::none, anim::teleportBeamIn } },
      {}
    } },
    { TkPlayer::anim::teleportBeamIn, {0U, 11U, &mTeleportBeamIn,
      [this] (uint32_t spriteIndex) {
        if (spriteIndex == mTeleportBeamIn.getFirstSprite()) {
          static const std::unordered_map<tk::gesture, sf::Vector2f> map {
            { tk::gesture::left,  {-128.f,    0.f} },
            { tk::gesture::right, { 128.f,    0.f} },
            { tk::gesture::up,    {   0.f, -128.f} },
            { tk::gesture::down,  {   0.f,  128.f} }
          };
          return map.at(teleportGesture);
        }
        return sf::Vector2f();
      },
      true,
      { {tk::gesture::none, anim::standFront } },
      {}
    } },
    { TkPlayer::anim::levelComplete, { 48U, 130U, &mAppear, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::finish } },
      { {48U, &mAppearSnd} } // Movement sound
    } }
  };

  updateIdleState(true);
}
// clang-format on

/**
 * @brief TkPlayer::move
 * @param level
 * @param gesture
 * @return
 */
sf::Vector2f TkPlayer::move(TkLevel& level, const enum tk::gesture& gesture) {
  sf::Vector2f offset(0.f, 0.f);

  static TkPlayer::anim lastAnimState = anim::none;
  auto currentState = animMap.find(animState);
  if (currentState != animMap.end()) {
    animMachine sm = currentState->second;

    if (animState != lastAnimState) {
      entity = sm.image;
      entity->resetSprite(sm.spriteBegin, sm.spriteEnd);
      entity->setPosition(position);
      // Update last animation state
      lastAnimState = animState;
    }
    uint32_t currentId = entity->nextSprite();
    if (sm.func) offset += sm.func(currentId);

    if (sm.sound.find(currentId) != sm.sound.end()) {
      sm.sound[currentId]->play();
    }

    entity->move(offset);
    position = entity->getPosition();
    level.eggChecker(entity->getTransform().transformRect({8,7,47,57}));

    if (!sm.transitionOnLast || entity->isAnimationFinished()) {
      for (auto pair : sm.transition) {
        if ((pair.first & gesture) == pair.first) {
          animState = static_cast<anim>(pair.second);
          break;
        }
      }

      // Manage transition constraints
      transitionHandler(level, gesture, lastAnimState);
    }
  }

  // clang-format off
    static const std::unordered_map<tk::gesture, sf::Vector2f> map {
      {tk::gesture::left,{-10.66f,   0.f  }}, {tk::gesture::right,{10.66f,  0.f  }},
      {tk::gesture::up,  {  0.f  , -10.66f}}, {tk::gesture::down, { 0.f  , 10.66f}}};
    switch (lastAnimState) {
      case anim::jumpUpLeft: return { -5.33f, -5.33f };
      case anim::jumpUpRight: return {  5.33f, -5.33f };
      case anim::jumpDownLeft: return { -4.57f,  4.57f };
      case anim::jumpDownRight: return {  4.57f,  4.57f };
      case anim::teleportBeamIn: return map.at(teleportGesture);
      default: break;
    }
  // clang-format on

  return offset;
}

/**
 * @brief TkPlayer::transitionHandler
 */
void TkPlayer::transitionHandler(const TkLevel& level,
                                 const enum tk::gesture& gesture,
                                 const TkPlayer::anim& currentAnimState) {

  // clang-format off
  switch (animState) {
    case anim::standFront: {
      tk::action action =
          level.isMovable(entity->getPosition(), tk::gesture::none);
      if (action == tk::action::fall)
        animState = anim::fallFront;
      else if (gesture != tk::gesture::left && gesture != tk::gesture::right)
        animState = (lastStand == anim::standLeft)  ? anim::quarterFrontLeft  :
                    (lastStand == anim::standRight) ? anim::quarterFrontRight :
                                                      standFront;
      break;
    }
    case anim::standLeft:
    case anim::standRight: {
      lastStand = animState;
      break;
    }
    case anim::runLeft01:
    case anim::runLeft02: {
      tk::action action = level.isMovable(
          entity->getPosition(), tk::gesture::left);

      animState = (action == tk::action::none)     ? anim::standLeft    :
                  (action == tk::action::jumpUp)   ? anim::jumpUpLeft   :
                  (action == tk::action::jumpDown) ? anim::jumpDownLeft :
                  (action == tk::action::run)      ? animState          :
                                                     anim::fallLeft;
      break;
    }
    case anim::runRight01:
    case anim::runRight02: {
      tk::action action = level.isMovable(
          entity->getPosition(), tk::gesture::right);

      animState = (action == tk::action::none)     ? anim::standRight    :
                  (action == tk::action::jumpUp)   ? anim::jumpUpRight   :
                  (action == tk::action::jumpDown) ? anim::jumpDownRight :
                  (action == tk::action::run)      ? animState           :
                                                     anim::fallRight;
      break;
    }
    case anim::climbUp: {
      tk::action action = level.isMovable(
          entity->getPosition(), tk::gesture::up);
      if (action != tk::action::ladderUp) {
        if (currentAnimState == anim::climbUp)
          animState = (lastStand == anim::standLeft)
                          ? anim::quarterBackLeft
                          : anim::quarterBackRight;
        else
          animState = lastStand;
      }
      break;
    }
    case anim::climbDown: {
      tk::action action = level.isMovable(
          entity->getPosition(), tk::gesture::down);
      if (action != tk::action::ladderDown) {
        if (currentAnimState == anim::climbDown)
          animState = (lastStand == anim::standLeft)
                          ? anim::quarterBackLeft
                          : anim::quarterBackRight;
        else
          animState = lastStand;
      }
      break;
    }
    case anim::fallFront: {
      tk::action action = level.isMovable(
          entity->getPosition(), tk::gesture::down);
      if (action != tk::action::fall) animState = landFront;
      break;
    }
    case anim::standBack: {
      animState = (lastStand == anim::standLeft)
                      ? anim::quarterBackLeft
                      : anim::quarterBackRight;
      break;
    }
    case teleportLookLeft:
    case teleportLookRight:
    case teleportLookUp:
    case teleportLookDown:
    case teleportLookFront: {
      if (!teleportActivated) {
        animState = (lastStand == anim::standLeft)
                        ? anim::quarterFrontLeft
                        : anim::quarterFrontRight;
      }
      break;
    }
    default: break;
  }
  // clang-format on

  idleTransitionHandler();
  dizzyTransitionHandler();
}

/**
 * @brief TkPlayer::updateIdleState
 */
void TkPlayer::updateIdleState(bool reset) {
  idleCounter = 0U;
  idleDelay = 200 + rand() % 75;
  reset ? idleRange = 1U : idleRange += (idleRange < 8);
}

/**
 * @brief TkPlayer::idleTransitionHandler
 */
void TkPlayer::idleTransitionHandler() {
  // Update idle counter
  switch (animState) {
    case anim::standLeft:
    case anim::standRight:
    case anim::standFront: {
      if (idleState == anim::none) idleCounter++;
      break;
    }
    default: {
      if (idleCounter) updateIdleState(true);
      break;
    }
  }

  if (idleState == anim::none) {
    if (idleCounter == idleDelay) {
      idleState = (anim)(anim::idle1 + (rand() % idleRange));
      updateIdleState();
    }

    // Enable idle animation
    if (idleState != anim::none) {
      // clang-format off
      animState = (lastStand == anim::standLeft)  ? anim::quarterLeftFront  :
                  (lastStand == anim::standRight) ? anim::quarterRightFront :
                                                         idleState;
      // clang-format on
    }
  }
}

/**
 * @brief TkPlayer::dizzyTransitionHandler
 */
void TkPlayer::dizzyTransitionHandler() {
  switch (animState) {
    case anim::turnLeft:
    case anim::turnRight: {
      dizzyCounter++;
      break;
    }
    case anim::standLeft:
    case anim::standRight: {
      // Nothing to do. Already managed in stateMachine
      break;
    }
    default: {
      dizzyCounter = 0;
      break;
    }
  }
  // Enable dizzy sprite if counter is reached
  if (dizzyCounter == 6) animState = anim::dizzy;
}
