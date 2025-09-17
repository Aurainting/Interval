#pragma once

namespace icl {

template <typename> struct is_discrete_interval {
  using type = is_discrete_interval;
  static constexpr bool value = false;
};

} // namespace icl
