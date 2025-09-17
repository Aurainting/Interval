#pragma once

#include "icl/concept/container.hpp"
#include "icl/concept/element_map.hpp"
#include "icl/concept/interval_associator.hpp"
#include "icl/concept/interval_associator_base.hpp"
#include "icl/concept/interval_set.hpp"
#include "icl/detail/element_iterator.hpp"
#include "icl/detail/exclusive_less_than.hpp"
#include "icl/type_traits/interval_type_default.hpp"
#include <set>

namespace icl {

/** \brief Implements a set as a set of intervals (base class) */
template <typename SubType, typename DomainT,
          template <typename> typename Compare = std::less,
          typename Interval = interval_type_default<DomainT, Compare>::type,
          template <typename> typename Alloc = std::allocator>
  requires std::default_initializable<DomainT> && std::totally_ordered<DomainT>
class interval_base_set {
public:
  //==========================================================================
  //= Associated types
  //==========================================================================
  using type = interval_base_set;

  /// The designated \e derived or \e sub_type of this base class
  using sub_type = SubType;

  /// Auxiliary type for overload resolution
  using overloadable_type = type;

  //--------------------------------------------------------------------------
  //- Associated types: Data
  //--------------------------------------------------------------------------
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

  //--------------------------------------------------------------------------
  //- Associated types: Size
  //--------------------------------------------------------------------------
  /// The difference type of interval which is sometimes different form the
  /// data_type
  using difference_type = difference_type_of<domain_type>::type;

  /// The size type of interval which is mostly std::size_t
  using size_type = size_type_of<domain_type>::type;

  //--------------------------------------------------------------------------
  //- Associated types: Order
  //--------------------------------------------------------------------------
  /// Comparison functor for domain values
  using domain_compare = Compare<DomainT>;

  using segment_compare = Compare<segment_type>;
  /// Comparison functor for intervals
  using interval_compare = exclusive_less_than<interval_type>;

  /// Comparison functor for keys
  using key_compare = exclusive_less_than<interval_type>;

  //--------------------------------------------------------------------------
  //- Associated types: Related types
  //--------------------------------------------------------------------------
  /// The atomized type representing the corresponding container of elements
  using atomized_type = std::set<DomainT, domain_compare, Alloc<DomainT>>;

  //--------------------------------------------------------------------------
  //- Associated types: Implementation and stl related
  //--------------------------------------------------------------------------
  /// The allocator type of the set
  using allocator_type = Alloc<interval_type>;

  /// allocator type of the corresponding element set
  using domain_allocator_type = Alloc<DomainT>;

  /// Container type for the implementation
  using ImplSetT = std::set<interval_type, key_compare, allocator_type>;

  /// key type of the implementing container
  using key_type = ImplSetT::key_type;
  /// data type of the implementing container
  using data_type = ImplSetT::key_type;
  /// value type of the implementing container
  using value_type = ImplSetT::value_type;

  /// pointer type
  using pointer = ImplSetT::pointer;
  /// const pointer type
  using const_pointer = ImplSetT::const_pointer;
  /// reference type
  using reference = ImplSetT::reference;
  /// const reference type
  using const_reference = ImplSetT::const_reference;

  /// iterator for iteration over intervals
  using iterator = ImplSetT::iterator;
  /// const_iterator for iteration over intervals
  using const_iterator = ImplSetT::const_iterator;
  /// iterator for reverse iteration over intervals
  using reverse_iterator = ImplSetT::reverse_iterator;
  /// const_iterator for iteration over intervals
  using const_reverse_iterator = ImplSetT::const_reverse_iterator;

  /// element iterator: Depreciated, see documentation.
  using element_iterator = icl::element_iterator<iterator>;
  /// element const iterator: Depreciated, see documentation.
  using element_const_iterator = icl::element_iterator<const_iterator>;
  /// element reverse iterator: Depreciated, see documentation.
  using element_reverse_iterator = icl::element_iterator<reverse_iterator>;
  /// element const reverse iterator: Depreciated, see documentation.
  using element_const_reverse_iterator =
      icl::element_iterator<const_reverse_iterator>;

  static constexpr int fineness = 0;

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor for the empty object */
  interval_base_set() = default;

  /** Copy constructor */
  interval_base_set(const interval_base_set& src) : _set(src._set) {}

  //==========================================================================
  //= Move semantics
  //==========================================================================

  /** Move constructor */
  interval_base_set(interval_base_set&& src) noexcept
      : _set(std::move(src._set)) {}

  /** Move assignment operator */
  interval_base_set& operator=(
      interval_base_set src) { // call by value since 'src' is a "sink value"
    this->_set = std::move(src._set);
    return *this;
  }

  //==========================================================================

  /** swap the content of containers */
  void swap(interval_base_set& operand) noexcept { _set.swap(operand._set); }

