#pragma once

enum class Key {
  Up,
  Down,
  Enter,
  Escape,
  Other
};

struct KeyEvent {
  Key key;
};
