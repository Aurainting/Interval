#pragma once

#include "icl/type_traits/is_element_container.hpp"
#include "icl/type_traits/is_interval_container.hpp"
#include <type_traits>

namespace icl {

template <typename Type> struct is_icl_container {
  using type = is_icl_container;
  static constexpr bool value = std::disjunction_v<is_element_container<Type>,
                                                   is_interval_container<Type>>;
};

} // namespace icl
