#include "tkshoebox.h"
#include "pugixml.hpp"

TkShoebox::TkShoebox() {}

TkShoebox::~TkShoebox() {
  backShoeboxList.clear();
  frontShoeboxList.clear();
}

void TkShoebox::init(const pugi::xml_document& domDocument,
                     const sf::Vector2f& center) {
  pugi::xml_node backgroundNode =
      domDocument.child("shoebox").child("background");
  initShoebox(backgroundNode, center, backShoeboxList, 0.9f);

  pugi::xml_node foregroundNode =
      domDocument.child("shoebox").child("foreground");
  initShoebox(foregroundNode, center, frontShoeboxList, 1.1f);
}


void TkShoebox::initShoebox(
    const pugi::xml_node& node, const sf::Vector2f& center,
    std::list<std::pair<float, std::unique_ptr<TkImage>>>& list, float ratio) {

  for (auto item : node.children("plane")) {
    std::string path = item.attribute("texture").value();

    if (textures.find(path) == textures.end()) {
      auto texture = std::make_unique<sf::Texture>();
      texture->setSmooth(true);
      if (texture->loadFromFile(path))
        textures.insert({path, std::move(texture)});
    }
    auto plane = std::make_unique<TkImage>(*textures[path]);
    auto textureSize = plane->texture.getSize();

    plane->setTkOrigin(TkImage::center);
    plane->setPosition(
        item.attribute("x").as_int(0) + center.x,
        item.attribute("y").as_int(0) + center.y);
    plane->setScale(
        ratio * item.attribute("width").as_float(1.f) / (float)textureSize.x,
        ratio * item.attribute("height").as_float(1.f) / (float)textureSize.y);
    plane->setRotation(-item.attribute("rotation").as_int(0));

    float zRatio = (item.attribute("z").as_float() / 1000.f + 1.f) * 0.1f;
    list.push_back({zRatio, std::move(plane)});
  }
  // Sort the list
  list.sort([](const auto& a, const auto& b) { return a.first < b.first; });
}

void TkShoebox::move(const sf::Vector2f& offset) {
  for (const auto& item : backShoeboxList)
    item.second->move(offset * item.first);
  for (const auto& item : frontShoeboxList)
    item.second->move(offset * item.first);
}
