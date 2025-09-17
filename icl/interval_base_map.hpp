#pragma once

#include "icl/concept/container.hpp"
#include "icl/concept/interval_associator.hpp"
#include "icl/concept/interval_associator_base.hpp"
#include "icl/concept/interval_map.hpp"
#include "icl/detail/element_iterator.hpp"
#include "icl/detail/exclusive_less_than.hpp"
#include "icl/detail/on_absorbtion.hpp"
#include "icl/map.hpp"
#include "icl/type_traits/has_set_semantics.hpp"
#include "icl/type_traits/interval_type_default.hpp"
#include "icl/type_traits/is_interval_splitter.hpp"
#include <cassert>
#include <concepts>
#include <type_traits>

namespace icl {

template <typename DomainT, typename CodomainT> struct mapping_pair {
  DomainT key;
  CodomainT data;

  mapping_pair() : key(), data() {}

  mapping_pair(const DomainT& key_value, const CodomainT& data_value)
      : key(key_value), data(data_value) {}

  explicit mapping_pair(const std::pair<DomainT, CodomainT>& std_pair)
      : key(std_pair.first), data(std_pair.second) {}
};

/** \brief Implements a map as a map of intervals (base class) */
template <typename SubType, typename DomainT, typename CodomainT,
          typename Traits = partial_absorber,
          template <typename> typename Compare = std::less,
          template <typename> typename Combine = inplace_plus,
          template <typename> typename Section = inter_section,
          typename Interval = interval_type_default<DomainT, Compare>::type,
          template <typename> typename Alloc = std::allocator>
  requires std::default_initializable<DomainT> &&
           std::totally_ordered<DomainT> &&
           std::default_initializable<CodomainT> &&
           std::equality_comparable<CodomainT>
class interval_base_map {
public:
  //==========================================================================
  //= Associated types
  //==========================================================================
  using type = interval_base_map;

  /// The designated \e derived or \e sub_type of this base class
  using sub_type = SubType;

  /// Auxiliary type for overload resolution
  using overloadable_type = type;

  /// Traits of an itl map
  using traits = Traits;

  //--------------------------------------------------------------------------
  //- Associated types: Related types
  //--------------------------------------------------------------------------
  /// The atomized type representing the corresponding container of elements
  using atomized_type =
      map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>;

  //--------------------------------------------------------------------------
  //- Associated types: Data
  //--------------------------------------------------------------------------
  /// Domain type (type of the keys) of the map
  using domain_type = DomainT;
  using domain_param = domain_param_traits<DomainT>::type;
  /// Domain type (type of the keys) of the map
  using codomain_type = CodomainT;
  /// Auxiliary type to help the compiler resolve ambiguities when using
  /// std::make_pair
  using domain_mapping_type = mapping_pair<domain_type, codomain_type>;
  /// Conceptual is a map a set of elements of type \c element_type
  using element_type = domain_mapping_type;
  /// The interval type of the map
  using interval_type = Interval;
  /// Auxiliary type for overload resolution
  using interval_mapping_type = std::pair<interval_type, CodomainT>;
  /// Type of interval containers segment, that is spanned by an interval
  using segment_type = std::pair<interval_type, CodomainT>;

  //--------------------------------------------------------------------------
  //- Associated types: Size
  //--------------------------------------------------------------------------
  /// The difference type of interval which is sometimes different form the
  /// domain_type
  using difference_type = difference_type_of<domain_type>::type;
  /// The size type of interval which is mostly std::size_t
  using size_type = size_type_of<domain_type>::type;

  //--------------------------------------------------------------------------
  //- Associated types: Functors
  //--------------------------------------------------------------------------
  /// Comparison functor for domain values
  using domain_compare = Compare<DomainT>;
  using segment_compare = Compare<segment_type>;
  /// Combine functor for codomain value aggregation
  using codomain_combine = Combine<CodomainT>;
  /// Inverse Combine functor for codomain value aggregation
  using inverse_codomain_combine = inverse<codomain_combine>::type;
  /// Intersection functor for codomain values

  using codomain_intersect =
      std::conditional_t<has_set_semantics<codomain_type>::value,
                         Section<CodomainT>, codomain_combine>;

  /// Inverse Combine functor for codomain value intersection
  using inverse_codomain_intersect = inverse<codomain_intersect>::type;

  /// Comparison functor for intervals which are keys as well
  using interval_compare = exclusive_less_than<interval_type>;

  /// Comparison functor for keys
  using key_compare = exclusive_less_than<interval_type>;

  //--------------------------------------------------------------------------
  //- Associated types: Implementation and stl related
  //--------------------------------------------------------------------------
  /// The allocator type of the set
  using allocator_type = Alloc<std::pair<const interval_type, codomain_type>>;

  /// Container type for the implementation
  using ImplMapT =
      std::map<interval_type, codomain_type, key_compare, allocator_type>;

