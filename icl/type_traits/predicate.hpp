#pragma once

namespace icl {
// naming convention
// predicate: n-ary predicate
// property:  unary predicate
// relation:  binary predicate

// Unary predicates

template <typename Type> class property {
public:
  using argument_type = Type;
  using result_type = bool;
};

template <typename Type> class member_property : public property<Type> {
public:
  explicit member_property(bool (Type::*pred)() const)
      : property<Type>(), m_pred(pred) {}
  bool operator()(const Type& x) const { return (x.*m_pred)(); }

private:
  bool (Type::*m_pred)() const;
};

// Binary predicates: relations

template <typename LeftT, typename RightT> class relation {
public:
  using first_argument_type = LeftT;
  using second_argument_type = RightT;
  using result_type = bool;
};

} // namespace icl
