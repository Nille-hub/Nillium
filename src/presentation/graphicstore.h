#pragma once

#include "assets/wad.h"
#include "presentation/texture.h"

#include <functional>
#include <map>
#include <span>
#include <string>
#include <string_view>

struct GraphicEntry {
  Texture texture;
  int width = 0;
  int height = 0;
  int leftOffset = 0;
  int topOffset = 0;
};

class GraphicStore {
public:
  GraphicStore(const Wad& wad, std::span<const std::string_view> names);

  const GraphicEntry& entry(std::string_view name) const;

private:
  std::map<std::string, GraphicEntry, std::less<>> m_entries;
};
