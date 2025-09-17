#pragma once

#include <complex>
#include <functional>
#include <limits>
#include <type_traits>

namespace icl {

template <typename Type> struct is_fixed_numeric {
  using type = is_fixed_numeric;
  static constexpr bool value = 0 < std::numeric_limits<Type>::digits;
};

template <typename Type> struct is_std_numeric {
  using type = is_std_numeric;
  static constexpr bool value = std::numeric_limits<Type>::is_specialized;
};

template <typename Type> struct is_std_integral {
  using type = is_std_integral;
  static constexpr bool value = std::numeric_limits<Type>::is_integer;
};

template <typename Type> struct is_numeric {
  using type = is_numeric;
  static constexpr bool value = is_std_numeric<Type>::value ||
                                std::is_integral_v<Type> ||
                                is_std_integral<Type>::value;
};

template <typename Type> struct is_numeric<std::complex<Type>> {
  using type = is_numeric;
  static constexpr bool value = true;
};

//--------------------------------------------------------------------------
template <typename Type, typename, bool = false> struct numeric_minimum {
  static bool is_less_than(Type) { return true; }
  static bool is_less_than_or(Type, bool) { return true; }
};

template <typename Type> struct numeric_minimum<Type, std::less<Type>, true> {
  static bool is_less_than(Type value) {
    return std::less<Type>()((std::numeric_limits<Type>::min)(), value);
  }

  static bool is_less_than_or(Type value, bool cond) {
    return cond || is_less_than(value);
  }
};

template <typename Type>
struct numeric_minimum<Type, std::greater<Type>, true> {
  static bool is_less_than(Type value) {
    return std::greater<Type>()((std::numeric_limits<Type>::max)(), value);
  }

  static bool is_less_than_or(Type value, bool cond) {
    return cond || is_less_than(value);
  }
};

} // namespace icl
