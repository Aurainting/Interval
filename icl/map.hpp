#pragma once

#include "icl/concept/container.hpp"
#include "icl/concept/element_associator.hpp"
#include "icl/concept/element_map.hpp"
#include "icl/detail/on_absorbtion.hpp"
#include "icl/functors.hpp"
#include "icl/type_traits/absorbs_identities.hpp"
#include "icl/type_traits/has_inverse.hpp"
#include "icl/type_traits/has_set_semantics.hpp"
#include "icl/type_traits/is_map.hpp"
#include "icl/type_traits/is_total.hpp"
#include <map>
#include <set>
#include <string>
#include <type_traits>

namespace icl {

template <typename T> struct domain_param_traits {
  using type = std::conditional_t<std::is_fundamental<T>::value ||
                                      sizeof(T) <= sizeof(void*) * 2,
                                  T, const T&>;
};

struct partial_absorber {
  enum { absorbs_identities = true };
  enum { is_total = false };
};

template <> inline std::string type_to_string<partial_absorber>::apply() {
  return "@0";
}

struct partial_enricher {
  enum { absorbs_identities = false };
  enum { is_total = false };
};

template <> inline std::string type_to_string<partial_enricher>::apply() {
  return "e0";
}

struct total_absorber {
  enum { absorbs_identities = true };
  enum { is_total = true };
};

template <> inline std::string type_to_string<total_absorber>::apply() {
  return "^0";
}

struct total_enricher {
  enum { absorbs_identities = false };
  enum { is_total = true };
};

template <> inline std::string type_to_string<total_enricher>::apply() {
  return "e^0";
}

/** \brief Addable, subractable and intersectable maps */
template <typename DomainT, typename CodomainT, class Traits = partial_absorber,
          template <typename> typename Compare = std::less,
          template <typename> typename Combine = inplace_plus,
          template <typename> typename Section = inter_section,
          template <typename> typename Alloc = std::allocator>
  requires std::default_initializable<DomainT> &&
           std::totally_ordered<DomainT> &&
           std::default_initializable<CodomainT> &&
           std::equality_comparable<CodomainT>
class map : std::map<DomainT, CodomainT, Compare<DomainT>,
                     Alloc<std::pair<const DomainT, CodomainT>>> {
public:
  using allocator_type = Alloc<std::pair<const DomainT, CodomainT>>;

  using type = map;
  using base_type =
      std::map<DomainT, CodomainT, Compare<DomainT>, allocator_type>;

  using traits = Traits;

  using domain_type = DomainT;
  using domain_param = domain_param_traits<DomainT>::type;
  using key_type = DomainT;
  using codomain_type = CodomainT;
  using mapped_type = CodomainT;
  using data_type = CodomainT;
  using element_type = std::pair<const DomainT, CodomainT>;
  using value_type = std::pair<const DomainT, CodomainT>;
  using domain_compare = Compare<DomainT>;
  using codomain_combine = Combine<CodomainT>;
  using key_compare = domain_compare;
  using element_compare = Compare<element_type>;
  using inverse_codomain_combine = inverse<codomain_combine>::type;
  using codomain_intersect =
      std::conditional<has_set_semantics<codomain_type>::value,
                       Section<CodomainT>, codomain_combine>;
  using inverse_codomain_intersect = inverse<codomain_intersect>::type;
  using value_compare = base_type::value_compare;

  using set_type = std::set<DomainT, domain_compare, Alloc<DomainT>>;
  using key_object_type = set_type;

  static constexpr bool _total = Traits::is_total;
  static constexpr bool _absorbs = Traits::absorbs_identities;
  static constexpr bool total_invertible =
      std::conjunction_v<is_total<type>, has_inverse<codomain_type>>;

  using on_identity_absorbtion =
      on_absorbtion<type, codomain_combine, Traits::absorbs_identities>;

  using pointer = base_type::pointer;
  using const_pointer = base_type::const_pointer;
  using reference = base_type::reference;
  using const_reference = base_type::const_reference;
  using iterator = base_type::iterator;
  using const_iterator = base_type::const_iterator;
  using size_type = base_type::size_type;
  using difference_type = base_type::difference_type;
  using reverse_iterator = base_type::reverse_iterator;
  using const_reverse_iterator = base_type::const_reverse_iterator;

  static constexpr bool is_total_invertible =
      Traits::is_total && has_inverse<codomain_type>::value;

  static constexpr int fineness = 4;

  //==========================================================================
  //= Construct, copy, destruct
  //==========================================================================
  map() = default;

  explicit map(const key_compare& comp) : base_type(comp) {}

  template <typename InputIterator>
  map(InputIterator first, InputIterator past) : base_type(first, past) {}

  template <typename InputIterator>
  map(InputIterator first, InputIterator past, const key_compare& comp)
      : base_type(first, past, comp) {}

  map(const map& src) : base_type(src) {}

  explicit map(const element_type& key_value_pair) : base_type::map() {
    insert(key_value_pair);
  }

  //==========================================================================
  //= Move semantics
  //==========================================================================

  map(map&& src) noexcept : base_type(std::move(src)) {}

  map& operator=(map src) {
    base_type::operator=(std::move(src));
    return *this;
  }
  //==========================================================================

  void swap(map& src) noexcept { base_type::swap(src); }

  //==========================================================================
  using base_type::clear;
  using base_type::empty;

  using base_type::begin;
  using base_type::end;
  using base_type::rbegin;
  using base_type::rend;

  using base_type::max_size;
  using base_type::size;

  using base_type::key_comp;
  using base_type::value_comp;

  using base_type::count;
  using base_type::erase;
  using base_type::find;

  using base_type::equal_range;
  using base_type::lower_bound;
  using base_type::upper_bound;

  using base_type::operator[];

  //==========================================================================
  //= Containedness
  //==========================================================================
  template <typename SubObject> bool contains(const SubObject& sub) const {
    return icl::contains(*this, sub);
  }

  bool within(const map& super) const { return icl::contains(super, *this); }

  //==========================================================================
  //= Size
  //==========================================================================
  /** \c iterative_size() yields the number of elements that is visited
      throu complete iteration. For interval sets \c iterative_size() is
      different from \c size(). */
  [[nodiscard]] std::size_t iterative_size() const { return base_type::size(); }

  //==========================================================================
  //= Selection
  //==========================================================================

  /** Total select function. */
  codomain_type operator()(const domain_type& key) const {
    const_iterator it = find(key);
    return it == end() ? identity_element<codomain_type>::value() : it->second;
  }

  //==========================================================================
  //= Addition
  //==========================================================================
  /** \c add inserts \c value_pair into the map if it's key does
      not exist in the map.
      If \c value_pairs's key value exists in the map, it's data
      value is added to the data value already found in the map. */
  map& add(const value_type& value_pair) {
    return _add<codomain_combine>(value_pair);
  }

  /** \c add \c value_pair into the map using \c prior as a hint to
      insert \c value_pair after the position \c prior is pointing to. */
  iterator add(iterator prior, const value_type& value_pair) {
    return _add<codomain_combine>(prior, value_pair);
  }

  //==========================================================================
  //= Subtraction
  //==========================================================================
  /** If the \c value_pair's key value is in the map, it's data value is
      subtracted from the data value stored in the map. */
  map& subtract(const element_type& value_pair) {
    on_invertible<type, is_total_invertible>::subtract(*this, value_pair);
    return *this;
  }

  map& subtract(const domain_type& key) {
    icl::erase(*this, key);
    return *this;
  }

  //==========================================================================
  //= Insertion, erasure
  //==========================================================================
  std::pair<iterator, bool> insert(const value_type& value_pair) {
    if (on_identity_absorbtion::is_absorbable(value_pair.second))
      return std::pair<iterator, bool>(end(), true);
    return base_type::insert(value_pair);
  }

  iterator insert(iterator prior, const value_type& value_pair) {
    if (on_identity_absorbtion::is_absorbable(value_pair.second))
      return end();
    return base_type::insert(prior, value_pair);
  }

  template <typename Iterator> iterator insert(Iterator first, Iterator last) {
    iterator prior = end(), it = first;
    while (it != last)
      prior = this->insert(prior, *it++);
    return prior;
  }

  /** With <tt>key_value_pair = (k,v)</tt> set value \c v for key \c k */
  map& set(const element_type& key_value_pair) {
    return icl::set_at(*this, key_value_pair);
  }

  /** erase \c key_value_pair from the map.
      Erase only if, the exact value content \c val is stored for the given key.
   */
  size_type erase(const element_type& key_value_pair) {
    return icl::erase(*this, key_value_pair);
  }

  //==========================================================================
  //= Intersection
  //==========================================================================
  /** The intersection of \c key_value_pair and \c *this map is added to \c
   * section. */
  void add_intersection(map& section,
                        const element_type& key_value_pair) const {
    on_definedness<type, Traits::is_total>::add_intersection(section, *this,
                                                             key_value_pair);
  }

  //==========================================================================
  //= Symmetric difference
  //==========================================================================

  map& flip(const element_type& operand) {
    on_total_absorbable<type, _total, _absorbs>::flip(*this, operand);
    return *this;
  }

private:
  template <typename Combiner> map& _add(const element_type& value_pair);

  template <typename Combiner>
  iterator _add(iterator prior, const element_type& value_pair);

  template <typename Combiner> map& _subtract(const element_type& value_pair);

  template <typename FragmentT>
  void total_add_intersection(type& section, const FragmentT& fragment) const {
    section += *this;
    section.add(fragment);
  }

  void partial_add_intersection(type& section,
                                const element_type& operand) const {
    const_iterator it_ = find(operand.first);
    if (it_ != end()) {
      section._add<codomain_combine>(*it_);
      section._add<codomain_intersect>(operand);
    }
  }

  //--------------------------------------------------------------------------
  template <typename Type, bool is_total_invertible> struct on_invertible;

  template <typename Type> struct on_invertible<Type, true> {
    using element_type = Type::element_type;
    using inverse_codomain_combine = Type::inverse_codomain_combine;

    static void subtract(Type& object, const element_type& operand) {
      object.template _add<inverse_codomain_combine>(operand);
    }
  };

  template <typename Type> struct on_invertible<Type, false> {
    using element_type = Type::element_type;
    using inverse_codomain_combine = Type::inverse_codomain_combine;

    static void subtract(Type& object, const element_type& operand) {
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
                                 const element_type& operand) {
      object.total_add_intersection(section, operand);
    }
  };

  template <typename Type> struct on_definedness<Type, false> {
    static void add_intersection(Type& section, const Type& object,
                                 const element_type& operand) {
      object.partial_add_intersection(section, operand);
    }
  };

  friend struct on_definedness<type, true>;
  friend struct on_definedness<type, false>;
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  template <typename Type, bool has_set_semantics, bool absorbs_identities>
  struct on_codomain_model;

  template <typename Type>
  struct on_codomain_model<Type, false,
                           false> { // !codomain_is_set, !absorbs_identities
    static void subtract(Type&, Type::iterator it_,
                         const Type::codomain_type&) {
      (*it_).second = identity_element<typename Type::codomain_type>::value();
    }
  };

  template <typename Type>
  struct on_codomain_model<Type, false,
                           true> { // !codomain_is_set, absorbs_identities
    static void subtract(Type& object, Type::iterator it_,
                         const Type::codomain_type&) {
      object.erase(it_);
    }
  };

  template <typename Type>
  struct on_codomain_model<Type, true,
                           false> { // !codomain_is_set, !absorbs_identities
    using inverse_codomain_intersect = Type::inverse_codomain_intersect;
    static void subtract(Type&, Type::iterator it_,
                         const Type::codomain_type& co_value) {
      inverse_codomain_intersect()((*it_).second, co_value);
    }
  };

  template <typename Type>
  struct on_codomain_model<Type, true,
                           true> { // !codomain_is_set, absorbs_identities
    using inverse_codomain_intersect = Type::inverse_codomain_intersect;
    static void subtract(Type& object, Type::iterator it_,
                         const Type::codomain_type& co_value) {
      inverse_codomain_intersect()((*it_).second, co_value);
      if ((*it_).second == identity_element<codomain_type>::value())
        object.erase(it_);
    }
  };
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  template <typename Type, bool is_total, bool absorbs_identities>
  struct on_total_absorbable;

  template <typename Type> struct on_total_absorbable<Type, true, true> {
    using element_type = Type::element_type;
    static void flip(Type& object, const Type::element_type&) {
      icl::clear(object);
    }
  };

  template <typename Type> struct on_total_absorbable<Type, true, false> {
    using element_type = Type::element_type;
    using codomain_type = Type::codomain_type;

    static void flip(Type& object, const element_type& operand) {
      object.add(operand);
      for (typename Type::iterator it_ = object.begin(); it_ != object.end();
           ++it_)
        (*it_).second = identity_element<codomain_type>::value();
    }
  };

  template <typename Type>
  struct on_total_absorbable<Type, false,
                             true> { // !is_total, absorbs_identities
    using element_type = Type::element_type;
    using codomain_type = Type::codomain_type;
    using iterator = Type::iterator;
    using inverse_codomain_intersect = Type::inverse_codomain_intersect;

    static void flip(Type& object, const element_type& operand) {
      std::pair<iterator, bool> insertion = object.insert(operand);
      if (!insertion.second)
        on_codomain_model<Type, has_set_semantics<codomain_type>::value,
                          true>::subtract(object, insertion.first,
                                          operand.second);
    }
  };

  template <typename Type>
  struct on_total_absorbable<Type, false,
                             false> { // !is_total  !absorbs_identities
    using element_type = Type::element_type;
    using codomain_type = Type::codomain_type;
    using iterator = Type::iterator;
    using inverse_codomain_intersect = Type::inverse_codomain_intersect;

    static void flip(Type& object, const element_type& operand) {
      std::pair<iterator, bool> insertion = object.insert(operand);
      if (!insertion.second)
        on_codomain_model<Type, has_set_semantics<codomain_type>::value,
                          false>::subtract(object, insertion.first,
                                           operand.second);
    }
  };

  friend struct on_total_absorbable<type, true, true>;
  friend struct on_total_absorbable<type, false, true>;
  friend struct on_total_absorbable<type, true, false>;
  friend struct on_total_absorbable<type, false, false>;
  //--------------------------------------------------------------------------
};

//==============================================================================
//= Addition<ElementMap>
//==============================================================================
template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
  requires std::default_initializable<DomainT> &&
           std::totally_ordered<DomainT> &&
           std::default_initializable<CodomainT> &&
           std::equality_comparable<CodomainT>
template <typename Combiner>
map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>&
map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>::_add(
    const element_type& addend) {
  using on_absorbtion_ =
      on_absorbtion<type, Combiner, absorbs_identities<type>::value>::type;

  const codomain_type& co_val = addend.second;
  if (on_absorbtion_::is_absorbable(co_val))
    return *this;

  std::pair<iterator, bool> insertion =
      base_type::insert(value_type(addend.first, version<Combiner>()(co_val)));

  if (!insertion.second) {
    iterator it = insertion.first;
    Combiner()((*it).second, co_val);

    if (on_absorbtion_::is_absorbable((*it).second))
      erase(it);
  }
  return *this;
}

template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
  requires std::default_initializable<DomainT> &&
           std::totally_ordered<DomainT> &&
           std::default_initializable<CodomainT> &&
           std::equality_comparable<CodomainT>
template <typename Combiner>
map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>::iterator
map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>::_add(
    iterator prior_, const value_type& addend) {
  using on_absorbtion_ =
      on_absorbtion<type, Combiner, absorbs_identities<type>::value>::type;

  const codomain_type& co_val = addend.second;
  if (on_absorbtion_::is_absorbable(co_val))
    return end();

  iterator inserted_ = base_type::insert(
      prior_, value_type(addend.first, Combiner::identity_element()));
  Combiner()((*inserted_).second, addend.second);

  if (on_absorbtion_::is_absorbable((*inserted_).second)) {
    erase(inserted_);
    return end();
  }
  return inserted_;
}

//==============================================================================
//= Subtraction<ElementMap>
//==============================================================================
template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
  requires std::default_initializable<DomainT> &&
           std::totally_ordered<DomainT> &&
           std::default_initializable<CodomainT> &&
           std::equality_comparable<CodomainT>
template <typename Combiner>
map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>&
map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>::_subtract(
    const value_type& minuend) {
  using on_absorbtion_ =
      on_absorbtion<type, Combiner, absorbs_identities<type>::value>::type;

  iterator it_ = find(minuend.first);
  if (it_ != end()) {
    Combiner()((*it_).second, minuend.second);
    if (on_absorbtion_::is_absorbable((*it_).second))
      erase(it_);
  }
  return *this;
}

//-----------------------------------------------------------------------------
// type traits
//-----------------------------------------------------------------------------
template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
struct is_map<
    map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>> {
  using type = is_map;
  static constexpr bool value = true;
};

template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
struct has_inverse<
    map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>> {
  using type = has_inverse;
  static constexpr bool value = has_inverse<CodomainT>::value;
};

template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
struct absorbs_identities<
    map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>> {
  using type = absorbs_identities;
  static constexpr int value = Traits::absorbs_identities;
};

template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
struct is_total<
    map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>> {
  using type = is_total;
  static constexpr int value = Traits::is_total;
};

template <
    typename DomainT, typename CodomainT, typename Traits,
    template <typename> typename Compare, template <typename> typename Combine,
    template <typename> typename Section, template <typename> typename Alloc>
struct type_to_string<
    map<DomainT, CodomainT, Traits, Compare, Combine, Section, Alloc>> {
  static std::string apply() {
    return "map<" + type_to_string<DomainT>::apply() + "," +
           type_to_string<CodomainT>::apply() + "," +
           type_to_string<Traits>::apply() + ">";
  }
};

} // namespace icl
