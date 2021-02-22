#ifndef TKGESTURE_H
#define TKGESTURE_H

#include <sys/types.h>

namespace tk {

  enum gesture : uint {
    none  = 0,
    left  = 1 << 0,
    right = 1 << 1,
    up    = 1 << 2,
    down  = 1 << 3,
    back  = 1 << 4
  };

  enum class action : uint {
    none        = 0,
    run         = 1 << 0,
    fall        = 1 << 1,
    jumpUp      = 1 << 2,
    jumpDown    = 1 << 3,
    ladderUp    = 1 << 4,
    ladderDown  = 1 << 5
  };

}

#endif // TKGESTURE_H
