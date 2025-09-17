#pragma once

#include "icl/concept/interval.hpp"
#include "icl/concept/interval_associator_base.hpp"
#include "icl/detail/interval_set_algo.hpp"
#include "icl/detail/set_algo.hpp"
#include "icl/type_traits/is_combinable.hpp"

namespace icl {

//==============================================================================
//= Containedness<IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- bool contains(c T&, c P&) T:{S} P:{e i S} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
bool contains(const Type& super, const typename Type::element_type& element) {
  return !(icl::find(super, element) == super.end());
}

template <typename Type>
  requires is_interval_set<Type>::value
bool contains(const Type& super, const typename Type::segment_type& inter_val) {
  using const_iterator = Type::const_iterator;
  if (icl::is_empty(inter_val))
    return true;

  std::pair<const_iterator, const_iterator> exterior =
      super.equal_range(inter_val);
  if (exterior.first == exterior.second)
    return false;

  const_iterator last_overlap = cyclic_prior(super, exterior.second);

  return icl::contains(hull(*exterior.first, *last_overlap), inter_val) &&
         Interval_Set::is_joinable(super, exterior.first, last_overlap);
}

template <typename Type, typename OperandT>
  requires has_same_concept<is_interval_set, Type, OperandT>::value
bool contains(const Type& super, const OperandT& sub) {
  return Interval_Set::contains(super, sub);
}

//==============================================================================
//= Addition<IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& add(T&, c P&) T:{S} P:{e i} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
Type& add(Type& object, const typename Type::segment_type& operand) {
  return object.add(operand);
}

template <typename Type>
  requires is_interval_set<Type>::value
Type& add(Type& object, const typename Type::element_type& operand) {
  using segment_type = Type::segment_type;
  return icl::add(object, icl::singleton<segment_type>(operand));
}

//------------------------------------------------------------------------------
//- T& add(T&, J, c P&) T:{S} P:{i} interval_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
Type::iterator add(Type& object, typename Type::iterator prior,
                   const typename Type::segment_type& operand) {
  return object.add(prior, operand);
}

//==============================================================================
//= Insertion<IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& insert(T&, c P&) T:{S} P:{e i} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
Type& insert(Type& object, const typename Type::segment_type& operand) {
  return icl::add(object, operand);
}

template <typename Type>
  requires is_interval_set<Type>::value
Type& insert(Type& object, const typename Type::element_type& operand) {
  return icl::add(object, operand);
}

//------------------------------------------------------------------------------
//- T& insert(T&, J, c P&) T:{S} P:{i} with hint
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
Type::iterator insert(Type& object, typename Type::iterator prior,
                      const typename Type::segment_type& operand) {
  return icl::add(object, prior, operand);
}

//==============================================================================
//= Subtraction<IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& subtract(T&, c P&) T:{S} P:{e i} fragment_type
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
Type& subtract(Type& object, const typename Type::segment_type& operand) {
  return object.subtract(operand);
}

template <typename Type>
  requires is_interval_set<Type>::value
Type& subtract(Type& object, const typename Type::element_type& operand) {
  using segment_type = Type::segment_type;
  return icl::subtract(object, icl::singleton<segment_type>(operand));
}

//==============================================================================
//= Erasure<IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& erase(T&, c P&) T:{S} P:{e i} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
Type& erase(Type& object, const typename Type::segment_type& minuend) {
  return icl::subtract(object, minuend);
}

template <typename Type>
  requires is_interval_set<Type>::value
Type& erase(Type& object, const typename Type::element_type& minuend) {
  return icl::subtract(object, minuend);
}

//==============================================================================
//= Intersection
//==============================================================================
//------------------------------------------------------------------------------
//- void add_intersection(T&, c T&, c P&) T:{S} P:{e i} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
void add_intersection(Type& section, const Type& object,
                      const typename Type::element_type& operand) {
  using const_iterator = Type::const_iterator;
  const_iterator found = icl::find(object, operand);
  if (found != object.end())
    icl::add(section, operand);
}

template <typename Type>
  requires is_interval_set<Type>::value
void add_intersection(Type& section, const Type& object,
                      const typename Type::segment_type& segment) {
  using const_iterator = Type::const_iterator;
  using iterator = Type::iterator;
  using interval_type = Type::interval_type;

  if (icl::is_empty(segment))
    return;

  std::pair<const_iterator, const_iterator> exterior =
      object.equal_range(segment);
  if (exterior.first == exterior.second)
    return;

  iterator prior_ = section.end();
  for (const_iterator it_ = exterior.first; it_ != exterior.second; ++it_) {
    interval_type common_interval = key_value<Type>(it_) & segment;
    if (!icl::is_empty(common_interval))
      prior_ = section.insert(prior_, common_interval);
  }
}

//==============================================================================
//= Symmetric difference<IntervalSet>
//==============================================================================
//------------------------------------------------------------------------------
//- T& flip(T&, c P&) T:{S} P:{e i S'} fragment_types
//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval_set<Type>::value
Type& flip(Type& object, const typename Type::element_type& operand) {
  if (icl::contains(object, operand))
    return object -= operand;

  return object += operand;
}

template <typename Type>
  requires is_interval_set<Type>::value
Type& flip(Type& object, const typename Type::segment_type& segment) {
  using const_iterator = Type::const_iterator;
  using interval_type = Type::interval_type;
  // That which is common shall be subtracted
  // That which is not shall be added
  // So x has to be 'complementary added' or flipped
  interval_type span = segment;
  std::pair<const_iterator, const_iterator> exterior = object.equal_range(span);

  const_iterator fst_ = exterior.first;
  const_iterator end_ = exterior.second;

  interval_type covered, left_over;
  const_iterator it_ = fst_;
  while (it_ != end_) {
    covered = *it_++;
    //[a      ...  : span
    //     [b ...  : covered
    //[a  b)       : left_over
    left_over = right_subtract(span, covered);
    icl::subtract(object,
                  span & covered); // That which is common shall be subtracted
    icl::add(object, left_over);   // That which is not shall be added

    //...      d) : span
    //... c)      : covered
    //     [c  d) : span'
    span = left_subtract(span, covered);
  }

  // If span is not empty here, it_ is not in the set so it_ shall be added
  icl::add(object, span);
  return object;
}

template <typename Type, typename OperandT>
  requires is_concept_compatible<is_interval_set, Type, OperandT>::value
Type& flip(Type& object, const OperandT& operand) {
  using const_iterator = OperandT::const_iterator;

  if (operand.empty())
    return object;

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
template <typename Type>
  requires is_interval_set<Type>::value
Type& domain(Type& dom, const Type& object) {
  using const_iterator = Type::const_iterator;
  using iterator = Type::iterator;
  dom.clear();
  const_iterator it_ = object.begin();
  iterator prior_ = dom.end();

  while (it_ != object.end())
    prior_ = icl::insert(dom, prior_, *it_++);

  return dom;
}

template <typename Type>
  requires is_interval_set<Type>::value
Type& between(Type& in_between, const Type& object) {
  using const_iterator = Type::const_iterator;
  using iterator = Type::iterator;
  in_between.clear();
  const_iterator it_ = object.begin(), pred_;
  iterator prior_ = in_between.end();

  if (it_ != object.end())
    pred_ = it_++;

  while (it_ != object.end())
    prior_ = icl::insert(in_between, prior_, icl::between(*pred_++, *it_++));

  return in_between;
}

//==============================================================================
//= Streaming
//==============================================================================
template <typename CharType, typename CharTraits, typename Type>
  requires is_interval_set<Type>::value
std::basic_ostream<CharType, CharTraits>&
operator<<(std::basic_ostream<CharType, CharTraits>& stream,
           const Type& object) {
  stream << "{";
  for (typename Type::const_iterator it_ = object.begin();
       !(it_ == object.end()); ++it_)
    stream << *it_;

  return stream << "}";
}

} // namespace icl