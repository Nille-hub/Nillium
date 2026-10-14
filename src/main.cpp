#include "app/app.h"

#include <cstdio>
#include <exception>
#include <filesystem>
#include <optional>
#include <print>
#include <string_view>

namespace {

std::optional<std::filesystem::path> iwadArgument(int argc, char* argv[]) {
  for (int i = 1; i + 1 < argc; ++i) {
    if (std::string_view(argv[i]) == "-iwad") {
      return std::filesystem::path(argv[i + 1]);
    }
  }
  return std::nullopt;
}

} // namespace

int main(int argc, char* argv[]) {
  const std::optional<std::filesystem::path> iwad = iwadArgument(argc, argv);
  if (!iwad) {
    std::println(stderr, "Usage: Nillium -iwad <path to IWAD>");
    return 1;
  }

  try {
    App app(*iwad);
    app.run();
  } catch (const std::exception& e) {
    std::println(stderr, "Nillium: {}", e.what());
    return 1;
  }

  return 0;
}
