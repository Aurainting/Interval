#pragma once

#include "icl/detail/interval_set_algo.hpp"
#include "icl/interval_base_set.hpp"
#include "icl/type_traits/is_interval_joiner.hpp"

namespace icl {

/** \brief Implements a set as a set of intervals - merging adjoining intervals
 */
template <typename DomainT, template <typename> typename Compare = std::less,
          typename Interval =
              typename icl::interval_type_default<DomainT, Compare>::type,
          template <typename> typename Alloc = std::allocator>
class interval_set
    : public interval_base_set<interval_set<DomainT, Compare, Interval, Alloc>,
                               DomainT, Compare, Interval, Alloc> {
public:
  using type = interval_set<DomainT, Compare, Interval, Alloc>;

  /// The base_type of this class
  using base_type = interval_base_set<type, DomainT, Compare, Interval, Alloc>;

  using overloadable_type = type;

  using joint_type = type;

  using key_object_type = type;

  /// The domain type of the set
  using domain_type = DomainT;
  /// The codomaintype is the same as domain_type
  using codomain_type = DomainT;

  /// The element type of the set
  using element_type = DomainT;
  /// The interval type of the set
  using interval_type = Interval;
  /// The segment type of the set
  using segment_type = interval_type;

  /// Comparison functor for domain values
  using domain_compare = Compare<DomainT>;

  /// Comparison functor for intervals
  using interval_compare = exclusive_less_than<interval_type>;

  /// Comparison functor for keys
  using key_compare = exclusive_less_than<interval_type>;

  /// The allocator type of the set
  using allocator_type = Alloc<interval_type>;

  /// allocator type of the corresponding element set
  using domain_allocator_type = Alloc<DomainT>;

  /// The corresponding atomized type representing this interval container of
  /// elements
  using atomized_type = typename base_type::atomized_type;

  /// Container type for the implementation
  using ImplSetT = typename base_type::ImplSetT;

  /// key type of the implementing container
  using key_type = typename ImplSetT::key_type;
  /// data type of the implementing container
  using data_type = typename ImplSetT::value_type;
  /// value type of the implementing container
  using value_type = typename ImplSetT::value_type;

  /// iterator for iteration over intervals
  using iterator = typename ImplSetT::iterator;
  /// const_iterator for iteration over intervals
  using const_iterator = typename ImplSetT::const_iterator;

  enum { fineness = 1 };

public:
  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /// Default constructor for the empty object
  interval_set() : base_type() {}

  /// Copy constructor
  interval_set(const interval_set& src) : base_type(src) {}

  /// Copy constructor for base_type
  template <typename SubType>
  explicit interval_set(const interval_base_set<SubType, DomainT, Compare,
                                                Interval, Alloc>& src) {
    this->assign(src);
  }

  /// Constructor for a single element
  explicit interval_set(const domain_type& value) : base_type() {
    this->add(interval_type(value));
  }

  /// Constructor for a single interval
  explicit interval_set(const interval_type& itv) : base_type() {
    this->add(itv);
  }

  /// Assignment from a base interval_set.
  template <typename SubType>
  void assign(const interval_base_set<SubType, DomainT, Compare, Interval,
                                      Alloc>& src) {
    using base_set_type =
        interval_base_set<SubType, DomainT, Compare, Interval, Alloc>;
    this->clear();
    // Has to be implemented via add. there might be touching borders to be
    // joined
    iterator prior_ = this->_set.end();
    for (typename base_set_type::const_iterator it_ = src.begin();
         !(it_ == src.end()); ++it_)
      prior_ = this->add(prior_, *it_);
  }

  /// Assignment operator for base type
  template <typename SubType>
  interval_set& operator=(const interval_base_set<SubType, DomainT, Compare,
                                                  Interval, Alloc>& src) {
    this->assign(src);
    return *this;
  }

  //==========================================================================
  //= Move semantics
  //==========================================================================
  /// Move constructor
  interval_set(interval_set&& src) : base_type(std::move(src)) {}

  /// Move assignment operator
  interval_set& operator=(interval_set src) {
    base_type::operator=(std::move(src));
    return *this;
  }

private:
  // Private functions that shall be accessible by the baseclass:
  friend class interval_base_set<
      interval_set<DomainT, Compare, Interval, Alloc>, DomainT, Compare,
      Interval, Alloc>;

  iterator handle_inserted(iterator it_) {
    return segmental::join_neighbours(*this, it_);
  }

  iterator add_over(const interval_type& addend, iterator last_) {
    iterator joined_ = segmental::join_under(*this, addend, last_);
    return segmental::join_neighbours(*this, joined_);
  }

  iterator add_over(const interval_type& addend) {
    iterator joined_ = segmental::join_under(*this, addend);
    return segmental::join_neighbours(*this, joined_);
  }
};

//-----------------------------------------------------------------------------
// type traits
//-----------------------------------------------------------------------------
template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_set<icl::interval_set<DomainT, Compare, Interval, Alloc>> {
  using type = is_set<icl::interval_set<DomainT, Compare, Interval, Alloc>>;
  static constexpr bool value = true;
};

template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_interval_container<
    icl::interval_set<DomainT, Compare, Interval, Alloc>> {
  using type = is_interval_container<
      icl::interval_set<DomainT, Compare, Interval, Alloc>>;
  static constexpr bool value = true;
};

template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_interval_joiner<
    icl::interval_set<DomainT, Compare, Interval, Alloc>> {
  using type =
      is_interval_joiner<icl::interval_set<DomainT, Compare, Interval, Alloc>>;
  static constexpr bool value = true;
};

//-----------------------------------------------------------------------------
// type representation
//-----------------------------------------------------------------------------
template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct type_to_string<icl::interval_set<DomainT, Compare, Interval, Alloc>> {
  static std::string apply() {
    return "itv_set<" + type_to_string<DomainT>::apply() + ">";
  }
};

} // namespace icl
