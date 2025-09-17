#pragma once

namespace icl {

template <typename> struct is_continuous_interval {
  using type = is_continuous_interval;
  static constexpr bool value = false;
};

} // namespace icl
