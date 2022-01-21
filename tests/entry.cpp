#include <spdlog/spdlog.h>
#include "gtest/gtest.h"

int main(int argc, char **argv) {
  spdlog::set_level(spdlog::level::trace);
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
