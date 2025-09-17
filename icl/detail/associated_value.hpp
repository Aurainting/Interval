#pragma once

#include "icl/type_traits/is_combinable.hpp"

namespace icl {

template <typename Type, typename CoType>
  requires(is_key_compare_equal<Type, CoType>::value && is_map<Type>::value &&
           is_map<CoType>::value)
bool co_equal(typename Type::const_iterator left_,
              typename CoType::const_iterator right_, const Type* = nullptr,
              const CoType* = nullptr) {
  return co_value<Type>(left_) == co_value<CoType>(right_);
}

template <typename Type, typename CoType>
  requires(is_key_compare_equal<Type, CoType>::value &&
           !(is_map<Type>::value && is_map<CoType>::value))
bool co_equal(typename Type::const_iterator, typename CoType::const_iterator,
              const Type* = nullptr, const CoType* = nullptr) {
  return true;
}

} // namespace icl
