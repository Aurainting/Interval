#pragma once

#include "icl/type_traits/no_type.hpp"

namespace icl {

template <typename Type>
concept has_segment_type = requires { typename Type::segment_type; };

template <typename> struct segment_type_of {
  using type = no_type;
};

template <has_segment_type Type> struct segment_type_of<Type> {
  using type = Type::segment_type;
};

} // namespace icl
