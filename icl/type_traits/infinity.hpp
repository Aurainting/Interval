#pragma once

#include "icl/type_traits/identity_element.hpp"
#include "icl/type_traits/is_numeric.hpp"
#include "icl/type_traits/rep_type_of.hpp"
#include "icl/type_traits/size_type_of.hpp"
#include <limits>
#include <string>
#include <type_traits>

namespace icl {

template <typename Type> struct has_std_infinity {
  using type = has_std_infinity;
  static constexpr bool value =
      std::conjunction_v<is_numeric<Type>,
                         typename std::numeric_limits<Type>::has_infinity>;
};

template <typename Type> struct has_max_infinity {
  using type = has_max_infinity;
  static constexpr bool value = std::conjunction_v<
      is_numeric<Type>,
      std::negation<typename std::numeric_limits<Type>::has_infinity>>;
};

//------------------------------------------------------------------------------
template <typename Type, bool has_std_inf = false, bool has_std_max = false>
struct get_numeric_infinity;

template <typename Type, bool has_std_max>
struct get_numeric_infinity<Type, true, has_std_max> {
  using type = get_numeric_infinity;
  static Type value() { return (std::numeric_limits<Type>::infinity)(); }
};

template <typename Type> struct get_numeric_infinity<Type, false, true> {
  using type = get_numeric_infinity;
  static Type value() { return (std::numeric_limits<Type>::max)(); }
};

template <typename Type> struct get_numeric_infinity<Type, false, false> {
  using type = get_numeric_infinity;
  static Type value() { return Type(); }
};

template <typename Type> struct numeric_infinity {
  using type = numeric_infinity;
  static Type value() {
    return get_numeric_infinity<Type, has_std_infinity<Type>::value,
                                has_max_infinity<Type>::value>::value();
  }
};

//------------------------------------------------------------------------------
template <typename Type, bool has_numeric_inf, bool has_repr_inf, bool has_size,
          bool has_diff>
struct get_infinity;

template <typename Type, bool has_repr_inf, bool has_size, bool has_diff>
struct get_infinity<Type, true, has_repr_inf, has_size, has_diff> {
  using type = get_infinity;

  static Type value() { return numeric_infinity<Type>::value(); }
};

template <typename Type, bool has_size, bool has_diff>
struct get_infinity<Type, false, true, has_size, has_diff> {
  using type = get_infinity;

  static Type value() {
    return Type(numeric_infinity<typename Type::rep>::value());
  }
};

template <typename Type, bool has_diff>
struct get_infinity<Type, false, false, true, has_diff> {
  using type = get_infinity;
  using size_type = Type::size_type;

  static Type value() { return Type(numeric_infinity<size_type>::value()); }
};

template <typename Type> struct get_infinity<Type, false, false, false, true> {
  using type = get_infinity;
  using difference_type = Type::difference_type;

  static Type value() { return identity_element<difference_type>::value(); }
};

template <typename Type> struct get_infinity<Type, false, false, false, false> {
  using type = get_infinity;

  static Type value() { return identity_element<Type>::value(); }
};

template <typename Type> struct infinity {
  using type = infinity;

  static Type value() {
    return get_infinity<Type, is_numeric<Type>::value, has_rep_type<Type>,
                        has_size_type<Type>,
                        has_difference<Type>::value>::value();
  }
};

template <> struct infinity<std::string> {
  using type = infinity;

  static std::string value() { return {}; }
};

} // namespace icl
