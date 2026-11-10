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

// Where the player's eyes are: map units, angle in radians (0 = east, counter-clockwise).
struct Viewpoint {
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
  float angle = 0.0f;
};

class Game {
public:
  GameMode mode() const;
  const MapData& map() const;
  const Viewpoint& viewpoint() const;

  void newGame(int episode, Skill skill, MapData map);
  bool automapOpen() const;
  void toggleAutomap();

  // Advances the running level by one tic. Does nothing on the title screen.
  void tic();

  // Tics since the current level started, like vanilla's leveltime.
  int levelTime() const;

private:
  GameMode m_mode = GameMode::Title;
  int m_episode = 1;
  int m_levelTime = 0;
  Skill m_skill = Skill::Medium;
  MapData m_map;
  Viewpoint m_viewpoint;
  bool m_automapOpen = false;
};

// The first map of an episode: E<episode>M1 for Doom, MAP01 for Doom II.
std::string firstMapName(GameVariant variant, int episode);
