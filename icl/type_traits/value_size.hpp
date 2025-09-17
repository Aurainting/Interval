#pragma once

#include "icl/concept/container.hpp"
#include "icl/concept/interval.hpp"
#include <cmath>
#include <type_traits>

namespace icl {

/// static class template for the size of a type's value
/** This function is needed to be able to order values according
    to their size. This is used to e.g. prefer simple test
    instances and to express this simplicity independent of the
    type of the test case.
*/
template <typename Type> struct value_size {
  /** The size of a value is used to be able to order values according to
      their simplicity */
  static constexpr std::size_t apply(const Type& val) {
    if constexpr (std::is_arithmetic_v<Type>) {
      return static_cast<std::size_t>(std::fabs(val));
    } else {
      return icl::iterative_size(val);
    }
  }
};

} // namespace icl
