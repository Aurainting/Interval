#pragma once

#include "icl/concept/element_map.hpp"
#include "icl/concept/element_set.hpp"
#include "icl/detail/subset_comparer.hpp"
#include "icl/type_traits/is_associative_element_container.hpp"
#include "icl/type_traits/is_element_container.hpp"
#include "icl/type_traits/is_key_container_of.hpp"
#include <cstddef>

namespace icl {

//==============================================================================
//= Size
//==============================================================================
template <typename Type>
  requires is_element_container<Type>::value
std::size_t iterative_size(const Type& object) {
  return object.size();
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type::size_type size(const Type& object) {
  return icl::iterative_size(object);
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type::size_type cardinality(const Type& object) {
  return icl::iterative_size(object);
}

//==============================================================================
//= Containedness<ElementSet|ElementMap>
//==============================================================================
//------------------------------------------------------------------------------
//- bool within(c P&, c T&) T:{s}|{m} P:{e}|{i} fragment_types|key_types
//------------------------------------------------------------------------------
/** Checks if a key is in the associative container */
template <typename Type>
  requires is_associative_element_container<Type>::value
bool within(const typename Type::key_type& key, const Type& super) {
  return !(super.find(key) == super.end());
}

//------------------------------------------------------------------------------
//- bool within(c P&, c T&) T:{s}|{m} P:{s'} fragment_types|key_types
//------------------------------------------------------------------------------
template <typename SubT, typename SuperT>
  requires(is_associative_element_container<SuperT>::value &&
           is_key_container_of<SubT, SuperT>::value)
bool within(const SubT& sub, const SuperT& super) {
  if (icl::is_empty(sub))
    return true;
  if (icl::is_empty(super))
    return false;
  if (icl::size(super) < icl::size(sub))
    return false;

  typename SubT::const_iterator common_lwb_;
  typename SubT::const_iterator common_upb_;
  if (!Set::common_range(common_lwb_, common_upb_, sub, super))
    return false;

  typename SubT::const_iterator sub_ = sub.begin();
  typename SuperT::const_iterator super_;
  while (sub_ != sub.end()) {
    super_ = super.find(key_value<SubT>(sub_));
    if (super_ == super.end())
      return false;
    if (!co_equal(sub_, super_, &sub, &super))
      return false;

    ++sub_;
  }
  return true;
}

//------------------------------------------------------------------------------
//- bool contains(c T&, c P&) T:{s}|{m} P:{e}|{i} fragment_types|key_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_associative_element_container<Type>::value
bool contains(const Type& super, const typename Type::key_type& key) {
  return icl::within(key, super);
}

//------------------------------------------------------------------------------
//- bool contains(c T&, c P&) T:{s}|{m} P:{s'} fragment_types|key_types
//------------------------------------------------------------------------------
template <typename SubT, typename SuperT>
  requires(is_associative_element_container<SuperT>::value &&
           is_key_container_of<SubT, SuperT>::value)
bool contains(const SuperT& super, const SubT& sub) {
  return icl::within(sub, super);
}

//==============================================================================
//= Equivalences and Orderings
//==============================================================================

/** Standard equality, which is lexicographical equality of the sets
    as sequences, that are given by their Compare order. */
template <typename Type>
  requires is_associative_element_container<Type>::value
bool operator==(const Type& left, const Type& right) {
  return left.size() == right.size() &&
         std::equal(left.begin(), left.end(), right.begin());
}

template <typename Type>
  requires is_associative_element_container<Type>::value
bool is_element_equal(const Type& left, const Type& right) {
  return left == right;
}

/* Strict weak less ordering which is given by the Compare order */
template <typename Type>
  requires is_associative_element_container<Type>::value
bool operator<(const Type& left, const Type& right) {
  return std::lexicographical_compare(left.begin(), left.end(), right.begin(),
                                      right.end(),
                                      typename Type::element_compare());
}

template <typename LeftT, typename RightT>
  requires is_concept_equivalent<is_element_container, LeftT, RightT>::value
int inclusion_compare(const LeftT& left, const RightT& right) {
  return Set::subset_compare(left, right, left.begin(), left.end(),
                             right.begin(), right.end());
}

//==============================================================================
//= Addition
//==============================================================================
template <typename Type>
  requires is_associative_element_container<Type>::value
Type& operator+=(Type& object,
                        const typename Type::value_type& operand) {
  return icl::add(object, operand);
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator+(Type object, const typename Type::value_type& operand) {
  return object += operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator+(const typename Type::value_type& operand, Type object) {
  return object += operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type& operator+=(Type& object, const Type& operand) {
  if (&object == &operand)
    return object;

  typename Type::iterator prior_ = object.end();
  for (typename Type::const_iterator it_ = operand.begin();
       !(it_ == operand.end()); ++it_)
    prior_ = icl::add(object, prior_, *it_);

  return object;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator+(Type object, const Type& operand) {
  return object += operand;
}

//==============================================================================
template <typename Type>
  requires is_associative_element_container<Type>::value
Type& operator|=(Type& object,
                        const typename Type::value_type& operand) {
  return icl::add(object, operand);
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator|(Type object, const typename Type::value_type& operand) {
  return object += operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator|(const typename Type::value_type& operand, Type object) {
  return object += operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type& operator|=(Type& object, const Type& operand) {
  return object += operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator|(Type object, const Type& operand) {
  return object += operand;
}

//==============================================================================
//= Insertion
//==============================================================================
//------------------------------------------------------------------------------
//- V insert(T&, c P&) T:{s}|{m} P:{e}|{b} fragment_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_associative_element_container<Type>::value
std::pair<typename Type::iterator, bool>
insert(Type& object, const typename Type::value_type& operand) {
  return object.insert(operand);
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type::iterator insert(Type& object, typename Type::iterator prior,
                               const typename Type::value_type& operand) {
  return object.insert(prior, operand);
}

//------------------------------------------------------------------------------
//- T insert(T&, c T&) T:{s m}  map fragment_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_associative_element_container<Type>::value
Type& insert(Type& object, const Type& addend) {
  using iterator = Type::iterator;

  iterator prior_ = object.end();
  for (typename Type::const_iterator elem_ = addend.begin();
       !(elem_ == addend.end()); ++elem_)
    icl::insert(object, prior_, *elem_);

  return object;
}

//==============================================================================
//= Erasure
//==============================================================================
template <typename Type>
  requires is_associative_element_container<Type>::value
Type::size_type erase(Type& object,
                               const typename Type::key_type& key_value) {
  using size_type = Type::size_type;
  typename Type::iterator it_ = object.find(key_value);
  if (it_ != object.end()) {
    object.erase(it_);
    return unit_element<size_type>::value();
  }
  return identity_element<size_type>::value();
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type& erase(Type& object, const Type& erasure) {
  for (typename Type::const_iterator elem_ = erasure.begin();
       !(elem_ == erasure.end()); ++elem_)
    icl::erase(object, *elem_);

  return object;
}

//==============================================================================
//= Subtraction<ElementSet|ElementMap>
//==============================================================================
template <typename Type>
  requires is_associative_element_container<Type>::value
Type& operator-=(Type& object,
                        const typename Type::value_type& operand) {
  return icl::subtract(object, operand);
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator-(Type object, const typename Type::value_type& operand) {
  return object -= operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type& operator-=(Type& object, const Type& subtrahend) {
  for (typename Type::const_iterator it_ = subtrahend.begin();
       !(it_ == subtrahend.end()); ++it_)
    icl::subtract(object, *it_);

  return object;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator-(Type object, const Type& subtrahend) {
  return object -= subtrahend;
}

//==============================================================================
//= Intersection
//==============================================================================
//------------------------------------------------------------------------------
//- void add_intersection(T&, c T&, c P&) T:{s}{m} P:{e}{e} key_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_associative_element_container<Type>::value
void add_intersection(Type& section, const Type& object,
                             const typename Type::key_type& operand) {
  using const_iterator = Type::const_iterator;
  const_iterator it_ = object.find(operand);
  if (it_ != object.end())
    icl::add(section, *it_);
}

//------------------------------------------------------------------------------
//- void add_intersection(T&, c T&, c P&) T:{s}{m} P:{s}{s} set key_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_associative_element_container<Type>::value
void
add_intersection(Type& section, const Type& object,
                 const typename key_container_type_of<Type>::type& operand) {
  using key_container_type = key_container_type_of<Type>::type;
  using const_iterator =key_container_type::const_iterator;
  const_iterator common_lwb_, common_upb_;
  if (!Set::common_range(common_lwb_, common_upb_, operand, object))
    return;

  const_iterator sec_ = common_lwb_;
  while (sec_ != common_upb_)
    add_intersection(section, object, *sec_++);
}

//------------------------------------------------------------------------------
//- Intersection<ElementMap|ElementSet>
//------------------------------------------------------------------------------
template <typename Type>
  requires is_associative_element_container<Type>::value
Type& operator&=(Type& object, const typename Type::key_type& operand) {
  Type section;
  add_intersection(section, object, operand);
  object.swap(section);
  return object;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator&(Type object, const typename Type::key_type& operand) {
  return object &= operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator&(const typename Type::key_type& operand, Type object) {
  return object &= operand;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type&
operator&=(Type& object,
           const typename key_container_type_of<Type>::type& operand) {
  Type section;
  add_intersection(section, object, operand);
  object.swap(section);
  return object;
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator&(Type object, const Type& operand) {
  return object &= operand;
}
//------------------------------------------------------------------------------

template <typename Type, typename >
  requires is_associative_element_container<Type>::value
bool disjoint(const Type& left, const Type& right) {
  return !intersects(left, right);
}

//==============================================================================
//= Symmetric difference<ElementSet|ElementMap>
//==============================================================================
template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator^(Type object, const typename Type::value_type& operand) {
  return icl::flip(object, operand);
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator^(const typename Type::value_type& operand, Type object) {
  return icl::flip(object, operand);
}

template <typename Type>
  requires is_associative_element_container<Type>::value
Type operator^(Type object, const Type& operand) {
  return object ^= operand;
}

//==============================================================================
//= Manipulation by predicates
//==============================================================================
template <typename Type, typename Predicate>
  requires is_associative_element_container<Type>::value
Type& erase_if(const Predicate& pred, Type& object) {
  typename Type::iterator it_ = object.begin();
  while (it_ != object.end())
    if (pred(*it_))
      icl::erase(object, it_++);
    else
      ++it_;
  return object;
}

template <typename Type, typename Predicate>
  requires is_associative_element_container<Type>::value
Type& add_if(const Predicate& pred, Type& object, const Type& src) {
  typename Type::const_iterator it_ = src.begin();
  while (it_ != src.end())
    if (pred(*it_))
      icl::add(object, *it_++);

  return object;
}

template <typename Type, typename Predicate>
  requires is_associative_element_container<Type>::value
Type& assign_if(const Predicate& pred, Type& object, const Type& src) {
  icl::clear(object);
  return add_if(object, src, pred);
}

} // namespace icl
