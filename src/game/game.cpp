#include "game/game.h"
#include "game/mapgeometry.h"

#include <algorithm>
#include <cstdint>
#include <format>
#include <numbers>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

constexpr int playerOneStart = 1;
constexpr float viewHeight = 41.0f;

} // namespace

GameMode Game::mode() const {
  return m_mode;
}

const MapData& Game::map() const {
  return m_map;
}

const Viewpoint& Game::viewpoint() const {
  return m_viewpoint;
}

void Game::newGame(int episode, Skill skill, MapData map) {
  m_episode = episode;
  m_skill = skill;
  m_mode = GameMode::Level;
  m_map = std::move(map);

  const auto playerThing = std::find_if(m_map.things.begin(), m_map.things.end(),
    [](const MapThing& thing) { return thing.type == playerOneStart; });

  if (playerThing == m_map.things.end()) {
    throw std::runtime_error("map has no player 1 start");
  }

  const std::uint16_t startPositionSectorIx =
    sectorAt(m_map, {static_cast<float>(playerThing->x), static_cast<float>(playerThing->y)});
  m_viewpoint = {
    static_cast<float>(playerThing->x),
    static_cast<float>(playerThing->y),
    static_cast<float>(m_map.sectors[startPositionSectorIx].floorHeight) + viewHeight,
    static_cast<float>(playerThing->angle) * std::numbers::pi_v<float> / 180.0f
  };
}

std::string firstMapName(GameVariant variant, int episode) {
  if (variant == GameVariant::Doom2) {
    return "MAP01";
  }

  return std::format("E{}M1", episode);
}
