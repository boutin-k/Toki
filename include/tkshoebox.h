#ifndef TKSHOEBOX_H
#define TKSHOEBOX_H

#include <list>
#include <memory>

#include "tkimage.h"

namespace pugi {
class xml_document;
class xml_node;
}

class TkShoebox
{
 public:
  TkShoebox();
  virtual ~TkShoebox();

  void init(const pugi::xml_document& domDocument,
            const sf::Vector2f& levelCenter);
  void move(const sf::Vector2f& offset);

  inline const auto& getBackList() {return backShoeboxList;}
  inline const auto& getFrontList() {return frontShoeboxList;}

 private:
  void initShoebox(const pugi::xml_node& node,
                   const sf::Vector2f& center,
                   std::list<std::pair<float, std::unique_ptr<TkImage>>>& list,
                   float ratio = 1.f);

  std::list<std::pair<float, std::unique_ptr<TkImage>>>     backShoeboxList;
  std::list<std::pair<float, std::unique_ptr<TkImage>>>     frontShoeboxList;
};

#endif // TKSHOEBOX_H