  /// key type of the implementing container
  using key_type = ImplMapT::key_type;
  /// value type of the implementing container
  using value_type = ImplMapT::value_type;
  /// data type of the implementing container
  using data_type = ImplMapT::value_type::second_type;

  /// pointer type
  using pointer = ImplMapT::pointer;
  /// const pointer type
  using const_pointer = ImplMapT::const_pointer;
  /// reference type
  using reference = ImplMapT::reference;
  /// const reference type
  using const_reference = ImplMapT::const_reference;

  /// iterator for iteration over intervals
  using iterator = ImplMapT::iterator;
  /// const_iterator for iteration over intervals
  using const_iterator = ImplMapT::const_iterator;
  /// iterator for reverse iteration over intervals
  using reverse_iterator = ImplMapT::reverse_iterator;
  /// const_iterator for iteration over intervals
  using const_reverse_iterator = ImplMapT::const_reverse_iterator;

  /// element iterator: Depreciated, see documentation.
  using element_iterator = icl::element_iterator<iterator>;
  /// const element iterator: Depreciated, see documentation.
  using element_const_iterator = icl::element_iterator<const_iterator>;
  /// element reverse iterator: Depreciated, see documentation.
  using element_reverse_iterator = icl::element_iterator<reverse_iterator>;
  /// element const reverse iterator: Depreciated, see documentation.
  using element_const_reverse_iterator =
      icl::element_iterator<const_reverse_iterator>;

  using on_codomain_absorbtion =
      on_absorbtion<type, codomain_combine, Traits::absorbs_identities>::type;

  static constexpr bool is_total_invertible =
      Traits::is_total && has_inverse<codomain_type>::value;

  static constexpr int fineness = 0;

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /** Default constructor for the empty object */
  interval_base_map() = default;

  /** Copy constructor */
  interval_base_map(const interval_base_map& src) : _map(src._map) {}

  //==========================================================================
  //= Move semantics
  //==========================================================================

  /** Move constructor */
  interval_base_map(interval_base_map&& src) noexcept
      : _map(std::move(src._map)) {}

  /** Move assignment operator */
  interval_base_map& operator=(
      interval_base_map src) { // call by value sice 'src' is a "sink value"
    this->_map = std::move(src._map);
    return *this;
  }

  //==========================================================================

  /** swap the content of containers */
  void swap(interval_base_map& object) noexcept { _map.swap(object._map); }

  //==========================================================================
  //= Containedness
  //==========================================================================
  /** clear the map */
  void clear() { icl::clear(*that()); }

  /** is the map empty? */
  [[nodiscard]] bool empty() const { return icl::is_empty(*that()); }

  //==========================================================================
  //= Size
  //==========================================================================
  /** An interval map's size is it's cardinality */
  size_type size() const { return icl::cardinality(*that()); }

  /** Size of the iteration over this container */
  [[nodiscard]] std::size_t iterative_size() const { return _map.size(); }

  //==========================================================================
  //= Selection
  //==========================================================================

  /** Find the interval value pair, that contains \c key */
  const_iterator find(const domain_type& key_value) const {
    return icl::find(*this, key_value);
  }

  /** Find the first interval value pair, that collides with interval
      \c key_interval */
  const_iterator find(const interval_type& key_interval) const {
    return _map.find(key_interval);
  }

  /** Total select function. */
  codomain_type operator()(const domain_type& key_value) const {
    const_iterator it_ = icl::find(*this, key_value);
    return it_ == end() ? identity_element<codomain_type>::value()
                        : (*it_).second;
  }

  //==========================================================================
  //= Addition
  //==========================================================================

  /** Addition of a key value pair to the map */
  SubType& add(const element_type& key_value_pair) {
    return icl::add(*that(), key_value_pair);
  }

  /** Addition of an interval value pair to the map. */
  SubType& add(const segment_type& interval_value_pair) {
    this->_add<codomain_combine>(interval_value_pair);
    return *that();
  }

  /** Addition of an interval value pair \c interval_value_pair to the map.
      Iterator \c prior_ is a hint to the position \c interval_value_pair can be
      inserted after. */
  iterator add(iterator prior_, const segment_type& interval_value_pair) {
    return this->_add<codomain_combine>(prior_, interval_value_pair);
  }

  //==========================================================================
  //= Subtraction
  //==========================================================================
  /** Subtraction of a key value pair from the map */
  SubType& subtract(const element_type& key_value_pair) {
    return icl::subtract(*that(), key_value_pair);
  }

  /** Subtraction of an interval value pair from the map. */
  SubType& subtract(const segment_type& interval_value_pair) {
    on_invertible<type, is_total_invertible>::subtract(*that(),
                                                       interval_value_pair);
    return *that();
  }

  //==========================================================================
  //= Insertion
  //==========================================================================
  /** Insertion of a \c key_value_pair into the map. */
  SubType& insert(const element_type& key_value_pair) {
    return icl::insert(*that(), key_value_pair);
  }

