#pragma once

enum class Key {
  Up,
  Down,
  Enter,
  Escape,
  Backspace,
  Tab,
  Other
};

struct KeyEvent {
  Key key;
};
