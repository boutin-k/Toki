#ifndef TKACTION_H
#define TKACTION_H

#include "tkimage.h"
#include "tkplayer.h"

#include "SFML/Window/Event.hpp"
#include "SFML/Graphics/Text.hpp"
#include "SFML/Graphics/Font.hpp"

class TkAction
{
  struct Action {
    TkImage  button;
    uint32_t counter{0U};
    bool     enabled{false};
    bool     infinite{false};
  };

 public:
  enum TkItem { bridge, teleport, bloc, crate, size };

  TkAction(const sf::Vector2u& windowSize);

  void update(const uint8_t buffer[]);

  TkAction::TkItem handleMouseButtonEvent(
      const sf::Event::MouseButtonEvent& event);
  TkAction::TkItem handleMouseMovedEvent(
      const sf::Event::MouseMoveEvent& event);

  inline bool authorized(TkItem item) {
    return (item < TkItem::size && list[item].enabled && list[item].counter);
  }

  inline void decrease(TkItem item) { if (authorized(item)) list[item].counter--; }
  inline bool isSelected() { return selectedVisible; }
  inline bool isHovered() { return hoveredVisible; }

  Action list[TkItem::size];
  TkImage selectedImage;
  bool selectedVisible{false};
  TkImage hoveredImage;
  bool hoveredVisible{false};

  TkImage   counterImage;
  sf::Text  counterText;
  sf::Font  counterFont;

  sf::Vector2u windowSize;
};

#endif // TKACTION_H