  /** Insertion of an \c interval_value_pair into the map. */
  SubType& insert(const segment_type& interval_value_pair) {
    _insert(interval_value_pair);
    return *that();
  }

  /** Insertion of an \c interval_value_pair into the map. Iterator \c prior_.
      serves as a hint to insert after the element \c prior point to. */
  iterator insert(iterator prior, const segment_type& interval_value_pair) {
    return _insert(prior, interval_value_pair);
  }

  /** With <tt>key_value_pair = (k,v)</tt> set value \c v for key \c k */
  SubType& set(const element_type& key_value_pair) {
    return icl::set_at(*that(), key_value_pair);
  }

  /** With <tt>interval_value_pair = (I,v)</tt> set value \c v
      for all keys in interval \c I in the map. */
  SubType& set(const segment_type& interval_value_pair) {
    return icl::set_at(*that(), interval_value_pair);
  }

  //==========================================================================
  //= Erasure
  //==========================================================================
  /** Erase a \c key_value_pair from the map. */
  SubType& erase(const element_type& key_value_pair) {
    icl::erase(*that(), key_value_pair);
    return *that();
  }

  /** Erase an \c interval_value_pair from the map. */
  SubType& erase(const segment_type& interval_value_pair) {
    interval_type inter_val = interval_value_pair.first;
    if (icl::is_empty(inter_val))
      return *that();

    const codomain_type& co_val = interval_value_pair.second;
    if (on_codomain_absorbtion::is_absorbable(co_val))
      return *that();

    std::pair<iterator, iterator> exterior = equal_range(inter_val);
    if (exterior.first == exterior.second)
      return *that();

    iterator first_ = exterior.first, end_ = exterior.second,
             last_ = cyclic_prior(*this, end_);
    iterator second_ = first_;
    ++second_;

    if (first_ == last_) { //     [----inter_val----)
      //   .....first_==last_.....
      // only for the last there can be a right_resid: a part of *it_ right of
      // minuend
      interval_type right_resid = left_subtract((*first_).first, inter_val);

      if ((*first_).second == co_val) {
        interval_type left_resid = right_subtract((*first_).first, inter_val);
        if (!icl::is_empty(left_resid)) //            [----inter_val----)
        {                               // [left_resid)..first_==last_......
          const_cast<interval_type&>((*first_).first) = left_resid;
          if (!icl::is_empty(right_resid))
            this->_map.insert(first_, value_type(right_resid, co_val));
        } else if (!icl::is_empty(right_resid))
          const_cast<interval_type&>((*first_).first) = right_resid;
        else
          this->_map.erase(first_);
      }
    } else {
      // first AND NOT last
      if ((*first_).second == co_val) {
        interval_type left_resid = right_subtract((*first_).first, inter_val);
        if (icl::is_empty(left_resid))
          this->_map.erase(first_);
        else
          const_cast<interval_type&>((*first_).first) = left_resid;
      }

      erase_rest(inter_val, co_val, second_, last_);
    }

    return *that();
  }

  /** Erase a key value pair for \c key. */
  SubType& erase(const domain_type& key) { return icl::erase(*that(), key); }

  /** Erase all value pairs within the range of the
      interval <tt>inter_val</tt> from the map.   */
  SubType& erase(const interval_type& inter_val) {
    if (icl::is_empty(inter_val))
      return *that();

    std::pair<iterator, iterator> exterior = equal_range(inter_val);
    if (exterior.first == exterior.second)
      return *that();

    iterator first_ = exterior.first, end_ = exterior.second,
             last_ = prior(end_);

    interval_type left_resid = right_subtract((*first_).first, inter_val);
    interval_type right_resid = left_subtract(last_->first, inter_val);

    if (first_ == last_)
      if (!icl::is_empty(left_resid)) {
        const_cast<interval_type&>((*first_).first) = left_resid;
        if (!icl::is_empty(right_resid))
          this->_map.insert(first_, value_type(right_resid, (*first_).second));
      } else if (!icl::is_empty(right_resid))
        const_cast<interval_type&>((*first_).first) =
            left_subtract((*first_).first, inter_val);
      else
        this->_map.erase(first_);
    else { //            [-------- minuend ---------)
      // [left_resid   fst)   . . . .    [lst  right_resid)
      iterator second_ = first_;
      ++second_;

      iterator start_ = icl::is_empty(left_resid) ? first_ : second_;
      iterator stop_ = icl::is_empty(right_resid) ? end_ : last_;
      this->_map.erase(start_, stop_); // erase [start_, stop_)

      if (!icl::is_empty(left_resid))
        const_cast<interval_type&>((*first_).first) = left_resid;

      if (!icl::is_empty(right_resid))
        const_cast<interval_type&>(last_->first) = right_resid;
    }

    return *that();
  }

  /** Erase all value pairs within the range of the interval that iterator
      \c position points to. */
  void erase(iterator position) { this->_map.erase(position); }

