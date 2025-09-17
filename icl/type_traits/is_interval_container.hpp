#pragma once

#include "icl/type_traits/is_map.hpp"
#include <type_traits>

namespace icl {

template <typename> struct is_interval_container {
  using type = is_interval_container;
  static constexpr bool value = false;
};

template <typename Type> struct is_interval_map {
  using type = is_interval_map;
  static constexpr bool value =
      std::conjunction_v<is_interval_container<Type>, is_map<Type>>;
};

template <typename Type> struct is_interval_set {
  using type = is_interval_set;
  static constexpr bool value =
      std::conjunction_v<is_interval_container<Type>,
                         std::negation<is_interval_map<Type>>>;
};

} // namespace icl
