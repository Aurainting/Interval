#pragma once

#include "icl/concept/interval.hpp"
#include "icl/concept/interval_associator_base.hpp"
#include "icl/concept/interval_set.hpp"
#include "icl/detail/interval_map_algo.hpp"
#include "icl/detail/set_algo.hpp"
#include "icl/type_traits/absorbs_identities.hpp"
#include "icl/type_traits/element_type_of.hpp"
#include "icl/type_traits/is_combinable.hpp"
#include "icl/type_traits/is_interval_splitter.hpp"
#include "icl/type_traits/segment_type_of.hpp"

namespace icl {

template <typename Type>
  requires is_interval_map<Type>::value
Type::segment_type make_segment(const typename Type::element_type& element) {
  using interval_type = Type::interval_type;
  using segment_type = Type::segment_type;
  return segment_type(icl::singleton<interval_type>(element.key), element.data);
}

//==============================================================================
//= Containedness<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- bool contains(c T&, c P&) T:{M} P:{b p M} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
bool contains(const Type& super,
              const typename Type::element_type& key_value_pair) {
  using const_iterator = Type::const_iterator;
  const_iterator it_ = icl::find(super, key_value_pair.key);
  return it_ != super.end() && (*it_).second == key_value_pair.data;
}

template <typename Type>
  requires is_interval_map<Type>::value
bool contains(const Type& super,
              const typename Type::segment_type& sub_segment) {
  using interval_type = Type::interval_type;
  using const_iterator = Type::const_iterator;

  interval_type sub_interval = sub_segment.first;
  if (icl::is_empty(sub_interval))
    return true;

  std::pair<const_iterator, const_iterator> exterior =
      super.equal_range(sub_interval);
  if (exterior.first == exterior.second)
    return false;

  const_iterator last_overlap = prior(exterior.second);

  if (!(sub_segment.second == exterior.first->second))
    return false;

  return icl::contains(hull(exterior.first->first, last_overlap->first),
                       sub_interval) &&
         Interval_Map::is_joinable(super, exterior.first, last_overlap);
}

template <typename Type, typename CoType>
  requires is_concept_compatible<is_interval_map, Type, CoType>::value
bool contains(const Type& super, const CoType& sub) {
  return Interval_Set::within(sub, super);
}

//------------------------------------------------------------------------------
//- bool contains(c T&, c P&) T:{M} P:{e i S} key_types : total
//------------------------------------------------------------------------------
template <typename Type, typename CoType>
  requires(is_interval_map<Type>::value && is_total<Type>::value &&
           is_cross_derivative<Type, CoType>::value)
bool contains(const Type&, const CoType&) {
  return true;
}

//------------------------------------------------------------------------------
//- bool contains(c T&, c P&) T:{M} P:{e i S} key_types : partial
//------------------------------------------------------------------------------
template <typename Type>
  requires(is_interval_map<Type>::value && (!is_total<Type>::value))
bool contains(const Type& super, const typename Type::domain_type& key) {
  return icl::find(super, key) != super.end();
}

template <typename Type>
  requires(is_interval_map<Type>::value && (!is_total<Type>::value))
bool contains(const Type& super,
              const typename Type::interval_type& sub_interval) {
  using const_iterator = Type::const_iterator;

  if (icl::is_empty(sub_interval))
    return true;

  std::pair<const_iterator, const_iterator> exterior =
      super.equal_range(sub_interval);
  if (exterior.first == exterior.second)
    return false;

  const_iterator last_overlap = prior(exterior.second);

  return icl::contains(hull(exterior.first->first, last_overlap->first),
                       sub_interval) &&
         Interval_Set::is_joinable(super, exterior.first, last_overlap);
}

template <typename Type, typename KeyT>
  requires(is_concept_combinable<is_interval_map, is_interval_set, Type,
                                 KeyT>::value &&
           (!is_total<Type>::value))
bool contains(const Type& super, const KeyT& sub) {
  return Interval_Set::within(sub, super);
}

//==============================================================================
//= Addition<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& add(T&, c P&) T:{M} P:{b p} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& add(Type& object, const typename Type::segment_type& operand) {
  return object.add(operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& add(Type& object, const typename Type::element_type& operand) {
  return icl::add(object, make_segment<Type>(operand));
}

//------------------------------------------------------------------------------
//- T& add(T&, J, c P&) T:{M} P:{p} segment_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type::iterator add(Type& object, typename Type::iterator prior_,
                   const typename Type::segment_type& operand) {
  return object.add(prior_, operand);
}

//==============================================================================
//= Insertion<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& insert(T&, c P&) T:{M} P:{b p} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& insert(Type& object, const typename Type::segment_type& operand) {
  return object.insert(operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& insert(Type& object, const typename Type::element_type& operand) {
  return icl::insert(object, make_segment<Type>(operand));
}

//------------------------------------------------------------------------------
//- T& insert(T&, J, c P&) T:{M} P:{p} with hint
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type::iterator insert(Type& object, typename Type::iterator prior,
                      const typename Type::segment_type& operand) {
  return object.insert(prior, operand);
}

//==============================================================================
//= Erasure<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& erase(T&, c P&) T:{M} P:{e i} key_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& erase(Type& object, const typename Type::interval_type& operand) {
  return object.erase(operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& erase(Type& object, const typename Type::domain_type& operand) {
  using interval_type = Type::interval_type;
  return icl::erase(object, icl::singleton<interval_type>(operand));
}

//------------------------------------------------------------------------------
//- T& erase(T&, c P&) T:{M} P:{b p} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& erase(Type& object, const typename Type::segment_type& operand) {
  return object.erase(operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& erase(Type& object, const typename Type::element_type& operand) {
  return icl::erase(object, make_segment<Type>(operand));
}

//==============================================================================
//= Subtraction<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{M} P:{b p} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& subtract(Type& object, const typename Type::segment_type& operand) {
  return object.subtract(operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& subtract(Type& object, const typename Type::element_type& operand) {
  return icl::subtract(object, make_segment<Type>(operand));
}

//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{M} P:{e i} key_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& subtract(Type& object, const typename Type::domain_type& operand) {
  return object.erase(operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& subtract(Type& object, const typename Type::interval_type& operand) {
  return object.erase(operand);
}

//==============================================================================
//= Selective Update<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& set_at(T&, c P&) T:{M} P:{e i}
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& set_at(Type& object, const typename Type::segment_type& operand) {
  icl::erase(object, operand.first);
  return icl::insert(object, operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& set_at(Type& object, const typename Type::element_type& operand) {
  return icl::set_at(object, make_segment<Type>(operand));
}

//==============================================================================
//= Intersection<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{M} P:{b p} fragment_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
void add_intersection(Type& section, const Type& object,
                      const typename Type::element_type& operand) {
  object.add_intersection(section, make_segment<Type>(operand));
}

template <typename Type>
  requires is_interval_map<Type>::value
void add_intersection(Type& section, const Type& object,
                      const typename Type::segment_type& operand) {
  object.add_intersection(section, operand);
}

//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{M} P:{M'} map fragment_type total
//------------------------------------------------------------------------------
template <typename Type, typename MapT>
  requires(is_total<Type>::value &&
           is_concept_compatible<is_interval_map, Type, MapT>::value)
void add_intersection(Type& section, const Type& object, const MapT& operand) {
  section += object;
  section += operand;
}

//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{M} P:{M'} map fragment_type partial
//------------------------------------------------------------------------------
template <typename Type, typename MapT>
  requires((!is_total<Type>::value) &&
           is_concept_compatible<is_interval_map, Type, MapT>::value)
void add_intersection(Type& section, const Type& object, const MapT& operand) {
  using const_iterator = MapT::const_iterator;

  if (operand.empty())
    return;
  const_iterator common_lwb, common_upb;
  if (!Set::common_range(common_lwb, common_upb, operand, object))
    return;
  const_iterator it_ = common_lwb;
  while (it_ != common_upb)
    add_intersection(section, object, *it_++);
}

//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{M} P:{e i S} key_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
void add_intersection(Type& section, const Type& object,
                      const typename Type::domain_type& key_value) {
  using interval_type = Type::interval_type;
  using segment_type = Type::segment_type;
  using const_iterator = Type::const_iterator;

  const_iterator it_ = icl::find(object, key_value);
  if (it_ != object.end())
    add(section, segment_type(interval_type(key_value), (*it_).second));
}

template <typename Type>
  requires is_interval_map<Type>::value
void add_intersection(Type& section, const Type& object,
                      const typename Type::interval_type& inter_val) {
  using interval_type = Type::interval_type;
  using value_type = Type::value_type;
  using const_iterator = Type::const_iterator;
  using iterator = Type::iterator;

  if (icl::is_empty(inter_val))
    return;

  std::pair<const_iterator, const_iterator> exterior =
      object.equal_range(inter_val);
  if (exterior.first == exterior.second)
    return;

  iterator prior_ = section.end();
  for (const_iterator it_ = exterior.first; it_ != exterior.second; ++it_) {
    interval_type common_interval = (*it_).first & inter_val;
    if (!icl::is_empty(common_interval))
      prior_ = add(section, prior_, value_type(common_interval, (*it_).second));
  }
}

template <typename Type, typename KeySetT>
  requires is_concept_combinable<is_interval_map, is_interval_set, Type,
                                 KeySetT>::value
void add_intersection(Type& section, const Type& object,
                      const KeySetT& key_set) {
  using const_iterator = KeySetT::const_iterator;

  if (icl::is_empty(key_set))
    return;

  const_iterator common_lwb, common_upb;
  if (!Set::common_range(common_lwb, common_upb, key_set, object))
    return;

  const_iterator it_ = common_lwb;
  while (it_ != common_upb)
    add_intersection(section, object, *it_++);
}

//------------------------------------------------------------------------------
//- intersects<IntervalMaps> fragment_types
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires(is_interval_map<Type>::value && is_total<Type>::value &&
           std::is_same_v<OperandT, typename segment_type_of<Type>::type>)
bool intersects(const Type&, const OperandT&) {
  return true;
}

template <typename Type, typename OperandT>
  requires(is_interval_map<Type>::value && (!is_total<Type>::value) &&
           std::is_same_v<OperandT, typename segment_type_of<Type>::type>)
bool intersects(const Type& object, const OperandT& operand) {
  Type intersection;
  icl::add_intersection(intersection, object, operand);
  return !icl::is_empty(intersection);
}

template <typename Type, typename OperandT>
  requires(is_interval_map<Type>::value &&
           std::is_same_v<OperandT, typename element_type_of<Type>::type>)
bool intersects(const Type& object, const OperandT& operand) {
  return icl::intersects(object, make_segment<Type>(operand));
}

//==============================================================================
//= Symmetric difference<IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& flip(T&, c P&) T:{M} P:{b p} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_map<Type>::value
Type& flip(Type& object, const typename Type::segment_type& operand) {
  return object.flip(operand);
}

template <typename Type>
  requires is_interval_map<Type>::value
Type& flip(Type& object, const typename Type::element_type& operand) {
  return icl::flip(object, make_segment<Type>(operand));
}

//------------------------------------------------------------------------------
//- T& flip(T&, c P&) T:{M} P:{M'} total absorber
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires(is_total<Type>::value && absorbs_identities<Type>::value &&
           is_concept_compatible<is_interval_map, Type, OperandT>::value)
Type& flip(Type& object, const OperandT&) {
  object.clear();
  return object;
}

//------------------------------------------------------------------------------
//- T& flip(T&, c P&) T:{M} P:{M'} total enricher
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires(is_total<Type>::value && (!absorbs_identities<Type>::value) &&
           is_concept_compatible<is_interval_map, Type, OperandT>::value)
Type& flip(Type& object, const OperandT& operand) {
  using codomain_type = Type::codomain_type;

  object += operand;
  for (typename Type::iterator it_ = object.begin(); it_ != object.end(); ++it_)
    (*it_).second = identity_element<codomain_type>::value();

  if constexpr (!is_interval_splitter<Type>::value)
    icl::join(object);

  return object;
}

//------------------------------------------------------------------------------
//- T& flip(T&, c P&) T:{M} P:{M'} partial
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires((!is_total<Type>::value) &&
           is_concept_compatible<is_interval_map, Type, OperandT>::value)
Type& flip(Type& object, const OperandT& operand) {
  using const_iterator = OperandT::const_iterator;

  const_iterator common_lwb, common_upb;

  if (!Set::common_range(common_lwb, common_upb, operand, object))
    return object += operand;

  const_iterator it_ = operand.begin();

  // All elements of operand left of the common range are added
  while (it_ != common_lwb)
    icl::add(object, *it_++);
  // All elements of operand in the common range are symmetrically subtracted
  while (it_ != common_upb)
    icl::flip(object, *it_++);
  // All elements of operand right of the common range are added
  while (it_ != operand.end())
    icl::add(object, *it_++);

  return object;
}

//==============================================================================
//= Set selection
//==============================================================================
template <typename Type, typename SetT>
  requires is_concept_combinable<is_interval_set, is_interval_map, SetT,
                                 Type>::value
SetT& domain(SetT& result, const Type& object) {
  using set_iterator = SetT::iterator;
  result.clear();
  set_iterator prior_ = result.end();
  for (typename Type::const_iterator it_ = object.begin();
       !(it_ == object.end()); ++it_)
    prior_ = icl::insert(result, prior_, (*it_).first);

  return result;
}

template <typename Type, typename SetT>
  requires is_concept_combinable<is_interval_set, is_interval_map, SetT,
                                 Type>::value
SetT& between(SetT& in_between, const Type& object) {
  using const_iterator = Type::const_iterator;
  using set_iterator = SetT::iterator;
  in_between.clear();
  const_iterator it_ = object.begin(), pred_;
  set_iterator prior_ = in_between.end();

  if (it_ != object.end())
    pred_ = it_++;

  while (it_ != object.end())
    prior_ = icl::insert(in_between, prior_,
                         between((*pred_++).first, (*it_++).first));

  return in_between;
}

//==============================================================================
//= Manipulation by predicates
//==============================================================================
template <typename MapT, typename Predicate>
  requires is_interval_map<MapT>::value
MapT& erase_if(const Predicate& pred, MapT& object) {
  typename MapT::iterator it_ = object.begin();
  while (it_ != object.end())
    if (pred(*it_))
      object.erase(it_++);
    else
      ++it_;
  return object;
}

template <typename MapT, typename Predicate>
  requires is_interval_map<MapT>::value
MapT& add_if(const Predicate& pred, MapT& object, const MapT& src) {
  typename MapT::const_iterator it_ = src.begin();
  while (it_ != src.end())
    if (pred(*it_))
      icl::add(object, *it_++);

  return object;
}

template <typename MapT, typename Predicate>
  requires is_interval_map<MapT>::value
MapT& assign_if(const Predicate& pred, MapT& object, const MapT& src) {
  icl::clear(object);
  return add_if(object, src, pred);
}

//==============================================================================
//= Morphisms
//==============================================================================
template <typename Type>
  requires(is_interval_map<Type>::value && absorbs_identities<Type>::value)
Type& absorb_identities(Type& object) {
  return object;
}

template <typename Type>
  requires(is_interval_map<Type>::value && (!absorbs_identities<Type>::value))
Type& absorb_identities(Type& object) {
  using segment_type = Type::segment_type;
  return icl::erase_if(content_is_identity_element<segment_type>(), object);
}

//==============================================================================
//= Streaming
//==============================================================================
template <typename CharType, typename CharTraits, typename Type>
  requires is_interval_map<Type>::value
std::basic_ostream<CharType, CharTraits>&
operator<<(std::basic_ostream<CharType, CharTraits>& stream,
           const Type& object) {
  stream << "{";
  for (typename Type::const_iterator it_ = object.begin();
       !(it_ == object.end()); ++it_)
    stream << "(" << (*it_).first << "->" << (*it_).second << ")";

  return stream << "}";
}

} // namespace icl
