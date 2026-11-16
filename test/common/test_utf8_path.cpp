#include <gtest/gtest.h>

#include <filesystem>
#include <jizai/common/io.hpp>
#include <jizai/isosurface/mesh.hpp>
#include <jizai/table.hpp>
#include <jizai/types.hpp>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

// Functions that take a filename as std::string must treat it as UTF-8 regardless of the system
// locale (on Windows, the active code page). No single legacy code page can represent Japanese,
// Korean, and Greek together.
class Utf8Path : public ::testing::Test {
 protected:
  void SetUp() override {
    dir_ = fs::temp_directory_path() / fs::path(u8"jizai_テスト_한글_Ω");
    fs::create_directories(dir_);
  }

  void TearDown() override { fs::remove_all(dir_); }

  std::string filename(const char8_t* name) const {
    auto s = (dir_ / fs::path(name)).u8string();
    return {s.begin(), s.end()};
  }

  // Checks that the file was created under its real name, not a mojibake one.
  static bool exists(const std::string& filename) {
    return fs::exists(fs::path(std::u8string(filename.begin(), filename.end())));
  }

 private:
  fs::path dir_;
};

}  // namespace

TEST_F(Utf8Path, table) {
  jizai::MatX table(2, 3);
  table << 1, 2, 3, 4, 5, 6;
  auto f = filename(u8"表_표_Ω.txt");

  jizai::write_table(f, table);

  EXPECT_TRUE(exists(f));
  EXPECT_EQ(table, jizai::read_table(f));
}

TEST_F(Utf8Path, binary) {
  std::vector<double> data{1.5, 2.5, 3.5};
  auto f = filename(u8"データ_데이터_Ω.bin");

  jizai::common::save(f, data);

  EXPECT_TRUE(exists(f));
  std::vector<double> loaded;
  jizai::common::load(f, loaded);
  EXPECT_EQ(data, loaded);
}

TEST_F(Utf8Path, export_obj) {
  auto f = filename(u8"メッシュ_메시_Ω.obj");

  jizai::isosurface::Mesh(jizai::isosurface::EntireTag{}).export_obj(f);

  EXPECT_TRUE(exists(f));
}
