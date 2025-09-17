#pragma once

#include "icl/type_traits/succ_pred.hpp"
#include "icl/type_traits/type_to_string.hpp"
#include "icl/type_traits/value_size.hpp"

namespace icl {

template <typename DomainT, template <typename> typename Compare = std::less>
  requires std::default_initializable<DomainT> && std::totally_ordered<DomainT>
class open_interval {
public:
  using type = open_interval;
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor; yields an empty interval <tt>(0,0)</tt>. */
  open_interval()
      : _lwb(identity_element<DomainT>::value()),
        _upb(identity_element<DomainT>::value()) {}

  // NOTE: Compiler generated copy constructor is used

  /** Constructor for an open singleton interval <tt>(val-1,val+1)</tt> */
  explicit open_interval(const DomainT& val)
      : _lwb(pred(val)), _upb(succ(val)) {
    // Only for discrete types this ctor creates an interval containing
    // a single element only.
    static_assert(is_discrete<DomainT>::value);
    static_assert(
        numeric_minimum<DomainT, domain_compare,
                        is_numeric<DomainT>::value>::is_less_than(val));
  }

  /** Interval from <tt>low</tt> to <tt>up</tt> with bounds<tt></tt> */
  open_interval(const DomainT& low, const DomainT& up) : _lwb(low), _upb(up) {}

  DomainT lower() const { return _lwb; }
  DomainT upper() const { return _upb; }

private:
  DomainT _lwb;
  DomainT _upb;
};

//==============================================================================
//=T open_interval -> concept intervals
//==============================================================================
template <typename DomainT, template <typename> typename Compare>
struct interval_traits<open_interval<DomainT, Compare>> {
  using domain_type = DomainT;
  using domain_compare = Compare<DomainT>;
  using interval_type = open_interval<DomainT, Compare>;

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
struct interval_bound_type<open_interval<DomainT, Compare>> {
  using type = interval_bound_type;
  static constexpr bound_type value = interval_bounds::static_open;
};

template <typename DomainT, template <typename> typename Compare>
struct type_to_string<open_interval<DomainT, Compare>> {
  static std::string apply() {
    return "(I)<" + type_to_string<DomainT>::apply() + ">";
  }
};

template <typename DomainT, template <typename> typename Compare>
struct value_size<open_interval<DomainT, Compare>> {
  static std::size_t apply(const open_interval<DomainT>&) { return 2; }
};

} // namespace icl
