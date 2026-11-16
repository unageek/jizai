#pragma once

#include <boost/program_options/options_description.hpp>
#include <jizai/common/print.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

struct GlobalOptions {
  bool help{};
};

class Command {
 public:
  Command() = default;

  virtual ~Command() = default;

  Command(const Command&) = delete;
  Command(Command&&) = delete;
  Command& operator=(const Command&) = delete;
  Command& operator=(Command&&) = delete;

  virtual const std::string& description() const = 0;

  virtual const std::string& name() const = 0;

  virtual void run(const std::vector<std::string>& args,
                   const GlobalOptions& global_opts) const = 0;
};

using CommandPtr = std::unique_ptr<Command>;

inline void print_usage(const std::string& name,
                        const boost::program_options::options_description& opts_desc) {
  std::ostringstream desc;
  desc << opts_desc;
  jizai::common::print("usage: jizai {} [OPTIONS]\n{}", name, desc.str());
}
