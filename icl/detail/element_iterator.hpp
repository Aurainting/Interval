#pragma once

#include "icl/concept/interval.hpp"
#include "icl/detail/mapped_reference.hpp"
#include "icl/interval_traits.hpp"
#include "icl/type_traits/succ_pred.hpp"
#include <utility>

namespace icl {

//------------------------------------------------------------------------------
template <typename> struct is_std_pair {
  using type = is_std_pair;
  static constexpr bool value = false;
};

template <typename FirstT, typename SecondT>
struct is_std_pair<std::pair<FirstT, SecondT>> {
  using type = is_std_pair;
  static constexpr bool value = true;
};

//------------------------------------------------------------------------------
template <typename Type> struct first_element {
  using type = Type;
};

template <typename FirstT, typename SecondT>
struct first_element<std::pair<FirstT, SecondT>> {
  using type = FirstT;
};

//------------------------------------------------------------------------------
template <typename SegmentIteratorT> class element_iterator;

template <typename> struct is_reverse {
  using type = is_reverse;
  static constexpr bool value = false;
};

template <typename BaseIteratorT>
struct is_reverse<std::reverse_iterator<BaseIteratorT>> {
  using type = is_reverse;
  static constexpr bool value = true;
};

template <typename BaseIteratorT>
struct is_reverse<element_iterator<BaseIteratorT>> {
  using type = is_reverse;
  static constexpr bool value = is_reverse<BaseIteratorT>::value;
};

//------------------------------------------------------------------------------
template <typename SegmentT> struct elemental;

template <typename Interval> struct elemental {
  using segment_type = Interval;
  using interval_type = segment_type;
  using domain_type = interval_traits<interval_type>::domain_type;
  using type = domain_type;
  using codomain_type = domain_type;
  using transit_type = domain_type;
};

template <typename CodomainT, typename Interval>
struct elemental<std::pair<Interval const, CodomainT>> {
  using segment_type = std::pair<Interval, CodomainT>;
  using interval_type = Interval;
  using domain_type = interval_traits<interval_type>::domain_type;
  using codomain_type = CodomainT;
  using type = std::pair<domain_type, codomain_type>;
  using transit_type = mapped_reference<domain_type, codomain_type>;
};

//------------------------------------------------------------------------------
//- struct segment_adapter
//------------------------------------------------------------------------------
template <typename SegmentIteratorT, typename SegmentT> struct segment_adapter;

template <typename SegmentIteratorT, typename Interval> struct segment_adapter {
  using type = segment_adapter;
  using segment_type = Interval;
  using interval_type = segment_type;
  using domain_type = interval_traits<interval_type>::domain_type;
  using codomain_type = domain_type;
  using element_type = domain_type;
  using transit_type = domain_type&;
  using domain_difference_type =
      difference_type_of<interval_traits<interval_type>>::type;

  static domain_type first(const SegmentIteratorT& leaper) {
    return leaper->first();
  }
  static domain_type last(const SegmentIteratorT& leaper) {
    return leaper->last();
  }
  static domain_difference_type length(const SegmentIteratorT& leaper) {
    return icl::length(*leaper);
  }

  static transit_type transient_element(domain_type& inter_pos,
                                        const SegmentIteratorT& leaper,
                                        const domain_difference_type& sneaker) {
    inter_pos = is_reverse<SegmentIteratorT>::value
                    ? icl::last(*leaper) - sneaker
                    : icl::first(*leaper) + sneaker;
    return inter_pos;
  }
};

template <typename SegmentIteratorT, typename CodomainT, typename Interval>
struct segment_adapter<SegmentIteratorT, std::pair<Interval const, CodomainT>> {
  using type = segment_adapter;
  using interval_type = Interval;
  using domain_type = interval_traits<interval_type>::domain_type;
  using codomain_type = CodomainT;
  using element_type = std::pair<domain_type, codomain_type>;
  using transit_type = mapped_reference<domain_type, CodomainT>;
  using domain_difference_type =
      difference_type_of<interval_traits<interval_type>>::type;

  static domain_type first(const SegmentIteratorT& leaper) {
    return leaper->first.first();
  }
  static domain_type last(const SegmentIteratorT& leaper) {
    return leaper->first.last();
  }
  static domain_difference_type length(const SegmentIteratorT& leaper) {
    return icl::length(leaper->first);
  }

  static transit_type transient_element(domain_type& inter_pos,
                                        const SegmentIteratorT& leaper,
                                        const domain_difference_type& sneaker) {
    inter_pos = is_reverse<SegmentIteratorT>::value
                    ? icl::last(leaper->first) - sneaker
                    : icl::first(leaper->first) + sneaker;
    return transit_type(inter_pos, leaper->second);
  }
};

template <typename SegmentIteratorT> class element_iterator {
public:
  using segment_iterator = SegmentIteratorT;
  using segment_type = std::iterator_traits<segment_iterator>::value_type;
  using interval_type = first_element<segment_type>::type;
  using element_type = elemental<segment_type>::type;
  using domain_type = elemental<segment_type>::domain_type;
  using codomain_type = elemental<segment_type>::codomain_type;
  using transit_type = elemental<segment_type>::transit_type;
  using value_type = transit_type;
  using domain_difference_type =
      difference_type_of<interval_traits<interval_type>>::type;

  using iterator_concept = std::bidirectional_iterator_tag;
  using iterator_category = std::bidirectional_iterator_tag;
  using difference_type = std::ptrdiff_t;
  using pointer = transit_type*;
  using reference = transit_type&;

private:
  using adapt = segment_adapter<segment_iterator, segment_type>::type;

  segment_iterator _saltator;
  domain_difference_type _reptator;
  mutable domain_type _inter_pos;

public:
  element_iterator()
      : _saltator(identity_element<segment_iterator>::value()),
        _reptator(identity_element<domain_difference_type>::value()),
        _inter_pos(identity_element<domain_type>::value()) {}

  explicit element_iterator(segment_iterator jumper)
      : _saltator(jumper),
        _reptator(identity_element<domain_difference_type>::value()),
        _inter_pos(identity_element<domain_type>::value()) {}

  element_iterator(const element_iterator&) = default;
  element_iterator& operator=(const element_iterator&) = default;

  ~element_iterator() = default;

  transit_type operator*() const {
    return adapt::transient_element(_inter_pos, _saltator, _reptator);
  }

  element_iterator& operator++() {
    increment();
    return *this;
  }

  element_iterator operator++(int) {
    element_iterator tmp = *this;
    ++*this;
    return tmp;
  }

  element_iterator& operator--() {
    decrement();
    return *this;
  }

  element_iterator operator--(int) {
    element_iterator tmp = *this;
    --*this;
    return tmp;
  }

  bool operator==(const element_iterator& other) const {
    return _saltator == other._saltator && (_reptator == other._reptator);
  }

  bool operator!=(const element_iterator& other) const {
    return !(*this == other);
  }

private:
  void increment() {
    if (_reptator < icl::pred(adapt::length(_saltator))) {
      ++_reptator;
    } else {
      ++_saltator;
      _reptator = identity_element<domain_difference_type>::value();
    }
  }

  void decrement() {
    if (identity_element<domain_difference_type>::value() < _reptator) {
      --_reptator;
    } else {
      --_saltator;
      _reptator = adapt::length(_saltator);
      --_reptator;
    }
  }
};

} // namespace icl
