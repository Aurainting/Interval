#pragma once

#include "icl/type_traits/is_container.hpp"
#include "icl/type_traits/is_interval_container.hpp"
#include "icl/type_traits/is_set.hpp"
#include <type_traits>

namespace icl {

template <typename Type> struct is_element_map {
  using type = is_element_map;

  static constexpr bool value =
      std::conjunction_v<is_map<Type>,
                         std::negation<is_interval_container<Type>>>;
};

template <typename Type> struct is_element_set {
  using type = is_element_set;
  static constexpr bool value = std::disjunction_v<
      std::conjunction<is_set<Type>,
                       std::negation<is_interval_container<Type>>>,
      is_std_set<Type>>;
};

template <typename Type> struct is_element_container {
  using type = is_element_container;
  static constexpr bool value =
      std::disjunction_v<is_element_set<Type>, is_element_map<Type>>;
};
} // namespace icl