  /** Erase all value pairs for a range of iterators <tt>[first,past)</tt>. */
  void erase(iterator first, iterator past) { this->_map.erase(first, past); }

  //==========================================================================
  //= Intersection
  //==========================================================================
  /** The intersection of \c interval_value_pair and \c *this map is added to \c
   * section. */
  void add_intersection(SubType& section,
                        const segment_type& interval_value_pair) const {
    on_definedness<SubType, Traits::is_total>::add_intersection(
        section, *that(), interval_value_pair);
  }

  //==========================================================================
  //= Symmetric difference
  //==========================================================================
  /** If \c *this map contains \c key_value_pair it is erased, otherwise it is
   * added. */
  SubType& flip(const element_type& key_value_pair) {
    return icl::flip(*that(), key_value_pair);
  }

  /** If \c *this map contains \c interval_value_pair it is erased, otherwise it
   * is added. */
  SubType& flip(const segment_type& interval_value_pair) {
    on_total_absorbable<SubType, Traits::is_total,
                        Traits::absorbs_identities>::flip(*that(),
                                                          interval_value_pair);
    return *that();
  }

  //==========================================================================
  //= Iterator related
  //==========================================================================

  iterator lower_bound(const key_type& interval) {
    return _map.lower_bound(interval);
  }

  iterator upper_bound(const key_type& interval) {
    return _map.upper_bound(interval);
  }

  const_iterator lower_bound(const key_type& interval) const {
    return _map.lower_bound(interval);
  }

  const_iterator upper_bound(const key_type& interval) const {
    return _map.upper_bound(interval);
  }

  std::pair<iterator, iterator> equal_range(const key_type& interval) {
    return std::pair<iterator, iterator>(lower_bound(interval),
                                         upper_bound(interval));
  }

  std::pair<const_iterator, const_iterator>
  equal_range(const key_type& interval) const {
    return std::pair<const_iterator, const_iterator>(lower_bound(interval),
                                                     upper_bound(interval));
  }

  iterator begin() { return _map.begin(); }
  iterator end() { return _map.end(); }
  const_iterator begin() const { return _map.begin(); }
  const_iterator end() const { return _map.end(); }
  reverse_iterator rbegin() { return _map.rbegin(); }
  reverse_iterator rend() { return _map.rend(); }
  const_reverse_iterator rbegin() const { return _map.rbegin(); }
  const_reverse_iterator rend() const { return _map.rend(); }

private:
  template <typename Combiner>
  iterator _add(const segment_type& interval_value_pair) {
    using on_absorbtion_ =
        on_absorbtion<type, Combiner, absorbs_identities<type>::value>::type;

    const interval_type& inter_val = interval_value_pair.first;
    if (icl::is_empty(inter_val))
      return this->_map.end();

    const codomain_type& co_val = interval_value_pair.second;
    if (on_absorbtion_::is_absorbable(co_val))
      return this->_map.end();

    std::pair<iterator, bool> insertion =
        this->_map.insert(value_type(inter_val, version<Combiner>()(co_val)));

    if (insertion.second)
      return that()->handle_inserted(insertion.first);
    // Detect the first and the end iterator of the collision sequence
    iterator first_ = this->_map.lower_bound(inter_val),
             last_ = prior(this->_map.upper_bound(inter_val));
    // assert(end_ == this->_map.upper_bound(inter_val));
    iterator it_ = first_;
    interval_type rest_interval = inter_val;

    add_front(rest_interval, it_);
    add_main<Combiner>(rest_interval, co_val, it_, last_);
    add_rear<Combiner>(rest_interval, co_val, it_);
    return it_;
  }

  template <typename Combiner>
  iterator _add(iterator prior_, const segment_type& interval_value_pair) {
    using on_absorbtion_ =
        on_absorbtion<type, Combiner, absorbs_identities<type>::value>::type;

    const interval_type& inter_val = interval_value_pair.first;
    if (icl::is_empty(inter_val))
      return prior_;

    const codomain_type& co_val = interval_value_pair.second;
    if (on_absorbtion_::is_absorbable(co_val))
      return prior_;

    std::pair<iterator, bool> insertion =
        add_at<Combiner>(prior_, inter_val, co_val);

    if (insertion.second)
      return that()->handle_inserted(insertion.first);
    // Detect the first and the end iterator of the collision sequence
    std::pair<iterator, iterator> overlap = equal_range(inter_val);
    iterator it_ = overlap.first, last_ = prior(overlap.second);
    interval_type rest_interval = inter_val;

    add_front(rest_interval, it_);
    add_main<Combiner>(rest_interval, co_val, it_, last_);
    add_rear<Combiner>(rest_interval, co_val, it_);
    return it_;
  }

  template <typename Combiner>
  void _subtract(const segment_type& interval_value_pair) {
    interval_type inter_val = interval_value_pair.first;
    if (icl::is_empty(inter_val))
      return;

    const codomain_type& co_val = interval_value_pair.second;
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable(co_val))
      return;

