#pragma once

#include <type_traits>

namespace icl {

template <template <typename> typename IsConcept, typename LeftT,
          typename RightT>
struct is_concept_equivalent {
  using type = is_concept_equivalent;
  static constexpr bool value =
      std::conjunction_v<IsConcept<LeftT>, IsConcept<RightT>>;
};

template <template <typename> typename IsConcept, typename LeftT,
          typename RightT>
struct has_same_concept {
  using type = has_same_concept;
  static constexpr bool value =
      std::conjunction_v<IsConcept<LeftT>,
                         is_concept_equivalent<IsConcept, LeftT, RightT>>;
};

} // namespace icl
