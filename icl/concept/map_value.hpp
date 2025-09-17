#pragma once

#include "icl/type_traits/identity_element.hpp"
#include "icl/type_traits/is_map.hpp"
#include "icl/type_traits/predicate.hpp"

namespace icl {

//==============================================================================
//= AlgoUnifiers<Map>
//==============================================================================
template <typename Type, typename Iterator>
  requires is_map<Type>::value
const Type::key_type& key_value(Iterator it_) {
  return (*it_).first;
}

template <typename Type, typename Iterator>
  requires is_map<Type>::value
const Type::codomain_type& co_value(Iterator it_) {
  return (*it_).second;
}

template <typename Type>
  requires is_map<Type>::value
Type::value_type make_value(const typename Type::key_type& key_val,
                            const typename Type::codomain_type& co_val) {
  return typename Type::value_type(key_val, co_val);
}

template <typename Type>
class content_is_identity_element : public property<Type> {
public:
  bool operator()(const Type& value_pair) const {
    return value_pair.second ==
           identity_element<typename Type::second_type>::value();
  }
};

} // namespace icl
