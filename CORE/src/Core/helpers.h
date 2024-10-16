#pragma once

#ifdef _DEBUG
#include <cstdio>
#endif

namespace debug {
	inline void printf(const char* format...) {
		#ifdef _DEBUG
		::printf(format);  // NOLINT(clang-diagnostic-format-security)
		#endif
	}
};