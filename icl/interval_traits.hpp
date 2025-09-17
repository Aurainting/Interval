#pragma once

#include "icl/type_traits/difference_type_of.hpp"
#include "icl/type_traits/domain_type_of.hpp"
#include "icl/type_traits/size_type_of.hpp"

namespace icl {

template <typename Type> struct interval_traits;

template <typename Type> struct domain_type_of<interval_traits<Type>> {
  using type = typename interval_traits<Type>::domain_type;
};

//------------------------------------------------------------------------------
//- Adapter typename
//------------------------------------------------------------------------------
template <typename Type> struct interval_traits {
  using type = interval_traits;
  using domain_type = typename domain_type_of<Type>::type;

  static Type construct(const domain_type& lo, const domain_type& up);

  static domain_type upper(const Type& inter_val);
  static domain_type lower(const Type& inter_val);
};

template <typename Type> struct difference_type_of<interval_traits<Type>> {
  using domain_type = typename interval_traits<Type>::domain_type;
  using type = typename difference_type_of<domain_type>::type;
};

template <typename Type> struct size_type_of<interval_traits<Type>> {
  using domain_type = typename interval_traits<Type>::domain_type;
  using type = typename size_type_of<domain_type>::type;
};

} // namespace icl
