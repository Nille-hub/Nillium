#include "game/game.h"

GameMode Game::mode() const {
  return m_mode;
}

void Game::newGame(int episode, Skill skill) {
  m_episode = episode;
  m_skill = skill;
  m_mode = GameMode::Level;
}
