#pragma once

#include "icl/type_traits/element_type_of.hpp"
#include "icl/type_traits/segment_type_of.hpp"
#include <type_traits>

namespace icl {

template <typename Type>
concept is_container = requires {
  typename Type::value_type;
  typename Type::iterator;
  typename Type::size_type;
  typename Type::reference;
};

template <typename Type> struct is_std_set {
  using type = is_std_set;

  static constexpr bool value =
      is_container<Type> && has_key_type<Type> &&
      std::is_same_v<key_type_of<Type>, value_type_of<Type>> &&
      !has_segment_type<Type>;
};

} // namespace icl
