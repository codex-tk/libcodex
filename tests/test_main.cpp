#include <gtest/gtest.h>
#include <codex/codex.hpp>

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  int result = RUN_ALL_TESTS();

#if defined (__codex_win32__)
  // vs cmake integration
  getchar();
#endif
  return result;
}
