#pragma once

#include "icl/type_traits/succ_pred.hpp"
#include "icl/type_traits/type_to_string.hpp"
#include "icl/type_traits/value_size.hpp"

namespace icl {

template <typename DomainT, template <typename> typename Compare = std::less>
  requires std::default_initializable<DomainT> && std::totally_ordered<DomainT>
class right_open_interval {
public:
  using type = right_open_interval<DomainT, Compare>;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;

public:
  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor; yields an empty interval <tt>[0,0)</tt>. */
  right_open_interval()
      : _lwb(identity_element<DomainT>::value()),
        _upb(identity_element<DomainT>::value()) {}

  // NOTE: Compiler generated copy constructor is used

  /** Constructor for a singleton interval <tt>[val,val+1)</tt> */
  explicit right_open_interval(const DomainT& val)
      : _lwb(val), _upb(icl::successor<DomainT, domain_compare>::apply(val)) {
    // Only for discrete types this ctor creates an interval containing
    // a single element only.
    static_assert(icl::is_discrete<DomainT>::value);
  }

  /** Interval from <tt>low</tt> to <tt>up</tt> with bounds <tt>bounds</tt> */
  right_open_interval(const DomainT& low, const DomainT& up)
      : _lwb(low), _upb(up) {}

  domain_type lower() const { return _lwb; }
  domain_type upper() const { return _upb; }

private:
  domain_type _lwb;
  domain_type _upb;
};

//==============================================================================
//=T right_open_interval -> concept intervals
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_traits<icl::right_open_interval<DomainT, Compare>> {
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using interval_type = icl::right_open_interval<DomainT, Compare>;

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
struct interval_bound_type<right_open_interval<DomainT, Compare>> {
  using type = interval_bound_type;
  static constexpr bound_type value = interval_bounds::static_right_open;
};

template <typename DomainT, template <typename> typename Compare>
struct type_to_string<icl::right_open_interval<DomainT, Compare>> {
  static std::string apply() {
    return "[I)<" + type_to_string<DomainT>::apply() + ">";
  }
};

template <typename DomainT, template <typename> typename Compare>
struct value_size<icl::right_open_interval<DomainT, Compare>> {
  static std::size_t apply(const icl::right_open_interval<DomainT>&) {
    return 2;
  }
};

} // namespace icl
