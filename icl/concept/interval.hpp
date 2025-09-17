#pragma once

#include "icl/concept/interval_bounds.hpp"
#include "icl/dynamic_interval_traits.hpp"
#include "icl/interval_bounds.hpp"
#include "icl/interval_traits.hpp"
#include "icl/type_traits/identity_element.hpp"
#include "icl/type_traits/infinity.hpp"
#include "icl/type_traits/is_asymmetric_interval.hpp"
#include "icl/type_traits/is_continuous.hpp"
#include "icl/type_traits/is_continuous_interval.hpp"
#include "icl/type_traits/is_discrete.hpp"
#include "icl/type_traits/is_discrete_interval.hpp"
#include "icl/type_traits/is_interval.hpp"
#include "icl/type_traits/is_numeric.hpp"
#include "icl/type_traits/succ_pred.hpp"
#include "icl/type_traits/unit_element.hpp"
#include <algorithm>
#include <cassert>
#include <iterator>

namespace icl {

//==============================================================================
//= Ordering
//==============================================================================
template <typename Type>
  requires is_interval<Type>::value
bool domain_less(const typename interval_traits<Type>::domain_type& left,
                 const typename interval_traits<Type>::domain_type& right) {
  return typename interval_traits<Type>::domain_compare()(left, right);
}

template <typename Type>
  requires is_interval<Type>::value
bool domain_less_equal(
    const typename interval_traits<Type>::domain_type& left,
    const typename interval_traits<Type>::domain_type& right) {
  return !(typename interval_traits<Type>::domain_compare()(right, left));
}

template <typename Type>
  requires is_interval<Type>::value
bool domain_equal(const typename interval_traits<Type>::domain_type& left,
                  const typename interval_traits<Type>::domain_type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;
  return !domain_compare()(left, right) && !domain_compare()(right, left);
}

template <typename Type>
  requires is_interval<Type>::value
interval_traits<Type>::domain_type
domain_next(const typename interval_traits<Type>::domain_type value) {
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  return successor<domain_type, domain_compare>::apply(value);
}

template <typename Type>
  requires is_interval<Type>::value
interval_traits<Type>::domain_type
domain_prior(const typename interval_traits<Type>::domain_type value) {
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  return predecessor<domain_type, domain_compare>::apply(value);
}

//==============================================================================
//= Construct<Interval> singleton
//==============================================================================
template <typename Type>
  requires(is_static_right_open<Type>::value &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
Type singleton(const typename interval_traits<Type>::domain_type& value) {
  // ASSERT: This always creates an interval with exactly one element
  return interval_traits<Type>::construct(value, domain_next<Type>(value));
}

template <typename Type>
  requires(is_static_left_open<Type>::value &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
Type singleton(const typename interval_traits<Type>::domain_type& value) {
  // ASSERT: This always creates an interval with exactly one element
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  static_assert(
      numeric_minimum<domain_type, domain_compare,
                      is_numeric<domain_type>::value>::is_less_than(value));

  return interval_traits<Type>::construct(domain_prior<Type>(value), value);
}

template <typename Type>
  requires is_discrete_static_open<Type>::value
Type singleton(const typename interval_traits<Type>::domain_type& value) {
  // ASSERT: This always creates an interval with exactly one element
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  static_assert(
      numeric_minimum<domain_type, domain_compare,
                      is_numeric<domain_type>::value>::is_less_than(value));

  return interval_traits<Type>::construct(domain_prior<Type>(value),
                                          domain_next<Type>(value));
}

template <typename Type>
  requires is_discrete_static_closed<Type>::value
Type singleton(const typename interval_traits<Type>::domain_type& value) {
  // ASSERT: This always creates an interval with exactly one element
  return interval_traits<Type>::construct(value, value);
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type singleton(const typename interval_traits<Type>::domain_type& value) {
  return dynamic_interval_traits<Type>::construct(value, value,
                                                  interval_bounds::closed());
}

namespace detail {

//==============================================================================
//= Construct<Interval> unit_trail == generalized singleton
// The smallest interval on an incrementable (and decrementable) type that can
// be constructed using ++ and -- and such that it contains a given value.
// If 'Type' is discrete, 'unit_trail' and 'singleton' are identical. So we
// can view 'unit_trail' as a generalized singleton for static intervals of
// continuous types.
//==============================================================================
template <typename Type>
  requires(is_static_right_open<Type>::value &&
           std::incrementable<typename interval_traits<Type>::domain_type>)
Type unit_trail(const typename interval_traits<Type>::domain_type& value) {
  return interval_traits<Type>::construct(value, domain_next<Type>(value));
}

template <typename Type>
  requires(is_static_left_open<Type>::value &&
           std::incrementable<typename interval_traits<Type>::domain_type>)
Type unit_trail(const typename interval_traits<Type>::domain_type& value) {
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  static_assert(
      numeric_minimum<domain_type, domain_compare,
                      is_numeric<domain_type>::value>::is_less_than(value));

  return interval_traits<Type>::construct(domain_prior<Type>(value), value);
}

template <typename Type>
  requires(is_static_open<Type>::value &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
Type unit_trail(const typename interval_traits<Type>::domain_type& value) {
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  static_assert(
      numeric_minimum<domain_type, domain_compare,
                      is_numeric<domain_type>::value>::is_less_than(value));

  return interval_traits<Type>::construct(domain_prior<Type>(value),
                                          domain_next<Type>(value));
}

template <typename Type>
  requires(is_static_closed<Type>::value &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
Type unit_trail(const typename interval_traits<Type>::domain_type& value) {
  return interval_traits<Type>::construct(value, value);
}

// NOTE: statically bounded closed or open intervals of continuous domain types
//  are NOT supported by ICL. They can not be used with interval containers
//  consistently.

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type unit_trail(const typename interval_traits<Type>::domain_type& value) {
  return dynamic_interval_traits<Type>::construct(value, value,
                                                  interval_bounds::closed());
}

} // namespace detail

//==============================================================================
//= Construct<Interval> multon
//==============================================================================
template <typename Type>
  requires has_static_bounds<Type>::value
Type construct(const typename interval_traits<Type>::domain_type& low,
               const typename interval_traits<Type>::domain_type& up) {
  return interval_traits<Type>::construct(low, up);
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type construct(const typename interval_traits<Type>::domain_type& low,
               const typename interval_traits<Type>::domain_type& up,
               interval_bounds bounds = interval_bounds::right_open()) {
  return dynamic_interval_traits<Type>::construct(low, up, bounds);
}

//- construct form bounded values ----------------------------------------------
template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type construct(const typename Type::bounded_domain_type& low,
               const typename Type::bounded_domain_type& up) {
  return dynamic_interval_traits<Type>::construct_bounded(low, up);
}

template <typename Type>
  requires is_interval<Type>::value
Type span(const typename interval_traits<Type>::domain_type& left,
          const typename interval_traits<Type>::domain_type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;
  if (domain_compare()(left, right))
    return construct<Type>(left, right);

  return construct<Type>(right, left);
}

//==============================================================================
template <typename Type>
  requires is_static_right_open<Type>::value
Type hull(const typename interval_traits<Type>::domain_type& left,
          const typename interval_traits<Type>::domain_type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;
  if (domain_compare()(left, right))
    return construct<Type>(left, domain_next<Type>(right));

  return construct<Type>(right, domain_next<Type>(left));
}

template <typename Type>
  requires is_static_left_open<Type>::value
Type hull(const typename interval_traits<Type>::domain_type& left,
          const typename interval_traits<Type>::domain_type& right) {
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  if (domain_compare()(left, right)) {
    static_assert(
        numeric_minimum<domain_type, domain_compare,
                        is_numeric<domain_type>::value>::is_less_than(left));
    return construct<Type>(domain_prior<Type>(left), right);
  }
  static_assert(
      numeric_minimum<domain_type, domain_compare,
                      is_numeric<domain_type>::value>::is_less_than(right));
  return construct<Type>(domain_prior<Type>(right), left);
}

template <typename Type>
  requires is_static_closed<Type>::value
Type hull(const typename interval_traits<Type>::domain_type& left,
          const typename interval_traits<Type>::domain_type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;
  if (domain_compare()(left, right))
    return construct<Type>(left, right);
  return construct<Type>(right, left);
}

template <typename Type>
  requires is_static_open<Type>::value
Type hull(const typename interval_traits<Type>::domain_type& left,
          const typename interval_traits<Type>::domain_type& right) {
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;

  if (domain_compare()(left, right)) {
    static_assert(
        numeric_minimum<domain_type, domain_compare,
                        is_numeric<domain_type>::value>::is_less_than(left));
    return construct<Type>(domain_prior<Type>(left), domain_next<Type>(right));
  }
  static_assert(
      numeric_minimum<domain_type, domain_compare,
                      is_numeric<domain_type>::value>::is_less_than(right));
  return construct<Type>(domain_prior<Type>(right), domain_next<Type>(left));
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type hull(const typename interval_traits<Type>::domain_type& left,
          const typename interval_traits<Type>::domain_type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;
  if (domain_compare()(left, right))
    return construct<Type>(left, right, interval_bounds::closed());
  return construct<Type>(right, left, interval_bounds::closed());
}

//==============================================================================
//= Selection
//==============================================================================

template <typename Type>
  requires is_interval<Type>::value
interval_traits<Type>::domain_type lower(const Type& object) {
  return interval_traits<Type>::lower(object);
}

template <typename Type>
  requires is_interval<Type>::value
interval_traits<Type>::domain_type upper(const Type& object) {
  return interval_traits<Type>::upper(object);
}

//- first ----------------------------------------------------------------------
template <typename Type>
  requires(is_static_right_open<Type>::value || is_static_closed<Type>::value)
interval_traits<Type>::domain_type first(const Type& object) {
  return lower(object);
}

template <typename Type>
  requires((is_static_left_open<Type>::value || is_static_open<Type>::value) &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
interval_traits<Type>::domain_type first(const Type& object) {
  return domain_next<Type>(lower(object));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
interval_traits<Type>::domain_type first(const Type& object) {
  return is_left_closed(object.bounds()) ? lower(object)
                                         : domain_next<Type>(lower(object));
}

//- last -----------------------------------------------------------------------
template <typename Type>
  requires(is_static_left_open<Type>::value || is_static_closed<Type>::value)
interval_traits<Type>::domain_type last(const Type& object) {
  return upper(object);
}

template <typename Type>
  requires((is_static_right_open<Type>::value || is_static_open<Type>::value) &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
interval_traits<Type>::domain_type last(const Type& object) {
  using domain_type = interval_traits<Type>::domain_type;
  using domain_compare = interval_traits<Type>::domain_compare;
  static_assert(numeric_minimum<
                domain_type, domain_compare,
                is_numeric<domain_type>::value>::is_less_than(upper(object)));

  return domain_prior<Type>(upper(object));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
interval_traits<Type>::domain_type last(const Type& object) {
  return is_right_closed(object.bounds()) ? upper(object)
                                          : domain_prior<Type>(upper(object));
}

//- last_next ------------------------------------------------------------------
template <typename Type>
  requires((is_static_left_open<Type>::value ||
            is_static_closed<Type>::value) &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
interval_traits<Type>::domain_type last_next(const Type& object) {
  return domain_next<Type>(upper(object));
}

template <typename Type>
  requires((is_static_right_open<Type>::value || is_static_open<Type>::value) &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
interval_traits<Type>::domain_type last_next(const Type& object) {
  return upper(
      object); // NOTE: last_next is implemented to avoid calling pred(object)
} // For unsigned integral types this may cause underflow.

template <typename Type>
  requires is_discrete_interval<Type>::value
interval_traits<Type>::domain_type last_next(const Type& object) {
  return is_right_closed(object.bounds()) ? domain_next<Type>(upper(object))
                                          : upper(object);
}

//------------------------------------------------------------------------------
template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type bounded_lower(const Type& object) {
  return
      typename Type::bounded_domain_type(lower(object), object.bounds().left());
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type reverse_bounded_lower(const Type& object) {
  return typename Type::bounded_domain_type(lower(object),
                                            object.bounds().reverse_left());
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type bounded_upper(const Type& object) {
  return typename Type::bounded_domain_type(upper(object),
                                            object.bounds().right());
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type reverse_bounded_upper(const Type& object) {
  return typename Type::bounded_domain_type(upper(object),
                                            object.bounds().reverse_right());
}

//- bounds ---------------------------------------------------------------------
template <typename Type>
  requires has_dynamic_bounds<Type>::value
interval_bounds bounds(const Type& object) {
  return object.bounds();
}

template <typename Type>
  requires has_static_bounds<Type>::value
interval_bounds bounds(const Type&) {
  return interval_bounds(interval_bound_type<Type>::value);
}

//==============================================================================
//= Emptiness
//==============================================================================
/** Is the interval empty? */
template <typename Type>
  requires is_asymmetric_interval<Type>::value
bool is_empty(const Type& object) {
  return domain_less_equal<Type>(upper(object), lower(object));
}

template <typename Type>
  requires is_static_closed<Type>::value
bool is_empty(const Type& object) {
  return domain_less<Type>(upper(object), lower(object));
}

template <typename Type>
  requires is_static_open<Type>::value
bool is_empty(const Type& object) {
  return domain_less_equal<Type>(upper(object), lower(object)) ||
         domain_less_equal<Type>(upper(object),
                                 domain_next<Type>(lower(object)));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool is_empty(const Type& object) {
  if (object.bounds() == interval_bounds::closed())
    return domain_less<Type>(upper(object), lower(object));
  if (object.bounds() == interval_bounds::open())
    return domain_less_equal<Type>(upper(object), lower(object)) ||
           domain_less_equal<Type>(upper(object),
                                   domain_next<Type>(lower(object)));
  return domain_less_equal<Type>(upper(object), lower(object));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool is_empty(const Type& object) {
  return domain_less<Type>(upper(object), lower(object)) ||
         (domain_equal<Type>(upper(object), lower(object)) &&
          object.bounds() != interval_bounds::closed());
}

//==============================================================================
//= Equivalences and Orderings
//==============================================================================
//- exclusive_less -------------------------------------------------------------
/** Maximal element of <tt>left</tt> is less than the minimal element of
    <tt>right</tt> */
template <typename Type>
  requires is_asymmetric_interval<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  return icl::is_empty(left) || icl::is_empty(right) ||
         domain_less_equal<Type>(upper(left), lower(right));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  return icl::is_empty(left) || icl::is_empty(right) ||
         domain_less<Type>(last(left), first(right));
}

template <typename Type>
  requires has_symmetric_bounds<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  return icl::is_empty(left) || icl::is_empty(right) ||
         domain_less<Type>(last(left), first(right));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  return icl::is_empty(left) || icl::is_empty(right) ||
         domain_less<Type>(upper(left), lower(right)) ||
         (domain_equal<Type>(upper(left), lower(right)) &&
          inner_bounds(left, right) != interval_bounds::open());
}

//------------------------------------------------------------------------------
template <typename Type>
  requires has_static_bounds<Type>::value
bool lower_less(const Type& left, const Type& right) {
  return domain_less<Type>(lower(left), lower(right));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool lower_less(const Type& left, const Type& right) {
  return domain_less<Type>(first(left), first(right));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool lower_less(const Type& left, const Type& right) {
  if (left_bounds(left, right) == interval_bounds::right_open()) //'[(' == 10
    return domain_less_equal<Type>(lower(left), lower(right));
  return domain_less<Type>(lower(left), lower(right));
}

//------------------------------------------------------------------------------
template <typename Type>
  requires has_static_bounds<Type>::value
bool upper_less(const Type& left, const Type& right) {
  return domain_less<Type>(upper(left), upper(right));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool upper_less(const Type& left, const Type& right) {
  return domain_less<Type>(last(left), last(right));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool upper_less(const Type& left, const Type& right) {
  if (right_bounds(left, right) == interval_bounds::left_open())
    return domain_less_equal<Type>(upper(left), upper(right));
  return domain_less<Type>(upper(left), upper(right));
}

//------------------------------------------------------------------------------
template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type lower_min(const Type& left, const Type& right) {
  return lower_less(left, right) ? bounded_lower(left) : bounded_lower(right);
}

//------------------------------------------------------------------------------
template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type lower_max(const Type& left, const Type& right) {
  return lower_less(left, right) ? bounded_lower(right) : bounded_lower(left);
}

//------------------------------------------------------------------------------
template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type upper_max(const Type& left, const Type& right) {
  return upper_less(left, right) ? bounded_upper(right) : bounded_upper(left);
}

//------------------------------------------------------------------------------
template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type::bounded_domain_type upper_min(const Type& left, const Type& right) {
  return upper_less(left, right) ? bounded_upper(left) : bounded_upper(right);
}

//------------------------------------------------------------------------------
template <typename Type>
  requires is_asymmetric_interval<Type>::value
bool lower_equal(const Type& left, const Type& right) {
  return domain_equal<Type>(lower(left), lower(right));
}

template <typename Type>
  requires has_symmetric_bounds<Type>::value
bool lower_equal(const Type& left, const Type& right) {
  return domain_equal<Type>(first(left), first(right));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool lower_equal(const Type& left, const Type& right) {
  return domain_equal<Type>(first(left), first(right));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool lower_equal(const Type& left, const Type& right) {
  return (left.bounds().left() == right.bounds().left()) &&
         domain_equal<Type>(lower(left), lower(right));
}

//------------------------------------------------------------------------------
template <typename Type>
  requires is_asymmetric_interval<Type>::value
bool upper_equal(const Type& left, const Type& right) {
  return domain_equal<Type>(upper(left), upper(right));
}

template <typename Type>
  requires has_symmetric_bounds<Type>::value
bool upper_equal(const Type& left, const Type& right) {
  return domain_equal<Type>(last(left), last(right));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool upper_equal(const Type& left, const Type& right) {
  return domain_equal<Type>(last(left), last(right));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool upper_equal(const Type& left, const Type& right) {
  return (left.bounds().right() == right.bounds().right()) &&
         domain_equal<Type>(upper(left), upper(right));
}

//------------------------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool lower_less_equal(const Type& left, const Type& right) {
  return lower_less(left, right) || lower_equal(left, right);
}

template <typename Type>
  requires is_interval<Type>::value
bool upper_less_equal(const Type& left, const Type& right) {
  return upper_less(left, right) || upper_equal(left, right);
}

//==============================================================================
//= Orderings, containedness (non empty)
//==============================================================================
namespace non_empty {

template <typename Type>
  requires is_asymmetric_interval<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  assert(!(icl::is_empty(left) || icl::is_empty(right)));
  return domain_less_equal<Type>(upper(left), lower(right));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  return domain_less<Type>(last(left), first(right));
}

template <typename Type>
  requires has_symmetric_bounds<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  assert(!(icl::is_empty(left) || icl::is_empty(right)));
  return domain_less<Type>(last(left), first(right));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool exclusive_less(const Type& left, const Type& right) {
  return domain_less<Type>(upper(left), lower(right)) ||
         (domain_equal<Type>(upper(left), lower(right)) &&
          inner_bounds(left, right) != interval_bounds::open());
}

template <typename Type>
  requires is_interval<Type>::value
bool contains(const Type& super, const Type& sub) {
  return lower_less_equal(super, sub) && upper_less_equal(sub, super);
}

} // namespace non_empty

//- contains -------------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool contains(const Type& super, const Type& sub) {
  return icl::is_empty(sub) || non_empty::contains(super, sub);
}

template <typename Type>
  requires is_discrete_static<Type>::value
bool contains(const Type& super,
              const typename interval_traits<Type>::domain_type& element) {
  return domain_less_equal<Type>(icl::first(super), element) &&
         domain_less_equal<Type>(element, icl::last(super));
}

template <typename Type>
  requires is_continuous_left_open<Type>::value
bool contains(const Type& super,
              const typename interval_traits<Type>::domain_type& element) {
  return domain_less<Type>(icl::lower(super), element) &&
         domain_less_equal<Type>(element, icl::upper(super));
}

template <typename Type>
  requires is_continuous_right_open<Type>::value
bool contains(const Type& super,
              const typename interval_traits<Type>::domain_type& element) {
  return domain_less_equal<Type>(icl::lower(super), element) &&
         domain_less<Type>(element, icl::upper(super));
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
bool contains(const Type& super,
              const typename interval_traits<Type>::domain_type& element) {
  return (is_left_closed(super.bounds())
              ? domain_less_equal<Type>(lower(super), element)
              : domain_less<Type>(lower(super), element)) &&
         (is_right_closed(super.bounds())
              ? domain_less_equal<Type>(element, upper(super))
              : domain_less<Type>(element, upper(super)));
}

//- within ---------------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool within(const Type& sub, const Type& super) {
  return contains(super, sub);
}

//- operator == ----------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool operator==(const Type& left, const Type& right) {
  return (icl::is_empty(left) && icl::is_empty(right)) ||
         (lower_equal(left, right) && upper_equal(left, right));
}

template <typename Type>
  requires is_interval<Type>::value
bool operator!=(const Type& left, const Type& right) {
  return !(left == right);
}

//- operator < -----------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool operator<(const Type& left, const Type& right) {
  return !icl::is_empty(left) && !icl::is_empty(right) &&
         (lower_less(left, right) ||
          (lower_equal(left, right) && upper_less(left, right)));
}

template <typename Type>
  requires is_interval<Type>::value
bool operator>(const Type& left, const Type& right) {
  return right < left;
}

//- operator <= ----------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool operator<=(const Type& left, const Type& right) {
  if (icl::is_empty(left) || icl::is_empty(right))
    return false;

  return !(left > right);
}

template <typename Type>
  requires is_interval<Type>::value
bool operator>=(const Type& left, const Type& right) {
  if (icl::is_empty(left) || icl::is_empty(right))
    return false;

  return !(left < right);
}

//------------------------------------------------------------------------------
template <typename Type>
  requires is_asymmetric_interval<Type>::value
bool touches(const Type& left, const Type& right) {
  return domain_equal<Type>(upper(left), lower(right));
}

template <typename Type>
  requires has_symmetric_bounds<Type>::value
bool touches(const Type& left, const Type& right) {
  return domain_equal<Type>(last_next(left), first(right));
}

template <typename Type>
  requires is_discrete_interval<Type>::value
bool touches(const Type& left, const Type& right) {
  return domain_equal<Type>(domain_next<Type>(last(left)), first(right));
}

template <typename Type>
  requires is_continuous_interval<Type>::value
bool touches(const Type& left, const Type& right) {
  return is_complementary(inner_bounds(left, right)) &&
         domain_equal<Type>(upper(left), lower(right));
}

//==============================================================================
//= Size
//==============================================================================
//- cardinality ----------------------------------------------------------------

template <typename Type>
  requires is_continuous_interval<Type>::value
size_type_of<interval_traits<Type>>::type cardinality(const Type& object) {
  using SizeT = size_type_of<interval_traits<Type>>::type;
  if (icl::is_empty(object))
    return identity_element<SizeT>::value();
  if (object.bounds() == interval_bounds::closed() &&
      domain_equal<Type>(lower(object), upper(object)))
    return unit_element<SizeT>::value();
  return infinity<SizeT>::value();
}

template <typename Type>
  requires is_discrete_interval<Type>::value
size_type_of<interval_traits<Type>>::type cardinality(const Type& object) {
  using SizeT = size_type_of<interval_traits<Type>>::type;
  return icl::is_empty(object)
             ? identity_element<SizeT>::value()
             : static_cast<SizeT>(last_next(object) - first(object));
}

template <typename Type>
  requires is_continuous_asymmetric<Type>::value
size_type_of<interval_traits<Type>>::type cardinality(const Type& object) {
  using SizeT = size_type_of<interval_traits<Type>>::type;
  if (icl::is_empty(object))
    return identity_element<SizeT>::value();
  return infinity<SizeT>::value();
}

template <typename Type>
  requires is_discrete_asymmetric<Type>::value
size_type_of<interval_traits<Type>>::type cardinality(const Type& object) {
  using SizeT = size_type_of<interval_traits<Type>>::type;
  return icl::is_empty(object)
             ? identity_element<SizeT>::value()
             : static_cast<SizeT>(last_next(object) - first(object));
}

template <typename Type>
  requires has_symmetric_bounds<Type>::value
size_type_of<interval_traits<Type>>::type cardinality(const Type& object) {
  using SizeT = size_type_of<interval_traits<Type>>::type;
  return icl::is_empty(object)
             ? identity_element<SizeT>::value()
             : static_cast<SizeT>(last_next(object) - first(object));
}

//- size -----------------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
size_type_of<interval_traits<Type>>::type size(const Type& object) {
  return cardinality(object);
}

//- length ---------------------------------------------------------------------
template <typename Type>
  requires is_continuous_interval<Type>::value
difference_type_of<interval_traits<Type>>::type length(const Type& object) {
  using DiffT = difference_type_of<interval_traits<Type>>::type;
  return icl::is_empty(object) ? identity_element<DiffT>::value()
                               : upper(object) - lower(object);
}

template <typename Type>
  requires is_discrete_interval<Type>::value
difference_type_of<interval_traits<Type>>::type length(const Type& object) {
  using DiffT = difference_type_of<interval_traits<Type>>::type;
  return icl::is_empty(object) ? identity_element<DiffT>::value()
                               : last_next(object) - first(object);
}

template <typename Type>
  requires is_continuous_asymmetric<Type>::value
difference_type_of<interval_traits<Type>>::type length(const Type& object) {
  using DiffT = difference_type_of<interval_traits<Type>>::type;
  return icl::is_empty(object) ? identity_element<DiffT>::value()
                               : upper(object) - lower(object);
}

template <typename Type>
  requires is_discrete_static<Type>::value
difference_type_of<interval_traits<Type>>::type length(const Type& object) {
  using DiffT = difference_type_of<interval_traits<Type>>::type;
  return icl::is_empty(object) ? identity_element<DiffT>::value()
                               : last_next(object) - first(object);
}

//- iterative_size -------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
size_type_of<interval_traits<Type>>::type iterative_size(const Type&) {
  return 2;
}

//==============================================================================
//= Addition
//==============================================================================
//- hull -----------------------------------------------------------------------
/** \c hull returns the smallest interval containing \c left and \c right. */
template <typename Type>
  requires has_static_bounds<Type>::value
Type hull(Type left, const Type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;

  if (icl::is_empty(right))
    return left;
  if (icl::is_empty(left))
    return right;

  return construct<Type>(
      (std::min)(lower(left), lower(right), domain_compare()),
      (std::max)(upper(left), upper(right), domain_compare()));
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type hull(Type left, const Type& right) {
  if (icl::is_empty(right))
    return left;
  if (icl::is_empty(left))
    return right;

  return dynamic_interval_traits<Type>::construct_bounded(
      lower_min(left, right), upper_max(left, right));
}

//==============================================================================
//= Subtraction
//==============================================================================
//- left_subtract --------------------------------------------------------------
/** subtract \c left_minuend from the \c right interval on it's left side.
    Return the difference: The part of \c right right of \c left_minuend.
\code
right_over = right - left_minuend; //on the left.
...      d) : right
... c)      : left_minuend
     [c  d) : right_over
\endcode
*/
template <typename Type>
  requires is_asymmetric_interval<Type>::value
Type left_subtract(Type right, const Type& left_minuend) {
  if (exclusive_less(left_minuend, right))
    return right;

  return construct<Type>(upper(left_minuend), upper(right));
}

template <typename Type>
  requires is_static_closed<Type>::value
Type left_subtract(Type right, const Type& left_minuend) {
  if (exclusive_less(left_minuend, right))
    return right;
  if (upper_less_equal(right, left_minuend))
    return identity_element<Type>::value();

  return construct<Type>(domain_next<Type>(upper(left_minuend)), upper(right));
}

template <typename Type>
  requires is_static_open<Type>::value
Type left_subtract(Type right, const Type& left_minuend) {
  if (exclusive_less(left_minuend, right))
    return right;

  return construct<Type>(domain_prior<Type>(upper(left_minuend)), upper(right));
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type left_subtract(Type right, const Type& left_minuend) {
  if (exclusive_less(left_minuend, right))
    return right;
  return dynamic_interval_traits<Type>::construct_bounded(
      reverse_bounded_upper(left_minuend), bounded_upper(right));
}

//- right_subtract -------------------------------------------------------------
/** subtract \c right_minuend from the \c left interval on it's right side.
    Return the difference: The part of \c left left of \c right_minuend.
\code
left_over = left - right_minuend; //on the right side.
[a      ...  : left
     [b ...  : right_minuend
[a  b)       : left_over
\endcode
*/
template <typename Type>
  requires is_asymmetric_interval<Type>::value
Type right_subtract(Type left, const Type& right_minuend) {
  if (exclusive_less(left, right_minuend))
    return left;
  return construct<Type>(lower(left), lower(right_minuend));
}

template <typename Type>
  requires is_static_closed<Type>::value
Type right_subtract(Type left, const Type& right_minuend) {
  if (exclusive_less(left, right_minuend))
    return left;
  if (lower_less_equal(right_minuend, left))
    return identity_element<Type>::value();

  return construct<Type>(lower(left), domain_prior<Type>(lower(right_minuend)));
}

template <typename Type>
  requires is_static_open<Type>::value
Type right_subtract(Type left, const Type& right_minuend) {
  if (exclusive_less(left, right_minuend))
    return left;

  return construct<Type>(lower(left), domain_next<Type>(lower(right_minuend)));
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type right_subtract(Type left, const Type& right_minuend) {
  if (exclusive_less(left, right_minuend))
    return left;

  return dynamic_interval_traits<Type>::construct_bounded(
      bounded_lower(left), reverse_bounded_lower(right_minuend));
}

//==============================================================================
//= Intersection
//==============================================================================
//- operator & -----------------------------------------------------------------
/** Returns the intersection of \c left and \c right interval. */
template <typename Type>
  requires is_asymmetric_interval<Type>::value
Type operator&(Type left, const Type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;

  if (icl::is_empty(left) || icl::is_empty(right))
    return identity_element<Type>::value();
  return construct<Type>(
      (std::max)(icl::lower(left), icl::lower(right), domain_compare()),
      (std::min)(icl::upper(left), icl::upper(right), domain_compare()));
}

template <typename Type>
  requires has_symmetric_bounds<Type>::value
Type operator&(Type left, const Type& right) {
  using domain_compare = interval_traits<Type>::domain_compare;

  if (icl::is_empty(left) || icl::is_empty(right))
    return identity_element<Type>::value();
  return construct<Type>(
      (std::max)(icl::lower(left), icl::lower(right), domain_compare()),
      (std::min)(icl::upper(left), icl::upper(right), domain_compare()));
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type operator&(Type left, const Type& right) {
  if (icl::is_empty(left) || icl::is_empty(right))
    return identity_element<Type>::value();
  return dynamic_interval_traits<Type>::construct_bounded(
      lower_max(left, right), upper_min(left, right));
}

//- intersects -----------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool intersects(const Type& left, const Type& right) {
  return !(icl::is_empty(left) || icl::is_empty(right) ||
           exclusive_less(left, right) || exclusive_less(right, left));
}

//- disjoint -------------------------------------------------------------------
template <typename Type>
  requires is_interval<Type>::value
bool disjoint(const Type& left, const Type& right) {
  return icl::is_empty(left) || icl::is_empty(right) ||
         exclusive_less(left, right) || exclusive_less(right, left);
}

//==============================================================================
//= Complement
//==============================================================================

template <typename Type>
  requires is_asymmetric_interval<Type>::value
Type inner_complement(const Type& left, const Type& right) {
  if (icl::is_empty(left) || icl::is_empty(right))
    return identity_element<Type>::value();
  if (exclusive_less(left, right))
    return construct<Type>(upper(left), lower(right));
  if (exclusive_less(right, left))
    return construct<Type>(upper(right), lower(left));
  return identity_element<Type>::value();
}

template <typename Type>
  requires is_discrete_static_closed<Type>::value
Type inner_complement(const Type& left, const Type& right) {
  if (icl::is_empty(left) || icl::is_empty(right))
    return identity_element<Type>::value();
  if (exclusive_less(left, right))
    return construct<Type>(domain_next<Type>(upper(left)),
                           domain_prior<Type>(lower(right)));
  if (exclusive_less(right, left))
    return construct<Type>(domain_next<Type>(upper(right)),
                           domain_prior<Type>(lower(left)));
  return identity_element<Type>::value();
}

template <typename Type>
  requires is_discrete_static_open<Type>::value
Type inner_complement(const Type& left, const Type& right) {
  if (icl::is_empty(left) || icl::is_empty(right))
    return identity_element<Type>::value();
  if (exclusive_less(left, right))
    return construct<Type>(last(left), first(right));
  if (exclusive_less(right, left))
    return construct<Type>(last(right), first(left));
  return identity_element<Type>::value();
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
Type inner_complement(const Type& left, const Type& right) {
  if (icl::is_empty(left) || icl::is_empty(right))
    return identity_element<Type>::value();
  if (exclusive_less(left, right))
    return right_subtract(left_subtract(hull(left, right), left), right);
  if (exclusive_less(right, left))
    return right_subtract(left_subtract(hull(right, left), right), left);
  return identity_element<Type>::value();
}

template <typename Type>
  requires is_interval<Type>::value
Type between(const Type& left, const Type& right) {
  return inner_complement(left, right);
}

//==============================================================================
//= Distance
//==============================================================================
template <typename Type>
  requires(is_interval<Type>::value &&
           has_difference<typename interval_traits<Type>::domain_type>::value &&
           is_discrete<typename interval_traits<Type>::domain_type>::value)
difference_type_of<interval_traits<Type>>::type distance(const Type& x1,
                                                         const Type& x2) {
  using difference_type = difference_type_of<interval_traits<Type>>::type;

  if (icl::is_empty(x1) || icl::is_empty(x2))
    return identity_element<difference_type>::value();
  if (domain_less<Type>(last(x1), first(x2)))
    return static_cast<difference_type>(icl::pred(first(x2) - last(x1)));
  if (domain_less<Type>(last(x2), first(x1)))
    return static_cast<difference_type>(icl::pred(first(x1) - last(x2)));
  return identity_element<difference_type>::value();
}

template <typename Type>
  requires(is_interval<Type>::value &&
           has_difference<typename interval_traits<Type>::domain_type>::value &&
           is_continuous<typename interval_traits<Type>::domain_type>::value)
difference_type_of<interval_traits<Type>>::type distance(const Type& x1,
                                                         const Type& x2) {
  using DiffT = difference_type_of<interval_traits<Type>>::type;

  if (icl::is_empty(x1) || icl::is_empty(x2))
    return identity_element<DiffT>::value();
  if (domain_less<Type>(upper(x1), lower(x2)))
    return lower(x2) - upper(x1);
  if (domain_less<Type>(upper(x2), lower(x1)))
    return lower(x1) - upper(x2);
  return identity_element<DiffT>::value();
}

//==============================================================================
//= Streaming, representation
//==============================================================================
template <typename Type>
  requires(is_static_left_open<Type>::value || is_static_open<Type>::value)
std::string left_bracket(const Type&) {
  return "(";
}

template <typename Type>
  requires(is_static_right_open<Type>::value || is_static_closed<Type>::value)
std::string left_bracket(const Type&) {
  return "[";
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
std::string left_bracket(const Type& object) {
  return left_bracket(object.bounds());
}

//------------------------------------------------------------------------------
template <typename Type>
  requires(is_static_right_open<Type>::value || is_static_open<Type>::value)
std::string right_bracket(const Type&) {
  return ")";
}

template <typename Type>
  requires(is_static_left_open<Type>::value || is_static_closed<Type>::value)
std::string right_bracket(const Type&) {
  return "]";
}

template <typename Type>
  requires has_dynamic_bounds<Type>::value
std::string right_bracket(const Type& object) {
  return right_bracket(object.bounds());
}

//------------------------------------------------------------------------------
template <typename CharType, typename CharTraits, typename Type>
  requires is_interval<Type>::value
std::basic_ostream<CharType, CharTraits>&
operator<<(std::basic_ostream<CharType, CharTraits>& stream,
           Type const& object) {
  if (icl::is_empty(object))
    return stream << left_bracket<Type>(object) << right_bracket<Type>(object);
  return stream << left_bracket<Type>(object)
                << interval_traits<Type>::lower(object) << ","
                << interval_traits<Type>::upper(object)
                << right_bracket<Type>(object);
}

} // namespace icl
