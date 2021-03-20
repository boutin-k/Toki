#include "tkaction.h"

#include <iostream>

/**
 * @brief TkAction::TkAction
 * @param windowSize
 */
TkAction::TkAction(const sf::Vector2u& windowSize)
    : windowSize(windowSize)
{
  selectedImage.loadFromFile("textures/actions/selection.png");
  selectedImage.setTkOrigin(TkImage::center);

  hoveredImage.loadFromFile("textures/actions/hover.png");
  hoveredImage.setTkOrigin(TkImage::center);

  counterImage.loadFromFile("textures/actions/counter.png");
  counterImage.setTkOrigin(TkImage::center);

  if (!counterFont.loadFromFile("fonts/Z003.ttf")) {
    std::cerr << "Could not load font Z003.ttf\n";
  } else {
    counterText.setFont(counterFont);
    counterText.setFillColor(sf::Color::White);
    counterText.setStyle(sf::Text::Bold);
    counterText.setCharacterSize(22);
  }
}

/**
 * @brief TkAction::update
 * @param action
 */
void TkAction::update(const uint8_t buffer[]) {
  // clang-format off
  static const std::vector<std::pair<std::string, sf::Vector2i>> data{
      {"textures/actions/bridge",   {-108, -57}},
      {"textures/actions/teleport", { -36, -57}},
      {"textures/actions/bloc",     {  36, -57}},
      {"textures/actions/crate",    { 108, -57}}
  };
  // clang-format on


  for (int item = 0; item < TkItem::size; ++item) {
    uint32_t value = buffer[item];

    if (value == 0xFF)
      list[item].infinite = true;
    else
      list[item].counter = value;

    // Enable/Disable button
    list[item].enabled = (value != 0);

    std::string filename = data[item].first;
    filename += (list[item].enabled) ? "01.png" : "Off.png";

    list[item].button.loadFromFile(filename);
    list[item].button.setTkOrigin(TkImage::center);
    list[item].button.setPosition((windowSize.x >> 1) + data[item].second.x,
                                   windowSize.y + data[item].second.y);
  }
}

/**
 * @brief TkAction::handleMouseButtonEvent
 * @param event
 */
TkAction::TkItem TkAction::handleMouseButtonEvent(
    const sf::Event::MouseButtonEvent& event) {
  for (int item = 0; item < TkItem::size; ++item) {
    if (list[item].button.getGlobalBounds().contains(event.x, event.y) &&
        list[item].enabled) {
      selectedImage.setPosition(list[item].button.getPosition());
      selectedVisible = true;

      return static_cast<TkAction::TkItem>(item);
    }
  }
  return TkItem::size;
}

/**
 * @brief TkAction::handleMouseMovedEvent
 * @param event
 */
TkAction::TkItem TkAction::handleMouseMovedEvent(
    const sf::Event::MouseMoveEvent& event) {

  for (int item = 0; item < TkItem::size; ++item) {
    if (list[item].button.getGlobalBounds().contains(event.x, event.y) && list[item].enabled) {
      hoveredImage.setPosition(list[item].button.getPosition());
      hoveredVisible = true;
      return static_cast<TkAction::TkItem>(item);
    }
  }
  hoveredVisible = false;
  return TkItem::size;
}
