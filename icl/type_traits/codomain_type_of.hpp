#pragma once

#include "icl/type_traits/is_container.hpp"
#include "icl/type_traits/no_type.hpp"

namespace icl {

template <typename Type>
concept has_codomain_type = requires { typename Type::codomain_type; };

template <typename> struct codomain_type_of {
  using type = no_type;
};

template <has_codomain_type Type> struct codomain_type_of<Type> {
  using type = Type::codomain_type;
};

template <typename Type>
  requires(!has_codomain_type<Type> && is_std_set<Type>::value)
struct codomain_type_of<Type> {
  using type = Type::value_type;
};

} // namespace icl
