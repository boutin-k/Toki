#include "tkplayer.h"
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
  mClimb( "Media/climb.png",  { 0, 0, 64, 74 }),
  mDizzy( "Media/dizzy.png",  { 0, 0, 64, 64 })
{
  if (!mAppearSound.openFromFile("Media/appear.wav"))
    std::cerr << "Error. Can't load sound file Media/appear.wav" << std::endl;
  if (!mFootLeftSnd.openFromFile("Media/sfx [1].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [1].wav" << std::endl;
  if (!mFootRightSnd.openFromFile("Media/sfx [2].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [2].wav" << std::endl;
  if (!mTurnSound.openFromFile("Media/sfx [38].wav"))
    std::cerr << "Error. Can't load sound file Media/sfx [38].wav" << std::endl;

  mStateMachine = {
    { TkPlayer::state::appear, { 0, 39, &mAppear, nullptr,
      true, // Transition on last sprite
      { {movement::none, state::standFront} },
      { {0, &mAppearSound} } // Movement Sound
    } },
    { TkPlayer::state::standFront, { 64, 83, &mStand, nullptr,
      false, // Transition on last sprite
      { {movement::back,  state::levelComplete},
        {movement::left,  state::quarterLeft  },
        {movement::right, state::quarterRight } },
      {} // Movement Sound
    } },
    { TkPlayer::state::quarterLeft, { 20, 23, &mTurn, nullptr,
      true, // Transition on last sprite
      { {movement::none, state::standLeft} },
      { {20, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::state::quarterRight, { 16, 19, &mTurn, nullptr,
      true, // Transition on last sprite
      { {movement::none, state::standRight} },
      { {16, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::state::standBack, { 96, 116, &mStand, nullptr, false, {}, {} } },
    { TkPlayer::state::standLeft, { 0, 19, &mStand,
      [this](uint spriteIndex){
        if (spriteIndex == mStand.getLastSprite()) mDizzyCounter = 0U;
      },
      false, // Transition on last sprite
      { {movement::back, state::levelComplete },
        {movement::left, state::runLeft       },
        {movement::right, state::turnRight    },
        {movement::up, state::climb           },
        {movement::down, state::climb         } },
      {} // Movement sound
    } },
    { TkPlayer::state::standRight, { 32, 51, &mStand,
      [this](uint spriteIndex){
        if (spriteIndex == mStand.getLastSprite()) mDizzyCounter = 0U;
      },
      false, // Transition on last sprite
      { {movement::back, state::levelComplete },
        {movement::left, state::turnLeft      },
        {movement::right, state::runRight     },
        {movement::up, state::climb           },
        {movement::down, state::climb         } },
      {} // Movement sound
    } },
    { TkPlayer::state::turnLeft, { 6, 11, &mTurn, nullptr,
      true, // Transition on last sprite
      { {movement::none, state::standLeft} },
      { {6, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::state::turnRight, { 0, 5, &mTurn, nullptr,
      true, // Transition on last sprite
      { {movement::none, state::standRight} },
      { {0, &mTurnSound} } // Movement Sound
    } },
    { TkPlayer::state::runLeft, { 0, 15, &mRun,
      [this](uint spriteIndex){
      if (spriteIndex == mRun.getLastSprite())
        if (movementState != movement::left) animationState = state::standLeft;
      },
      true, { }, // Transition on last sprite
      { {0, &mFootLeftSnd}, {8, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::state::runRight, { 16, 31, &mRun,
      [this](uint spriteIndex){
      if (spriteIndex == mRun.getLastSprite())
        if (movementState != movement::right) animationState = state::standRight;
      },
      true, { }, // Transition on last sprite
      { {16, &mFootLeftSnd}, {24, &mFootRightSnd} } // Movement Sound
    } },
    { TkPlayer::state::dizzy, { 0, 23, &mDizzy, nullptr,
      true, // Transition on last sprite
      { {movement::none, state::standFront} },
      {} // Movement sound
    } },
    { TkPlayer::state::climb, { 0, 7, &mClimb, nullptr,
      true, // Transition on last sprite
      { {movement::left, state::turnLeft },
        {movement::right, state::turnRight } },
      { {0, &mFootLeftSnd}, {4, &mFootRightSnd} } // Movement sound
    } },
    { TkPlayer::state::levelComplete, { 48, 130, &mAppear, nullptr,
      true, // Transition on last sprite
      { {movement::none, state::finish } },
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
void TkPlayer::setMovement(const enum movement& movement) {
  movementState = movement;
}

/**
 * @brief TkPlayer::move
 * @param deltaTime
 */
void TkPlayer::move(const sf::Time& deltaTime) {
  sf::Vector2f offset(0.f, 0.f);

  auto currentState = mStateMachine.find(animationState);
  if (currentState != mStateMachine.end()) {
    stateMachine sm = currentState->second;

    if (sm.image != mCurrentImage) {
      mCurrentImage = sm.image;
      mCurrentImage->resetSprite(sm.startSprite, sm.endSprite);
      mCurrentImage->setPosition(mPosition);
    }
    uint currentId = mCurrentImage->nextSprite();
    if (sm.func) sm.func(currentId);

    if (sm.sound.find(currentId) != sm.sound.end()) {
      sm.sound[currentId]->stop();
      sm.sound[currentId]->play();
    }
//    // TEST TO REMOVE
//    if (animationState == state::runLeft || animationState == state::runRight) {
//      offset.x += animationState == state::runRight ? 240.f : -240.f;
//    }

    if ((sm.transitionOnLast && currentId == sm.endSprite) ||
        !sm.transitionOnLast) {
      for (auto pair : sm.transition) {
        if ((pair.first & movementState) == pair.first) {
          animationState = static_cast<state>(pair.second);
          break;
        }
      }
    }
  }

  if (nullptr != mCurrentImage) {
    mCurrentImage->move(offset*deltaTime.asSeconds());
    mPosition = mCurrentImage->getPosition();
  }
}

/**
 * @brief TkPlayer::drawableSprite
 * @return
 */
const sf::Drawable& TkPlayer::drawableSprite(void) { return *mCurrentImage; }

//void TkPlayer::handleEvent() {

//}
