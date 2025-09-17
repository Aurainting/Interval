#pragma once

namespace icl {

template <typename Type, typename Combiner, bool absorbs_identities>
struct on_absorbtion;

template <typename Type, typename Combiner>
struct on_absorbtion<Type, Combiner, false> {
  using type = on_absorbtion;
  using codomain_type = Type::codomain_type;

  static bool is_absorbable(const codomain_type&) { return false; }
};

template <typename Type, typename Combiner>
struct on_absorbtion<Type, Combiner, true> {
  using type = on_absorbtion;
  using codomain_type = Type::codomain_type;
  using codomain_combine = Type::codomain_combine;

  static bool is_absorbable(const codomain_type& co_value) {
    return co_value == Combiner::identity_element();
  }
};

} // namespace icl
