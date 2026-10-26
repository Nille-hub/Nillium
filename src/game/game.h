#pragma once

#include "assets/map.h"
#include "game/variant.h"

#include <string>

enum class GameMode {
  Title,
  Level,
};

enum class Skill {
  Baby,
  Easy,
  Medium,
  Hard,
  Nightmare,
};

class Game {
public:
  GameMode mode() const;
  const MapData& map() const;

  void newGame(int episode, Skill skill, MapData map);

private:
  GameMode m_mode = GameMode::Title;
  int m_episode = 1;
  Skill m_skill = Skill::Medium;
  MapData m_map;
};

// The first map of an episode: E<episode>M1 for Doom, MAP01 for Doom II.
std::string firstMapName(GameVariant variant, int episode);
