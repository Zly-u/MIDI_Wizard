#pragma once

#ifdef _DEBUG
#include <cstdio>
#endif

template<class Base>
class Singleton {
    friend Base;

protected:
    Singleton() = default;


public:
    Singleton(const Singleton&) = delete;
    Singleton(const Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;

    static Base& Get() {
        static Base s_instance;
        return s_instance;
    }
};

namespace debug {
	inline void printf(const char* format...) {
		#ifdef _DEBUG
		::printf(format);  // NOLINT(clang-diagnostic-format-security)
		#endif
	}
}