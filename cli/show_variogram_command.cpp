#include <boost/program_options.hpp>
#include <cmath>
#include <format>
#include <jizai/common/print.hpp>
#include <jizai/geostats.hpp>
#include <jizai/jizai.hpp>
#include <memory>
#include <numbers>
#include <stdexcept>
#include <string>
#include <vector>

#include "commands.hpp"

using jizai::Index;
using jizai::geostats::VariogramSet;

namespace common = jizai::common;

namespace {

class ShowVariogramCommand : public Command {
  static inline const std::string kDescription = "Show a variogram file";
  static inline const std::string kName = "show-variogram";

 public:
  const std::string& description() const override { return kDescription; }

  const std::string& name() const override { return kName; }

  void run(const std::vector<std::string>& args, const GlobalOptions& global_opts) const override {
    namespace po = boost::program_options;

    Options opts;

    po::options_description opts_desc("Options", 80, 50);
    opts_desc.add_options()  //
        ("in", po::value(&opts.in_file)->required()->value_name("FILE"),
         "Input variogram file")  //
        ("dim", po::value(&opts.dim)->required()->value_name("1|2|3"),
         "Dimension of input points")  //
        ("id", po::value(&opts.id)->default_value(-1, "NONE")->value_name("ID"),
         "ID of the variogram")  //
        ;

    if (global_opts.help) {
      print_usage(kName, opts_desc);
      return;
    }

    po::variables_map vm;
    try {
      po::store(po::command_line_parser{args}
                    .options(opts_desc)
                    .style(po::command_line_style::unix_style ^ po::command_line_style::allow_short)
                    .run(),
                vm);
      po::notify(vm);
    } catch (const po::error&) {
      print_usage(kName, opts_desc);
      throw;
    }

    switch (opts.dim) {
      case 1:
        run_impl<1>(opts);
        break;
      case 2:
        run_impl<2>(opts);
        break;
      case 3:
        run_impl<3>(opts);
        break;
      default:
        throw std::runtime_error(std::format("unsupported dimension: {}", opts.dim));
    }
  }

 private:
  static int format_precision(double x) {
    if (x == 0.0) {
      return 0;
    }

    return std::max(0, 5 - static_cast<int>(std::floor(std::log10(std::abs(x)))));
  }

  struct Options {
    std::string in_file;
    int dim{};
    int id{};
  };

  template <int Dim>
  static void run_impl(const Options& opts) {
    using VariogramSet = VariogramSet<Dim>;

    auto variog_set = VariogramSet::load(opts.in_file);
    auto deg = std::numbers::pi / 180.0;

    if (opts.id == -1) {
      common::println("  {:>4}  {:>10}  {:>10}  {:>10}", "id", "azimuth", "elevation", "num_pairs");
      for (Index i = 0; i < variog_set.num_variograms(); ++i) {
        const auto& v = variog_set.variograms().at(i);
        const auto& dir = v.direction();

        auto elev = std::asin(dir(2)) / deg;
        auto az = std::atan2(dir(0), dir(1)) / deg;
        if (elev < 0.0) {
          elev = -elev;
          az += 180.0;
        }
        if (az < 0.0) {
          az += 360.0;
        }

        common::println("  {:>4}  {:>10.3f}  {:>10.3f}  {:>10}", i, az, elev, v.num_pairs());
      }
    } else {
      const auto& v = variog_set.variograms().at(opts.id);
      auto max_distance = *std::ranges::max_element(v.bin_distance());
      auto max_gamma = *std::ranges::max_element(v.bin_gamma());
      auto distance_prec = format_precision(max_distance);
      auto gamma_prec = format_precision(max_gamma);

      common::println("  {:>10}  {:>10}  {:>10}", "distance", "gamma", "num_pairs");
      for (Index i = 0; i < v.num_bins(); ++i) {
        common::println("  {:>10.{}f}  {:>10.{}f}  {:>10}", v.bin_distance().at(i), distance_prec,
                        v.bin_gamma().at(i), gamma_prec, v.bin_num_pairs().at(i));
      }
    }
  }
};

}  // namespace

CommandPtr make_show_variogram_command() { return std::make_unique<ShowVariogramCommand>(); }
