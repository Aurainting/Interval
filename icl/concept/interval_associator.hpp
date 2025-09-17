#pragma once

#include "icl/concept/interval.hpp"
#include "icl/concept/interval_map.hpp"
#include "icl/concept/interval_set.hpp"
#include "icl/detail/interval_set_algo.hpp"
#include "icl/detail/map_algo.hpp"
#include "icl/detail/set_algo.hpp"
#include "icl/type_traits/domain_type_of.hpp"
#include "icl/type_traits/interval_type_of.hpp"
#include "icl/type_traits/is_combinable.hpp"
#include <cstddef>
#include <ranges>

namespace icl {

template <typename T> T prior(T x) { return --x; }

//==============================================================================
//= Containedness<IntervalSet|IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- bool within(c T&, c P&) T={Set,Map} P={e i b p S M}
//------------------------------------------------------------------------------
template <typename SubT, typename SuperT>
  requires is_interval_container<SuperT>::value
bool within(const SubT& sub, const SuperT& super) {
  return icl::contains(super, sub);
}

//==============================================================================
//= Equivalences and Orderings<IntervalSet|IntervalMap>
//==============================================================================
template <typename Type>
  requires is_interval_container<Type>::value
bool operator==(const Type& left, const Type& right) {
  return Set::lexicographical_equal(left, right);
}

template <typename Type>
  requires is_interval_container<Type>::value
bool operator<(const Type& left, const Type& right) {
  using segment_compare = Type::segment_compare;
  return std::lexicographical_compare(left.begin(), left.end(), right.begin(),
                                      right.end(), segment_compare());
}

/** Returns true, if \c left and \c right contain the same elements.
    Complexity: linear. */
template <typename LeftT, typename RightT>
  requires is_intra_combinable<LeftT, RightT>::value
bool is_element_equal(const LeftT& left, const RightT& right) {
  return Interval_Set::is_element_equal(left, right);
}

/** Returns true, if \c left is lexicographically less than \c right.
    Intervals are interpreted as sequence of elements.
    Complexity: linear. */
template <typename LeftT, typename RightT>
  requires is_intra_combinable<LeftT, RightT>::value
bool is_element_less(const LeftT& left, const RightT& right) {
  return Interval_Set::is_element_less(left, right);
}

/** Returns true, if \c left is lexicographically greater than \c right.
    Intervals are interpreted as sequence of elements.
    Complexity: linear. */
template <typename LeftT, typename RightT>
  requires is_intra_combinable<LeftT, RightT>::value
bool is_element_greater(const LeftT& left, const RightT& right) {
  return Interval_Set::is_element_greater(left, right);
}

//------------------------------------------------------------------------------
template <typename LeftT, typename RightT>
  requires is_inter_combinable<LeftT, RightT>::value
int inclusion_compare(const LeftT& left, const RightT& right) {
  return Interval_Set::subset_compare(left, right, left.begin(), left.end(),
                                      right.begin(), right.end());
}

//------------------------------------------------------------------------------
template <typename LeftT, typename RightT>
  requires is_concept_compatible<is_interval_map, LeftT, RightT>::value
bool is_distinct_equal(const LeftT& left, const RightT& right) {
  return Map::lexicographical_distinct_equal(left, right);
}

//==============================================================================
//= Size<IntervalSet|IntervalMap>
//==============================================================================
template <typename Type>
  requires is_interval_container<Type>::value
std::size_t iterative_size(const Type& object) {
  return object.iterative_size();
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           is_discrete<typename Type::domain_type>::value)
Type::size_type cardinality(const Type& object) {
  using size_type = Type::size_type;

  size_type size = identity_element<size_type>::value();
  for (typename Type::const_iterator it = object.begin(); !(it == object.end());
       ++it)
    size += icl::cardinality(key_value<Type>(it));
  return size;
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_discrete<typename Type::domain_type>::value))
Type::size_type cardinality(const Type& object) {
  using size_type = Type::size_type;

  size_type size = identity_element<size_type>::value();
  size_type interval_size;
  for (typename Type::const_iterator it = object.begin(); !(it == object.end());
       ++it) {
    interval_size = icl::cardinality(key_value<Type>(it));
    if (interval_size == infinity<size_type>::value())
      return interval_size;
    size += interval_size;
  }
  return size;
}

