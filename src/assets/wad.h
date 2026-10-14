#pragma once

#include <cstddef>
#include <filesystem>
#include <span>
#include <string>
#include <string_view>
#include <vector>

class Wad {
public:
  explicit Wad(const std::filesystem::path& path);

  Wad(const Wad&) = delete;
  Wad& operator=(const Wad&) = delete;

  std::span<const std::byte> lump(std::string_view) const;

private:
  struct Entry {
    std::string name;
    std::size_t offset = 0;
    std::size_t size = 0;
  };

  std::vector<std::byte> m_data;
  std::vector<Entry> m_directory;
};
