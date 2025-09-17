#pragma once

namespace icl {

template <typename> struct is_interval_splitter {
  using type = is_interval_splitter;
  static constexpr bool value = false;
};

} // namespace icl
