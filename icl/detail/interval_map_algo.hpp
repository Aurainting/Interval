#pragma once

#include "icl/concept/interval_set.hpp"
#include "icl/type_traits/is_total.hpp"

namespace icl {
namespace Interval_Map {
using namespace segmental;

template <typename IntervalMapT>
bool is_joinable(const IntervalMapT& container,
                 typename IntervalMapT::const_iterator first,
                 typename IntervalMapT::const_iterator past) {
  if (first == container.end())
    return true;

  typename IntervalMapT::const_iterator it_ = first, next_ = first;
  ++next_;

  const typename IntervalMapT::codomain_type& co_value =
      icl::co_value<IntervalMapT>(first);
  while (it_ != past) {
    if (icl::co_value<IntervalMapT>(next_) != co_value)
      return false;
    if (!icl::touches(key_value<IntervalMapT>(it_++),
                      key_value<IntervalMapT>(next_++)))
      return false;
  }

  return true;
}

//------------------------------------------------------------------------------
//- Containedness of key objects
//------------------------------------------------------------------------------

//- domain_type ----------------------------------------------------------------
template <typename IntervalMapT>
  requires(!is_total<IntervalMapT>::value)
bool contains(const IntervalMapT& container,
              const typename IntervalMapT::domain_type& key) {
  return container.find(key) != container.end();
}

template <typename IntervalMapT>
  requires is_total<IntervalMapT>::value
bool contains(const IntervalMapT&, const typename IntervalMapT::domain_type&) {
  return true;
}

//- interval_type --------------------------------------------------------------
template <typename IntervalMapT>
  requires(!is_total<IntervalMapT>::value)
bool contains(const IntervalMapT& container,
              const typename IntervalMapT::interval_type& sub_interval) {
  using const_iterator = IntervalMapT::const_iterator;
  if (icl::is_empty(sub_interval))
    return true;

  std::pair<const_iterator, const_iterator> exterior =
      container.equal_range(sub_interval);
  if (exterior.first == exterior.second)
    return false;

  const_iterator last_overlap = prior(exterior.second);

  return icl::contains(hull(exterior.first->first, last_overlap->first),
                       sub_interval) &&
         Interval_Set::is_joinable(container, exterior.first, last_overlap);
}

template <typename IntervalMapT>
  requires is_total<IntervalMapT>::value
bool contains(const IntervalMapT&,
              const typename IntervalMapT::interval_type&) {
  return true;
}

//- set_type -------------------------------------------------------------------
template <typename IntervalMapT, typename IntervalSetT>
  requires(!is_total<IntervalMapT>::value &&
           is_interval_set<IntervalSetT>::value)
bool contains(const IntervalMapT& super_map, const IntervalSetT& sub_set) {
  return Interval_Set::within(sub_set, super_map);
}

template <typename IntervalMapT, typename IntervalSetT>
  requires(is_total<IntervalMapT>::value &&
           is_interval_set<IntervalSetT>::value)
bool contains(const IntervalMapT&, const IntervalSetT&) {
  return true;
}

//------------------------------------------------------------------------------
//- Containedness of sub objects
//------------------------------------------------------------------------------

template <typename IntervalMapT>
bool contains(const IntervalMapT& container,
              const typename IntervalMapT::element_type& key_value_pair) {
  typename IntervalMapT::const_iterator it_ =
      container.find(key_value_pair.key);
  return it_ != container.end() && (*it_).second == key_value_pair.data;
}

template <typename IntervalMapT>
bool contains(const IntervalMapT& container,
              const typename IntervalMapT::segment_type sub_segment) {
  using const_iterator = IntervalMapT::const_iterator;
  typename IntervalMapT::interval_type sub_interval = sub_segment.first;
  if (icl::is_empty(sub_interval))
    return true;

  std::pair<const_iterator, const_iterator> exterior =
      container.equal_range(sub_interval);
  if (exterior.first == exterior.second)
    return false;

  const_iterator last_overlap = prior(exterior.second);

  if (!(sub_segment.second == exterior.first->second))
    return false;

  return icl::contains(hull(exterior.first->first, last_overlap->first),
                       sub_interval) &&
         Interval_Map::is_joinable(container, exterior.first, last_overlap);
}

template <typename IntervalMapT>
bool contains(const IntervalMapT& super, const IntervalMapT& sub) {
  return Interval_Set::within(sub, super);
}

} // namespace Interval_Map
} // namespace icl
