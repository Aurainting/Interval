#pragma once

#include <string>

namespace icl {

//--------------------------------------------------------------------------
template <typename> struct type_to_string {
  /** Convert the type to it's typestring */
  static std::string apply();
};

//--------------------------------------------------------------------------
template <> inline std::string type_to_string<bool>::apply() { return "bool"; }
template <> inline std::string type_to_string<char>::apply() { return "char"; }
template <> inline std::string type_to_string<short>::apply() {
  return "short";
}
template <> inline std::string type_to_string<int>::apply() { return "int"; }
template <> inline std::string type_to_string<long>::apply() { return "long"; }
template <> inline std::string type_to_string<long long>::apply() {
  return "Long";
}

template <> inline std::string type_to_string<unsigned char>::apply() {
  return "char+";
}
template <> inline std::string type_to_string<unsigned short>::apply() {
  return "short+";
}
template <> inline std::string type_to_string<unsigned int>::apply() {
  return "int+";
}
template <> inline std::string type_to_string<unsigned long>::apply() {
  return "long+";
}
template <> inline std::string type_to_string<unsigned long long>::apply() {
  return "Long+";
}

template <> inline std::string type_to_string<float>::apply() { return "flt"; }
template <> inline std::string type_to_string<double>::apply() { return "dbl"; }

//-------------------------------------------------------------------------
template <template <typename> typename> struct unary_template_to_string {
  static std::string apply();
};

template <template <typename> typename Unary, typename Type>
struct type_to_string<Unary<Type>> {
  static std::string to_string() {
    return unary_template_to_string<Unary>::apply() + "<" +
           type_to_string<Type>::apply() + ">";
  }
};

// ---------------------------------------------------------------------------
template <template <typename, typename> typename>
struct binary_template_to_string {
  static std::string apply();
};

template <template <typename Type1, typename Type2> typename Binary,
          typename Type1, typename Type2>
struct type_to_string<Binary<Type1, Type2>> {
  static std::string apply() {
    return binary_template_to_string<Binary>::apply() + "<" +
           type_to_string<Type1>::apply() + "," +
           type_to_string<Type2>::apply() + ">";
  }
};

// ---------------------------------------------------------------------------
template <> struct type_to_string<std::string> {
  static std::string apply() { return "string"; }
};

} // namespace icl
