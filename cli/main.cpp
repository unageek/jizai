#include <algorithm>
#include <boost/program_options.hpp>
#include <exception>
#include <format>
#include <jizai/common/print.hpp>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "commands.hpp"

int main(int argc, const char* argv[]) {
  try {
    namespace po = boost::program_options;

    GlobalOptions opts;

    po::options_description opts_desc("Global options", 80, 50);
    opts_desc.add_options()  //
        ("help,h", po::bool_switch(&opts.help),
         "Display this help");  //

    auto parsed = po::command_line_parser(argc, argv).options(opts_desc).allow_unregistered().run();

    po::variables_map vm;
    po::store(parsed, vm);
    po::notify(vm);

    auto args = po::collect_unrecognized(parsed.options, po::include_positional);

    std::vector<CommandPtr> commands;
    commands.push_back(make_create_model_command());
    commands.push_back(make_cross_validate_command());
    commands.push_back(make_estimate_normals_command());
    commands.push_back(make_evaluate_command());
    commands.push_back(make_extract_model_command());
    commands.push_back(make_fit_command());
    commands.push_back(make_fit_model_to_variogram_command());
    commands.push_back(make_isosurface_command());
    commands.push_back(make_normals_to_sdf_command());
    commands.push_back(make_show_model_command());
    commands.push_back(make_show_variogram_command());
    commands.push_back(make_surface_25d_command());
    commands.push_back(make_unique_command());
    commands.push_back(make_variogram_command());

    if (args.empty()) {
      std::ostringstream desc;
      desc << opts_desc;
      jizai::common::print("usage: jizai [OPTIONS] COMMAND [ARGS]\n{}\nCommands:\n", desc.str());
      for (const auto& command : commands) {
        jizai::common::println("  {:24}{}", command->name(), command->description());
      }
      return opts.help ? 0 : 1;
    }

    auto name = args.at(0);
    args.erase(args.begin());

    auto it = std::ranges::find(commands, name, &Command::name);
    if (it == commands.end()) {
      throw std::runtime_error(std::format("unknown command: '{}'", name));
    }

    (*it)->run(args, opts);

    return 0;
  } catch (const std::exception& e) {
    jizai::common::eprintln("error: {}", e.what());
    return 1;
  } catch (...) {
    jizai::common::eprintln("unknown error");
    return 1;
  }
}
