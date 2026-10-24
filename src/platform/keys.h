#pragma once

enum class Key {
  Up,
  Down,
  Enter,
  Escape,
  Backspace,
  Other
};

struct KeyEvent {
  Key key;
};
