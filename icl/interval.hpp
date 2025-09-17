#pragma once

#include "icl/interval_traits.hpp"
#include "icl/type_traits/interval_type_default.hpp"
#include "icl/type_traits/is_discrete.hpp"

namespace icl {

template <typename DomainT, template <typename> typename Compare = std::less>
struct interval {
  using interval_type = interval_type_default<DomainT, Compare>::type;
  using type = interval_type;

  static interval_type right_open(const DomainT& low, const DomainT& up) {
    return icl::construct<interval_type>(low, up,
                                         interval_bounds::right_open());
  }

  static interval_type left_open(const DomainT& low, const DomainT& up) {
    return icl::construct<interval_type>(low, up, interval_bounds::left_open());
  }

  static interval_type open(const DomainT& low, const DomainT& up) {
    return icl::construct<interval_type>(low, up, interval_bounds::open());
  }

  static interval_type closed(const DomainT& low, const DomainT& up) {
    return icl::construct<interval_type>(low, up, interval_bounds::closed());
  }

  static interval_type construct(const DomainT& low, const DomainT& up) {
    return icl::construct<interval_type>(low, up);
  }
};

template <typename IntervalT, bound_type PretendedBounds,
          bound_type RepresentedBounds>
struct static_interval {
  using domain_type = interval_traits<IntervalT>::domain_type;

  static IntervalT construct(const domain_type& low, const domain_type& up) {
    return icl::construct<IntervalT>(
        shift_lower(interval_bounds(PretendedBounds),
                    interval_bounds(RepresentedBounds), low),
        shift_upper(interval_bounds(PretendedBounds),
                    interval_bounds(RepresentedBounds), up));
  }
};

template <typename IntervalT, bound_type PretendedBounds,
          bound_type RepresentedBounds>
  requires(
      !is_discrete<typename interval_traits<IntervalT>::domain_type>::value)
struct static_interval<IntervalT, PretendedBounds, RepresentedBounds> {
  using domain_type = interval_traits<IntervalT>::domain_type;

  static IntervalT construct(const domain_type& low, const domain_type& up) {
    static_assert(is_discrete<domain_type>::value ||
                  PretendedBounds == RepresentedBounds);
    return icl::construct<IntervalT>(low, up);
  }
};

} // namespace icl
