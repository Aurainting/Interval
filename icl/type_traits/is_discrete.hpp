#pragma once

#include "icl/type_traits/rep_type_of.hpp"
#include <iterator>
#include <type_traits>

namespace icl {

template <typename Type> struct is_discrete {
  using type = is_discrete;

  static constexpr bool value =
      std::incrementable<Type> &&
      ((!has_rep_type<Type> && !std::is_floating_point_v<Type>) ||
       (has_rep_type<Type> &&
        is_discrete<typename rep_type_of<Type>::type>::value));
};

} // namespace icl
