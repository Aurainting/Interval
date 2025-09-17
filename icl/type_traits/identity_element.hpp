#pragma once

#include "icl/type_traits/type_to_string.hpp"

namespace icl {

template <typename Type> struct identity_element {
  static Type value();
  Type operator()() const { return value(); }
};

template <typename Type> Type identity_element<Type>::value() {
  static Type _value;
  return _value;
}

template <>
inline std::string unary_template_to_string<identity_element>::apply() {
  return "0";
}

} // namespace icl
