#pragma once

#include "icl/detail/set_algo.hpp"

namespace icl {

//==============================================================================
//= Addition<ElementSet>
//==============================================================================
/** \c add inserts \c operand into the map if it's key does
    not exist in the map.
    If \c operands's key value exists in the map, it's data
    value is added to the data value already found in the map. */
template <typename Type>
  requires is_element_set<Type>::value
Type& add(Type& object, const typename Type::value_type& operand) {
  object.insert(operand);
  return object;
}

/** \c add \c operand into the map using \c prior as a hint to
    insert \c operand after the position \c prior is pointing to. */
template <typename Type>
  requires is_element_set<Type>::value
Type::iterator add(Type& object, typename Type::iterator prior,
                   const typename Type::value_type& operand) {
  return object.insert(prior, operand);
}

//==============================================================================
//= Subtraction
//==============================================================================
/** If the \c operand's key value is in the map, it's data value is
    subtracted from the data value stored in the map. */
template <typename Type>
  requires is_element_set<Type>::value
Type& subtract(Type& object, const typename Type::value_type& operand) {
  object.erase(operand);
  return object;
}

//==============================================================================
//= Intersection
//==============================================================================
template <typename Type>
  requires is_element_set<Type>::value
bool intersects(const Type& object, const typename Type::key_type& operand) {
  return !(object.find(operand) == object.end());
}

template <typename Type>
  requires is_element_set<Type>::value
bool intersects(const Type& object, const Type& operand) {
  if (iterative_size(object) < iterative_size(operand))
    return Set::intersects(object, operand);
  return Set::intersects(operand, object);
}

//==============================================================================
//= Symmetric difference
//==============================================================================
template <typename Type>
  requires is_element_set<Type>::value
Type& flip(Type& object, const typename Type::value_type& operand) {
  using iterator = Type::iterator;
  if (std::pair<iterator, bool> insertion = object.insert(operand);
      !insertion.second)
    object.erase(insertion.first);

  return object;
}

template <typename Type>
  requires is_element_set<Type>::value
Type& operator^=(Type& object, const typename Type::element_type& operand) {
  return icl::flip(object, operand);
}

/** Symmetric subtract map \c x2 and \c *this.
    So \c *this becomes the symmetric difference of \c *this and \c x2 */
template <typename Type>
  requires is_element_set<Type>::value
Type& operator^=(Type& object, const Type& operand) {
  using const_iterator = Type::const_iterator;
  const_iterator it_ = operand.begin();
  while (it_ != operand.end())
    icl::flip(object, *it_++);

  return object;
}

//==============================================================================
//= Streaming<ElementSet>
//==============================================================================
template <typename CharType, typename CharTraits, typename Type>
  requires is_element_set<Type>::value
std::basic_ostream<CharType, CharTraits>&
operator<<(std::basic_ostream<CharType, CharTraits>& stream,
           const Type& object) {
  stream << "{";
  for (typename Type::const_iterator it = object.begin(); !(it == object.end());
       ++it)
    stream << *it << " ";

  return stream << "}";
}

} // namespace icl
