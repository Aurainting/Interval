#pragma once

#include "icl/type_traits/succ_pred.hpp"
#include "icl/type_traits/type_to_string.hpp"
#include "icl/type_traits/value_size.hpp"

namespace icl {

template <typename DomainT, template <typename> typename Compare = std::less>
  requires std::default_initializable<DomainT> && std::totally_ordered<DomainT>
class left_open_interval {
public:
  using type = left_open_interval<DomainT, Compare>;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;

public:
  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor; yields an empty interval <tt>(0,0]</tt>. */
  left_open_interval()
      : _lwb(identity_element<DomainT>::value()),
        _upb(identity_element<DomainT>::value()) {}

  // NOTE: Compiler generated copy constructor is used

  /** Constructor for a left-open singleton interval <tt>(val-1,val]</tt> */
  explicit left_open_interval(const DomainT& val)
      : _lwb(predecessor<DomainT, domain_compare>::apply(val)), _upb(val) {
    // Only for discrete types this ctor creates an interval containing
    // a single element only.
    static_assert(icl::is_discrete<DomainT>::value);
    static_assert(
        numeric_minimum<DomainT, domain_compare,
                        is_numeric<DomainT>::value>::is_less_than(val));
  }

  /** Interval from <tt>low</tt> to <tt>up</tt> with bounds <tt>bounds</tt> */
  left_open_interval(const DomainT& low, const DomainT& up)
      : _lwb(low), _upb(up) {}

  DomainT lower() const { return _lwb; }
  DomainT upper() const { return _upb; }

private:
  DomainT _lwb;
  DomainT _upb;
};

//==============================================================================
//=T left_open_interval -> concept intervals
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_traits<icl::left_open_interval<DomainT, Compare>> {
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using interval_type = icl::left_open_interval<DomainT, Compare>;

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
struct interval_bound_type<left_open_interval<DomainT, Compare>> {
  using type = interval_bound_type;
  static constexpr bound_type value = interval_bounds::static_left_open;
};

template <typename DomainT, template <typename> typename Compare>
struct type_to_string<icl::left_open_interval<DomainT, Compare>> {
  static std::string apply() {
    return "(I]<" + type_to_string<DomainT>::apply() + ">";
  }
};

template <typename DomainT, template <typename> typename Compare>
struct value_size<icl::left_open_interval<DomainT, Compare>> {
  static std::size_t apply(const icl::left_open_interval<DomainT>&) {
    return 2;
  }
};

} // namespace icl
