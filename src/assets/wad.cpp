#include "assets/wad.h"
#include "assets/bytes.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <utility>

namespace {
constexpr std::size_t headerSize = 12;
constexpr std::size_t entrySize = 16;
constexpr std::size_t nameSize = 8;

std::vector<std::byte> readFile(const std::filesystem::path& path) {
  std::ifstream file(path, std::ios::binary);

  if (!file) {
    throw std::runtime_error("cannot open " + path.string());
  }

  std::vector<std::byte> data(std::filesystem::file_size(path));
  file.read(reinterpret_cast<char*>(data.data()), static_cast<std::streamsize>(data.size()));

  if (!file) {
    throw std::runtime_error("cannot read " + path.string());
  }

  return data;
}

std::string toUpper(std::string_view text) {
  std::string result(text);

  for (char& c : result) {
    c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  }

  return result;
}

std::string readName(std::span<const std::byte> bytes, std::size_t offset) {
  const auto* first = reinterpret_cast<const char*>(bytes.data() + offset);
  const std::string_view raw(first, nameSize);

  return toUpper(raw.substr(0, raw.find('\0')));
}
} // namespace

Wad::Wad(const std::filesystem::path& path) : m_data(readFile(path)) {
  const std::span<const std::byte> bytes(m_data);
  const std::string file = path.string();

  if (bytes.size() < headerSize) {
    throw std::runtime_error(file + ": too small to be a WAD");
  }

  const std::string_view id(reinterpret_cast<const char*>(bytes.data()), 4);
  if (id != "IWAD" && id != "PWAD") {
    throw std::runtime_error(file + ": not a WAD file");
  }

  const std::int32_t lumpCount = readInt32(bytes, 4);
  const std::int32_t directoryOffset = readInt32(bytes, 8);
  if (lumpCount < 0 || directoryOffset < 0 ||
      static_cast<std::size_t>(directoryOffset) + static_cast<std::size_t>(lumpCount) * entrySize > bytes.size()) {
    throw std::runtime_error(file + ": directory lies outside the file");
  }

  m_directory.reserve(static_cast<std::size_t>(lumpCount));
  for (std::size_t i = 0; i < static_cast<std::size_t>(lumpCount); ++i) {
    const std::size_t at = static_cast<std::size_t>(directoryOffset) + i * entrySize;
    const std::int32_t offset = readInt32(bytes, at);
    const std::int32_t size = readInt32(bytes, at + 4);
    std::string name = readName(bytes, at + 8);

    if (offset < 0 || size < 0 ||
        static_cast<std::size_t>(offset) + static_cast<std::size_t>(size) > bytes.size()) {
      throw std::runtime_error(file + ": lump " + name + " lies outside the file");
    }

    m_directory.push_back({std::move(name), static_cast<std::size_t>(offset), static_cast<std::size_t>(size)});
  }
}

bool Wad::contains(std::string_view name) const {
  const std::string wanted = toUpper(name);
  return std::any_of(m_directory.begin(), m_directory.end(),
                    [&](const Entry& entry){ return entry.name == wanted; });
}

std::span<const std::byte> Wad::lump(std::string_view name) const {
  const std::string wanted = toUpper(name);
  const auto found = std::find_if(m_directory.rbegin(), m_directory.rend(),
                                  [&](const Entry& entry) { return entry.name == wanted; });
  if (found == m_directory.rend()) {
    throw std::runtime_error("lump " + wanted + " not found");
  }
  return std::span<const std::byte>(m_data).subspan(found->offset, found->size);
}
