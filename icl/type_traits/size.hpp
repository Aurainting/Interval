#pragma once

#include <cstddef>

namespace icl {

template <typename> struct size {
  using type = std::size_t;
};

} // namespace icl
