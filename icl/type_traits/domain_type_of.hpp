#pragma once

#include "icl/type_traits/no_type.hpp"

namespace icl {

template <typename Type>
concept has_domain_type = requires { typename Type::domain_type; };

template <typename> struct domain_type_of {
  using type = no_type;
};

template <has_domain_type Type> struct domain_type_of<Type> {
  using type = Type::domain_type;
};

} // namespace icl
