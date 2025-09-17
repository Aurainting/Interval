#pragma once

#include "icl/continuous_interval.hpp"
#include "icl/discrete_interval.hpp"
#include "icl/type_traits/is_discrete.hpp"

namespace icl {

template <typename DomainT, template <typename> typename Compare>
struct interval_type_default {
  using type = continuous_interval<DomainT, Compare>;
};

template <typename DomainT, template <typename> typename Compare>
  requires(is_discrete<DomainT>::value)
struct interval_type_default<DomainT, Compare> {
  using type = discrete_interval<DomainT, Compare>;
};

} // namespace icl
