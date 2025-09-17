#pragma once

namespace icl {

class interval_bounds;
template <typename DomainT> class bounded_value;

//------------------------------------------------------------------------------
//- Adapter class
//------------------------------------------------------------------------------
template <typename Type> struct dynamic_interval_traits {
  using domain_type = Type::domain_type;
  using domain_compare = Type::domain_compare;

  static Type construct(const domain_type& lo, const domain_type& up,
                        interval_bounds bounds);
  static Type construct_bounded(const bounded_value<domain_type>& lo,
                                const bounded_value<domain_type>& up);
};

} // namespace icl
