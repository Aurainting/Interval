#pragma once

#include "icl/interval_bounds.hpp"
#include "icl/type_traits/is_discrete.hpp"
#include "icl/type_traits/is_interval.hpp"
#include "icl/type_traits/succ_pred.hpp"

namespace icl {

inline interval_bounds left(interval_bounds x1) {
  return interval_bounds(x1._bits & interval_bounds::_left);
}

inline interval_bounds right(interval_bounds x1) {
  return interval_bounds(x1._bits & interval_bounds::_right);
}

inline interval_bounds all(interval_bounds x1) {
  return interval_bounds(x1._bits & interval_bounds::_all);
}

inline bool operator==(const interval_bounds x1, const interval_bounds x2) {
  return x1._bits == x2._bits;
}

inline bool operator!=(const interval_bounds x1, const interval_bounds x2) {
  return x1._bits != x2._bits;
}

inline interval_bounds operator&(interval_bounds x1, interval_bounds x2) {
  return interval_bounds(x1._bits & x2._bits);
}

inline interval_bounds operator|(interval_bounds x1, interval_bounds x2) {
  return interval_bounds(x1._bits | x2._bits);
}

// left shift (multiplies by 2^shift)
inline interval_bounds operator<<(interval_bounds bounds, unsigned int shift) {
  return interval_bounds(bounds._bits << shift);
}

// right shift (divides by 2^shift)
inline interval_bounds operator>>(interval_bounds bounds, unsigned int shift) {
  return interval_bounds(bounds._bits >> shift);
}

inline interval_bounds operator~(interval_bounds x1) {
  return all(interval_bounds(~(x1._bits)));
}

inline interval_bounds outer_bounds(interval_bounds x1, interval_bounds x2) {
  return left(x1) | right(x2);
}

inline interval_bounds inner_bounds(interval_bounds x1, interval_bounds x2) {
  return interval_bounds(x1.reverse_right() | x2.reverse_left());
}

inline interval_bounds left_bounds(interval_bounds x1, interval_bounds x2) {
  return left(x1) | left(x2) >> 1;
}

inline interval_bounds right_bounds(interval_bounds x1, interval_bounds x2) {
  return right(x1) << 1 | right(x2);
}

inline interval_bounds left_subtract_bounds(interval_bounds x1,
                                            interval_bounds x2) {
  return right(x1) | ~(right(x2) << 1);
}

inline interval_bounds right_subtract_bounds(interval_bounds x1,
                                             interval_bounds x2) {
  return left(x1) | ~(left(x2) >> 1);
}

inline bool is_complementary(interval_bounds x1) {
  return x1 == interval_bounds::right_open() ||
         x1 == interval_bounds::left_open();
}

inline bool is_left_closed(interval_bounds bounds) {
  return bounds.left().bits() == 2;
}

inline bool is_right_closed(interval_bounds bounds) {
  return bounds.right().bits() == 1;
}

inline std::string left_bracket(interval_bounds bounds) {
  return is_left_closed(bounds) ? "[" : "(";
}

inline std::string right_bracket(interval_bounds bounds) {
  return is_right_closed(bounds) ? "]" : ")";
}

template <typename Type>
  requires is_discrete<Type>::value
Type shift_lower(interval_bounds decl, interval_bounds repr, const Type& low) {
  if (is_left_closed(decl) && !is_left_closed(repr))
    return icl::pred(low);
  if (!is_left_closed(decl) && is_left_closed(repr))
    return icl::succ(low);
  return low;
}

template <typename Type>
  requires is_discrete<Type>::value
Type shift_upper(interval_bounds decl, interval_bounds repr, const Type& up) {
  if (!is_right_closed(decl) && is_right_closed(repr))
    return icl::pred(up);
  if (is_right_closed(decl) && !is_right_closed(repr))
    return icl::succ(up);
  return up;
}

template <typename CharType, typename CharTraits>
std::basic_ostream<CharType, CharTraits>&
operator<<(std::basic_ostream<CharType, CharTraits>& stream,
           interval_bounds const& object) {
  return stream << left_bracket(object) << right_bracket(object);
}

template <typename IntervalT>
  requires has_dynamic_bounds<IntervalT>::value
interval_bounds outer_bounds(const IntervalT& x1, const IntervalT& x2) {
  return outer_bounds(x1.bounds(), x2.bounds());
}

template <typename IntervalT>
  requires has_dynamic_bounds<IntervalT>::value
interval_bounds inner_bounds(const IntervalT& x1, const IntervalT& x2) {
  return inner_bounds(x1.bounds(), x2.bounds());
}

template <typename IntervalT>
  requires has_dynamic_bounds<IntervalT>::value
interval_bounds left_bounds(const IntervalT& x1, const IntervalT& x2) {
  return left_bounds(x1.bounds(), x2.bounds());
}

template <typename IntervalT>
  requires has_dynamic_bounds<IntervalT>::value
interval_bounds right_bounds(const IntervalT& x1, const IntervalT& x2) {
  return right_bounds(x1.bounds(), x2.bounds());
}

template <typename IntervalT>
  requires has_dynamic_bounds<IntervalT>::value
interval_bounds left_subtract_bounds(const IntervalT& x1, const IntervalT& x2) {
  return left_subtract_bounds(x1.bounds(), x2.bounds());
}

template <typename IntervalT>
  requires has_dynamic_bounds<IntervalT>::value
interval_bounds right_subtract_bounds(const IntervalT& x1,
                                      const IntervalT& x2) {
  return right_subtract_bounds(x1.bounds(), x2.bounds());
}

} // namespace icl