    std::pair<iterator, iterator> exterior = equal_range(inter_val);
    if (exterior.first == exterior.second)
      return;

    iterator last_ = prior(exterior.second);
    iterator it_ = exterior.first;
    subtract_front(inter_val, it_);
    subtract_main<Combiner>(co_val, it_, last_);
    subtract_rear<Combiner>(inter_val, co_val, it_);
  }

  iterator _insert(const segment_type& interval_value_pair) {
    interval_type inter_val = interval_value_pair.first;
    if (icl::is_empty(inter_val))
      return this->_map.end();

    const codomain_type& co_val = interval_value_pair.second;
    if (on_codomain_absorbtion::is_absorbable(co_val))
      return this->_map.end();

    std::pair<iterator, bool> insertion =
        this->_map.insert(interval_value_pair);

    if (insertion.second)
      return that()->handle_inserted(insertion.first);
    // Detect the first and the end iterator of the collision sequence
    iterator first_ = this->_map.lower_bound(inter_val),
             last_ = prior(this->_map.upper_bound(inter_val));
    // assert((++last_) == this->_map.upper_bound(inter_val));
    iterator it_ = first_;
    insert_main(inter_val, co_val, it_, last_);
    return it_;
  }

  iterator _insert(iterator prior_, const segment_type& interval_value_pair) {
    interval_type inter_val = interval_value_pair.first;
    if (icl::is_empty(inter_val))
      return prior_;

    const codomain_type& co_val = interval_value_pair.second;
    if (on_codomain_absorbtion::is_absorbable(co_val))
      return prior_;

    std::pair<iterator, bool> insertion = insert_at(prior_, inter_val, co_val);

    if (insertion.second)
      return that()->handle_inserted(insertion.first);
    {
      // Detect the first and the end iterator of the collision sequence
      std::pair<iterator, iterator> overlap = equal_range(inter_val);
      iterator it_ = overlap.first, last_ = prior(overlap.second);
      insert_main(inter_val, co_val, it_, last_);
      return it_;
    }
  }

private:
  template <typename Combiner>
  void add_segment(const interval_type& inter_val, const CodomainT& co_val,
                   iterator& it_) {
    interval_type lead_gap = right_subtract(inter_val, (*it_).first);
    if (!icl::is_empty(lead_gap)) {
      // [lead_gap--- . . .
      //          [-- it_ ...
      iterator prior_ = it_ == this->_map.begin() ? it_ : prior(it_);
      iterator inserted_ = this->gap_insert<Combiner>(prior_, lead_gap, co_val);
      that()->handle_inserted(prior_, inserted_);
    }

    // . . . --------- . . . addend interval
    //      [-- it_ --)      has a common part with the first overval
    Combiner()((*it_).second, co_val);
    that()->template handle_left_combined<Combiner>(it_++);
  }

  template <typename Combiner>
  void add_main(interval_type& inter_val, const CodomainT& co_val,
                iterator& it_, const iterator& last_) {
    interval_type cur_interval;
    while (it_ != last_) {
      cur_interval = (*it_).first;
      add_segment<Combiner>(inter_val, co_val, it_);
      // shrink interval
      inter_val = left_subtract(inter_val, cur_interval);
    }
  }

  template <typename Combiner>
  void add_rear(const interval_type& inter_val, const CodomainT& co_val,
                iterator& it_) {
    iterator prior_ = cyclic_prior(*that(), it_);
    interval_type cur_itv = (*it_).first;

    interval_type lead_gap = right_subtract(inter_val, cur_itv);
    if (!icl::is_empty(lead_gap)) { //         [lead_gap--- . . .
      // [prior)          [-- it_ ...
      iterator inserted_ =
          this->template gap_insert<Combiner>(prior_, lead_gap, co_val);
      that()->handle_inserted(prior_, inserted_);
    }

    interval_type end_gap = left_subtract(inter_val, cur_itv);
    if (!icl::is_empty(end_gap)) {
      // [----------------end_gap)
      //  . . . -- it_ --)
      Combiner()((*it_).second, co_val);
      that()->template gap_insert_at<Combiner>(it_, prior_, end_gap, co_val);
    } else {
      // only for the last there can be a right_resid: a part of *it_ right of x
      interval_type right_resid = left_subtract(cur_itv, inter_val);

      if (icl::is_empty(right_resid)) {
        // [---------------)
        //      [-- it_ ---)
        Combiner()((*it_).second, co_val);
        that()->template handle_preceeded_combined<Combiner>(prior_, it_);
      } else {
        // [--------------)
        //      [-- it_ --right_resid)
        const_cast<interval_type&>((*it_).first) =
            right_subtract((*it_).first, right_resid);

        // NOTE: This is NOT an insertion that has to take care for correct
        // application of
        //  the Combiner functor. It only reestablished that state after
        //  splitting the 'it_' interval value pair. Using _map_insert<Combiner>
        //  does not work here.
        iterator insertion_ =
            this->_map.insert(it_, value_type(right_resid, (*it_).second));
        that()->handle_reinserted(insertion_);

        Combiner()((*it_).second, co_val);
        that()->template handle_preceeded_combined<Combiner>(insertion_, it_);
      }
    }
  }

