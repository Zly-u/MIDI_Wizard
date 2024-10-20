#pragma once

#include <utility>

namespace math {
    template<typename T>
    constexpr T Lerp(const T A, const T B, const double val) {
        return A * (1.f - val) + B * val;
    }

    template<typename T>
    constexpr float InvLerp(const T A, const T B, const T val) {
        return static_cast<float>(val - A) / static_cast<float>(B - A);
    }

    template<typename T>
    constexpr T MapRanges(std::pair<T, T> RangeA, std::pair<T, T> RangeB, const double val)
    {
        return math::Lerp(
            RangeB.first, RangeB.second,
            math::InvLerp(
                RangeA.first, RangeA.second,
                val
            )
        );
    }
}
