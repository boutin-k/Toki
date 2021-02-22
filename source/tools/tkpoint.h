#ifndef TKPOINT_H
#define TKPOINT_H

#include <sys/types.h>
#include "SFML/System/Vector2.hpp"

class TkPoint : public sf::Vector2<ushort>
{
 public:
  TkPoint(void);
  TkPoint(ushort x, ushort y);

  inline uint hash(void) { return hash(x, y); }

  static uint hash(const sf::Vector2<ushort>& v) { return hash(v.x, v.y); }
  static uint hash(ushort x, ushort y) { return (uint)(x << sizeof(short) * 8) + y; }
};

#endif // TKPOINT_H
