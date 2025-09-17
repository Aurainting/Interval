#pragma once

#include "icl/type_traits/no_type.hpp"

namespace icl {

//--------------------------------------------------------------------------
template <typename Type>
concept has_element_type = requires { typename Type::element_type; };

template <typename> struct element_type_of {
  using type = no_type;
};

template <has_element_type Type> struct element_type_of<Type> {
  using type = Type::element_type;
};

//--------------------------------------------------------------------------
template <typename Type>
concept has_value_type = requires { typename Type::value_type; };

template <typename> struct value_type_of {
  using type = no_type;
};

template <has_value_type Type> struct value_type_of<Type> {
  using type = Type::value_type;
};

//--------------------------------------------------------------------------
template <typename Type>
concept has_key_type = requires { typename Type::key_type; };

template <typename> struct key_type_of {
  using type = no_type;
};

template <has_key_type Type> struct key_type_of<Type> {
  using type = Type::key_type;
};

} // namespace icl
