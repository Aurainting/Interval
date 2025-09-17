#pragma once

#include "icl/type_traits/is_discrete.hpp"
#include <type_traits>

namespace icl {

template <typename Type> struct is_continuous {
  using type = is_continuous;
  static constexpr bool value = std::negation_v<is_discrete<Type>>;
};

} // namespace icl
