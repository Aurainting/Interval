#pragma once

#include "icl/type_traits/has_set_semantics.hpp"
#include "icl/type_traits/identity_element.hpp"
#include "icl/type_traits/unit_element.hpp"
#include <type_traits>

namespace icl {

// ------------------------------------------------------------------------
template <typename Type> struct identity_based_inplace_combine {
  using first_argument_type = Type&;
  using second_argument_type = const Type&;
  using result_type = void;

  static Type identity_element() {
    return icl::identity_element<Type>::value();
  }
};

// ------------------------------------------------------------------------
template <typename Type> struct unit_element_based_inplace_combine {
  using first_argument_type = Type&;
  using second_argument_type = const Type&;
  using result_type = void;

  static Type identity_element() { return unit_element<Type>::value(); }
};

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_identity : identity_based_inplace_combine<Type> {
  using type = inplace_identity;

  void operator()(Type&, const Type&) const {}
};

template <>
inline std::string unary_template_to_string<inplace_identity>::apply() {
  return "i=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_erasure : identity_based_inplace_combine<Type> {
  using type = inplace_erasure;
  using base_type = identity_based_inplace_combine<Type>;

  void operator()(Type& object, const Type& operand) const {
    if (object == operand)
      // identity_element(); //JODO Old gcc-3.4.4 does not compile this
      object = base_type::identity_element(); //<-- but this.
  }
};

template <>
inline std::string unary_template_to_string<inplace_erasure>::apply() {
  return "0=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_plus : identity_based_inplace_combine<Type> {
  using type = inplace_plus;

  void operator()(Type& object, const Type& operand) const {
    object += operand;
  }

  static void version(Type&) {}
};

template <> inline std::string unary_template_to_string<inplace_plus>::apply() {
  return "+=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_minus : identity_based_inplace_combine<Type> {
  using type = inplace_minus;

  void operator()(Type& object, const Type& operand) const {
    object -= operand;
  }
};

template <>
inline std::string unary_template_to_string<inplace_minus>::apply() {
  return "-=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_bit_add : identity_based_inplace_combine<Type> {
  using type = inplace_bit_add;

  void operator()(Type& object, const Type& operand) const {
    object |= operand;
  }

  static void version(Type&) {}
};

template <>
inline std::string unary_template_to_string<inplace_bit_add>::apply() {
  return "b|=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_bit_subtract : identity_based_inplace_combine<Type> {
  using type = inplace_bit_subtract;

  void operator()(Type& object, const Type& operand) const {
    object &= ~operand;
  }
};

template <>
inline std::string unary_template_to_string<inplace_bit_subtract>::apply() {
  return "b-=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_bit_and : identity_based_inplace_combine<Type> {
  using type = inplace_bit_and;

  void operator()(Type& object, const Type& operand) const {
    object &= operand;
  }
};

template <>
inline std::string unary_template_to_string<inplace_bit_and>::apply() {
  return "b&=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_bit_xor : identity_based_inplace_combine<Type> {
  using type = inplace_bit_xor;

  void operator()(Type& object, const Type& operand) const {
    object ^= operand;
  }
};

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_et : identity_based_inplace_combine<Type> {
  using type = inplace_et;

  void operator()(Type& object, const Type& operand) const {
    object &= operand;
  }
};

template <> inline std::string unary_template_to_string<inplace_et>::apply() {
  return "&=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_caret : identity_based_inplace_combine<Type> {
  using type = inplace_caret;

  void operator()(Type& object, const Type& operand) const {
    object ^= operand;
  }
};

template <>
inline std::string unary_template_to_string<inplace_caret>::apply() {
  return "^=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_insert : identity_based_inplace_combine<Type> {
  using type = inplace_insert;

  void operator()(Type& object, const Type& operand) const {
    insert(object, operand);
  }
};

template <>
inline std::string unary_template_to_string<inplace_insert>::apply() {
  return "ins=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_erase : identity_based_inplace_combine<Type> {
  using type = inplace_erase;

  void operator()(Type& object, const Type& operand) const {
    erase(object, operand);
  }
};

template <>
inline std::string unary_template_to_string<inplace_erase>::apply() {
  return "ers=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_star : identity_based_inplace_combine<Type> // JODO unit_element_
{
  using type = inplace_star;

  void operator()(Type& object, const Type& operand) const {
    object *= operand;
  }
};

template <> inline std::string unary_template_to_string<inplace_star>::apply() {
  return "*=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_slash
    : identity_based_inplace_combine<Type> // JODO unit_element_
{
  using type = inplace_slash;

  void operator()(Type& object, const Type& operand) const {
    object /= operand;
  }
};

template <>
inline std::string unary_template_to_string<inplace_slash>::apply() {
  return "/=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_max : identity_based_inplace_combine<Type> {
  using type = inplace_max;

  void operator()(Type& object, const Type& operand) const {
    if (object < operand)
      object = operand;
  }
};

template <> inline std::string unary_template_to_string<inplace_max>::apply() {
  return "max=";
}

// ------------------------------------------------------------------------
template <typename Type>
struct inplace_min : identity_based_inplace_combine<Type> {
  using type = inplace_min;

  void operator()(Type& object, const Type& operand) const {
    if (object > operand)
      object = operand;
  }
};

template <> inline std::string unary_template_to_string<inplace_min>::apply() {
  return "min=";
}

//--------------------------------------------------------------------------
// Inter_section functor
//--------------------------------------------------------------------------
template <typename Type>
struct inter_section : identity_based_inplace_combine<Type> {
  using type = std::conditional_t<has_set_semantics<Type>::value,
                                  inplace_et<Type>, inplace_plus<Type>>;

  void operator()(Type& object, const Type& operand) const {
    type()(object, operand);
  }
};

//--------------------------------------------------------------------------
// Inverse functor
//--------------------------------------------------------------------------
template <typename Functor> struct inverse;

template <typename Type> struct inverse<inplace_plus<Type>> {
  using type = inplace_minus<Type>;
};

template <typename Type> struct inverse<inplace_minus<Type>> {
  using type = inplace_plus<Type>;
};

template <typename Type> struct inverse<inplace_bit_add<Type>> {
  using type = inplace_bit_subtract<Type>;
};

template <typename Type> struct inverse<inplace_bit_subtract<Type>> {
  using type = inplace_bit_add<Type>;
};

template <typename Type> struct inverse<inplace_et<Type>> {
  using type = inplace_caret<Type>;
};

template <typename Type> struct inverse<icl::inplace_caret<Type>> {
  using type = icl::inplace_et<Type>;
};

template <typename Type> struct inverse<icl::inplace_bit_and<Type>> {
  using type = icl::inplace_bit_xor<Type>;
};

template <typename Type> struct inverse<icl::inplace_bit_xor<Type>> {
  using type = icl::inplace_bit_and<Type>;
};

template <typename Type> struct inverse<icl::inplace_star<Type>> {
  using type = icl::inplace_slash<Type>;
};

template <typename Type> struct inverse<icl::inplace_slash<Type>> {
  using type = icl::inplace_star<Type>;
};

template <typename Type> struct inverse<icl::inplace_max<Type>> {
  using type = icl::inplace_min<Type>;
};

template <typename Type> struct inverse<icl::inplace_min<Type>> {
  using type = icl::inplace_max<Type>;
};

template <typename Type> struct inverse<icl::inplace_identity<Type>> {
  using type = icl::inplace_erasure<Type>;
};

// If a Functor
template <typename Functor> struct inverse {
  using argument_type = typename std::remove_reference<
      typename Functor::first_argument_type>::type;
  using type = icl::inplace_erasure<argument_type>;
};

//--------------------------------------------------------------------------
// Inverse inter_section functor
//--------------------------------------------------------------------------
template <typename Type>
struct inverse<icl::inter_section<Type>>
    : public identity_based_inplace_combine<Type> {
  using type = typename std::conditional_t<has_set_semantics<Type>::value,
                                           icl::inplace_caret<Type>,
                                           icl::inplace_minus<Type>>;

  void operator()(Type& object, const Type& operand) const {
    type()(object, operand);
  }
};

//--------------------------------------------------------------------------
// Positive or negative functor trait
//--------------------------------------------------------------------------

// A binary operation - is negative (or inverting) with respect to the
// neutral element iff it yields the inverse element if it is applied to the
// identity element:
// 0 - x = -x
// For a functor that wraps the inplace of op-assign version this is
// equivalent to
//
// T x = ..., y;
// y = Functor::identity_element();
// Functor()(y, x); // y == inverse_of(x)

template <typename Functor> struct is_negative;

template <typename Functor> struct is_negative {
  using type = is_negative<Functor>;
  static constexpr bool value = false;
};

template <typename Type> struct is_negative<icl::inplace_minus<Type>> {
  using type = is_negative;

  static constexpr bool value = true;
};

template <typename Type> struct is_negative<icl::inplace_bit_subtract<Type>> {
  using type = is_negative;

  static constexpr bool value = true;
};

//--------------------------------------------------------------------------
// Pro- or in-version functor
//--------------------------------------------------------------------------
template <typename Combiner> struct conversion;

template <typename Combiner> struct conversion {
  using type = conversion<Combiner>;
  using argument_type =
      typename std::remove_const<typename std::remove_reference<
          typename Combiner::first_argument_type>::type>::type;
  // The proversion of an op-assign functor o= lets the value unchanged
  // (0 o= x) == x;
  // Example += :  (0 += x) == x
  static argument_type proversion(const argument_type& value) { return value; }

  // The inversion of an op-assign functor o= inverts the value x
  // to it's inverse element -x
  // (0 o= x) == -x;
  // Example -= :  (0 -= x) == -x
  static argument_type inversion(const argument_type& value) {
    argument_type inverse = Combiner::identity_element();
    Combiner()(inverse, value);
    return inverse;
  }
};

template <typename Combiner> struct version : public conversion<Combiner> {
  using type = version<Combiner>;
  using base_type = conversion<Combiner>;
  using argument_type = typename base_type::argument_type;

  argument_type operator()(const argument_type& value) {
    return base_type::proversion(value);
  }
};

template <> struct version<inplace_minus<short>> {
  short operator()(short val) const { return -val; }
};
template <> struct version<inplace_minus<int>> {
  int operator()(int val) const { return -val; }
};
template <> struct version<inplace_minus<long>> {
  long operator()(long val) const { return -val; }
};
template <> struct version<inplace_minus<long long>> {
  long long operator()(const long long val) const { return -val; }
};
template <> struct version<inplace_minus<float>> {
  float operator()(const float val) const { return -val; }
};
template <> struct version<inplace_minus<double>> {
  double operator()(const double val) const { return -val; }
};
template <> struct version<inplace_minus<long double>> {
  long double operator()(const long double val) const { return -val; }
};

template <typename Type>
struct version<inplace_minus<Type>> : conversion<inplace_minus<Type>> {
  using type = version;
  using base_type = conversion<inplace_minus<Type>>;
  using argument_type = base_type::argument_type;

  Type operator()(const Type& value) { return base_type::inversion(value); }
};

} // namespace icl
