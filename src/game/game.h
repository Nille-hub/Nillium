#pragma once

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

  void newGame(int episode, Skill skill);

private:
  GameMode m_mode = GameMode::Title;
  int m_episode = 1;
  Skill m_skill = Skill::Medium;
};
