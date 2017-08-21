#include <gtest/gtest.h>
#include <codex/vision/bitmap.hpp>

TEST(vision, bitmap) {
  codex::vision::image img = codex::vision::load_from("C:\\Users\\codex\\works\\libcodex\\tests.qt\\freedive.bmp");

  codex::vision::image base(320, 320, 1);
  base.reset(0);
  codex::vision::image img1(base);
  codex::vision::image img2(base);

  double incx = 255.0 / base.width();
  double incy = 255.0 / base.height();
  for (int r = 0; r < base.height(); ++r) {
    for (int c = 0; c < base.width(); ++c) {
      img1.at(c, r) = c * incx;
      img2.at(c, r) = r * incy;
    }
  }
  codex::vision::save_to(img1, "C:\\Users\\codex\\works\\libcodex\\tests.qt\\1.bmp");
  codex::vision::save_to(img2, "C:\\Users\\codex\\works\\libcodex\\tests.qt\\2.bmp");
}
