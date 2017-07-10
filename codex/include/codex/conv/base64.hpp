#ifndef __libcodex_base64_h__
#define __libcodex_base64_h__

#include <codex/codex.hpp>

namespace codex {
	
  /*!
  @brief base64 enc / dec
  */
	class base64 {
	public:
		static std::string encode(const std::vector<uint8_t>& src);
		static std::vector<uint8_t> decode(const std::string& src);
	};
}

#endif