  //==========================================================================
  //= Containedness
  //==========================================================================
  /** sets the container empty */
  void clear() { icl::clear(*that()); }
  /** is the container empty? */
  [[nodiscard]] bool empty() const { return icl::is_empty(*that()); }

  //==========================================================================
  //= Size
  //==========================================================================
  /** An interval set's size is it's cardinality */
  size_type size() const { return icl::cardinality(*that()); }

  /** Size of the iteration over this container */
  [[nodiscard]] std::size_t iterative_size() const { return _set.size(); }

  //==========================================================================
  //= Selection
  //==========================================================================

  /** Find the interval, that contains element \c key_value */
  const_iterator find(const element_type& key_value) const {
    return icl::find(*this, key_value);
  }

  /** Find the first interval, that collides with interval \c key_interval */
  const_iterator find(const interval_type& key_interval) const {
    return this->_set.find(key_interval);
  }

  //==========================================================================
  //= Addition
  //==========================================================================

  /** Add a single element \c key to the set */
  SubType& add(const element_type& key) { return icl::add(*that(), key); }

  /** Add an interval of elements \c inter_val to the set */
  SubType& add(const segment_type& inter_val) {
    _add(inter_val);
    return *that();
  }

  /** Add an interval of elements \c inter_val to the set. Iterator
      \c prior_ is a hint to the position \c inter_val can be
      inserted after. */
  iterator add(iterator prior_, const segment_type& inter_val) {
    return _add(prior_, inter_val);
  }

  //==========================================================================
  //= Subtraction
  //==========================================================================

  /** Subtract a single element \c key from the set */
  SubType& subtract(const element_type& key) {
    return icl::subtract(*that(), key);
  }

  /** Subtract an interval of elements \c inter_val from the set */
  SubType& subtract(const segment_type& inter_val) {
    if (icl::is_empty(inter_val))
      return *that();

    std::pair<iterator, iterator> exterior = equal_range(inter_val);
    if (exterior.first == exterior.second)
      return *that();

    iterator first_ = exterior.first;
    iterator end_ = exterior.second;
    iterator last_ = prior(end_);

    interval_type left_resid = right_subtract(*first_, inter_val);
    interval_type right_resid;
    if (first_ != end_)
      right_resid = left_subtract(*last_, inter_val);

    this->_set.erase(first_, end_);

    if (!icl::is_empty(left_resid))
      this->_set.insert(left_resid);

    if (!icl::is_empty(right_resid))
      this->_set.insert(right_resid);

    return *that();
  }

  //==========================================================================
  //= Insertion
  //==========================================================================
  /** Insert an element \c key into the set */
  SubType& insert(const element_type& key) { return add(key); }

  /** Insert an interval of elements \c inter_val to the set */
  SubType& insert(const segment_type& inter_val) { return add(inter_val); }

  /** Insert an interval of elements \c inter_val to the set. Iterator
      \c prior_ is a hint to the position \c inter_val can be
      inserted after. */
  iterator insert(iterator prior_, const segment_type& inter_val) {
    return add(prior_, inter_val);
  }

  //==========================================================================
  //= Erasure
  //==========================================================================
  /** Erase an element \c key from the set */
  SubType& erase(const element_type& key) { return subtract(key); }

  /** Erase an interval of elements \c inter_val from the set */
  SubType& erase(const segment_type& inter_val) { return subtract(inter_val); }

  /** Erase the interval that iterator \c position points to. */
  void erase(iterator position) { _set.erase(position); }

  /** Erase all intervals in the range <tt>[first,past)</tt> of iterators. */
  void erase(iterator first, iterator past) { _set.erase(first, past); }

  //==========================================================================
  //= Symmetric difference
  //==========================================================================
  /** If \c *this set contains \c key it is erased, otherwise it is added. */
  SubType& flip(const element_type& key) { return icl::flip(*that(), key); }

  /** If \c *this set contains \c inter_val it is erased, otherwise it is added.
   */
  SubType& flip(const segment_type& inter_val) {
    return icl::flip(*that(), inter_val);
  }

  //==========================================================================
  //= Iterator related
  //==========================================================================

  iterator begin() { return _set.begin(); }
  iterator end() { return _set.end(); }
  const_iterator begin() const { return _set.begin(); }
  const_iterator end() const { return _set.end(); }
  reverse_iterator rbegin() { return _set.rbegin(); }
  reverse_iterator rend() { return _set.rend(); }
  const_reverse_iterator rbegin() const { return _set.rbegin(); }
  const_reverse_iterator rend() const { return _set.rend(); }

  iterator lower_bound(const value_type& interval) {
    return _set.lower_bound(interval);
  }

  iterator upper_bound(const value_type& interval) {
    return _set.upper_bound(interval);
  }

  const_iterator lower_bound(const value_type& interval) const {
    return _set.lower_bound(interval);
  }

