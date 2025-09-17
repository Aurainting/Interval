#pragma once

#include "icl/interval_base_map.hpp"
#include "icl/interval_set.hpp"
#include "icl/type_traits/interval_type_default.hpp"
#include "icl/type_traits/is_map.hpp"

namespace icl {

template <typename DomainT, typename CodomainT, class Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
class split_interval_map;

/** \brief implements a map as a map of intervals - on insertion
    overlapping intervals are split and associated values are combined.*/
template <typename DomainT, typename CodomainT,
          typename Traits = icl::partial_absorber,
          template <typename> typename Compare = std::less,
          template <typename> typename Combine = icl::inplace_plus,
          template <typename> typename Section = icl::inter_section,
          typename Interval =
              typename icl::interval_type_default<DomainT, Compare>::type,
          template <typename> typename Alloc = std::allocator>
class interval_map
    : public interval_base_map<interval_map<DomainT, CodomainT, Traits, Compare,
                                            Combine, Section, Interval, Alloc>,
                               DomainT, CodomainT, Traits, Compare, Combine,
                               Section, Interval, Alloc> {
public:
  using traits = Traits;
  using type = interval_map;
  using split_type = split_interval_map<DomainT, CodomainT, Traits, Compare,
                                        Combine, Section, Interval, Alloc>;
  using overloadable_type = type;
  using joint_type = type;
  using base_type = interval_base_map<type, DomainT, CodomainT, Traits, Compare,
                                      Combine, Section, Interval, Alloc>;

  using interval_type = Interval;
  using iterator = typename base_type::iterator;
  using value_type = typename base_type::value_type;
  using element_type = typename base_type::element_type;
  using segment_type = typename base_type::segment_type;
  using domain_type = typename base_type::domain_type;
  using codomain_type = typename base_type::codomain_type;
  using domain_mapping_type = typename base_type::domain_mapping_type;
  using interval_mapping_type = typename base_type::interval_mapping_type;
  using ImplMapT = typename base_type::ImplMapT;

  using size_type = typename base_type::size_type;
  using codomain_combine = typename base_type::codomain_combine;

  using interval_set_type = interval_set<DomainT, Compare, Interval, Alloc>;
  using set_type = interval_set_type;
  using key_object_type = set_type;

  enum { fineness = 1 };

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /// Default constructor for the empty object
  interval_map() : base_type() {}

  /// Copy constructor
  interval_map(const interval_map& src) : base_type(src) {}

  /// Copy constructor for base_type
  template <typename SubType>
  explicit interval_map(
      const interval_base_map<SubType, DomainT, CodomainT, Traits, Compare,
                              Combine, Section, Interval, Alloc>& src) {
    this->assign(src);
  }

  explicit interval_map(const domain_mapping_type& base_pair) : base_type() {
    this->add(base_pair);
  }

  explicit interval_map(const value_type& value_pair) : base_type() {
    this->add(value_pair);
  }

  /// Assignment from a base interval_map.
  template <typename SubType>
  void
  assign(const interval_base_map<SubType, DomainT, CodomainT, Traits, Compare,
                                 Combine, Section, Interval, Alloc>& src) {
    using base_map_type =
        interval_base_map<SubType, DomainT, CodomainT, Traits, Compare, Combine,
                          Section, Interval, Alloc>;
    this->clear();
    iterator prior_ = this->_map.end();
    for (typename base_map_type::const_iterator it_ = src.begin();
         !(it_ == src.end()); ++it_)
      prior_ = this->add(prior_, *it_);
  }

  /// Assignment operator for base type
  template <typename SubType>
  interval_map& operator=(
      const interval_base_map<SubType, DomainT, CodomainT, Traits, Compare,
                              Combine, Section, Interval, Alloc>& src) {
    this->assign(src);
    return *this;
  }

  //==========================================================================
  //= Move semantics
  //==========================================================================

  /// Move constructor
  interval_map(interval_map&& src) noexcept : base_type(std::move(src)) {}

  /// Move assignment operator
  interval_map& operator=(interval_map src) {
    base_type::operator=(std::move(src));
    return *this;
  }

  //==========================================================================

private:
  // Private functions that shall be accessible by the baseclass:
  friend class interval_base_map<interval_map, DomainT, CodomainT, Traits,
                                 Compare, Combine, Section, Interval, Alloc>;

  iterator handle_inserted(iterator it_) {
    return segmental::join_neighbours(*this, it_);
  }

  void handle_inserted(iterator prior_, iterator it_) {
    if (prior_ != this->_map.end() && segmental::joinable(*this, prior_, it_))
      segmental::join_on_right(*this, prior_, it_);
  }

  template <typename Combiner> void handle_left_combined(iterator it_) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_).second))
      this->_map.erase(it_);
    else
      segmental::join_left(*this, it_);
  }

  template <typename Combiner> void handle_combined(iterator it_) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_).second))
      this->_map.erase(it_);
    else
      segmental::join_neighbours(*this, it_);
  }

  template <typename Combiner>
  void handle_preceeded_combined(iterator prior_, iterator& it_) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_)
                                                                     .second)) {
      this->_map.erase(it_);
      it_ = prior_;
    } else // After a new combination (e.g. combiner=max) joining neighbours may
           // be possible
      segmental::join_neighbours(*this, it_);
  }

  template <typename Combiner>
  void handle_succeeded_combined(iterator it_, iterator next_) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_)
                                                                     .second)) {
      this->_map.erase(it_);
      segmental::join_right(*this, next_);
    } else {
      segmental::join_left(*this, it_);
      segmental::join_neighbours(*this, next_);
    }
  }

  void handle_reinserted(iterator insertion_) {
    segmental::join_right(*this, insertion_);
  }

  template <typename Combiner>
  void gap_insert_at(iterator& it_, iterator prior_,
                     const interval_type& end_gap,
                     const codomain_type& co_val) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_)
                                                                     .second)) {
      this->_map.erase(it_);
      it_ = this->template gap_insert<Combiner>(prior_, end_gap, co_val);
      segmental::join_right(*this, it_);
    } else {
      segmental::join_left(*this, it_);
      iterator inserted_ =
          this->template gap_insert<Combiner>(it_, end_gap, co_val);
      it_ = segmental::join_neighbours(*this, inserted_);
    }
  }
};

