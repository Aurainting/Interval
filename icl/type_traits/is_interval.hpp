#pragma once

#include "icl/interval_bounds.hpp"
#include "icl/interval_traits.hpp"
#include "icl/type_traits/is_continuous.hpp"
#include "icl/type_traits/is_discrete.hpp"
#include <type_traits>

namespace icl {

template <typename> struct interval_bound_type {
  using type = interval_bound_type;
  static constexpr bound_type value = (interval_bounds::undefined);
};

template <typename Type> struct is_interval {
  using type = is_interval;
  static constexpr bool value =
      interval_bound_type<Type>::value < interval_bounds::undefined;
};

template <typename Type> struct has_static_bounds {
  using type = has_static_bounds;
  static constexpr bool value =
      interval_bound_type<Type>::value < interval_bounds::dynamic;
};

template <typename Type> struct has_dynamic_bounds {
  using type = has_dynamic_bounds;
  static constexpr bool value =
      interval_bound_type<Type>::value == interval_bounds::dynamic;
};

template <typename Type> struct has_asymmetric_bounds {
  using type = has_asymmetric_bounds;

  static constexpr bound_type bounds = interval_bound_type<Type>::value;
  static constexpr bool value = bounds == interval_bounds::static_left_open ||
                                bounds == interval_bounds::static_right_open;
};

template <typename Type> struct has_symmetric_bounds {
  using type = has_symmetric_bounds;
  static constexpr bound_type bounds = interval_bound_type<Type>::value;
  static constexpr bool value = bounds == interval_bounds::static_closed ||
                                bounds == interval_bounds::static_open;
};

//------------------------------------------------------------------------------
template <typename Type> struct is_discrete_static {
  using type = is_discrete_static;
  using domain_type = interval_traits<Type>::domain_type;

  static constexpr bool value =
      std::conjunction_v<has_static_bounds<Type>, is_discrete<domain_type>>;
};

//------------------------------------------------------------------------------
template <typename Type> struct is_continuous_static {
  using type = is_continuous_static;
  using domain_type = interval_traits<Type>::domain_type;
  static constexpr bool value =
      std::conjunction_v<has_static_bounds<Type>, is_continuous<domain_type>,
                         has_asymmetric_bounds<Type>>;
};

//------------------------------------------------------------------------------
template <typename Type> struct is_static_right_open {
  using type = is_static_right_open;

  static constexpr bool value =
      interval_bound_type<Type>::value == interval_bounds::static_right_open;
};

template <typename Type> struct is_static_left_open {
  using type = is_static_left_open;
  static constexpr bool value =
      interval_bound_type<Type>::value == interval_bounds::static_left_open;
};

template <typename Type> struct is_static_open {
  using type = is_static_open;
  static constexpr bool value =
      interval_bound_type<Type>::value == interval_bounds::static_open;
};

template <typename Type> struct is_static_closed {
  using type = is_static_closed;
  static constexpr bool value =
      interval_bound_type<Type>::value == interval_bounds::static_closed;
};

template <typename Type> struct is_discrete_static_closed {
  using type = is_static_closed<Type>;
  using domain_type = interval_traits<Type>::domain_type;

  static constexpr bool value =
      std::conjunction_v<is_static_closed<Type>, is_discrete<domain_type>>;
};

template <typename Type> struct is_discrete_static_open {
  using type = is_static_closed<Type>;
  using domain_type = interval_traits<Type>::domain_type;

  static constexpr bool value =
      std::conjunction_v<is_static_open<Type>, is_discrete<domain_type>>;
};

//------------------------------------------------------------------------------
template <typename Type> struct is_continuous_right_open {
  using type = is_continuous_right_open;
  using domain_type = interval_traits<Type>::domain_type;

  static constexpr bool value = std::conjunction_v<is_static_right_open<Type>,
                                                   is_continuous<domain_type>>;
};

template <typename Type> struct is_continuous_left_open {
  using type = is_continuous_left_open;
  using domain_type = interval_traits<Type>::domain_type;

  static constexpr bool value =
      std::conjunction_v<is_static_left_open<Type>, is_continuous<domain_type>>;
};

//------------------------------------------------------------------------------

template <typename Type> struct is_singelizable {
  using type = is_singelizable;
  using domain_type = interval_traits<Type>::domain_type;

  static constexpr bool value =
      std::disjunction_v<has_dynamic_bounds<Type>, is_discrete<domain_type>>;
};

} // namespace icl
