#pragma once

#include "icl/type_traits/identity_element.hpp"
#include "icl/type_traits/succ_pred.hpp"
#include <string>

namespace icl {

template <typename Type> struct unit_element {
  static Type value();
};

template <> inline bool unit_element<bool>::value() { return true; }
template <> inline float unit_element<float>::value() { return 1.0; }
template <> inline double unit_element<double>::value() { return 1.0; }
template <> inline long double unit_element<long double>::value() {
  return 1.0;
}

// Smallest 'visible' string that is greater than the empty string.
template <> inline std::string unit_element<std::string>::value() {
  return {" "};
}

template <typename Type> Type unit_element<Type>::value() {
  return icl::succ(identity_element<Type>::value());
}

} // namespace icl
