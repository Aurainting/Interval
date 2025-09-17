#pragma once

#include "icl/type_traits/is_element_container.hpp"
#include <type_traits>

namespace icl {

template <typename Type> struct is_associative_element_container {
  using type = is_associative_element_container;
  static constexpr bool value =
      std::disjunction_v<is_element_set<Type>, is_element_map<Type>>;
};

} // namespace icl
