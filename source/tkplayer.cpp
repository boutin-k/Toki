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
  mClimb.setOrigin(32, 32);

  if (!mAppearSound.openFromFile("Media/appear.wav"))
    std::cerr << "Error. Can't load sound file Media/appear.wav" << std::endl;
  if (!mFootLeftSnd.openFromFile("Media/sfx [1].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [1].wav" << std::endl;
  if (!mFootRightSnd.openFromFile("Media/sfx [2].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [2].wav" << std::endl;
  if (!mJumpSound.openFromFile("Media/sfx [24].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [24].wav" << std::endl;
  if (!mTurnSound.openFromFile("Media/sfx [38].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [38].wav" << std::endl;

  mAnimation = {
    { TkPlayer::anim::appear, { 0, 39, &mAppear, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standFront} },
      { {0, &mAppearSound} } // Movement Sound
    } },
    { TkPlayer::anim::standFront, { 64, 83, &mStand, nullptr,
      false, // Transition on last sprite
      { {tk::gesture::back,  anim::levelComplete},
        {tk::gesture::left,  anim::quarterLeft  },
        {tk::gesture::right, anim::quarterRight } },
      {} // Movement Sound
    } },
    { TkPlayer::anim::quarterLeft, { 20, 23, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {20, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::anim::quarterRight, { 16, 19, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {16, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::anim::standBack, { 96, 116, &mStand, nullptr, false, {}, {} } },
    { TkPlayer::anim::standLeft, { 0, 19, &mStand,
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
    { TkPlayer::anim::standRight, { 32, 51, &mStand,
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
    { TkPlayer::anim::turnLeft, { 6, 11, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {6, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::anim::turnRight, { 0, 5, &mTurn, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {0, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::anim::runLeft, { 0, 15, &mRun,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(-2.f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::left, anim::runLeft  },
        {tk::gesture::none, anim::standLeft} },
      { {0, &mFootLeftSnd}, {8, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::anim::runRight, { 16, 31, &mRun,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(2.f, 0.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::right, anim::runRight },
        {tk::gesture::none, anim::standRight} },
      { {16, &mFootLeftSnd}, {24, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::anim::jumpUpLeft, { 0, 5, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 6> anim =
          {{ {0.f, 0.f}, {-4.f, 0.f}, {-6.f, -18.f}, {-8.f, -11.f}, {-9.f, -7.f}, {-5.f, 4.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {0, &mTurnSound}, {3, &mJumpSound} } // Movement sound
    } },
    { TkPlayer::anim::jumpUpRight, { 8, 13, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 6> anim =
          {{ {0.f, 0.f}, {4.f, 0.f}, {6.f, -18.f}, {8.f, -11.f}, {9.f, -7.f}, {5.f, 4.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {8, &mTurnSound}, {11, &mJumpSound} } // Movement sound
    } },
    { TkPlayer::anim::jumpDownLeft, { 16, 22, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 7> anim =
          {{ {-1.f, 0.f}, {-11.f, -3.f}, {-7.f, 3.f}, {-7.f, 9.f}, {-4.f, 16.f}, {-2.f, 7.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standLeft} },
      { {16, &mTurnSound}, {20, &mJumpSound} } // Movement sound
    } },
    { TkPlayer::anim::jumpDownRight, { 24, 30, &mJump,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 7> anim =
          {{ {1.f, 0.f}, {11.f, -3.f}, {7.f, 3.f}, {7.f, 9.f}, {4.f, 16.f}, {2.f, 7.f} }};
        return anim[spriteIndex - mJump.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standRight} },
      { {24, &mTurnSound}, {28, &mJumpSound} } // Movement sound
    } },
    { TkPlayer::anim::fallFront, { 0, 11, &mFall01,
      [](uint /*spriteIndex*/){
//        if ((spriteIndex)%3 == 2) mFall01.nextSprite();
        return sf::Vector2f(0.f, 4.f);
      },
      true, {},// Transition on last sprite
      {} // Movement sound
    } },
    { TkPlayer::anim::fallLeft, { 0, 11, &mFall02,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 12> anim =
          {{ {-1.f, 0.f}, {-11.f, -3.f}, {-7.f, 3.f}, {-7.f, 9.f}, {-4.f, 16.f}, {-2.f, 7.f},
             { 0.f, 6.f}, {  0.f,  6.f}, { 0.f, 5.f}, { 0.f, 5.f}, { 0.f, 5.f},  { 0.f, 5.f} }};
        return anim[spriteIndex - mFall02.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::fallFront} },
      {} // Movement sound
    } },
    { TkPlayer::anim::fallRight, { 13, 24, &mFall02,
      [this](uint spriteIndex){
        static std::array<sf::Vector2f, 12> anim =
          {{ {1.f, 0.f}, {11.f, -3.f}, {7.f, 3.f}, {7.f, 9.f}, {4.f, 16.f}, {2.f, 7.f},
             {0.f, 6.f}, { 0.f,  6.f}, {0.f, 5.f}, {0.f, 5.f}, {0.f, 5.f},  {0.f, 5.f} }};
        return anim[spriteIndex - mFall02.getFirstSprite()];
      },
      true, // Transition on last sprite
      { {tk::gesture::none, anim::fallFront} },
      {} // Movement sound
    } },
    { TkPlayer::anim::dizzy, { 0, 23, &mDizzy, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::standFront} },
      {} // Movement sound
    } },
    { TkPlayer::anim::climbUp, { 0, 7, &mClimb,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(0.f, -4.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::up, anim::climbUp },
        {tk::gesture::down, anim::climbDown },
        {tk::gesture::left, anim::turnLeft },
        {tk::gesture::right, anim::turnRight },
        {tk::gesture::none, anim::standRight } },
      { {0, &mFootLeftSnd}, {4, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::anim::climbDown, { 0, 7, &mClimb,
      [](uint /*spriteIndex*/){
        return sf::Vector2f(0.f, 4.f);
      },
      true, // Transition on last sprite
      { {tk::gesture::down, anim::climbDown },
        {tk::gesture::up, anim::climbUp },
        {tk::gesture::left, anim::turnLeft },
        {tk::gesture::right, anim::turnRight },
        {tk::gesture::none, anim::standRight } },
      { {0, &mFootLeftSnd}, {4, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::anim::levelComplete, { 48, 130, &mAppear, nullptr,
      true, // Transition on last sprite
      { {tk::gesture::none, anim::finish } },
      { {48, &mAppearSound} } // Movement sound
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

    if (sm.image != mCurrentImage) {
      mCurrentImage = sm.image;
      mCurrentImage->resetSprite(sm.spriteBegin, sm.spriteEnd);
      mCurrentImage->setPosition(mPosition);
    }
    uint currentId = mCurrentImage->nextSprite();
    if (sm.func) offset += sm.func(currentId);

    if (sm.sound.find(currentId) != sm.sound.end()) {
      sm.sound[currentId]->stop();
      sm.sound[currentId]->play();
    }

    if ((mPosition.x >= 464.f && mPosition.x <= 496.f) ||
        (mPosition.y >= 284.f && mPosition.y <= 316.f))
      offset = mCurrentWorld->move(offset);
    mCurrentImage->move(offset);
    mPosition = mCurrentImage->getPosition();

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
      tk::action action = mCurrentWorld->isMovable(
          mCurrentImage->getPosition(), tk::gesture::left);
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
      tk::action action = mCurrentWorld->isMovable(
          mCurrentImage->getPosition(), tk::gesture::right);
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
      tk::action action = mCurrentWorld->isMovable(
          mCurrentImage->getPosition(), tk::gesture::up);
      if (action != tk::action::ladderUp) {
        animationState = standRight;
      }
      break;
    }
    case anim::climbDown: {
      tk::action action = mCurrentWorld->isMovable(
          mCurrentImage->getPosition(), tk::gesture::down);
      if (action != tk::action::ladderDown) animationState = standRight;
      break;
    }
    case anim::fallFront: {
      std::cout << "anim::fallFront\n";
      tk::action action = mCurrentWorld->isMovable(
          mCurrentImage->getPosition(), tk::gesture::down);
      std::cout << "action: " << (int)action << "\n";
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
const sf::Drawable& TkPlayer::drawableSprite(void) { return *mCurrentImage; }
