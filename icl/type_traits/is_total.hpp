#pragma once

namespace icl {

template <typename> struct is_total {
  using type = is_total;
  static constexpr bool value = false;
};

} // namespace icl