  void add_front(const interval_type& inter_val, iterator& first_) {
    // If the collision sequence has a left residual 'left_resid' it will
    // be split, to provide a standardized start of algorithms:
    // The addend interval 'inter_val' covers the beginning of the collision
    // sequence.

    // only for the first there can be a left_resid: a part of *first_ left of
    // inter_val
    interval_type left_resid = right_subtract((*first_).first, inter_val);

    if (!icl::is_empty(left_resid)) { //            [------------ . . .
      // [left_resid---first_ --- . . .
      iterator prior_ = cyclic_prior(*this, first_);
      const_cast<interval_type&>((*first_).first) =
          left_subtract((*first_).first, left_resid);
      // NOTE: Only splitting
      this->_map.insert(prior_, segment_type(left_resid, (*first_).second));
    }
    // POST:
    //  [----- inter_val ---- . . .
    //  ...[-- first_ --...
  }

private:
  void subtract_front(const interval_type& inter_val, iterator& first_) {
    if (interval_type left_resid = right_subtract((*first_).first, inter_val);
        !icl::is_empty(left_resid)) //                     [--- inter_val ---)
    {                               //[prior_) [left_resid)[--- it_ . . .
      iterator prior_ = cyclic_prior(*this, first_);
      const_cast<interval_type&>((*first_).first) =
          left_subtract((*first_).first, left_resid);
      this->_map.insert(prior_, value_type(left_resid, (*first_).second));
      // The segemnt *it_ is split at inter_val.first(), so as an invariant
      // segment *it_ is always "under" inter_val and a left_resid is empty.
    }
  }

  template <typename Combiner>
  void subtract_main(const CodomainT& co_val, iterator& it_,
                     const iterator& last_) {
    while (it_ != last_) {
      Combiner()((*it_).second, co_val);
      that()->template handle_left_combined<Combiner>(it_++);
    }
  }

  template <typename Combiner>
  void subtract_rear(interval_type& inter_val, const CodomainT& co_val,
                     iterator& it_) {
    interval_type right_resid = left_subtract((*it_).first, inter_val);

    if (icl::is_empty(right_resid)) {
      Combiner()((*it_).second, co_val);
      that()->template handle_combined<Combiner>(it_);
    } else {
      const_cast<interval_type&>((*it_).first) =
          right_subtract((*it_).first, right_resid);
      iterator next_ =
          this->_map.insert(it_, value_type(right_resid, (*it_).second));
      Combiner()((*it_).second, co_val);
      that()->template handle_succeeded_combined<Combiner>(it_, next_);
    }
  }

private:
  void insert_main(const interval_type& inter_val, const CodomainT& co_val,
                   iterator& it_, const iterator& last_) {
    iterator end_ = std::next(last_);
    iterator prior_ = cyclic_prior(*this, it_), inserted_;
    interval_type rest_interval = inter_val, left_gap, cur_itv;
    interval_type last_interval = last_->first;

    while (it_ != end_) {
      cur_itv = (*it_).first;
      left_gap = right_subtract(rest_interval, cur_itv);

      if (!icl::is_empty(left_gap)) {
        inserted_ = this->_map.insert(prior_, value_type(left_gap, co_val));
        it_ = that()->handle_inserted(inserted_);
      }

      // shrink interval
      rest_interval = left_subtract(rest_interval, cur_itv);
      prior_ = it_;
      ++it_;
    }

    // insert_rear(rest_interval, co_val, last_):
    if (interval_type end_gap = left_subtract(rest_interval, last_interval);
        !icl::is_empty(end_gap)) {
      inserted_ = this->_map.insert(prior_, value_type(end_gap, co_val));
      it_ = that()->handle_inserted(inserted_);
    } else
      it_ = prior_;
  }

  void erase_rest(interval_type& inter_val, const CodomainT& co_val,
                  iterator& it_, const iterator& last_) {
    // For all intervals within loop: (*it_).first are contained_in inter_val
    while (it_ != last_)
      if ((*it_).second == co_val)
        this->_map.erase(it_++);
      else
        it_++;

    // erase_rear:
    if ((*it_).second == co_val) {
      interval_type right_resid = left_subtract((*it_).first, inter_val);
      if (icl::is_empty(right_resid))
        this->_map.erase(it_);
      else
        const_cast<interval_type&>((*it_).first) = right_resid;
    }
  }

  template <typename FragmentT>
  void total_add_intersection(SubType& section,
                              const FragmentT& fragment) const {
    section += *that();
    section.add(fragment);
  }

