#pragma once

namespace icl {

template <typename> struct absorbs_identities {
  using type = absorbs_identities;
  static constexpr bool value = false;
};

} // namespace icl
