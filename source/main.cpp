#include "game.h"

//#define CODING_GAME
#ifdef CODING_GAME
#include <unordered_set>
#include <iostream>
#include <chrono>
#include <string.h>
#include <stdio.h>

struct change {
  unsigned long coin2;
  unsigned long bill5;
  unsigned long bill10;
};

// Entrainement en C++
bool exists(int ints[], int size, int k) {
  std::unordered_set<int> hash(ints, ints + size, size);
  return (hash.find(k) != hash.end());
}

// Rendu de monnaie en C++
bool rendu(unsigned long cash, change& c) {
  if (cash < 2 || cash == 3) return false;

  static constexpr unsigned long tab[5]{0, 3, 1, 4, 2};
  c.coin2 = tab[cash % 5];
  c.bill5 = cash & 1;
  c.bill10 = (cash - (c.coin2 * 2 + c.bill5 * 5)) / 10;
  return true;
}

bool findSub(uint imgH, uint imgW, const char img[][401], uint subH,
                    uint subW, const char subImg[][401], uint* coord) {
  // Check exception
  if (imgH < subH || imgW < subH) return false;

  // Loop on the height
  for (uint h = 0U; h <= imgH - subH; ++h) {
    // Loop on the width
    for (uint w = 0U; w <= imgW - subW; ++w) {
      // Search the 1st line occurence
      const char* pch = strstr(*(img + h) + w, *subImg);
      if (pch != NULL) {
        w = pch - *(img + h);
        if (w + subW <= imgW) {
          bool found = true;
          // Check next lines
          for (uint nLine = 1U; nLine < subH; ++nLine) {
            if (strncmp(*(img + h + nLine) + w, *(subImg + nLine), subW) != 0) {
              found = false;
              break;
            }
          }
          // If found return the values
          if (found == true) {
            coord[0] = h;
            coord[1] = w;
            return true;
          }
        }
      } else
        break;
    }
  }

  return false;
}
#endif

int main() {
  Game game;
  game.run();
  return 0;
}
