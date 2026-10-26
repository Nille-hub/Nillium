#include "game/game.h"

#include <format>
#include <string>
#include <utility>

GameMode Game::mode() const {
  return m_mode;
}

const MapData& Game::map() const {
  return m_map;
}

void Game::newGame(int episode, Skill skill, MapData map) {
  m_episode = episode;
  m_skill = skill;
  m_mode = GameMode::Level;
  m_map = std::move(map);
}

std::string firstMapName(GameVariant variant, int episode) {
  if (variant == GameVariant::Doom2) {
    return "MAP01";
  }

  return std::format("E{}M1", episode);
}
