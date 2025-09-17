#pragma once

#include "icl/concept/interval.hpp"
#include "icl/concept/joinable.hpp"
#include "icl/concept/map_value.hpp"
#include "icl/concept/set_value.hpp"
#include "icl/type_traits/domain_type_of.hpp"
#include "icl/type_traits/interval_type_of.hpp"
#include "icl/type_traits/is_discrete.hpp"
#include "icl/type_traits/is_interval_container.hpp"
#include <iterator>

namespace icl {

//==============================================================================
//= Selection<IntervalSet|IntervalMap>
//==============================================================================
template <typename Type>
  requires(is_interval_container<Type>::value &&
           is_discrete<typename domain_type_of<Type>::type>::value)
Type::const_iterator find(const Type& object,
                          const typename domain_type_of<Type>::type& key_val) {
  using interval_type = Type::interval_type;
  return object.find(detail::unit_trail<interval_type>(key_val));
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           is_continuous<typename domain_type_of<Type>::type>::value &&
           has_dynamic_bounds<typename interval_type_of<Type>::type>::value)
Type::const_iterator find(const Type& object,
                          const typename domain_type_of<Type>::type& key_val) {
  using interval_type = Type::interval_type;
  return object.find(icl::singleton<interval_type>(key_val));
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           is_continuous<typename domain_type_of<Type>::type>::value &&
           is_static_right_open<typename interval_type_of<Type>::type>::value &&
           std::incrementable<typename domain_type_of<Type>::type>)
Type::const_iterator find(const Type& object,
                          const typename domain_type_of<Type>::type& key_val) {
  using const_iterator = Type::const_iterator;
  using interval_type = Type::interval_type;
  const_iterator first_collision =
      object.lower_bound(detail::unit_trail<interval_type>(key_val));
  // A part of the unit_trail(key_value)-interval may be found in the container,
  // that does not contain key_value. Therefore, we have to check for its
  // existence:
  return first_collision == object.end() ||
                 icl::contains(key_value<Type>(first_collision), key_val)
             ? first_collision
             : object.end();
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           is_continuous<typename domain_type_of<Type>::type>::value &&
           is_static_left_open<typename interval_type_of<Type>::type>::value &&
           std::incrementable<typename domain_type_of<Type>::type>)
Type::const_iterator find(const Type& object,
                          const typename domain_type_of<Type>::type& key_val) {
  using const_iterator = Type::const_iterator;
  using interval_type = Type::interval_type;
  const_iterator last_collision =
      object.upper_bound(detail::unit_trail<interval_type>(key_val));
  if (last_collision != object.begin())
    --last_collision;
  // A part of the unit_trail(key_value)-interval may be found in the container,
  // that does not contain key_value. Therefore, we have to check for its
  // existence:
  return last_collision == object.end() ||
                 icl::contains(key_value<Type>(last_collision), key_val)
             ? last_collision
             : object.end();
}

// NOTE: find(object, key) won't compile if key is of continuous type that does
// not implement in(de)crementation (e.g. std::string).

template <typename Type>
  requires is_interval_container<Type>::value
Type::const_iterator
find(const Type& object,
     const typename interval_type_of<Type>::type& inter_val) {
  return object.find(inter_val);
}

//==============================================================================
//= Morphisms
//==============================================================================
template <typename Type>
  requires is_interval_container<Type>::value
Type& join(Type& object) {
  using interval_type = Type::interval_type;
  using iterator = Type::iterator;

  iterator it_ = object.begin();
  if (it_ == object.end())
    return object;

  iterator next_ = it_;
  ++next_;

  while (next_ != object.end()) {
    if (segmental::is_joinable<Type>(it_, next_)) {
      iterator fst_mem = it_; // hold the first member

      // Go on while touching members are found
      ++it_;
      ++next_;
      while (next_ != object.end() &&
             segmental::is_joinable<Type>(it_, next_)) {
        ++it_;
        ++next_;
      }

      // finally we arrive at the end of a sequence of joinable intervals,
      // and it points to the last member of that sequence
      const_cast<interval_type&>(key_value<Type>(it_)) =
          hull(key_value<Type>(it_), key_value<Type>(fst_mem));
      object.erase(fst_mem, it_);

      ++it_;
      next_ = it_;
      if (next_ != object.end())
        ++next_;
    } else {
      ++it_;
      ++next_;
    }
  }
  return object;
}

} // namespace icl
