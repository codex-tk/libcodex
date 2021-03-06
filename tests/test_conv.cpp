#include "gtest/gtest.h"
#include <codex/conv/base64.hpp>
#include <codex/conv/checksum.hpp>

#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

std::string decode64(const std::string &val) {
  using namespace boost::archive::iterators;
  using It = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
  return boost::algorithm::trim_right_copy_if(std::string(It(std::begin(val)), It(std::end(val))), [](char c) {
    return c == '\0';
  });
}

std::string encode64(const std::string &val) {
  using namespace boost::archive::iterators;
  using It = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
  auto tmp = std::string(It(std::begin(val)), It(std::end(val)));
  return tmp.append((3 - val.size() % 3) % 3, '=');
}

class conv_b64_test : public testing::Test {
public:
  void SetUp() override {
    src = std::string("base64test");
    enc = std::string("YmFzZTY0dGVzdA==");
    src0 = std::string("Man is distinguished, not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.");
    enc0 = std::string("TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0\
aGlzIHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1\
c3Qgb2YgdGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0\
aGUgY29udGludWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdl\
LCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=");

  }
  void TearDown() override {
  }
protected:
  std::string src;
  std::string enc;
  std::string src0;
  std::string enc0;
};

TEST_F(conv_b64_test, boost) {
  ASSERT_STREQ(enc.c_str(), encode64(src).c_str());
  ASSERT_STREQ(enc0.c_str(), encode64(src0).c_str());
}

TEST_F(conv_b64_test, enc) {
	std::vector<uint8_t> src_vec;
	std::copy(src.begin(), src.end(), std::back_inserter(src_vec));
	ASSERT_STREQ(enc.c_str(), codex::base64::encode(src_vec).c_str());

  src_vec.clear();
  std::copy(src0.begin(), src0.end(), std::back_inserter(src_vec));
  ASSERT_STREQ(enc0.c_str(), codex::base64::encode(src_vec).c_str());
}

TEST_F(conv_b64_test, dec ) {
  std::vector<uint8_t> res = codex::base64::decode(enc);
  std::string dec;
  std::copy(res.begin(), res.end(), std::back_inserter(dec));
  ASSERT_STREQ(src.c_str(), dec.c_str());

  res = codex::base64::decode(enc0);
  dec.clear();
  std::copy(res.begin(), res.end(), std::back_inserter(dec));
  ASSERT_STREQ(src0.c_str(), dec.c_str());
}

TEST(conv, checksum) {
  {
    codex::checksum<uint16_t> sum;
    uint16_t val = 0;
    for (uint16_t i = 0x1000; i < 0x1111; ++i) {
      sum.update(i);
      val += i;
    }
    uint16_t chksum = sum.final();
    val += chksum;
    ASSERT_EQ(val, 0);
  }
  {
    codex::checksum<char> sum;
    char val = 0;
    for (char i = 0; i < 0x7f; ++i) {
      sum.update(i);
      val += i;
    }
    char chksum = sum.final();
    val += chksum;
    ASSERT_EQ(val, 0);
  }
}
