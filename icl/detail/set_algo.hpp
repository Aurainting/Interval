#pragma once

#include "icl/concept/container.hpp"
#include "icl/concept/map_value.hpp"
#include "icl/concept/set_value.hpp"

namespace icl {
namespace Set {

template <typename ObjectT, typename ConstObjectT, typename IteratorT>
bool common_range(IteratorT& lwb, IteratorT& upb, ObjectT& x1,
                  const ConstObjectT& x2) {
  // lwb and upb are iterators of x1 marking the lower and upper bound of
  // the common range of x1 and x2.
  using ConstObject_iterator = ConstObjectT::const_iterator;
  // ObjectT may be const or non const.
  using PureObjectT = std::remove_const<ObjectT>::type;

  lwb = x1.end();
  upb = x1.end();

  if (icl::is_empty(x1) || icl::is_empty(x2))
    return false;

  IteratorT x1_fst_ = x1.begin();
  IteratorT x1_lst_ = x1.end();
  --x1_lst_;

  ConstObject_iterator x2_fst_ = x2.begin();
  ConstObject_iterator x2_lst_ = x2.end();
  --x2_lst_;

  typename ObjectT::key_compare key_less;
  if (key_less(icl::key_value<PureObjectT>(x1_lst_),
               icl::key_value<ConstObjectT>(x2_fst_))) // {x1}   {x2}
    return false;
  if (key_less(icl::key_value<ConstObjectT>(x2_lst_),
               icl::key_value<PureObjectT>(x1_fst_))) // {x2}   {x1}
    return false;

  // We do have a common range
  lwb = x1.lower_bound(icl::key_value<ConstObjectT>(x2_fst_));
  upb = x1.upper_bound(icl::key_value<ConstObjectT>(x2_lst_));

  return true;
}

/** Function template <tt>contained_in</tt> implements the subset relation.
<tt>contained_in(sub, super)</tt> is true if <tt>sub</tt> is contained in
<tt>super</tt> */
template <typename SetType>
bool within(const SetType& sub, const SetType& super) {
  if (&super == &sub)
    return true;
  if (icl::is_empty(sub))
    return true;
  if (icl::is_empty(super))
    return false;

  typename SetType::const_iterator common_lwb_, common_upb_;
  if (!common_range(common_lwb_, common_upb_, sub, super))
    return false;

  typename SetType::const_iterator sub_ = common_lwb_, super_;
  while (sub_ != common_upb_) {
    super_ = super.find(*sub_++);
    if (super_ == super.end())
      return false;
  }
  return true;
}

template <typename SetType>
bool intersects(const SetType& left, const SetType& right) {
  typename SetType::const_iterator common_lwb_right_, common_upb_right_;
  if (!common_range(common_lwb_right_, common_upb_right_, right, left))
    return false;

  typename SetType::const_iterator right_ = common_lwb_right_, found_;
  while (right_ != common_upb_right_) {
    found_ = left.find(*right_++);
    if (found_ != left.end())
      return true; // found a common element
  }
  // found no common element
  return false;
}

/** Function template <tt>lexicographical_equal</tt> implements
    lexicographical equality. */
template <typename SetType>
bool lexicographical_equal(const SetType& left, const SetType& right) {
  if (&left == &right)
    return true;
  return left.iterative_size() == right.iterative_size() &&
         std::equal(left.begin(), left.end(), right.begin());
}

} // namespace Set
} // namespace icl
