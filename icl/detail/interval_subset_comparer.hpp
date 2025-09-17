#pragma once

#include "icl/concept/interval_set_value.hpp"
#include "icl/detail/relation_state.hpp"
#include "icl/type_traits/is_concept_equivalent.hpp"
#include "icl/type_traits/is_interval_container.hpp"
#include "icl/type_traits/is_map.hpp"
#include "icl/type_traits/is_set.hpp"
#include <type_traits>

namespace icl {
namespace Interval_Set {

//------------------------------------------------------------------------------
template <typename LeftT, typename RightT> struct settic_codomain_compare {
  static int apply(LeftT::const_iterator& left_,
                   RightT::const_iterator& right_) {
    return inclusion_compare(icl::co_value<LeftT>(left_),
                             icl::co_value<RightT>(right_));
  }
};

template <typename LeftT, typename RightT> struct atomic_codomain_compare {
  static int apply(LeftT::const_iterator& left_,
                   RightT::const_iterator& right_) {
    if (icl::co_value<LeftT>(left_) == icl::co_value<RightT>(right_))
      return inclusion::equal;
    return inclusion::unrelated;
  }
};

template <typename LeftT, typename RightT> struct empty_codomain_compare {
  static int apply(LeftT::const_iterator&, RightT::const_iterator) {
    return inclusion::equal;
  }
};

template <typename LeftT, typename RightT> struct map_codomain_compare {
  static int apply(LeftT::const_iterator& left_,
                   RightT::const_iterator& right_) {
    using LeftCodomainT = LeftT::codomain_type;
    using RightCodomainT = RightT::codomain_type;
    if constexpr (is_concept_equivalent<is_set, LeftCodomainT,
                                        RightCodomainT>::value) {
      return settic_codomain_compare<LeftT, RightT>::apply(left_, right_);
    } else {
      return atomic_codomain_compare<LeftT, RightT>::apply(left_, right_);
    }
  }
};

//------------------------------------------------------------------------------
template <typename LeftT, typename RightT> class subset_comparer {
public:
  using LeftIterT = LeftT::const_iterator;
  using RightIterT = RightT::const_iterator;

  static constexpr bool _compare_codomain =
      std::conjunction_v<is_map<LeftT>, is_map<RightT>>;

  subset_comparer(const LeftT& left, const RightT& right,
                  const LeftIterT& left_end, const RightIterT& right_end)
      : _left(left), _right(right), _left_end(left_end), _right_end(right_end),
        _result(equal) {}

  enum { nextboth, nextleft, nextright, stop };

  enum {
    unrelated = inclusion::unrelated,
    subset = inclusion::subset,     // left is_subset_of   right
    superset = inclusion::superset, // left is_superset_of right
    equal = inclusion::equal        // equal = subset | superset
  };

  subset_comparer& operator=(const subset_comparer&) = delete;

  [[nodiscard]] int result() const { return _result; }

  int co_compare(LeftIterT& left, RightIterT& right) {
    if constexpr (is_concept_equivalent<is_interval_map, LeftT,
                                        RightT>::value) {
      return map_codomain_compare<LeftT, RightT>::apply(left, right);
    } else {
      return empty_codomain_compare<LeftT, RightT>::apply(left, right);
    }
  }

  int restrict_result(int state) { return _result &= state; }

  int proceed(LeftIterT& left, RightIterT& right) {
    if (upper_less(key_value<LeftT>(left),
                   key_value<RightT>(right))) { // left  ..)
      // right .....)
      _prior_left = left;
      ++left;
      return nextleft;
    }
    if (upper_less(key_value<RightT>(right),
                   key_value<LeftT>(left))) { // left  .....)
      // right ..)
      _prior_right = right;
      ++right;
      return nextright;
    } // key_value<LeftT>(left).upper_equal(key_value<RightT>(right))
    // left  ..)
    // right ..)
    ++left;
    ++right;
    return nextboth;
  }

  int next_both(LeftIterT& left, RightIterT& right) {
    if (left == _left_end && right == _right_end)
      return stop;
    if (left == _left_end) { // left: ....end    left could be subset
      // right:....[..
      restrict_result(subset);
      return stop;
    }
    if (right == _right_end) { // left: ....[..    left could be superset
      // right:....end
      restrict_result(superset);
      return stop;
    }
    if (exclusive_less(key_value<LeftT>(left),
                       key_value<RightT>(right))) { // left: [..) . . .[---)
                                                    // left could be superset
      // right:           [..)....  if [---) exists
      restrict_result(superset);
      if (unrelated == _result)
        return stop;
      LeftIterT joint_ = _left.lower_bound(key_value<RightT>(right));
      if (joint_ == _left.end()) {
        _result = unrelated;
        return stop;
      }
      left = joint_;
      return nextboth;
    }
    if (exclusive_less(
            key_value<RightT>(right),
            key_value<LeftT>(
                left))) { // left:             [..  left could be subset
      // right:....) . . .[---) if [---) exists
      restrict_result(subset);
      if (unrelated == _result)
        return stop;
      RightIterT joint_ = _right.lower_bound(key_value<LeftT>(left));
      if (joint_ == _right.end()) {
        _result = unrelated;
        return stop;
      }
      right = joint_;
      return nextboth;
    }

    // left and right have intervals with nonempty intersection:
    if (_compare_codomain)
      if (unrelated == restrict_result(co_compare(left, right)))
        return stop;

    // examine left borders only. Right borders are checked in proceed
    if (lower_less(key_value<LeftT>(left),
                   key_value<RightT>(
                       right))) { // left: ....[...     left could be superset
      // right:....   [..
      if (unrelated == restrict_result(superset))
        return stop;
    } else if (lower_less(key_value<RightT>(right),
                          key_value<LeftT>(
                              left))) { // left: ....   [..   left can be subset
      // right:....[...
      if (unrelated == restrict_result(subset))
        return stop;
    }
    // else key_value<LeftT>(right).lower_equal(key_value<RightT>(left))
    //  left: ....[..   both can be equal
    //  right:....[..
    //  nothing to do: proceed

    return proceed(left, right);
  }

  int next_left(LeftIterT& left, RightIterT& right) {
    if (left == _left_end) { // left: ..)end    left could be subset
      // right:......)
      restrict_result(subset);
      return stop;
    }
    if (!touches(key_value<LeftT>(_prior_left),
                 key_value<LeftT>(left))) { // left: ..)   [..
      // right:.........)
      if (lower_less(
              key_value<RightT>(right),
              key_value<LeftT>(left))) { //   ..)   [..   left could be subset
        //   ..........)
        if (unrelated == restrict_result(subset))
          return stop;
      }
      // else   ..)   [...
      //           [..
      if (_compare_codomain &&
          intersects(key_value<LeftT>(left), key_value<RightT>(right)))
        if (unrelated == restrict_result(co_compare(left, right)))
          return stop;
    } else { // left: ..)[..  left could be subset
      // right:.......)
      if (_compare_codomain &&
          intersects(key_value<LeftT>(left), key_value<RightT>(right)))
        if (unrelated == restrict_result(co_compare(left, right)))
          return stop;
    }

    return proceed(left, right);
  }

  int next_right(LeftIterT& left, RightIterT& right) {
    if (right == _right_end) { // left: ......)    left could be superset
      // right:..)end
      restrict_result(superset);
      return stop;
    }
    if (!touches(key_value<RightT>(_prior_right),
                 key_value<RightT>(right))) { // left: .........)
      // right:..)   [..
      if (lower_less(key_value<LeftT>(left),
                     key_value<RightT>(
                         right))) { //       [....)  left could be superset
        //   ..)   [..
        if (unrelated == restrict_result(superset))
          return stop;
      }
      // else       [....)
      //    ..)   [..
      if (_compare_codomain &&
          intersects(key_value<LeftT>(left), key_value<RightT>(right)))
        if (unrelated == restrict_result(co_compare(left, right)))
          return stop;
    } else {
      if (_compare_codomain &&
          intersects(key_value<LeftT>(left), key_value<RightT>(right)))
        if (unrelated == restrict_result(co_compare(left, right)))
          return stop;
    }

    return proceed(left, right);
  }

private:
  const LeftT& _left;
  const RightT& _right;
  LeftIterT _left_end;
  RightIterT _right_end;
  LeftIterT _prior_left;
  RightIterT _prior_right;
  int _result;
};

//------------------------------------------------------------------------------
// Subset/superset comparison on ranges of two interval container
//------------------------------------------------------------------------------
template <typename LeftT, typename RightT>
int subset_compare(const LeftT& left,   // sub
                   const RightT& right, // super
                   typename LeftT::const_iterator left_begin,
                   typename LeftT::const_iterator left_end,
                   typename RightT::const_iterator right_begin,
                   typename RightT::const_iterator right_end) {
  using Step = subset_comparer<LeftT, RightT>;
  Step step(left, right, left_end, right_end);

  typename LeftT::const_iterator left_ = left_begin;
  typename RightT::const_iterator right_ = right_begin;

  int state = Step::nextboth;
  while (state != Step::stop) {
    switch (state) {
    case Step::nextboth:
      state = step.next_both(left_, right_);
      break;
    case Step::nextleft:
      state = step.next_left(left_, right_);
      break;
    case Step::nextright:
      state = step.next_right(left_, right_);
      break;
    default:;
    }
  }
  return step.result();
}

} // namespace Interval_Set
} // namespace icl