  const_iterator upper_bound(const value_type& interval) const {
    return _set.upper_bound(interval);
  }

  std::pair<iterator, iterator> equal_range(const key_type& interval) {
    return std::pair<iterator, iterator>(_set.lower_bound(interval),
                                         _set.upper_bound(interval));
  }

  std::pair<const_iterator, const_iterator>
  equal_range(const key_type& interval) const {
    return std::pair<const_iterator, const_iterator>(
        _set.lower_bound(interval), _set.upper_bound(interval));
  }

private:
  iterator _add(const segment_type& addend) {
    using iterator = iterator;
    if (icl::is_empty(addend))
      return this->_set.end();

    std::pair<iterator, bool> insertion = this->_set.insert(addend);

    if (insertion.second)
      return that()->handle_inserted(insertion.first);
    iterator last_ = prior(this->_set.upper_bound(addend));
    return that()->add_over(addend, last_);
  }

  iterator _add(iterator prior_, const segment_type& addend) {
    if (icl::is_empty(addend))
      return prior_;

    iterator insertion = this->_set.insert(prior_, addend);

    if (*insertion == addend)
      return that()->handle_inserted(insertion);
    iterator last_ = prior(this->_set.upper_bound(addend));
    return that()->add_over(addend, last_);
  }

protected:
  void add_front(const interval_type& inter_val, iterator& first_) {
    // If the collision sequence has a left residual 'left_resid' it will
    // be split, to provide a standardized start of algorithms:
    // The addend interval 'inter_val' covers the beginning of the collision
    // sequence.

    // only for the first there can be a left_resid: a part of *first_ left of
    // inter_val
    interval_type left_resid = right_subtract(*first_, inter_val);

    if (!icl::is_empty(left_resid)) { //            [------------ . . .
      // [left_resid---first_ --- . . .
      iterator prior_ = cyclic_prior(*this, first_);
      const_cast<interval_type&>(*first_) = left_subtract(*first_, left_resid);
      // NOTE: Only splitting
      this->_set.insert(prior_, left_resid);
    }

    // POST:
    //  [----- inter_val ---- . . .
    //  ...[-- first_ --...
  }

  void add_main(interval_type& inter_val, iterator& it_,
                const iterator& last_) {
    interval_type cur_interval;
    while (it_ != last_) {
      cur_interval = *it_;
      add_segment(inter_val, it_);
      // shrink interval
      inter_val = left_subtract(inter_val, cur_interval);
    }
  }

  void add_segment(const interval_type& inter_val, iterator& it_) {
    interval_type lead_gap = right_subtract(inter_val, *it_);
    if (!icl::is_empty(lead_gap))
      //           [lead_gap--- . . .
      // [prior_)           [-- it_ ...
      this->_set.insert(cyclic_prior(*this, it_), lead_gap);

    // . . . --------- . . . addend interval
    //      [-- it_ --)      has a common part with the first overval
    ++it_;
  }

  void add_rear(const interval_type& inter_val, iterator& it_) {
    iterator prior_ = cyclic_prior(*this, it_);
    interval_type cur_itv = *it_;

    interval_type lead_gap = right_subtract(inter_val, cur_itv);
    if (!icl::is_empty(lead_gap))
      //          [lead_gap--- . . .
      // [prior_)          [-- it_ ...
      this->_set.insert(prior_, lead_gap);

    interval_type end_gap = left_subtract(inter_val, cur_itv);
    if (!icl::is_empty(end_gap))
      // [---------------end_gap)
      //      [-- it_ --)
      it_ = this->_set.insert(it_, end_gap);
    else {
      // only for the last there can be a right_resid: a part of *it_ right of
      // addend
      interval_type right_resid = left_subtract(cur_itv, inter_val);

      if (!icl::is_empty(right_resid)) {
        // [--------------)
        //      [-- it_ --right_resid)
        const_cast<interval_type&>(*it_) = right_subtract(*it_, right_resid);
        it_ = this->_set.insert(it_, right_resid);
      }
    }
  }

  sub_type* that() { return static_cast<sub_type*>(this); }
  const sub_type* that() const { return static_cast<const sub_type*>(this); }

  ImplSetT _set;
};

//-----------------------------------------------------------------------------
// type traits
//-----------------------------------------------------------------------------
template <typename SubType, typename DomainT,
          template <typename> typename Compare, typename Interval,
          template <typename> typename Alloc>
struct is_set<
    icl::interval_base_set<SubType, DomainT, Compare, Interval, Alloc>> {
  using type = is_set;
  static constexpr bool value = true;
};

template <typename SubType, typename DomainT,
          template <typename> typename Compare, typename Interval,
          template <typename> typename Alloc>
struct is_interval_container<
    icl::interval_base_set<SubType, DomainT, Compare, Interval, Alloc>> {
  using type = is_interval_container;
  static constexpr bool value = true;
};

} // namespace icl
