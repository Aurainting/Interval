#pragma once

namespace icl {

template <typename> struct is_interval_joiner {
  using type = is_interval_joiner;
  static constexpr bool value = false;
};

} // namespace icl
