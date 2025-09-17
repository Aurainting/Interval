#pragma once

namespace icl {

template <typename> struct is_interval_separator {
  using type = is_interval_separator;
  static constexpr bool value = false;
};

} // namespace icl
