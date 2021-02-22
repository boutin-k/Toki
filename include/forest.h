#ifndef FOREST_H
#define FOREST_H

#include "tklevel.h"
#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Vector2.hpp"

#include <vector>

class Forest: public TkLevel
{
  struct mapData {
    uint width;
    uint height;
    std::vector<uint> board;
  };

 public:
  Forest(const sf::Vector2u& windowSize);

  void createLevel(void) override;
  sf::Vector2f move(const sf::Vector2f& offset) override;
  tk::action isMovable(const sf::Vector2f& origin, tk::gesture) override;

 private:
  mapData data;
  sf::Vector2u _windowSize;

  // clang-format off
  sf::IntRect sprites[59]{
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
    {256, 96, 32, 32}, {288, 96, 32, 32}, {320, 96, 32, 32}
  };
  // clang-format on
};

#endif // FOREST_H
