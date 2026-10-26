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

  bool contains(std::string_view name) const;
  std::span<const std::byte> lump(std::string_view name) const;

  // Directory position of the last lump with this name. Throws if there is none.
  std::size_t index(std::string_view name) const;
  // Name and contents of the lump at a directory position. Throw if out of range.
  std::string_view nameAt(std::size_t position) const;
  // Not an overload of lump(): lump(0) would be ambiguous, because 0 is also a
  // null pointer and std::string_view can be constructed from const char*.
  std::span<const std::byte> lumpAt(std::size_t position) const;

private:
  struct Entry {
    std::string name;
    std::size_t offset = 0;
    std::size_t size = 0;
  };

  std::vector<std::byte> m_data;
  std::vector<Entry> m_directory;
};
