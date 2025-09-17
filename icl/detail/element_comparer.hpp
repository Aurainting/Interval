#pragma once

#include "icl/detail/relation_state.hpp"
#include "icl/type_traits/is_map.hpp"
#include <type_traits>

namespace icl {
namespace Interval_Set {

template <typename LeftT, typename RightT> class element_comparer {
public:
  using LeftIterT = LeftT::const_iterator;
  using RightIterT = RightT::const_iterator;

  static constexpr bool _compare_codomain =
      std::conjunction_v<is_map<LeftT>, is_map<RightT>>;

  element_comparer(const LeftT& left, const RightT& right,
                   const LeftIterT& left_end, const RightIterT& right_end)
      : _left(left), _right(right), _left_end(left_end), _right_end(right_end),
        _result(equal) {}

  enum { nextboth, nextleft, nextright, stop };

  enum {
    less = comparison::less,
    equal = comparison::equal,
    greater = comparison::greater
  };

  [[nodiscard]] int result() const { return _result; }

  bool covalues_are_equal(LeftIterT& left, RightIterT& right) {
    if (co_value<LeftT>(left) < co_value<RightT>(right))
      _result = less;
    if (co_value<RightT>(right) < co_value<LeftT>(left))
      _result = greater;
    return _result == equal;
  }

  int proceed(LeftIterT& left, RightIterT& right) {
    if (upper_less(key_value<LeftT>(left), key_value<RightT>(right))) {
      _prior_left = left;
      ++left;
      return nextleft;
    }
    if (upper_less(key_value<RightT>(right), key_value<LeftT>(left))) {
      _prior_right = right;
      ++right;
      return nextright;
    }
    ++left;
    ++right;
    return nextboth;
  }

  int next_both(LeftIterT& left, RightIterT& right) {
    if (left == _left_end) {
      _result = (right == _right_end) ? equal : less;
      return stop;
    }

    // left != _left_end
    if (right == _right_end) {
      _result = greater;
      return stop;
    }

    // The starting intervals have to begin equally
    if (lower_less(key_value<LeftT>(left),
                   key_value<RightT>(right))) { // left: same A... = sameA...
      // right:same  B.. = sameB...
      _result = less;
      return stop;
    }

    if (lower_less(key_value<LeftT>(right),
                   key_value<RightT>(left))) { // left: same  B.. = sameB...
      // right:same A... = sameA...
      _result = greater;
      return stop;
    }

    if (_compare_codomain && !covalues_are_equal(left, right))
      return stop;

    return proceed(left, right);
  }

  int next_left(LeftIterT& left, RightIterT& right) {
    if (left == _left_end) { // left: same
      // right:sameA...
      _result = less;
      return stop;
    }

    if (!key_value<LeftT>(_prior_left)
             .touches(key_value<LeftT>(left))) { // left: same B = sameB...
      // right:sameA  = sameA...
      _result = greater;
      return stop;
    }

    if (_compare_codomain && !covalues_are_equal(left, right))
      return stop;

    return proceed(left, right);
  }

  int next_right(LeftIterT& left, RightIterT& right) {
    if (right == _right_end) { // left: sameA...
      // right:same
      _result = greater;
      return stop;
    }

    if (!key_value<RightT>(_prior_right).touches(key_value<RightT>(right))) {
      // left: sameA... = sameA...
      // right:same B.. = sameB...
      _result = less;
      return stop;
    }

    if (_compare_codomain && !covalues_are_equal(left, right))
      return stop;

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

template <typename LeftT, typename RightT>
int element_compare(const LeftT& left,   // sub
                    const RightT& right, // super
                    typename LeftT::const_iterator left_begin,
                    typename LeftT::const_iterator left_end,
                    typename RightT::const_iterator right_begin,
                    typename RightT::const_iterator right_end) {
  using Step = element_comparer<LeftT, RightT>;
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
