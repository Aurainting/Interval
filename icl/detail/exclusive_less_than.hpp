#pragma once

#include "icl/concept/interval.hpp"

namespace icl {

/// Comparison functor on intervals implementing an overlap free less
template <typename IntervalT> struct exclusive_less_than {
  /** Operator <tt>operator()</tt> implements a strict weak ordering on
   * intervals. */
  bool operator()(const IntervalT& left, const IntervalT& right) const {
    return non_empty::exclusive_less(left, right);
  }
};

} // namespace icl
