#pragma once

#include "icl/interval_base_set.hpp"
#include "icl/interval_set.hpp"
#include "icl/type_traits/interval_type_default.hpp"
#include "icl/type_traits/is_interval_splitter.hpp"

namespace icl {

/** \brief implements a set as a set of intervals - on insertion
    overlapping intervals are split */
template <typename DomainT, template <typename> typename Compare = std::less,
          typename Interval =
              typename icl::interval_type_default<DomainT, Compare>::type,
          template <typename> typename Alloc = std::allocator>
class split_interval_set
    : public interval_base_set<
          split_interval_set<DomainT, Compare, Interval, Alloc>, DomainT,
          Compare, Interval, Alloc> {
public:
  using type = split_interval_set<DomainT, Compare, Interval, Alloc>;
  using base_type = interval_base_set<type, DomainT, Compare, Interval, Alloc>;

  using joint_type = interval_set<DomainT, Compare, Interval, Alloc>;
  using overloadable_type = type;
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

  enum { fineness = 3 };

public:
  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /// Default constructor for the empty object
  split_interval_set() : base_type() {}

  /// Copy constructor
  split_interval_set(const split_interval_set& src) : base_type(src) {}

  /// Copy constructor for base_type
  template <typename SubType>
  split_interval_set(const interval_base_set<SubType, DomainT, Compare,
                                             Interval, Alloc>& src) {
    this->assign(src);
  }

  /// Constructor for a single element
  explicit split_interval_set(const interval_type& elem) : base_type() {
    this->add(elem);
  }
  /// Constructor for a single interval
  explicit split_interval_set(const domain_type& itv) : base_type() {
    this->add(itv);
  }

  /// Assignment from a base interval_set.
  template <typename SubType>
  void assign(const interval_base_set<SubType, DomainT, Compare, Interval,
                                      Alloc>& src) {
    this->clear();
    this->_set.insert(src.begin(), src.end());
  }

  /// Assignment operator for base type
  template <typename SubType>
  split_interval_set&
  operator=(const interval_base_set<SubType, DomainT, Compare, Interval, Alloc>&
                src) {
    this->assign(src);
    return *this;
  }

  //==========================================================================
  //= Move semantics
  //==========================================================================

  /// Move constructor
  split_interval_set(split_interval_set&& src) : base_type(std::move(src)) {}

  /// Move assignment operator
  split_interval_set& operator=(split_interval_set src) {
    base_type::operator=(std::move(src));
    return *this;
  }
  //==========================================================================

private:
  // Private functions that shall be accessible by the baseclass:
  friend class interval_base_set<
      split_interval_set<DomainT, Compare, Interval, Alloc>, DomainT, Compare,
      Interval, Alloc>;

  iterator handle_inserted(iterator inserted_) { return inserted_; }

  iterator add_over(const interval_type& addend, iterator last_) {
    iterator first_ = this->_set.lower_bound(addend);
    // BOOST_ASSERT(next(last_) == this->_set.upper_bound(inter_val));

    iterator it_ = first_;
    interval_type rest_interval = addend;

    this->add_front(rest_interval, it_);
    this->add_main(rest_interval, it_, last_);
    this->add_rear(rest_interval, it_);
    return it_;
  }

  iterator add_over(const interval_type& addend) {
    std::pair<iterator, iterator> overlap = this->equal_range(addend);
    iterator first_ = overlap.first, end_ = overlap.second, last_ = end_;
    --last_;

    iterator it_ = first_;
    interval_type rest_interval = addend;

    this->add_front(rest_interval, it_);
    this->add_main(rest_interval, it_, last_);
    this->add_rear(rest_interval, it_);

    return it_;
  }
};

//-----------------------------------------------------------------------------
// type traits
//-----------------------------------------------------------------------------
template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_set<icl::split_interval_set<DomainT, Compare, Interval, Alloc>> {
  using type =
      is_set<icl::split_interval_set<DomainT, Compare, Interval, Alloc>>;
  static constexpr bool value = true;
};

template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_interval_container<
    icl::split_interval_set<DomainT, Compare, Interval, Alloc>> {
  using type = is_interval_container<
      icl::split_interval_set<DomainT, Compare, Interval, Alloc>>;
  static constexpr bool value = true;
};

template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_interval_splitter<
    icl::split_interval_set<DomainT, Compare, Interval, Alloc>> {
  using type = is_interval_splitter<
      icl::split_interval_set<DomainT, Compare, Interval, Alloc>>;
  static constexpr bool value = true;
};

//-----------------------------------------------------------------------------
// type representation
//-----------------------------------------------------------------------------
template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct type_to_string<
    icl::split_interval_set<DomainT, Compare, Interval, Alloc>> {
  static std::string apply() {
    return "sp_itv_set<" + type_to_string<DomainT>::apply() + ">";
  }
};

} // namespace icl
