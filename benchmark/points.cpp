#include <exception>
#include <jizai/common/print.hpp>
#include <jizai/geometry/sphere3d.hpp>
#include <jizai/jizai.hpp>
#include <jizai/point_cloud/random_points.hpp>
#include <string>

using jizai::kAll;
using jizai::write_table;
using jizai::geometry::Sphere3;
using jizai::point_cloud::DistanceFilter;
using jizai::point_cloud::random_points;

namespace common = jizai::common;

int main(int /*argc*/, char* argv[]) {
  try {
    auto n_points = std::stoi(argv[1]);
    auto seed = std::stoi(argv[2]);
    auto points = random_points(Sphere3(), n_points, seed);

    auto indices = DistanceFilter(points).filtered_indices(1e-6);
    points = points(indices, kAll).eval();

    write_table(argv[3], points);

    return 0;
  } catch (const std::exception& e) {
    common::eprintln("error: {}", e.what());
    return 1;
  } catch (...) {
    common::eprintln("unknown error");
    return 1;
  }
}
