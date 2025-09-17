#pragma once

#include "icl/type_traits/difference_type_of.hpp"

namespace icl {

//--------------------------------------------------------------------------
template <typename Type>
concept has_size_type = requires { typename Type::size_type; };

//--------------------------------------------------------------------------
template <typename> struct size_type_of {
  using type = no_type;
};

template <typename Type>
  requires(!has_size_type<Type> && !has_difference_type<Type> &&
           !has_rep_type<Type>)
struct size_type_of<Type> {
  using type = std::size_t;
};

template <typename Type>
  requires(has_size_type<Type>)
struct size_type_of<Type> {
  using type = Type::size_type;
};

template <typename Type>
  requires(!has_size_type<Type> && has_difference_type<Type>)
struct size_type_of<Type> {
  using type = Type::difference_type;
};

template <typename Type>
  requires(!has_size_type<Type> && !has_difference_type<Type> &&
           has_rep_type<Type>)
struct size_type_of<Type> {
  using type = Type;
};

} // namespace icl
