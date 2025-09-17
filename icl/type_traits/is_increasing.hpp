#pragma once

#include <functional>

namespace icl {

template <typename, typename> struct is_increasing {
  using type = is_increasing;
  static constexpr bool value = true;
};

template <typename DomainT>
struct is_increasing<DomainT, std::greater<DomainT>> {
  using type = is_increasing;
  static constexpr bool value = false;
};

} // namespace icl
