#pragma once

#include "icl/type_traits/is_interval_container.hpp"

namespace icl {
namespace segmental {

template <typename Type>
  requires is_interval_set<Type>::value
bool is_joinable(typename Type::iterator it_, typename Type::iterator next_,
                 Type* = nullptr) {
  return touches(*it_, *next_);
}

template <typename Type>
  requires is_interval_map<Type>::value
bool is_joinable(typename Type::iterator it_, typename Type::iterator next_,
                 Type* = nullptr) {
  return touches((*it_).first, (*next_).first) &&
         (*it_).second == (*next_).second;
}

} // namespace segmental
} // namespace icl
