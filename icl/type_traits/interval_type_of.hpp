#pragma once

#include "icl/type_traits/no_type.hpp"

namespace icl {

template <typename Type>
concept has_interval_type = requires { typename Type::interval_type; };

template <typename> struct interval_type_of {
  using type = no_type;
};

template <has_interval_type Type> struct interval_type_of<Type> {
  using type = Type::interval_type;
};

} // namespace icl