//-----------------------------------------------------------------------------
// type traits
//-----------------------------------------------------------------------------
template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_map<icl::interval_map<DomainT, CodomainT, Traits, Compare, Combine,
                                Section, Interval, Alloc>> {
  using type = is_map;
  static constexpr bool value = true;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct has_inverse<icl::interval_map<DomainT, CodomainT, Traits, Compare,
                                     Combine, Section, Interval, Alloc>> {
  using type = has_inverse;
  static constexpr bool value = has_inverse<CodomainT>::value;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_interval_container<icl::interval_map<
    DomainT, CodomainT, Traits, Compare, Combine, Section, Interval, Alloc>> {
  using type = is_interval_container;
  static constexpr bool value = true;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct absorbs_identities<icl::interval_map<
    DomainT, CodomainT, Traits, Compare, Combine, Section, Interval, Alloc>> {
  using type = absorbs_identities;
  static constexpr bool value = Traits::absorbs_identities;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_total<icl::interval_map<DomainT, CodomainT, Traits, Compare, Combine,
                                  Section, Interval, Alloc>> {
  using type = is_total;
  static constexpr bool value = Traits::is_total;
};

//-----------------------------------------------------------------------------
// type representation
//-----------------------------------------------------------------------------
template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct type_to_string<icl::interval_map<DomainT, CodomainT, Traits, Compare,
                                        Combine, Section, Interval, Alloc>> {
  static std::string apply() {
    return "itv_map<" + type_to_string<DomainT>::apply() + "," +
           type_to_string<CodomainT>::apply() + "," +
           type_to_string<Traits>::apply() + ">";
  }
};

} // namespace icl
