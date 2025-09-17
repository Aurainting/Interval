#pragma once

#include "icl/interval_traits.hpp"
#include "icl/type_traits/domain_type_of.hpp"
#include "icl/type_traits/is_continuous.hpp"
#include "icl/type_traits/is_interval.hpp"
#include <type_traits>

namespace icl {

template <typename Type> struct is_asymmetric_interval {
  using type = is_asymmetric_interval;
  static constexpr bool value =
      std::conjunction_v<is_interval<Type>, has_static_bounds<Type>,
                         has_asymmetric_bounds<Type>>;
};

template <typename Type> struct is_continuous_asymmetric {
  using type = is_continuous_asymmetric;
  static constexpr bool value = std::conjunction_v<
      is_asymmetric_interval<Type>,
      is_continuous<typename domain_type_of<interval_traits<Type>>::type>>;
};

template <typename Type> struct is_discrete_asymmetric {
  using type = is_discrete_asymmetric;
  static constexpr bool value = std::conjunction_v<
      is_asymmetric_interval<Type>,
      std::negation<
          is_continuous<typename domain_type_of<interval_traits<Type>>::type>>>;
};

} // namespace icl