template <typename Type>
  requires is_interval_container<Type>::value
Type::size_type size(const Type& object) {
  return icl::cardinality(object);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type::difference_type length(const Type& object) {
  using difference_type = Type::difference_type;
  using const_iterator = Type::const_iterator;
  difference_type length = identity_element<difference_type>::value();
  const_iterator it_ = object.begin();

  while (it_ != object.end())
    length += icl::length(key_value<Type>(it_++));
  return length;
}

template <typename Type>
  requires is_interval_container<Type>::value
std::size_t interval_count(const Type& object) {
  return icl::iterative_size(object);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type::difference_type distance(const Type& object) {
  using DiffT = Type::difference_type;
  using const_iterator = Type::const_iterator;
  const_iterator it_ = object.begin(), pred_;
  DiffT dist = identity_element<DiffT>::value();

  if (it_ != object.end())
    pred_ = it_++;

  while (it_ != object.end())
    dist += icl::distance(key_value<Type>(pred_++), key_value<Type>(it_++));

  return dist;
}

//==============================================================================
//= Range<IntervalSet|IntervalMap>
//==============================================================================
template <typename Type>
  requires is_interval_container<Type>::value
Type::interval_type hull(const Type& object) {
  return icl::is_empty(object)
             ? identity_element<typename Type::interval_type>::value()
             : icl::hull(key_value<Type>(object.begin()),
                         key_value<Type>(object.rbegin()));
}

template <typename Type>
  requires is_interval_container<Type>::value
domain_type_of<Type>::type lower(const Type& object) {
  using DomainT = domain_type_of<Type>::type;
  return icl::is_empty(object) ? unit_element<DomainT>::value()
                               : icl::lower(key_value<Type>(object.begin()));
}

template <typename Type>
  requires is_interval_container<Type>::value
domain_type_of<Type>::type upper(const Type& object) {
  using DomainT = domain_type_of<Type>::type;
  return icl::is_empty(object) ? identity_element<DomainT>::value()
                               : icl::upper(key_value<Type>(object.rbegin()));
}

//------------------------------------------------------------------------------
template <typename Type>
  requires(is_interval_container<Type>::value &&
           is_discrete<typename domain_type_of<Type>::type>::value)
domain_type_of<Type>::type first(const Type& object) {
  using DomainT = domain_type_of<Type>::type;
  return icl::is_empty(object) ? unit_element<DomainT>::value()
                               : icl::first(key_value<Type>(object.begin()));
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           is_discrete<typename domain_type_of<Type>::type>::value)
domain_type_of<Type>::type last(const Type& object) {
  using DomainT = domain_type_of<Type>::type;
  return icl::is_empty(object) ? identity_element<DomainT>::value()
                               : icl::last(key_value<Type>(object.rbegin()));
}

//==============================================================================
//= Addition<IntervalSet|IntervalMap>
//==============================================================================
//------------------------------------------------------------------------------
//- T& op +=(T&, c P&) T:{S}|{M} P:{e i}|{b p}
//------------------------------------------------------------------------------
/* \par \b Requires: \c OperandT is an addable derivative type of \c Type.
    \b Effects: \c operand is added to \c object.
    \par \b Returns: A reference to \c object.
    \b Complexity:
\code
                  \ OperandT:
                   \         element     segment
Type:
       interval container    O(log n)     O(n)

             interval_set               amortized
    separate_interval_set                O(log n)

n = object.interval_count()
\endcode

For the addition of \b elements or \b segments
complexity is \b logarithmic or \b linear respectively.
For \c interval_sets and \c separate_interval_sets addition of segments
is \b amortized \b logarithmic.
*/
template <typename Type, typename OperandT>
  requires is_intra_derivative<Type, OperandT>::value
Type& operator+=(Type& object, const OperandT& operand) {
  return icl::add(object, operand);
}

//------------------------------------------------------------------------------
//- T& op +=(T&, c P&) T:{S}|{M} P:{S'}|{M'}
//------------------------------------------------------------------------------
/** \par \b Requires: \c OperandT is an interval container addable to \c Type.
    \b Effects: \c operand is added to \c object.
    \par \b Returns: A reference to \c object.
    \b Complexity: loglinear */
template <typename Type, typename OperandT>
  requires is_intra_combinable<Type, OperandT>::value
Type& operator+=(Type& object, const OperandT& operand) {
  typename Type::iterator prior_ = object.end();
  for (typename OperandT::const_iterator elem_ = operand.begin();
       !(elem_ == operand.end()); ++elem_)
    prior_ = icl::add(object, prior_, *elem_);

  return object;
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator+(const Type& object, const OperandT& operand) {
  Type temp = object;
  return std::move(temp += operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator+(Type&& object, const OperandT& operand) {
  return std::move(object += operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator+(const OperandT& operand, const Type& object) {
  Type temp = object;
  return std::move(temp += operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator+(const OperandT& operand, Type&& object) {
  return std::move(object += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator+(const Type& object, const Type& operand) {
  Type temp = object;
  return std::move(temp += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator+(Type&& object, const Type& operand) {
  return std::move(object += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator+(const Type& operand, Type&& object) {
  return std::move(object += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator+(Type&& object, Type&& operand) {
  return std::move(object += operand);
}

//------------------------------------------------------------------------------
//- Addition |=, |
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//- T& op |=(c P&) T:{S}|{M} P:{e i}|{b p}
//------------------------------------------------------------------------------
/** \par \b Requires: Types \c Type and \c OperandT are addable.
    \par \b Effects: \c operand is added to \c object.
    \par \b Returns: A reference to \c object.
    \b Complexity:
\code
                  \ OperandT:                      interval
                   \         element     segment   container
Type:
       interval container    O(log n)     O(n)     O(m log(n+m))

             interval_set               amortized
    spearate_interval_set                O(log n)

n = object.interval_count()
m = operand.interval_count()
\endcode

For the addition of \b elements, \b segments and \b interval \b containers
complexity is \b logarithmic, \b linear and \b loglinear respectively.
For \c interval_sets and \c separate_interval_sets addition of segments
is \b amortized \b logarithmic.
*/
template <typename Type, typename OperandT>
  requires is_right_intra_combinable<Type, OperandT>::value
Type& operator|=(Type& object, const OperandT& operand) {
  return object += operand;
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator|(const Type& object, const OperandT& operand) {
  Type temp = object;
  return std::move(temp += operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator|(Type&& object, const OperandT& operand) {
  return std::move(object += operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator|(const OperandT& operand, const Type& object) {
  Type temp = object;
  return std::move(temp += operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator|(const OperandT& operand, Type&& object) {
  return std::move(object += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator|(const Type& object, const Type& operand) {
  Type temp = object;
  return std::move(temp += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator|(Type&& object, const Type& operand) {
  return std::move(object += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator|(const Type& operand, Type&& object) {
  return std::move(object += operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator|(Type&& object, Type&& operand) {
  return std::move(object += operand);
}

//==============================================================================
//= Insertion<IntervalSet|IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& insert(T&, c P&) T:{S}|{M} P:{S'}|{M'}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires is_intra_combinable<Type, OperandT>::value
Type& insert(Type& object, const OperandT& operand) {
  typename Type::iterator prior_ = object.end();
  for (typename OperandT::const_iterator elem_ = operand.begin();
       !(elem_ == operand.end()); ++elem_)
    insert(object, prior_, *elem_);

  return object;
}

//==============================================================================
//= Erasure<IntervalSet|IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& erase(T&, c P&) T:{S}|{M} P:{S'}|{S' M'}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires combines_right_to_interval_container<Type, OperandT>::value
Type& erase(Type& object, const OperandT& operand) {
  using const_iterator = OperandT::const_iterator;

  if (icl::is_empty(operand))
    return object;

  const_iterator common_lwb, common_upb;
  if (!Set::common_range(common_lwb, common_upb, operand, object))
    return object;

  const_iterator it_ = common_lwb;
  while (it_ != common_upb)
    icl::erase(object, *it_++);

  return object;
}

//==============================================================================
//= Subtraction<IntervalSet|IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& op -= (c P&) T:{M} P:{M'}
//------------------------------------------------------------------------------
/** \par \b Requires: Types \c Type and \c OperandT are subtractable.
    \par \b Effects: \c operand is subtracted from \c object.
    \par \b Returns: A reference to \c object.
    \b Complexity:
\code
                  \ OperandT:                      interval
                   \         element    segment    container
Type:
       interval container    O(log n)     O(n)     O(m log(n+m))

                                       amortized
            interval_sets               O(log n)

n = object.interval_count()
m = operand.interval_count()
\endcode

For the subtraction of \em elements, \b segments and \b interval \b containers
complexity is \b logarithmic, \b linear and \b loglinear respectively.
For interval sets subtraction of segments
is \b amortized \b logarithmic.
*/
template <typename Type, typename OperandT>
  requires is_concept_compatible<is_interval_map, Type, OperandT>::value
Type& operator-=(Type& object, const OperandT& operand) {
  for (typename OperandT::const_iterator elem_ = operand.begin();
       !(elem_ == operand.end()); ++elem_)
    icl::subtract(object, *elem_);

  return object;
}

//------------------------------------------------------------------------------
//- T& op -= (c P&) T:{S}|{M} P:{e i}|{b p}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires is_intra_derivative<Type, OperandT>::value
Type& operator-=(Type& object, const OperandT& operand) {
  return icl::subtract(object, operand);
}

//------------------------------------------------------------------------------
//- T& op -= (c P&) T:{M} P:{e i}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires is_cross_derivative<Type, OperandT>::value
Type& operator-=(Type& object, const OperandT& operand) {
  return icl::erase(object, operand);
}

//------------------------------------------------------------------------------
//- T& op -= (c P&) T:{S M} P:{S'}
//------------------------------------------------------------------------------
template <typename Type, typename IntervalSetT>
  requires combines_right_to_interval_set<Type, IntervalSetT>::value
Type& operator-=(Type& object, const IntervalSetT& operand) {
  return erase(object, operand);
}

template <typename Type, typename OperandT>
  requires is_right_inter_combinable<Type, OperandT>::value
Type operator-(const Type& object, const OperandT& operand) {
  Type temp = object;
  return std::move(temp -= operand);
}

template <typename Type, typename OperandT>
  requires is_right_inter_combinable<Type, OperandT>::value
Type operator-(Type&& object, const OperandT& operand) {
  return std::move(object -= operand);
}

//==============================================================================
//= Intersection<IntervalSet|IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- void add_intersection(T&, c T&, c P&) T:{S M} P:{S'}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires(is_interval_set<Type>::value &&
           combines_right_to_interval_set<Type, OperandT>::value)
void add_intersection(Type& section, const Type& object,
                      const OperandT& operand) {
  using const_iterator = OperandT::const_iterator;

  if (operand.empty())
    return;

  const_iterator common_lwb, common_upb;
  if (!Set::common_range(common_lwb, common_upb, operand, object))
    return;

  const_iterator it_ = common_lwb;
  while (it_ != common_upb)
    icl::add_intersection(section, object, key_value<OperandT>(it_++));
}

//------------------------------------------------------------------------------
//- T& op &=(T&, c P&) T:{S}|{M} P:{e i S'}|{e i b p S' M'}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires is_right_inter_combinable<Type, OperandT>::value
Type& operator&=(Type& object, const OperandT& operand) {
  Type intersection;
  add_intersection(intersection, object, operand);
  object.swap(intersection);
  return object;
}

template <typename Type, typename OperandT>
  requires is_binary_inter_combinable<Type, OperandT>::value
Type operator&(const Type& object, const OperandT& operand) {
  Type temp = object;
  return std::move(temp &= operand);
}

template <typename Type, typename OperandT>
  requires is_binary_inter_combinable<Type, OperandT>::value
Type operator&(Type&& object, const OperandT& operand) {
  return std::move(object &= operand);
}

template <typename Type, typename OperandT>
  requires is_binary_inter_combinable<Type, OperandT>::value
Type operator&(const OperandT& operand, const Type& object) {
  Type temp = object;
  return std::move(temp &= operand);
}

template <typename Type, typename OperandT>
  requires is_binary_inter_combinable<Type, OperandT>::value
Type operator&(const OperandT& operand, Type&& object) {
  return std::move(object &= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator&(const Type& object, const Type& operand) {
  Type temp = object;
  return std::move(temp &= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator&(Type&& object, const Type& operand) {
  return std::move(object &= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator&(const Type& operand, Type&& object) {
  return std::move(object &= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator&(Type&& object, Type&& operand) {
  return std::move(object &= operand);
}

//------------------------------------------------------------------------------
//- intersects<IntervalSet|IntervalMap>
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//- bool intersects(c T&, c P&) T:{S}|{M} P:{e i}
//------------------------------------------------------------------------------
template <typename Type, typename CoType>
  requires(is_interval_container<Type>::value &&
           std::is_same_v<CoType, typename domain_type_of<Type>::type>)
bool intersects(const Type& left, const CoType& right) {
  return icl::contains(left, right);
}

template <typename Type, typename CoType>
  requires(is_interval_container<Type>::value &&
           std::is_same_v<CoType, typename interval_type_of<Type>::type>)
bool intersects(const Type& left, const CoType& right) {
  return !is_empty(right) && icl::find(left, right) != left.end();
}

template <typename LeftT, typename RightT>
  requires(is_intra_combinable<LeftT, RightT>::value &&
           (is_total<LeftT>::value || is_total<RightT>::value))
bool intersects(const LeftT&, const RightT&) {
  return true;
}

template <typename LeftT, typename RightT>
  requires(is_intra_combinable<LeftT, RightT>::value &&
           (!(is_total<LeftT>::value || is_total<RightT>::value)))
bool intersects(const LeftT& left, const RightT& right) {
  using const_iterator = RightT::const_iterator;
  LeftT intersection;

  const_iterator right_common_lower_, right_common_upper_;
  if (!Set::common_range(right_common_lower_, right_common_upper_, right, left))
    return false;

  const_iterator it_ = right_common_lower_;
  while (it_ != right_common_upper_) {
    icl::add_intersection(intersection, left, *it_++);
    if (!icl::is_empty(intersection))
      return true;
  }
  return false;
}

template <typename LeftT, typename RightT>
  requires is_cross_combinable<LeftT, RightT>::value
bool intersects(const LeftT& left, const RightT& right) {
  using const_iterator = RightT::const_iterator;
  LeftT intersection;

  if (icl::is_empty(left) || icl::is_empty(right))
    return false;

  const_iterator right_common_lower_, right_common_upper_;
  if (!Set::common_range(right_common_lower_, right_common_upper_, right, left))
    return false;

  typename RightT::const_iterator it_ = right_common_lower_;
  while (it_ != right_common_upper_) {
    icl::add_intersection(intersection, left, key_value<RightT>(it_++));
    if (!icl::is_empty(intersection))
      return true;
  }

  return false;
}

/** \b Returns true, if \c left and \c right have no common elements.
    Intervals are interpreted as sequence of elements.
    \b Complexity: loglinear, if \c left and \c right are interval containers.
 */
template <typename LeftT, typename RightT>
  requires is_inter_combinable<LeftT, RightT>::value
bool disjoint(const LeftT& left, const RightT& right) {
  return !intersects(left, right);
}

/** \b Returns true, if \c left and \c right have no common elements.
    Intervals are interpreted as sequence of elements.
    \b Complexity: logarithmic, if \c AssociateT is an element type \c
   Type::element_type. linear, if \c AssociateT is a segment type \c
   Type::segment_type. */
template <typename Type, typename AssociateT>
  requires is_inter_derivative<Type, AssociateT>::value
bool disjoint(const Type& left, const AssociateT& right) {
  return !intersects(left, right);
}

//==============================================================================
//= Symmetric difference<IntervalSet|IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- Symmetric difference ^=, ^
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//- T& op ^=(T&, c P&) T:{S}|{M} P:{S'}|{M'}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires is_intra_combinable<Type, OperandT>::value
Type& operator^=(Type& object, const OperandT& operand) {
  return icl::flip(object, operand);
}

//------------------------------------------------------------------------------
//- T& op ^=(T&, c P&) T:{S}|{M} P:{e i}|{b p}
//------------------------------------------------------------------------------
template <typename Type, typename OperandT>
  requires is_intra_derivative<Type, OperandT>::value
Type& operator^=(Type& object, const OperandT& operand) {
  return icl::flip(object, operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator^(const Type& object, const OperandT& operand) {
  Type temp = object;
  return std::move(temp ^= operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator^(Type&& object, const OperandT& operand) {
  return std::move(object ^= operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator^(const OperandT& operand, const Type& object) {
  Type temp = object;
  return std::move(temp ^= operand);
}

template <typename Type, typename OperandT>
  requires is_binary_intra_combinable<Type, OperandT>::value
Type operator^(const OperandT& operand, Type&& object) {
  return std::move(object ^= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator^(const Type& object, const Type& operand) {
  Type temp = object;
  return std::move(temp ^= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator^(Type&& object, const Type& operand) {
  return std::move(object ^= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator^(const Type& operand, Type&& object) {
  return std::move(object ^= operand);
}

template <typename Type>
  requires is_interval_container<Type>::value
Type operator^(Type&& object, Type&& operand) {
  return std::move(object ^= operand);
}

//==========================================================================
//= Element Iteration <IntervalSet|IntervalMap>
//==========================================================================
//--------------------------------------------------------------------------
//- Forward
//--------------------------------------------------------------------------
template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
Type::element_iterator elements_begin(Type& object) {
  return typename Type::element_iterator(object.begin());
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
Type::element_iterator

elements_end(Type& object) {
  return typename Type::element_iterator(object.end());
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           !is_continuous_interval<typename Type::interval_type>::value)
Type::element_const_iterator elements_begin(const Type& object) {
  return typename Type::element_const_iterator(object.begin());
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
Type::element_const_iterator elements_end(const Type& object) {
  return typename Type::element_const_iterator(object.end());
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
std::ranges::subrange<typename Type::element_iterator> elements(Type& object) {
  return make_iterator_range(typename Type::element_iterator(object.begin()),
                             typename Type::element_iterator(object.end()));
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
std::ranges::subrange<typename Type::element_const_iterator>
elements(Type const& object) {
  return make_iterator_range(
      typename Type::element_const_iterator(object.begin()),
      typename Type::element_const_iterator(object.end()));
}

//--------------------------------------------------------------------------
//- Reverse
//--------------------------------------------------------------------------
template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
Type::element_reverse_iterator elements_rbegin(Type& object) {
  return typename Type::element_reverse_iterator(object.rbegin());
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
Type::element_reverse_iterator elements_rend(Type& object) {
  return typename Type::element_reverse_iterator(object.rend());
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
Type::element_const_reverse_iterator elements_rbegin(const Type& object) {
  return typename Type::element_const_reverse_iterator(object.rbegin());
}

template <typename Type>
  requires(is_interval_container<Type>::value &&
           (!is_continuous_interval<typename Type::interval_type>::value))
Type::element_const_reverse_iterator elements_rend(const Type& object) {
  return typename Type::element_const_reverse_iterator(object.rend());
}

} // namespace icl
