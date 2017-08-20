#include <codex/conv/base64.hpp>
#include <map>

namespace codex {
namespace detail{
  /*!
  @brief encode table
  */
	std::string b64enc_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  /*!
  @brief decode table
  */
  std::map<char, int> b64dec_map;
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
      for (int i = 0; i < detail::b64enc_map.length(); ++i) {
        detail::b64dec_map[detail::b64enc_map[i]] = i;
      }
    });
    std::vector< uint8_t > result;
    uint32_t bits = 0;
    int bit_remain = 0;
    auto b = src.begin();
    auto e = src.end();
    while (b != e) {
      if (*b == '=') break;
      bits = (bits << 6) | detail::b64dec_map[static_cast<int>(*b)];
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
