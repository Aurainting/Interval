#pragma once

#include "icl/type_traits/is_increasing.hpp"

namespace icl {
template <typename IncrementableT>
static IncrementableT succ(IncrementableT x) {
  return ++x;
}

template <typename DecrementableT>
static DecrementableT pred(DecrementableT x) {
  return --x;
}

namespace detail {
template <typename DomainT, bool increasing = true> struct successor;

template <typename DomainT> struct successor<DomainT, true> {
  using type = successor;
  static DomainT apply(DomainT value) { return ++value; }
};

template <typename DomainT> struct successor<DomainT, false> {
  using type = successor;
  static DomainT apply(DomainT value) { return --value; }
};

template <typename DomainT, bool increasing = true> struct predecessor;

template <typename DomainT> struct predecessor<DomainT, true> {
  using type = predecessor;
  static DomainT apply(DomainT value) { return --value; }
};

template <typename DomainT> struct predecessor<DomainT, false> {
  using type = predecessor;
  static DomainT apply(DomainT value) { return ++value; }
};
} // namespace detail

//------------------------------------------------------------------------------
template <typename DomainT, typename Compare> struct successor {
  static DomainT apply(DomainT value) {
    return detail::successor<
        DomainT, is_increasing<DomainT, Compare>::value>::apply(value);
  }
};

template <typename DomainT, typename Compare> struct predecessor {
  static DomainT apply(DomainT value) {
    return detail::predecessor<
        DomainT, is_increasing<DomainT, Compare>::value>::apply(value);
  }
};

} // namespace icl
