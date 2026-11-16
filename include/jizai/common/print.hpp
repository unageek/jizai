#pragma once

#include <atomic>
#include <format>
#include <iostream>
#include <print>
#include <string>
#include <string_view>
#include <utility>

namespace jizai::common {

// Programs embedding jizai can replace the writers. The Python bindings write through sys.stdout
// and sys.stderr, so that the output appears wherever print() output does; e.g., Jupyter on Windows
// does not capture output written to the process's stdout.
using Writer = void (*)(std::string_view text);

inline void write_to_cout(std::string_view text) {
  std::print(std::cout, "{}", text);
  // Show progress promptly even when stdout is a pipe or a file.
  std::cout.flush();
}

inline void write_to_cerr(std::string_view text) { std::print(std::cerr, "{}", text); }

inline std::atomic<Writer> stdout_writer{write_to_cout};
inline std::atomic<Writer> stderr_writer{write_to_cerr};

template <class... Args>
void print(std::format_string<Args...> fmt, Args&&... args) {
  stdout_writer.load()(std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
void println(std::format_string<Args...> fmt, Args&&... args) {
  auto text = std::format(fmt, std::forward<Args>(args)...);
  text += '\n';
  stdout_writer.load()(text);
}

template <class... Args>
void eprint(std::format_string<Args...> fmt, Args&&... args) {
  stderr_writer.load()(std::format(fmt, std::forward<Args>(args)...));
}

template <class... Args>
void eprintln(std::format_string<Args...> fmt, Args&&... args) {
  auto text = std::format(fmt, std::forward<Args>(args)...);
  text += '\n';
  stderr_writer.load()(text);
}

}  // namespace jizai::common