  void partial_add_intersection(SubType& section,
                                const segment_type& operand) const {
    interval_type inter_val = operand.first;
    if (icl::is_empty(inter_val))
      return;

    std::pair<const_iterator, const_iterator> exterior = equal_range(inter_val);
    if (exterior.first == exterior.second)
      return;

    for (const_iterator it_ = exterior.first; it_ != exterior.second; it_++) {
      interval_type common_interval = (*it_).first & inter_val;
      if (!icl::is_empty(common_interval)) {
        section.template _add<codomain_combine>(
            value_type(common_interval, (*it_).second));
        section.template _add<codomain_intersect>(
            value_type(common_interval, operand.second));
      }
    }
  }

  void partial_add_intersection(SubType& section,
                                const element_type& operand) const {
    partial_add_intersection(section, make_segment<type>(operand));
  }

protected:
  template <typename Combiner>
  iterator gap_insert(iterator prior_, const interval_type& inter_val,
                      const codomain_type& co_val) {
    // inter_val is not conained in this map. Insertion will be successful
    assert(!this->_map.contains(inter_val));
    assert((!on_absorbtion<type, Combiner,
                           Traits::absorbs_identities>::is_absorbable(co_val)));
    return this->_map.insert(
        prior_, value_type(inter_val, version<Combiner>()(co_val)));
  }

  template <typename Combiner>
  std::pair<iterator, bool> add_at(const iterator& prior_,
                                   const interval_type& inter_val,
                                   const codomain_type& co_val) {
    // Never try to insert an identity element into an identity element absorber
    // here:
    assert((!on_absorbtion<type, Combiner,
                           Traits::absorbs_identities>::is_absorbable(co_val)));

    iterator inserted_ = this->_map.insert(
        prior_, value_type(inter_val, Combiner::identity_element()));

    if ((*inserted_).first == inter_val &&
        (*inserted_).second == Combiner::identity_element()) {
      Combiner()((*inserted_).second, co_val);
      return std::pair<iterator, bool>(inserted_, true);
    }
    return std::pair<iterator, bool>(inserted_, false);
  }

  std::pair<iterator, bool> insert_at(const iterator& prior_,
                                      const interval_type& inter_val,
                                      const codomain_type& co_val) {
    iterator inserted_ =
        this->_map.insert(prior_, value_type(inter_val, co_val));

    if (inserted_ == prior_)
      return std::pair<iterator, bool>(inserted_, false);
    if ((*inserted_).first == inter_val)
      return std::pair<iterator, bool>(inserted_, true);
    return std::pair<iterator, bool>(inserted_, false);
  }

  sub_type* that() { return static_cast<sub_type*>(this); }
  const sub_type* that() const { return static_cast<const sub_type*>(this); }

  ImplMapT _map;

private:
  //--------------------------------------------------------------------------
  template <typename Type, bool is_total_invertible> struct on_invertible;

  template <typename Type> struct on_invertible<Type, true> {
    using segment_type = typename Type::segment_type;
    using inverse_codomain_combine = typename Type::inverse_codomain_combine;

    static void subtract(Type& object, const segment_type& operand) {
      object.template _add<inverse_codomain_combine>(operand);
    }
  };

  template <typename Type> struct on_invertible<Type, false> {
    using segment_type = Type::segment_type;
    using inverse_codomain_combine = Type::inverse_codomain_combine;

    static void subtract(Type& object, const segment_type& operand) {
      object.template _subtract<inverse_codomain_combine>(operand);
    }
  };

  friend struct on_invertible<type, true>;
  friend struct on_invertible<type, false>;
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  template <typename Type, bool is_total> struct on_definedness;

  template <typename Type> struct on_definedness<Type, true> {
    static void add_intersection(Type& section, const Type& object,
                                 const segment_type& operand) {
      object.total_add_intersection(section, operand);
    }
  };

  template <typename Type> struct on_definedness<Type, false> {
    static void add_intersection(Type& section, const Type& object,
                                 const segment_type& operand) {
      object.partial_add_intersection(section, operand);
    }
  };

  friend struct on_definedness<type, true>;
  friend struct on_definedness<type, false>;
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  template <typename Type, bool has_set_semantics> struct on_codomain_model;

  template <typename Type> struct on_codomain_model<Type, true> {
    using interval_type = Type::interval_type;
    using codomain_type = Type::codomain_type;
    using segment_type = Type::segment_type;
    using codomain_combine = Type::codomain_combine;
    using inverse_codomain_intersect = Type::inverse_codomain_intersect;

    static void add(Type& intersection, interval_type& common_interval,
                    const codomain_type& flip_value,
                    const codomain_type& co_value) {
      codomain_type common_value = flip_value;
      inverse_codomain_intersect()(common_value, co_value);
      intersection.template _add<codomain_combine>(
          segment_type(common_interval, common_value));
    }
  };

