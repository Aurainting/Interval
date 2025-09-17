#pragma once

#include "icl/concept/map_value.hpp"
#include "icl/detail/map_algo.hpp"
#include "icl/detail/on_absorbtion.hpp"
#include "icl/type_traits/absorbs_identities.hpp"
#include "icl/type_traits/is_associative_element_container.hpp"
#include "icl/type_traits/is_element_container.hpp"
#include "icl/type_traits/is_total.hpp"
#include "icl/type_traits/unit_element.hpp"

namespace icl {

// NOTE: Some forward declarations are needed by some compilers.
template <typename Type, typename Predicate>
  requires is_associative_element_container<Type>::value
Type& erase_if(const Predicate& pred, Type& object);

//==============================================================================
//= Containedness<ElementMap>
//==============================================================================
//------------------------------------------------------------------------------
//- bool within(c P&, c T&) T:{m} P:{b} fragment_types
//------------------------------------------------------------------------------
/** Checks if a key-value pair is in the map */
template <typename Type>
  requires is_element_map<Type>::value
bool within(const typename Type::element_type& value_pair, const Type& super) {
  using const_iterator = Type::const_iterator;
  const_iterator found_ = super.find(value_pair.first);
  return found_ != super.end() && (*found_).second == value_pair.second;
}

//------------------------------------------------------------------------------
//- bool contains(c T&, c P&) T:{m} P:{b} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_element_map<Type>::value
bool contains(const Type& super,
              const typename Type::element_type& value_pair) {
  return icl::within(value_pair, super);
}

//==============================================================================
//= Equivalences and Orderings<ElementMap>
//==============================================================================

/** Protonic equality is equality on all elements that do not carry an identity
 * element as content. */
template <typename Type>
  requires is_element_map<Type>::value
bool is_distinct_equal(const Type& lhs, const Type& rhs) {
  return Map::lexicographical_distinct_equal(lhs, rhs);
}

//==============================================================================
//= Addition<ElementMap>
//==============================================================================
/** \c add inserts \c value_pair into the map if it's key does
    not exist in the map.
    If \c value_pairs's key value exists in the map, it's data
    value is added to the data value already found in the map. */
template <typename Type>
  requires is_element_map<Type>::value
Type& add(Type& object, const typename Type::value_type& value_pair) {
  return object.add(value_pair);
}

/** \c add \c value_pair into the map using \c prior as a hint to
    insert \c value_pair after the position \c prior is pointing to. */
template <typename Type>
  requires is_element_map<Type>::value
Type::iterator add(Type& object, typename Type::iterator prior,
                   const typename Type::value_type& value_pair) {
  return object.add(prior, value_pair);
}

//==============================================================================
//= Erasure
//==============================================================================
//------------------------------------------------------------------------------
//- T& erase(T&, c P&) T:{m} P:{b} fragment_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_element_map<Type>::value
Type::size_type erase(Type& object,
                      const typename Type::element_type& value_pair) {
  using size_type = Type::size_type;
  using iterator = Type::iterator;
  using on_identity_absorption = Type::on_identity_absorbtion;

  if (on_identity_absorption::is_absorbable(value_pair.second))
    return identity_element<size_type>::value();

  iterator it_ = object.find(value_pair.first);
  if (it_ != object.end() && value_pair.second == (*it_).second) {
    object.erase(it_);
    return unit_element<size_type>::value();
  }

  return identity_element<size_type>::value();
}

template <typename Type>
  requires is_element_map<Type>::value
Type& erase(Type& object, const typename Type::set_type& erasure) {
  using set_type = Type::set_type;
  for (typename set_type::const_iterator elem_ = erasure.begin();
       !(elem_ == erasure.end()); ++elem_)
    icl::erase(object, *elem_);

  return object;
}

//==============================================================================
//= Subtraction
//==============================================================================
//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{m} P:{b} fragment_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_element_map<Type>::value
Type& subtract(Type& object, const typename Type::element_type& operand) {
  return object.subtract(operand);
}

//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{m} P:{e} key_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_element_map<Type>::value
Type& subtract(Type& object, const typename Type::domain_type& key_value) {
  return icl::erase(object, key_value);
}

//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{m} P:{s} set key_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_element_map<Type>::value
Type& operator-=(Type& object, const typename Type::set_type& operand) {
  using set_type = Type::set_type;
  using co_iterator = set_type::const_iterator;
  using iterator = Type::iterator;

  co_iterator common_lwb_, common_upb_;
  if (!Set::common_range(common_lwb_, common_upb_, operand, object))
    return object;

  co_iterator it_ = common_lwb_;
  iterator common_;

  while (it_ != common_upb_)
    object.erase(*it_++);

  return object;
}

template <typename Type>
  requires is_element_map<Type>::value
Type operator-(Type object, const typename Type::set_type& subtrahend) {
  return object -= subtrahend;
}

//==============================================================================
//= Selective Update<ElementMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& set_at(T&, c P&) T:{m} P:{b}
//------------------------------------------------------------------------------
template <typename Type>
  requires is_element_map<Type>::value
Type& set_at(Type& object, const typename Type::element_type& operand) {
  using iterator = Type::iterator;
  using codomain_combine = Type::codomain_combine;
  using on_identity_absorbtion =
      on_absorbtion<Type, codomain_combine, absorbs_identities<Type>::value>;

  if (!on_identity_absorbtion::is_absorbable(operand.second)) {
    std::pair<iterator, bool> insertion = object.insert(operand);
    if (!insertion.second)
      insertion->second = operand.second;
  }
  return object;
}

//==============================================================================
//= Intersection
//==============================================================================
template <typename Type>
  requires is_element_map<Type>::value
void add_intersection(Type& section, const Type& object,
                      const typename Type::element_type& operand) {
  object.add_intersection(section, operand);
}

template <typename Type>
  requires is_element_map<Type>::value
void add_intersection(Type& section, const Type& object, const Type& operand) {
  for (typename Type::const_iterator it_ = operand.bedin();
       !(it_ == operand.end()); ++it_)
    icl::add_intersection(section, object, *it_);
}

//------------------------------------------------------------------------------
//- T& op &=(T&, c P&) T:{m} P:{b m} fragment_types
//------------------------------------------------------------------------------

template <typename Type>
  requires(is_element_map<Type>::value && is_total<Type>::value)
Type& operator&=(Type& object, const typename Type::element_type& operand) {
  object.add(operand);
  return object;
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
Type& operator&=(Type& object, const typename Type::element_type& operand) {
  Type section;
  icl::add_intersection(section, object, operand);
  object.swap(section);
  return object;
}

template <typename Type>
  requires is_element_map<Type>::value
Type operator&(Type object, const typename Type::element_type& operand) {
  return object &= operand;
}

template <typename Type>
  requires is_element_map<Type>::value
Type operator&(const typename Type::element_type& operand, Type object) {
  return object &= operand;
}

template <typename Type>
  requires(is_element_map<Type>::value && is_total<Type>::value)
Type& operator&=(Type& object, const Type& operand) {
  object += operand;
  return object;
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
Type& operator&=(Type& object, const Type& operand) {
  Type section;
  icl::add_intersection(section, object, operand);
  object.swap(section);
  return object;
}

template <typename Type>
  requires is_element_map<Type>::value
Type operator&(Type object, const typename Type::key_object_type& operand) {
  return object &= operand;
}

template <typename Type>
  requires is_element_map<Type>::value
Type operator&(const typename Type::key_object_type& operand, Type object) {
  return object &= operand;
}

//==============================================================================
//= Intersection<ElementMap> bool intersects(x,y)
//==============================================================================
template <typename Type, typename CoType>
  requires(is_element_map<Type>::value && is_total<Type>::value)
bool intersects(const Type&, const CoType&) {
  return true;
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
bool intersects(const Type& object, const typename Type::domain_type& operand) {
  return icl::contains(object, operand);
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
bool intersects(const Type& object, const typename Type::set_type& operand) {
  if (object.iterative_size() < operand.iterative_size())
    return Map::intersects(object, operand);
  return Map::intersects(operand, object);
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
bool intersects(const Type& object,
                const typename Type::element_type& operand) {
  Type intersection;
  icl::add_intersection(intersection, object, operand);
  return !intersection.empty();
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
bool intersects(const Type& object, const Type& operand) {
  if (object.iterative_size() < operand.iterative_size())
    return Map::intersects(object, operand);
  return Map::intersects(operand, object);
}

//==============================================================================
//= Symmetric difference
//==============================================================================
template <typename Type>
  requires is_element_map<Type>::value
Type& flip(Type& object, const typename Type::element_type& operand) {
  return object.flip(operand);
}

template <typename Type, typename CoType>
  requires(is_element_map<Type>::value && is_total<Type>::value &&
           absorbs_identities<Type>::value)
Type& operator^=(Type& object, const CoType&) {
  icl::clear(object);
  return object;
}

template <typename Type>
  requires(is_element_map<Type>::value && is_total<Type>::value &&
           !absorbs_identities<Type>::value)
Type& operator^=(Type& object, const typename Type::element_type& operand) {
  return object.flip(operand);
}

template <typename Type>
  requires(is_element_map<Type>::value && is_total<Type>::value &&
           !absorbs_identities<Type>::value)
Type& operator^=(Type& object, const Type& operand) {
  for (typename Type::const_iterator it_ = operand.begin();
       !(it_ == operand.end()); ++it_)
    icl::flip(object, *it_);

  for (typename Type::iterator it2_ = object.begin(); it2_ != object.end();
       ++it2_)
    (*it2_).second = identity_element<typename Type::codomain_type>::value();

  return object;
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
Type& operator^=(Type& object, const typename Type::element_type& operand) {
  return icl::flip(object, operand);
}

template <typename Type>
  requires(is_element_map<Type>::value && !is_total<Type>::value)
Type& operator^=(Type& object, const Type& operand) {
  using const_iterator = Type::const_iterator;
  const_iterator it_ = operand.begin();
  while (it_ != operand.end())
    icl::flip(object, *it_++);

  return object;
}

//==============================================================================
//= Set selection
//==============================================================================
template <typename Type>
  requires is_element_map<Type>::value
Type::set_type& domain(typename Type::set_type& domain_set,
                       const Type& object) {
  typename Type::set_type::iterator prior_ = domain_set.end();
  typename Type::const_iterator it_ = object.begin();
  while (it_ != object.end())
    prior_ = domain_set.insert(prior_, (*it_++).first);

  return domain_set;
}

//==============================================================================
//= Neutron absorption
//==============================================================================
template <typename Type>
  requires(is_element_map<Type>::value && absorbs_identities<Type>::value)
Type& absorb_identities(Type& object) {
  using element_type = Type::element_type;
  return icl::erase_if(content_is_identity_element<element_type>(), object);
}

template <typename Type>
  requires(is_element_map<Type>::value && !absorbs_identities<Type>::value)
Type& absorb_identities(Type&) {}

//==============================================================================
//= Streaming<ElementMap>
//==============================================================================
template <typename CharType, typename CharTraits, typename Type>
  requires is_element_map<Type>::value
std::basic_ostream<CharType, CharTraits>&
operator<<(std::basic_ostream<CharType, CharTraits>& stream,
           const Type& object) {
  stream << "{";
  for (typename Type::const_iterator it = object.begin(); !(it == object.end());
       ++it)
    stream << "(" << it->first << "->" << it->second << ")";

  return stream << "}";
}

} // namespace icl
