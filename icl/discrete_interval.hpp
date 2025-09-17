#pragma once

#include "icl/interval_bounds.hpp"
#include "icl/type_traits/is_discrete_interval.hpp"
#include "icl/type_traits/type_to_string.hpp"
#include "icl/type_traits/value_size.hpp"
#include <concepts>

namespace icl {

template <typename DomainT, template <typename> typename Compare = std::less>
  requires std::default_initializable<DomainT> && std::totally_ordered<DomainT>
class discrete_interval {
public:
  using type = discrete_interval;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using bounded_domain_type = bounded_value<DomainT>::type;

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor; yields an empty interval <tt>[0,0)</tt>. */
  discrete_interval()
      : _lwb(identity_element<DomainT>::value()),
        _upb(identity_element<DomainT>::value()),
        _bounds(interval_bounds::right_open()) {
    static_assert(is_discrete<DomainT>::value);
  }

  // NOTE: Compiler generated copy constructor is used

  /** Constructor for a closed singleton interval <tt>[val,val]</tt> */
  explicit discrete_interval(const DomainT& val)
      : _lwb(val), _upb(val), _bounds(interval_bounds::closed()) {
    static_assert(is_discrete<DomainT>::value);
  }

  /** Interval from <tt>low</tt> to <tt>up</tt> with bounds<tt></tt> */
  discrete_interval(const DomainT& low, const DomainT& up,
                    interval_bounds bounds = interval_bounds::right_open(),
                    discrete_interval* = nullptr)
      : _lwb(low), _upb(up), _bounds(bounds) {
    static_assert(is_discrete<DomainT>::value);
  }

  domain_type lower() const { return _lwb; }
  domain_type upper() const { return _upb; }
  [[nodiscard]] interval_bounds bounds() const { return _bounds; }

  static discrete_interval open(const DomainT& lo, const DomainT& up) {
    return discrete_interval(lo, up, interval_bounds::open());
  }
  static discrete_interval right_open(const DomainT& lo, const DomainT& up) {
    return discrete_interval(lo, up, interval_bounds::right_open());
  }
  static discrete_interval left_open(const DomainT& lo, const DomainT& up) {
    return discrete_interval(lo, up, interval_bounds::left_open());
  }
  static discrete_interval closed(const DomainT& lo, const DomainT& up) {
    return discrete_interval(lo, up, interval_bounds::closed());
  }

private:
  domain_type _lwb;
  domain_type _upb;
  interval_bounds _bounds;
};

//==============================================================================
//=T discrete_interval -> concept intervals
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_traits<discrete_interval<DomainT, Compare>> {
  using type = interval_traits;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using interval_type = discrete_interval<DomainT, Compare>;

  static interval_type construct(const domain_type& lo, const domain_type& up) {
    return interval_type(lo, up);
  }

  static domain_type lower(const interval_type& inter_val) {
    return inter_val.lower();
  }
  static domain_type upper(const interval_type& inter_val) {
    return inter_val.upper();
  }
};

//==============================================================================
//=T discrete_interval -> concept dynamic_interval_traits
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct dynamic_interval_traits<discrete_interval<DomainT, Compare>> {
  using type = dynamic_interval_traits;
  using interval_type = discrete_interval<DomainT, Compare>;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;

  static interval_type construct(const domain_type& lo, const domain_type& up,
                                 interval_bounds bounds) {
    return interval_type(lo, up, bounds, static_cast<interval_type*>(nullptr));
  }

  static interval_type construct_bounded(const bounded_value<DomainT>& lo,
                                         const bounded_value<DomainT>& up) {
    return interval_type(lo.value(), up.value(),
                         lo.bound().left() | up.bound().right(),
                         static_cast<interval_type*>(nullptr));
  }
};

//==============================================================================
//= Type traits
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_bound_type<discrete_interval<DomainT, Compare>> {
  using type = interval_bound_type;
  static constexpr bound_type value = interval_bounds::dynamic;
};

template <typename DomainT, template <typename> typename Compare>
struct is_discrete_interval<discrete_interval<DomainT, Compare>> {
  using type = is_discrete_interval;
  static constexpr bool value = is_discrete<DomainT>::value;
};

template <typename DomainT, template <typename> typename Compare>
struct type_to_string<discrete_interval<DomainT, Compare>> {
  static std::string apply() {
    return "dI<" + type_to_string<DomainT>::apply() + ">";
  }
};

template <typename DomainT> struct value_size<discrete_interval<DomainT>> {
  static std::size_t apply(const discrete_interval<DomainT>&) { return 2; }
};

} // namespace icl
