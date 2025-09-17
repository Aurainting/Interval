#pragma once

#include "icl/interval_base_map.hpp"
#include "icl/interval_map.hpp"
#include "icl/interval_set.hpp"
#include "icl/type_traits/interval_type_default.hpp"

namespace icl {

/** \brief implements a map as a map of intervals - on insertion
    overlapping intervals are split and associated values are combined. */
template <typename DomainT, typename CodomainT, class Traits = partial_absorber,
          template <typename> typename Compare = std::less,
          template <typename> typename Combine = inplace_plus,
          template <typename> typename Section = inter_section,
          typename Interval = interval_type_default<DomainT, Compare>::type,
          template <typename> typename Alloc = std::allocator>
class split_interval_map
    : public interval_base_map<
          split_interval_map<DomainT, CodomainT, Traits, Compare, Combine,
                             Section, Interval, Alloc>,
          DomainT, CodomainT, Traits, Compare, Combine, Section, Interval,
          Alloc> {
public:
  using traits = Traits;
  using type = split_interval_map;
  using joint_type = interval_map<DomainT, CodomainT, Traits, Compare, Combine,
                                  Section, Interval, Alloc>;
  using overloadable_type = type;

  using base_type = interval_base_map<type, DomainT, CodomainT, Traits, Compare,
                                      Combine, Section, Interval, Alloc>;

  using domain_type = DomainT;
  using codomain_type = CodomainT;
  using interval_type = Interval;
  using iterator = typename base_type::iterator;
  using value_type = typename base_type::value_type;
  using element_type = typename base_type::element_type;
  using segment_type = typename base_type::segment_type;
  using domain_mapping_type = typename base_type::domain_mapping_type;
  using interval_mapping_type = typename base_type::interval_mapping_type;
  using ImplMapT = typename base_type::ImplMapT;

  using codomain_combine = typename base_type::codomain_combine;

  using interval_set_type = interval_set<DomainT, Compare, Interval, Alloc>;
  using set_type = interval_set_type;
  using key_object_type = set_type;

  enum { fineness = 3 };

public:
  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  /// Default constructor for the empty object
  split_interval_map() : base_type() {}

  /// Copy constructor
  split_interval_map(const split_interval_map& src) : base_type(src) {}

  explicit split_interval_map(const domain_mapping_type& base_pair)
      : base_type() {
    this->add(base_pair);
  }

  explicit split_interval_map(const value_type& value_pair) : base_type() {
    this->add(value_pair);
  }

  /// Assignment from a base interval_map.
  template <typename SubType>
  void
  assign(const interval_base_map<SubType, DomainT, CodomainT, Traits, Compare,
                                 Combine, Section, Interval, Alloc>& src) {
    this->clear();
    this->_map.insert(src.begin(), src.end());
  }

  /// Assignment operator for base type
  template <typename SubType>
  split_interval_map& operator=(
      const interval_base_map<SubType, DomainT, CodomainT, Traits, Compare,
                              Combine, Section, Interval, Alloc>& src) {
    this->assign(src);
    return *this;
  }

  //==========================================================================
  //= Move semantics
  //==========================================================================

  /// Move constructor
  split_interval_map(split_interval_map&& src) : base_type(std::move(src)) {}

  /// Move assignment operator
  split_interval_map& operator=(split_interval_map src) {
    base_type::operator=(std::move(src));
    return *this;
  }

  //==========================================================================

private:
  // Private functions that shall be accessible by the baseclass:
  friend class interval_base_map<
      split_interval_map<DomainT, CodomainT, Traits, Compare, Combine, Section,
                         Interval, Alloc>,
      DomainT, CodomainT, Traits, Compare, Combine, Section, Interval, Alloc>;

  iterator handle_inserted(iterator it_) const { return it_; }
  void handle_inserted(iterator, iterator) const {}

  template <typename Combiner> void handle_left_combined(iterator it_) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_).second))
      this->_map.erase(it_);
  }

  template <typename Combiner> void handle_combined(iterator it_) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_).second))
      this->_map.erase(it_);
  }

  template <typename Combiner>
  void handle_preceeded_combined(iterator prior_, iterator& it_) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_)
                                                                     .second)) {
      this->_map.erase(it_);
      it_ = prior_;
    }
  }

  template <typename Combiner>
  void handle_succeeded_combined(iterator it_, iterator) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_).second))
      this->_map.erase(it_);
  }

  void handle_reinserted(iterator) {}

  template <typename Combiner>
  void gap_insert_at(iterator& it_, iterator prior_,
                     const interval_type& end_gap,
                     const codomain_type& co_val) {
    if (on_absorbtion<type, Combiner,
                      Traits::absorbs_identities>::is_absorbable((*it_)
                                                                     .second)) {
      this->_map.erase(it_);
      it_ = this->template gap_insert<Combiner>(prior_, end_gap, co_val);
    } else
      it_ = this->template gap_insert<Combiner>(it_, end_gap, co_val);
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
struct is_map<split_interval_map<DomainT, CodomainT, Traits, Compare, Combine,
                                 Section, Interval, Alloc>> {
  using type = is_map;
  static constexpr bool value = true;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct has_inverse<split_interval_map<DomainT, CodomainT, Traits, Compare,
                                      Combine, Section, Interval, Alloc>> {
  using type = has_inverse;
  static constexpr bool value = has_inverse<CodomainT>::value;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_interval_container<split_interval_map<
    DomainT, CodomainT, Traits, Compare, Combine, Section, Interval, Alloc>> {
  using type = is_interval_container;
  static constexpr bool value = true;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_interval_splitter<split_interval_map<
    DomainT, CodomainT, Traits, Compare, Combine, Section, Interval, Alloc>> {
  using type = is_interval_splitter;
  static constexpr bool value = true;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct absorbs_identities<split_interval_map<
    DomainT, CodomainT, Traits, Compare, Combine, Section, Interval, Alloc>> {
  using type = absorbs_identities;
  static constexpr bool value = Traits::absorbs_identities;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct is_total<split_interval_map<DomainT, CodomainT, Traits, Compare, Combine,
                                   Section, Interval, Alloc>> {
  using type = is_total;
  static constexpr bool value = Traits::is_total;
};

template <typename DomainT, typename CodomainT, typename Traits,
          template <typename> typename Compare,
          template <typename> typename Combine,
          template <typename> typename Section, typename Interval,
          template <typename> typename Alloc>
struct type_to_string<split_interval_map<DomainT, CodomainT, Traits, Compare,
                                         Combine, Section, Interval, Alloc>> {
  static std::string apply() {
    return "sp_itv_map<" + type_to_string<DomainT>::apply() + "," +
           type_to_string<CodomainT>::apply() + "," +
           type_to_string<Traits>::apply() + ">";
  }
};

} // namespace icl
