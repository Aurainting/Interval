#pragma once

#include "icl/interval_base_set.hpp"
#include "icl/interval_set.hpp"
#include "icl/type_traits/interval_type_default.hpp"
#include "icl/type_traits/is_interval_separator.hpp"

namespace icl {

/** \brief Implements a set as a set of intervals - leaving adjoining intervals
 * separate */
template <typename DomainT, template <typename> typename Compare = std::less,
          typename Interval = interval_type_default<DomainT, Compare>::type,
          template <typename> typename Alloc = std::allocator>
class separate_interval_set
    : public interval_base_set<
          separate_interval_set<DomainT, Compare, Interval, Alloc>, DomainT,
          Compare, Interval, Alloc> {
public:
  using type = separate_interval_set;

  using base_type = interval_base_set<type, DomainT, Compare, Interval, Alloc>;

  using overloadable_type = type;
  using key_object_type = type;

  using joint_type = interval_set<DomainT, Compare, Interval, Alloc>;

  /// The domain type of the set
  using domain_type = DomainT;
  /// The codomain type is the same as domain_type
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
  using atomized_type = base_type::atomized_type;

  /// Container type for the implementation
  using ImplSetT = base_type::ImplSetT;

  /// key type of the implementing container
  using key_type = ImplSetT::key_type;
  /// data type of the implementing container
  using data_type = ImplSetT::value_type;
  /// value type of the implementing container
  using value_type = ImplSetT::value_type;

  /// iterator for iteration over intervals
  using iterator = ImplSetT::iterator;
  /// const_iterator for iteration over intervals
  using const_iterator = ImplSetT::const_iterator;

  enum { fineness = 2 };

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /// Default constructor for the empty object
  separate_interval_set() : base_type() {}
  /// Copy constructor
  separate_interval_set(const separate_interval_set& src) : base_type(src) {}

  /// Copy constructor for base_type
  template <typename SubType>
  explicit separate_interval_set(
      const interval_base_set<SubType, DomainT, Compare, Interval, Alloc>&
          src) {
    this->assign(src);
  }

  /// Constructor for a single element
  explicit separate_interval_set(const domain_type& elem) : base_type() {
    this->add(elem);
  }
  /// Constructor for a single interval
  explicit separate_interval_set(const interval_type& itv) : base_type() {
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
  separate_interval_set&
  operator=(const interval_base_set<SubType, DomainT, Compare, Interval, Alloc>&
                src) {
    this->assign(src);
    return *this;
  }

  //==========================================================================
  //= Move semantics
  //==========================================================================
  /// Move constructor
  separate_interval_set(separate_interval_set&& src) noexcept
      : base_type(std::move(src)) {}

  /// Move assignment operator
  separate_interval_set& operator=(separate_interval_set src) {
    base_type::operator=(std::move(src));
    return *this;
  }
  //==========================================================================

private:
  // Private functions that shall be accessible by the baseclass:
  friend class interval_base_set<separate_interval_set, DomainT, Compare,
                                 Interval, Alloc>;

  static iterator handle_inserted(iterator inserted_) { return inserted_; }

  iterator add_over(const interval_type& addend, iterator last_) {
    return segmental::join_under(*this, addend, last_);
  }

  iterator add_over(const interval_type& addend) {
    return segmental::join_under(*this, addend);
  }
};

//-----------------------------------------------------------------------------
// type traits
//-----------------------------------------------------------------------------
template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_set<separate_interval_set<DomainT, Compare, Interval, Alloc>> {
  using type = is_set;
  static constexpr bool value = true;
};

template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_interval_container<
    separate_interval_set<DomainT, Compare, Interval, Alloc>> {
  using type = is_interval_container;
  static constexpr bool value = true;
};

template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct is_interval_separator<
    separate_interval_set<DomainT, Compare, Interval, Alloc>> {
  using type = is_interval_separator;
  static constexpr bool value = true;
};

//-----------------------------------------------------------------------------
// type representation
//-----------------------------------------------------------------------------
template <typename DomainT, template <typename> typename Compare,
          typename Interval, template <typename> typename Alloc>
struct type_to_string<
    separate_interval_set<DomainT, Compare, Interval, Alloc>> {
  static std::string apply() {
    return "se_itv_set<" + type_to_string<DomainT>::apply() + ">";
  }
};

} // namespace icl