  template <typename Type> struct on_codomain_model<Type, false> {
    using interval_type = Type::interval_type;
    using codomain_type = Type::codomain_type;
    using segment_type = Type::segment_type;
    using codomain_combine = Type::codomain_combine;

    static void add(Type& intersection, interval_type& common_interval,
                    const codomain_type&, const codomain_type&) {
      intersection.template _add<codomain_combine>(segment_type(
          common_interval, identity_element<codomain_type>::value()));
    }
  };

  friend struct on_codomain_model<type, true>;
  friend struct on_codomain_model<type, false>;
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  template <typename Type, bool is_total, bool absorbs_identities>
  struct on_total_absorbable;

  template <typename Type> struct on_total_absorbable<Type, true, true> {
    static void flip(Type& object, const Type::segment_type&) {
      icl::clear(object);
    }
  };

  template <typename Type> struct on_total_absorbable<Type, true, false> {
    using segment_type = Type::segment_type;
    using codomain_type = Type::codomain_type;

    static void flip(Type& object, const segment_type& operand) {
      object += operand;
      for (typename Type::iterator it_ = object.begin(); it_ != object.end();
           ++it_)
        (*it_).second = identity_element<codomain_type>::value();

      if constexpr (!is_interval_splitter<Type>::value)
        icl::join(object);
    }
  };

  template <typename Type, bool absorbs_identities>
  struct on_total_absorbable<Type, false, absorbs_identities> {
    using segment_type = Type::segment_type;
    using codomain_type = Type::codomain_type;
    using interval_type = Type::interval_type;
    using value_type = Type::value_type;
    using const_iterator = Type::const_iterator;
    using set_type = Type::set_type;
    using inverse_codomain_intersect = Type::inverse_codomain_intersect;

    static void flip(Type& object, const segment_type& interval_value_pair) {
      // That which is common shall be subtracted
      // That which is not shall be added
      // So interval_value_pair has to be 'complementary added' or flipped
      interval_type span = interval_value_pair.first;
      std::pair<const_iterator, const_iterator> exterior =
          object.equal_range(span);

      const_iterator first_ = exterior.first;
      const_iterator end_ = exterior.second;

      interval_type covered, left_over, common_interval;
      const codomain_type& x_value = interval_value_pair.second;
      const_iterator it_ = first_;

      set_type eraser;
      Type intersection;

      while (it_ != end_) {
        const codomain_type& co_value = (*it_).second;
        covered = (*it_++).first;
        //[a      ...  : span
        //     [b ...  : covered
        //[a  b)       : left_over
        left_over = right_subtract(span, covered);

        // That which is common ...
        common_interval = span & covered;
        if (!icl::is_empty(common_interval)) {
          // ... shall be subtracted
          icl::add(eraser, common_interval);

          on_codomain_model<Type, has_set_semantics<codomain_type>::value>::add(
              intersection, common_interval, x_value, co_value);
        }

        icl::add(
            object,
            value_type(left_over, x_value)); // That which is not shall be added
        // Because this is a collision free addition I don't have to distinguish
        // codomain_types.

        //...      d) : span
        //... c)      : covered
        //     [c  d) : span'
        span = left_subtract(span, covered);
      }

      // If span is not empty here, it is not in the set so it shall be added
      icl::add(object, value_type(span, x_value));

      // finally rewrite the common segments
      icl::erase(object, eraser);
      object += intersection;
    }
  };
  //--------------------------------------------------------------------------
};

//-----------------------------------------------------------------------------
// type traits
//-----------------------------------------------------------------------------
template <typename SubType, typename DomainT, typename CodomainT,
          typename Traits, template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_map<interval_base_map<SubType, DomainT, CodomainT, Traits, Compare,
                                Combine, Section, Interval, Alloc>> {
  using type = is_map;
  static constexpr bool value = true;
};

template <typename SubType, typename DomainT, typename CodomainT,
          typename Traits, template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct has_inverse<
    interval_base_map<SubType, DomainT, CodomainT, Traits, Compare, Combine,
                      Section, Interval, Alloc>> {
  using type = has_inverse;
  static constexpr bool value = has_inverse<CodomainT>::value;
};

template <typename SubType, typename DomainT, typename CodomainT,
          typename Traits, template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_interval_container<
    interval_base_map<SubType, DomainT, CodomainT, Traits, Compare, Combine,
                      Section, Interval, Alloc>> {
  using type = is_interval_container;
  static constexpr bool value = true;
};

template <typename SubType, typename DomainT, typename CodomainT,
          typename Traits, template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct absorbs_identities<
    interval_base_map<SubType, DomainT, CodomainT, Traits, Compare, Combine,
                      Section, Interval, Alloc>> {
  using type = absorbs_identities;
  static constexpr bool value = Traits::absorbs_identities;
};

template <typename SubType, typename DomainT, typename CodomainT,
          typename Traits, template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_total<interval_base_map<SubType, DomainT, CodomainT, Traits, Compare,
                                  Combine, Section, Interval, Alloc>> {
  using type = is_total;
  static constexpr bool value = Traits::is_total;
};

} // namespace icl
