#include "tkplayer.h"
#include "tklevel.h"

#include <iostream>
#include <unordered_map>

/**
 * @brief TkPlayer::TkPlayer
 */
// clang-format off
TkPlayer::TkPlayer(void)
: mAppear("Media/appear.png", { 0, 0, 85, 85 }),
  mRun(   "Media/run.png",    { 0, 0, 64, 64 }),
  mStand( "Media/stand.png",  { 0, 0, 64, 64 }),
  mTurn(  "Media/turn.png",   { 0, 0, 64, 64 }),
  mJump(  "Media/jump.png",   { 0, 0, 64, 64 }),
  mFall01("Media/fall01.png", { 0, 0, 75, 64 }),
  mFall02("Media/fall02.png", { 0, 0, 75, 64 }),
  mClimb( "Media/climb.png",  { 0, 0, 64, 74 }),
  mDizzy( "Media/dizzy.png",  { 0, 0, 64, 64 })
{
  // Force climb origin due to height value generating glitch
  mClimb.setOrigin(32.f, 32.f);

  if (!mAppearSnd.openFromFile("Media/appear.wav"))
    std::cerr << "Error. Can't load sound file Media/appear.wav" << std::endl;
  if (!mFootLeftSnd.openFromFile("Media/sfx [1].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [1].wav" << std::endl;
  if (!mFootRightSnd.openFromFile("Media/sfx [2].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [2].wav" << std::endl;
  if (!mJumpSnd.openFromFile("Media/sfx [24].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [24].wav" << std::endl;
  if (!mTurnSnd.openFromFile("Media/sfx [38].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [38].wav" << std::endl;
  if (!mFallSnd.openFromFile("Media/sfx [0].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [0].wav" << std::endl;

  mAnimation = {
    { TkPlayer::anim::appear, { 0U, 39U, 1.f, &mAppear, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standFront} },
      { {0U, &mAppearSnd} } // Movement Sound
    } },
    { TkPlayer::anim::standFront, { 64U, 83U, 1.f, &mStand, nullptr,
      false, // Transition on last sprite
      { {tk::gesture::back,  anim::levelComplete},
        {tk::gesture::left,  anim::quarterLeft  },
        {tk::gesture::right, anim::quarterRight } },
      {} // Movement Sound
    } },
    { TkPlayer::anim::quarterLeft, { 20U, 23U, 1.f, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {20U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::quarterRight, { 16U, 19U, 1.f, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {16U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::standBack, { 96U, 116U, 1.f, &mStand, nullptr, false, {}, {} } },
    { TkPlayer::anim::standLeft, { 0U, 19U, 1.f, &mStand,
      [this](uint spriteIndex){
        if (spriteIndex == mStand.getLastSprite()) mDizzyCounter = 0U;
        return sf::Vector2f();
      },
      false, // Transition on last sprite
      { {tk::gesture::back, anim::levelComplete },
        {tk::gesture::left, anim::runLeft       },
        {tk::gesture::right, anim::turnRight    },
        {tk::gesture::up, anim::climbUp         },
        {tk::gesture::down, anim::climbDown     } },
      {} // Movement sound
    } },
    { TkPlayer::anim::standRight, { 32U, 51U, 1.f, &mStand,
      [this](uint spriteIndex){
        if (spriteIndex == mStand.getLastSprite()) mDizzyCounter = 0U;
        return sf::Vector2f();
      },
      false, // Transition on last sprite
      { {tk::gesture::back, anim::levelComplete },
        {tk::gesture::left, anim::turnLeft      },
        {tk::gesture::right, anim::runRight     },
        {tk::gesture::up, anim::climbUp         },
        {tk::gesture::down, anim::climbDown     } },
      {} // Movement sound
    } },
    { TkPlayer::anim::turnLeft, { 6U, 11U, 1.f, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {6U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::turnRight, { 0U, 5U, 1.f, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {0U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::runLeft, { 0U, 15U, 1.f, &mRun,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(-2.f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::left, anim::runLeft  },
        {tk::gesture::none, anim::standLeft} },
      { {0U, &mFootLeftSnd}, {8U, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::anim::runRight, { 16U, 31U, 1.f, &mRun,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(2.f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::right, anim::runRight },
        {tk::gesture::none, anim::standRight} },
      { {16U, &mFootLeftSnd}, {24U, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::anim::jumpUpLeft, { 0U, 5U, 1.f, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 6UL> anim =
          {{ {0.f, 0.f}, {-4.f, 0.f}, {-6.f, -18.f}, {-8.f, -11.f}, {-9.f, -7.f}, {-5.f, 4.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {0U, &mTurnSnd}, {3U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::jumpUpRight, { 8U, 13U, 1.f, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 6UL> anim =
          {{ {0.f, 0.f}, {4.f, 0.f}, {6.f, -18.f}, {8.f, -11.f}, {9.f, -7.f}, {5.f, 4.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {8U, &mTurnSnd}, {11U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::jumpDownLeft, { 16U, 22U, 1.f, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 7UL> anim =
          {{ {-1.f, 0.f}, {-11.f, -3.f}, {-7.f, 3.f}, {-7.f, 9.f}, {-4.f, 16.f}, {-2.f, 7.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {16U, &mTurnSnd}, {20U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::jumpDownRight, { 24U, 30U, 1.f, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 7UL> anim =
          {{ {1.f, 0.f}, {11.f, -3.f}, {7.f, 3.f}, {7.f, 9.f}, {4.f, 16.f}, {2.f, 7.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {24U, &mTurnSnd}, {28U, &mJumpSnd} } // Movement sound
    } },
    { TkPlayer::anim::fallFront, { 0U, 7U/*11*/, 1.f, &mFall01,
      [](uint /*spriteIndex*/){
//        if ((spriteIndex)%3 == 2) mFall01.nextSprite();
        return sf::Vector2f(0.f, 4.f);
      },
      true, {},// Transition on last sprite
      { {0U, &mFallSnd}, {4U, &mFallSnd} } // Movement sound
    } },
    { TkPlayer::anim::fallLeft, { 0U, 11U, 1.f, &mFall02,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 12UL> anim =
          {{ {-1.f, 0.f}, {-11.f, -3.f}, {-7.f, 3.f}, {-7.f, 9.f}, {-4.f, 16.f}, {-2.f, 7.f},
             { 0.f, 6.f}, {  0.f,  6.f}, { 0.f, 5.f}, { 0.f, 5.f}, { 0.f, 5.f},  { 0.f, 5.f} }};
        return anim[spriteIndex - mFall02.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::fallFront} },
      { {0U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::fallRight, { 13U, 24U, 1.f, &mFall02,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 12UL> anim =
          {{ {1.f, 0.f}, {11.f, -3.f}, {7.f, 3.f}, {7.f, 9.f}, {4.f, 16.f}, {2.f, 7.f},
             {0.f, 6.f}, { 0.f,  6.f}, {0.f, 5.f}, {0.f, 5.f}, {0.f, 5.f},  {0.f, 5.f} }};
        return anim[spriteIndex - mFall02.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::fallFront} },
      { {13U, &mTurnSnd} } // Movement Sound
    } },
    { TkPlayer::anim::dizzy, { 0U, 23U, 1.f, &mDizzy, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standFront} },
      {} // Movement sound
    } },
    { TkPlayer::anim::climbUp, { 0U, 7U, 1.f, &mClimb,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(0.f, -4.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::up, anim::climbUp },
        {tk::gesture::down, anim::climbDown },
        {tk::gesture::left, anim::turnLeft },
        {tk::gesture::right, anim::turnRight },
        {tk::gesture::none, anim::standRight } },
      { {0U, &mFootLeftSnd}, {4U, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::anim::climbDown, { 0U, 7U, 1.f, &mClimb,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(0.f, 4.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::down, anim::climbDown },
        {tk::gesture::up, anim::climbUp },
        {tk::gesture::left, anim::turnLeft },
        {tk::gesture::right, anim::turnRight },
        {tk::gesture::none, anim::standRight } },
      { {0U, &mFootLeftSnd}, {4U, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::anim::levelComplete, { 48U, 130U, 1.f, &mAppear, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::finish } },
      { {48U, &mAppearSnd} } // Movement sound
    } }
  };
}
// clang-format on

/**
 * @brief TkPlayer::setPosition
 * @param positionX
 * @param positionY
 */
void TkPlayer::setPosition(float positionX, float positionY) {
  setPosition(sf::Vector2f(positionX, positionY));
}

/**
 * @brief TkPlayer::setPosition
 * @param position
 */
void TkPlayer::setPosition(const sf::Vector2f& position) {
  mPosition = position;
}

/**
 * @brief TkPlayer::setMovement
 * @param movement
 */
void TkPlayer::setMovement(const enum tk::gesture& movement) {
  movementState = movement;
}

/**
 * @brief TkPlayer::move
 * @param deltaTime
 */
void TkPlayer::move(const sf::Time& /*deltaTime*/) {
  sf::Vector2f offset(0.f, 0.f);

  auto currentState = mAnimation.find(animationState);
  if (currentState != mAnimation.end()) {
    animMachine sm = currentState->second;

    if (sm.image != mToki) {
      mToki = sm.image;
      mToki->resetSprite(sm.spriteBegin, sm.spriteEnd);
      mToki->setPosition(mPosition);
    }
    uint currentId = mToki->nextSprite();
    if (sm.func) offset += sm.func(currentId);

    if (sm.sound.find(currentId) != sm.sound.end()) {
      sm.sound[currentId]->stop();
      sm.sound[currentId]->play();
    }

    if ((mPosition.x >= 464.f && mPosition.x <= 496.f) ||
        (mPosition.y >= 284.f && mPosition.y <= 316.f))
      offset = mLevel->move(offset);
    mToki->move(offset);
    mPosition = mToki->getPosition();
    mLevel->eggChecker(mToki->getGlobalBounds());

    if (!sm.transitionOnLast || currentId == sm.spriteEnd) {
      for (auto pair : sm.transition) {
        if ((pair.first & movementState) == pair.first) {
          animationState = static_cast<anim>(pair.second);
          break;
        }
      }

      // Manage transition constraints
      gestureHandler();
    }
  }
}

/**
 * @brief TkPlayer::gestureHandler
 */
void TkPlayer::gestureHandler() {
  switch (animationState) {
    case anim::runLeft: {
      tk::action action = mLevel->isMovable(
          mToki->getPosition(), tk::gesture::left);
      // clang-format off
      animationState = (action == tk::action::none)     ? TkPlayer::anim::standLeft    :
                       (action == tk::action::jumpUp)   ? TkPlayer::anim::jumpUpLeft   :
                       (action == tk::action::jumpDown) ? TkPlayer::anim::jumpDownLeft :
                       (action == tk::action::run)      ? TkPlayer::anim::runLeft      :
                                                          TkPlayer::anim::fallLeft;
      // clang-format on
      break;
    }
    case anim::runRight: {
      tk::action action = mLevel->isMovable(
          mToki->getPosition(), tk::gesture::right);
      // clang-format off
      animationState = (action == tk::action::none)     ? TkPlayer::anim::standRight    :
                       (action == tk::action::jumpUp)   ? TkPlayer::anim::jumpUpRight   :
                       (action == tk::action::jumpDown) ? TkPlayer::anim::jumpDownRight :
                       (action == tk::action::run)      ? TkPlayer::anim::runRight      :
                                                          TkPlayer::anim::fallRight;
      // clang-format on
      break;
    }
    case anim::climbUp: {
      tk::action action = mLevel->isMovable(
          mToki->getPosition(), tk::gesture::up);
      if (action != tk::action::ladderUp) {
        animationState = standRight;
      }
      break;
    }
    case anim::climbDown: {
      tk::action action = mLevel->isMovable(
          mToki->getPosition(), tk::gesture::down);
      if (action != tk::action::ladderDown) animationState = standRight;
      break;
    }
    case anim::fallFront: {
      tk::action action = mLevel->isMovable(
          mToki->getPosition(), tk::gesture::down);
      if (action != tk::action::fall) animationState = standFront;
      break;
    }
    default: break;
  }
}

/**
 * @brief TkPlayer::drawableSprite
 * @return
 */
const sf::Drawable& TkPlayer::drawableSprite(void) { return *mToki; }
