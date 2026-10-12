#include "app/app.h"

#include <cstdio>
#include <exception>
#include <print>

int main() {
  try {
    App app;
    app.run();
  } catch (const std::exception &e) {
    std::println(stderr, "Nillium: {}", e.what());
    return 1;
  }

  return 0;
}
