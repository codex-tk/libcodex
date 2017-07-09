#include <codex/conv/base64.hpp>

namespace codex {
namespace detail{
  /*!
  @brief encode table
  */
	std::string b64enc_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  /*!
  @brief decode table
  */
  char b64dec_map[256] = { 0 };
}
	std::string base64::encode(const std::vector< uint8_t >& src) {
		std::string result;
		uint32_t bits = 0;
		int bit_remain = 0;
		auto i = src.begin();
		auto e = src.end();
		while (i != e) {
			char data = *i;
			bits = (bits << 8) | data;
			bit_remain += 8;
			while (bit_remain > 6) {
				bit_remain -= 6;
				result.push_back(
					detail::b64enc_map[(bits >> bit_remain) & 0x3f]
				);
			}
			++i;
		}
		if (bit_remain != 0) {
			result.push_back(
				detail::b64enc_map[bits << (6 - bit_remain) & 0x3f]
			);
		}
		int pad = src.size() % 3;
		if (pad != 0 ) {
      pad = 3 - pad;
			while (pad-- != 0) {
				result.push_back('=');
			}
		}
		return result;
	}

	std::vector< uint8_t > base64::decode(const std::string& src) {
    static std::once_flag dec_map_init_flag;
    // reverse table generate
    std::call_once(dec_map_init_flag, []() {
      int upper_cnt = 'Z' - 'A' + 1;
      int low_cnt = 'z' - 'a' + 1;
      int num_cnt = '9' - '0' + 1;
      for (int i = 0; i < 256; ++i) {
        if (i >= 'A' && i <= 'Z') {
          detail::b64dec_map[i] = static_cast<char>( i - 'A' );
        }
        else if (i >= 'a' && i <= 'z') {
          detail::b64dec_map[i] = static_cast<char>(i - 'a' + upper_cnt);
        }
        else if (i >= '0' && i <= '9') {
          detail::b64dec_map[i] = static_cast<char>(i - '0' + upper_cnt + low_cnt);
        }
        else if (i == '+') {
          detail::b64dec_map[i] = static_cast<char>(upper_cnt + low_cnt + num_cnt);
        }
        else if (i == '/') {
          detail::b64dec_map[i] = static_cast<char>(upper_cnt + low_cnt + num_cnt + 1);
        }
        else {
          detail::b64dec_map[i] = 0;
        }
      }
    });
    std::vector< uint8_t > result;
    uint32_t bits = 0;
    int bit_remain = 0;
    auto b = src.begin();
    auto e = src.end();
    while (b != e) {
      if (*b == '=') break;
      bits = (bits << 6) | detail::b64dec_map[*b];
      bit_remain += 6;
      while (bit_remain >= 8) {
        bit_remain -= 8;
        result.push_back((bits >> bit_remain) & 0xff);
      }
      ++b;
    }
    return result;
	}
}