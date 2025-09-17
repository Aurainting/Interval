#pragma once

#include <type_traits>

namespace icl {

template <typename Type> struct has_inverse {
  using type = has_inverse;
  static constexpr bool value =
      std::is_signed_v<Type> || std::is_floating_point_v<Type>;
};

} // namespace icl
