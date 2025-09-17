#pragma once

#include "icl/concept/interval_set_value.hpp"
#include "icl/concept/map_value.hpp"
#include "icl/detail/relation_state.hpp"
#include "icl/type_traits/is_concept_equivalent.hpp"
#include "icl/type_traits/is_element_container.hpp"
#include "icl/type_traits/is_map.hpp"
#include "icl/type_traits/is_set.hpp"
#include <type_traits>

namespace icl {
namespace Set {

//------------------------------------------------------------------------------
template <typename LeftT, typename RightT> struct settic_codomain_compare {
  static int apply(LeftT::const_iterator& left_,
                   RightT::const_iterator& right_) {
    return inclusion_compare(co_value<LeftT>(left_), co_value<RightT>(right_));
  }
};

template <typename LeftT, typename RightT> struct atomic_codomain_compare {
  static int apply(LeftT::const_iterator& left_,
                   RightT::const_iterator& right_) {
    if (co_value<LeftT>(left_) == co_value<RightT>(right_))
      return inclusion::equal;
    return inclusion::unrelated;
  }
};

template <typename LeftT, typename RightT> struct empty_codomain_compare {
  static int apply(LeftT::const_iterator&, RightT::const_iterator&) {
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

  enum { nextboth, stop };

  enum {
    unrelated = inclusion::unrelated,
    subset = inclusion::subset,     // left is_subset_of   right
    superset = inclusion::superset, // left is_superset_of right
    equal = inclusion::equal        // equal = subset | superset
  };

  subset_comparer& operator=(const subset_comparer&) = delete;

  [[nodiscard]] int result() const { return _result; }

  int co_compare(LeftIterT& left, RightIterT& right) {
    if constexpr (is_concept_equivalent<is_element_map, LeftT, RightT>::value) {
      return map_codomain_compare<LeftT, RightT>::apply(left, right);
    } else {
      return empty_codomain_compare<LeftT, RightT>::apply(left, right);
    }
  }

  int restrict_result(int state) { return _result &= state; }

  int next_both(LeftIterT& left, RightIterT& right) {
    if (left == _left_end && right == _right_end)
      return stop;
    if (left == _left_end) {
      restrict_result(subset);
      return stop;
    }
    if (right == _right_end) {
      restrict_result(superset);
      return stop;
    }
    if (typename LeftT::key_compare()(
            key_value<LeftT>(left),
            key_value<RightT>(right))) { // left:  *left . . *joint_ left
                                         // could be superset
      // right:           *right ...  if joint_ exists
      restrict_result(superset);
      if (unrelated == _result)
        return stop;
      LeftIterT joint_ = _left.lower_bound(key_value<RightT>(right));
      if (joint_ == _left.end() ||
          typename LeftT::key_compare()(key_value<RightT>(right),
                                        key_value<LeftT>(joint_))) {
        _result = unrelated;
        return stop;
      }
      left = joint_;
    } else if (typename LeftT::key_compare()(
                   key_value<RightT>(right),
                   key_value<LeftT>(left))) { // left:             *left   left
                                              // could be subset
      // right:*right . . .*joint_ if *joint_ exists
      restrict_result(subset);
      if (unrelated == _result)
        return stop;
      RightIterT joint_ = _right.lower_bound(key_value<LeftT>(left));
      if (joint_ == _right.end() ||
          typename LeftT::key_compare()(key_value<LeftT>(left),
                                        key_value<RightT>(joint_))) {
        _result = unrelated;
        return stop;
      }
      right = joint_;
    }

    // left =key= right
    if (_compare_codomain)
      if (unrelated == restrict_result(co_compare(left, right)))
        return stop;

    ++left;
    ++right;
    return nextboth;
  }

private:
  const LeftT& _left;
  const RightT& _right;
  LeftIterT _left_end;
  RightIterT _right_end;
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
  while (state != Step::stop)
    state = step.next_both(left_, right_);

  return step.result();
}

template <typename LeftT, typename RightT>
int subset_compare(const LeftT& left, const RightT& right) {
  return subset_compare(left, right, left.begin(), left.end(), right.begin(),
                        right.end());
}

} // namespace Set
} // namespace icl
