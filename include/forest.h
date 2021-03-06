#ifndef FOREST_H
#define FOREST_H

#include "tklevel.h"

#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

class Forest: public TkLevel
{
 public:
  Forest(const sf::Vector2u& windowSize);
  virtual ~Forest();

  void buildBoard() override;
  void update(const enum tk::gesture& gesture) override;
  void render(sf::RenderWindow& window) override;
  tk::action isMovable(const sf::Vector2f& origin, tk::gesture,
                       bool recursiveLocked = false) const override;

  bool bridgeBuilderHandler(const sf::Vector2f& origin,
                            const tk::gesture& gesture) override;
  void teleportHandler(const sf::Vector2f& origin) override;

 private:
  static constexpr auto bridgeBuilderPath =
      "textures/sprites/objects/BridgeBuilder_forest.png";
  static constexpr auto bridgeBuilderNoCanDoPath =
      "textures/sprites/objects/BridgeBuilder_nocando.png";

  TkAnimation bridgeBuildAnim;
  TkAnimation* bridgeBuildAnimPtr{nullptr};

  TkAnimation bridgeBuildNoAnim;
  TkAnimation* bridgeBuildNoAnimPtr{nullptr};

  // clang-format off
  sf::IntRect sprites[107]{
    // Line 01
    {  0,  0, 32, 32}, { 32,  0, 32, 32}, { 64,  0, 32, 32}, { 96,  0, 32, 32},
    {128,  0, 32, 32}, {160,  0, 32, 32}, {192,  0, 32, 32}, {224,  0, 32, 32},
    {256,  0, 32, 32}, {288,  0, 32, 32}, {320,  0, 32, 32}, {352,  0, 32, 32},
    {384,  0, 32, 32}, {416,  0, 32, 32}, {448,  0, 32, 32}, {480,  0, 32, 32},
    // Line 02
    {  0, 32, 32, 32}, { 32, 32, 32, 32}, { 64, 32, 32, 32}, { 96, 32, 32, 32},
    {128, 32, 32, 32}, {160, 32, 32, 32}, {192, 32, 32, 32}, {224, 32, 32, 32},
    {256, 32, 32, 32}, {288, 32, 32, 32}, {320, 32, 32, 32}, {352, 32, 32, 32},
    {384, 32, 32, 32}, {416, 32, 32, 32}, {448, 32, 32, 32}, {480, 32, 32, 32},
    // Line 03
    {  0, 64, 32, 32}, { 32, 64, 32, 32}, { 64, 64, 32, 32}, { 96, 64, 32, 32},
    {128, 64, 32, 32}, {160, 64, 32, 32}, {192, 64, 32, 32}, {224, 64, 32, 32},
    {256, 64, 32, 32}, {288, 64, 32, 32}, {320, 64, 32, 32}, {352, 64, 32, 32},
    {384, 64, 32, 32}, {416, 64, 32, 32}, {448, 64, 32, 32}, {480, 64, 32, 32},
    // Line 04
    {  0, 96, 32, 32}, { 32, 96, 32, 32}, { 64, 96, 32, 32}, { 96, 96, 32, 32},
    {128, 96, 32, 32}, {160, 96, 32, 32}, {192, 96, 32, 32}, {224, 96, 32, 32},
    {256, 96, 32, 32}, {288, 96, 32, 32}, {320, 96, 32, 32}, {352, 96, 32, 32},
    {384, 96, 32, 32}, {416, 96, 32, 32}, {448, 96, 32, 32}, {480, 96, 32, 32},
    // Line 05
    {  0,128, 32, 32}, { 32,128, 32, 32}, { 64,128, 32, 32}, { 96,128, 32, 32},
    {128,128, 32, 32}, {160,128, 32, 32}, {192,128, 32, 32}, {224,128, 32, 32},
    {256,128, 32, 32}, {288,128, 32, 32}, {320,128, 32, 32}, {352,128, 32, 32},
    {384,128, 32, 32}, {416,128, 32, 32}, {448,128, 32, 32}, {480,128, 32, 32},
    // Line 06
    {  0,160, 32, 32}, { 32,160, 32, 32}, { 64,160, 32, 32}, { 96,160, 32, 32},
    {128,160, 32, 32}, {160,160, 32, 32}, {192,160, 32, 32}, {224,160, 32, 32},
    {256,160, 32, 32}, {288,160, 32, 32}, {320,160, 32, 32}, {352,160, 32, 32},
    {384,160, 32, 32}, {416,160, 32, 32}, {448,160, 32, 32}, {480,160, 32, 32},
    // Line 07
    {  0,192, 32, 32}, { 32,192, 32, 32}, { 64,192, 32, 32}, { 96,192, 32, 32},
    {128,192, 32, 32}, {160,192, 32, 32}, {192,192, 32, 32}, {224,192, 32, 32},
    {256,192, 32, 32}, {288,192, 32, 32}, {320,192, 32, 32}
  };
  // clang-format on
};

#endif // FOREST_H
