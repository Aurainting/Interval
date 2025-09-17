#pragma once

#include "icl/type_traits/is_icl_container.hpp"

namespace icl {

//==============================================================================
//= Equivalences and Orderings<Comparable>
//==============================================================================
template <typename Type>
  requires is_icl_container<Type>::value
bool operator!=(const Type& left, const Type& right) {
  return !(left == right);
}

template <typename Type>
  requires is_icl_container<Type>::value
bool operator>(const Type& left, const Type& right) {
  return right < left;
}

/** Partial ordering which is induced by Compare */
template <typename Type>
  requires is_icl_container<Type>::value
bool operator<=(const Type& left, const Type& right) {
  return !(left > right);
}

template <typename Type>
  requires is_icl_container<Type>::value
bool operator>=(const Type& left, const Type& right) {
  return !(left < right);
}

} // namespace icl
