#pragma once

#include "icl/type_traits/type_to_string.hpp"
#include "icl/type_traits/value_size.hpp"

namespace icl {

template <typename DomainT, template <typename> typename Compare = std::less>
  requires std::default_initializable<DomainT> && std::totally_ordered<DomainT>
class closed_interval {
public:
  using type = closed_interval;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor; yields an empty interval <tt>[0,0)</tt>. */
  closed_interval()
      : _lwb(unit_element<DomainT>::value()),
        _upb(identity_element<DomainT>::value()) {
    static_assert(is_discrete<DomainT>::value);
  }

  // NOTE: Compiler generated copy constructor is used

  /** Constructor for a closed singleton interval <tt>[val,val]</tt> */
  explicit closed_interval(const DomainT& val) : _lwb(val), _upb(val) {
    static_assert(!is_continuous<DomainT>::value);
  }

  /** Interval from <tt>low</tt> to <tt>up</tt> with bounds<tt></tt> */
  closed_interval(const DomainT& low, const DomainT& up)
      : _lwb(low), _upb(up) {}

  DomainT lower() const { return _lwb; }
  DomainT upper() const { return _upb; }

  DomainT first() const { return _lwb; }
  DomainT last() const { return _upb; }

private:
  DomainT _lwb;
  DomainT _upb;
};

//==============================================================================
//=T closed_interval -> concept intervals
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_traits<closed_interval<DomainT, Compare>> {
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using interval_type = closed_interval<DomainT, Compare>;

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
//= Type traits
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_bound_type<closed_interval<DomainT, Compare>> {
  using type = interval_bound_type;
  static constexpr bound_type value = interval_bounds::static_closed;
};

template <typename DomainT, template <typename> typename Compare>
struct type_to_string<closed_interval<DomainT, Compare>> {
  static std::string apply() {
    return "[I]<" + type_to_string<DomainT>::apply() + ">";
  }
};

template <typename DomainT> struct value_size<closed_interval<DomainT>> {
  static std::size_t apply(const closed_interval<DomainT>&) { return 2; }
};

} // namespace icl
