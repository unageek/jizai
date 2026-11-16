#pragma once

#include <filesystem>
#include <string>

namespace jizai::common {

// On Windows, opening a file stream with a narrow string converts it from the active code page,
// so a path containing characters that the code page cannot represent cannot be opened.
// Constructing the path from std::u8string instead converts it from UTF-8 to UTF-16 on Windows
// (throwing std::system_error if it is not valid UTF-8) and leaves the bytes unchanged on other
// platforms.
inline std::filesystem::path path_from_utf8(const std::string& filename) {
  return {std::u8string(filename.begin(), filename.end())};
}

}  // namespace jizai::common
