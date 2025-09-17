#pragma once

#include "icl/type_traits/codomain_type_of.hpp"
#include "icl/type_traits/is_set.hpp"

namespace icl {

//==============================================================================
//= AlgoUnifiers<Set>
//==============================================================================
template <typename Type, typename Iterator>
  requires is_set<Type>::value
const Type::key_type& key_value(Iterator it_) {
  return *it_;
}

template <typename Type>
  requires is_set<Type>::value
Type::value_type make_value(const typename Type::key_type& key_val,
                            const typename codomain_type_of<Type>::type&) {
  return typename Type::value_type(key_val);
}

} // namespace icl
