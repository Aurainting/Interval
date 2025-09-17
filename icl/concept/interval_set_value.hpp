#pragma once

#include "icl/concept/interval.hpp"
#include "icl/type_traits/is_interval_container.hpp"

namespace icl {

//==============================================================================
//= AlgoUnifiers<Set>
//==============================================================================
template <typename Type, typename Iterator>
  requires is_interval_set<Type>::value
Type::codomain_type co_value(Iterator value_) {
  using codomain_type = Type::codomain_type;
  return icl::is_empty(*value_) ? codomain_type() : (*value_).lower();
}

} // namespace icl
