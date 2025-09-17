#pragma once

#include "icl/type_traits/no_type.hpp"

namespace icl {

//--------------------------------------------------------------------------
template <typename Type>
concept has_rep_type = requires { typename Type::rep; };

//--------------------------------------------------------------------------
template <typename> struct rep_type_of {
  using type = no_type;
};

template <has_rep_type Type> struct rep_type_of<Type> {
  using type = Type::rep;
};

} // namespace icl
