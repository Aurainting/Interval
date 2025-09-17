#pragma once

#include "icl/interval_bounds.hpp"
#include "icl/type_traits/is_continuous.hpp"
#include "icl/type_traits/is_continuous_interval.hpp"
#include "icl/type_traits/type_to_string.hpp"
#include "icl/type_traits/value_size.hpp"
#include <concepts>

namespace icl {

template <typename DomainT, template <typename> typename Compare = std::less>
  requires std::default_initializable<DomainT> && std::totally_ordered<DomainT>
class continuous_interval {
public:
  using type = continuous_interval;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using bounded_domain_type = bounded_value<DomainT>::type;

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor; yields an empty interval <tt>[0,0)</tt>. */
  continuous_interval()
      : _lwb(identity_element<DomainT>::value()),
        _upb(identity_element<DomainT>::value()),
        _bounds(interval_bounds::right_open()) {
    static_assert(icl::is_continuous<DomainT>::value);
  }

  // NOTE: Compiler generated copy constructor is used

  /** Constructor for a closed singleton interval <tt>[val,val]</tt> */
  explicit continuous_interval(const DomainT& val)
      : _lwb(val), _upb(val), _bounds(interval_bounds::closed()) {
    static_assert(is_continuous<DomainT>::value);
  }

  /** Interval from <tt>low</tt> to <tt>up</tt> with bounds<tt></tt> */
  continuous_interval(const DomainT& low, const DomainT& up,
                      interval_bounds bounds = interval_bounds::right_open(),
                      continuous_interval* = 0)
      : _lwb(low), _upb(up), _bounds(bounds) {
    static_assert(is_continuous<DomainT>::value);
  }

  domain_type lower() const { return _lwb; }
  domain_type upper() const { return _upb; }
  [[nodiscard]] interval_bounds bounds() const { return _bounds; }

  static continuous_interval open(const DomainT& lo, const DomainT& up) {
    return continuous_interval(lo, up, interval_bounds::open());
  }
  static continuous_interval right_open(const DomainT& lo, const DomainT& up) {
    return continuous_interval(lo, up, interval_bounds::right_open());
  }
  static continuous_interval left_open(const DomainT& lo, const DomainT& up) {
    return continuous_interval(lo, up, interval_bounds::left_open());
  }
  static continuous_interval closed(const DomainT& lo, const DomainT& up) {
    return continuous_interval(lo, up, interval_bounds::closed());
  }

private:
  domain_type _lwb;
  domain_type _upb;
  interval_bounds _bounds;
};

//==============================================================================
//=T continuous_interval -> concept interval
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_traits<continuous_interval<DomainT, Compare>> {
  using type = interval_traits;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using interval_type = continuous_interval<DomainT, Compare>;

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
//=T continuous_interval -> concept dynamic_interval
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct dynamic_interval_traits<continuous_interval<DomainT, Compare>> {
  using type = dynamic_interval_traits;
  using interval_type = continuous_interval<DomainT, Compare>;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;

  static interval_type construct(const domain_type lo, const domain_type up,
                                 interval_bounds bounds) {
    return icl::continuous_interval<DomainT, Compare>(
        lo, up, bounds, static_cast<continuous_interval<DomainT, Compare>*>(0));
  }

  static interval_type construct_bounded(const bounded_value<DomainT>& lo,
                                         const bounded_value<DomainT>& up) {
    return icl::continuous_interval<DomainT, Compare>(
        lo.value(), up.value(), lo.bound().left() | up.bound().right(),
        static_cast<continuous_interval<DomainT, Compare>*>(0));
  }
};

//==============================================================================
//= Type traits
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_bound_type<continuous_interval<DomainT, Compare>> {
  using type = interval_bound_type;
  static constexpr bound_type value = interval_bounds::dynamic;
};

template <typename DomainT, template <typename> typename Compare>
struct is_continuous_interval<continuous_interval<DomainT, Compare>> {
  using type = is_continuous_interval<continuous_interval<DomainT, Compare>>;
  static constexpr bool value = true;
};

template <typename DomainT, template <typename> typename Compare>
struct type_to_string<continuous_interval<DomainT, Compare>> {
  static std::string apply() {
    return "cI<" + type_to_string<DomainT>::apply() + ">";
  }
};

template <typename DomainT> struct value_size<continuous_interval<DomainT>> {
  static std::size_t apply(const continuous_interval<DomainT>&) { return 2; }
};

} // namespace icl
