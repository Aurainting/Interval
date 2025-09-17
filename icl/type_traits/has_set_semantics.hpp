#pragma once

#include "icl/type_traits/codomain_type_of.hpp"
#include "icl/type_traits/is_map.hpp"
#include "icl/type_traits/is_set.hpp"

namespace icl {

template <typename Type> struct has_set_semantics {
  using type = has_set_semantics;
  static constexpr bool value =
      is_set<Type>::value ||
      (is_map<Type>::value &&
       has_set_semantics<typename codomain_type_of<Type>::type>::value);
};

} // namespace icl
