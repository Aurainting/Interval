#pragma once

#include "icl/type_traits/is_container.hpp"
#include "icl/type_traits/is_icl_container.hpp"

namespace icl {

//==============================================================================
//= Emptiness
//==============================================================================

/** Tests if the container is empty.
    Complexity: constant. */
template <typename Type>
  requires is_container<Type>
bool is_empty(const Type& object) {
  return object.begin() == object.end();
}

/** All content of the container is dropped.
    Complexity: linear. */
template <typename Type>
  requires is_container<Type>
void clear(Type& object) {
  object.erase(object.begin(), object.end());
}

//==============================================================================
//= Size
//==============================================================================

template <typename Type>
  requires(is_container<Type> && !is_icl_container<Type>::value)
std::size_t iterative_size(const Type& object) {
  return object.size();
}

//==============================================================================
//= Swap
//==============================================================================

template <typename Type>
  requires is_container<Type>
void swap(Type& left, Type& right) noexcept {
  left.swap(right);
}

//==============================================================================
//= Iteration
//==============================================================================

template <typename Type>
  requires is_container<Type>
Type::iterator cyclic_prior(Type& object, typename Type::iterator it_) {
  return it_ == object.begin() ? object.end() : --it_;
}

template <typename Type>
  requires is_container<Type>
Type::const_iterator cyclic_prior(const Type& object,
                                  typename Type::const_iterator it_) {
  return it_ == object.begin() ? object.end() : --it_;
}

} // namespace icl
