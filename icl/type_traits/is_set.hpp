#pragma once

#include "icl/type_traits/is_container.hpp"

namespace icl {

template <typename Type> struct is_set {
  using type = is_set;
  static constexpr bool value = is_std_set<Type>::value;
};

} // namespace icl
