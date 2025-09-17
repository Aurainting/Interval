#pragma once

namespace icl {

template <typename> struct is_map {
  using type = is_map;
  static constexpr bool value = false;
};

} // namespace icl
