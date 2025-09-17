#pragma once

#include "icl/type_traits/is_numeric.hpp"
#include "icl/type_traits/no_type.hpp"
#include "icl/type_traits/rep_type_of.hpp"
#include <type_traits>

namespace icl {

//--------------------------------------------------------------------------
template <typename Type>
concept has_difference_type = requires { typename Type::difference_type; };

//--------------------------------------------------------------------------
template <typename Type> // type_of(T-T)==T
struct is_subtraction_closed {
  using type = is_subtraction_closed;
  static constexpr bool value =
      is_numeric<Type>::value ||
      (has_rep_type<Type> && !has_difference_type<Type>);
};

//--------------------------------------------------------------------------
template <typename Type> struct has_difference {
  using type = has_difference;
  static constexpr bool value = is_subtraction_closed<Type>::value ||
                                std::is_pointer_v<Type> ||
                                has_difference_type<Type>;
};

//--------------------------------------------------------------------------
template <typename> struct difference_type_of {
  using type = no_type;
};

template <typename Type>
  requires(std::is_pointer_v<Type> && has_difference<Type>::value &&
           !has_difference_type<Type>)
struct difference_type_of<Type> {
  using type = std::ptrdiff_t;
};

template <typename Type>
  requires(!std::is_pointer_v<Type> && has_difference<Type>::value &&
           !has_difference_type<Type>)
struct difference_type_of<Type> {
  using type = Type;
};

template <typename Type>
  requires(!std::is_pointer_v<Type> && has_difference<Type>::value &&
           has_difference_type<Type>)
struct difference_type_of<Type> {
  using type = Type::difference_type;
};

} // namespace icl
