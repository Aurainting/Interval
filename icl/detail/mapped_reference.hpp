#pragma once

#include <type_traits>
#include <utility>

namespace icl {

template <typename FirstT, typename SecondT> class mapped_reference;

//------------------------------------------------------------------------------
template <typename> struct is_mapped_reference_combinable {
  using type = is_mapped_reference_combinable;
  static constexpr bool value = false;
};

template <typename FirstT, typename SecondT>
struct is_mapped_reference_combinable<std::pair<const FirstT, SecondT>> {
  using type = is_mapped_reference_combinable;
  static constexpr bool value = true;
};

template <typename FirstT, typename SecondT>
struct is_mapped_reference_combinable<std::pair<FirstT, SecondT>> {
  using type = is_mapped_reference_combinable;
  static constexpr bool value = true;
};

//------------------------------------------------------------------------------
template <typename Type> struct is_mapped_reference_or_combinable {
  using type = is_mapped_reference_or_combinable;
  static constexpr bool value = is_mapped_reference_combinable<Type>::value;
};

template <typename FirstT, typename SecondT>
struct is_mapped_reference_or_combinable<mapped_reference<FirstT, SecondT>> {
  using type = is_mapped_reference_or_combinable;
  static constexpr bool value = true;
};

//------------------------------------------------------------------------------
template <typename FirstT, typename SecondT> class mapped_reference {
public:
  using first_type = FirstT;
  using second_type = SecondT;
  using type = mapped_reference;

  using second_reference_type =
      std::conditional_t<std::is_const_v<second_type>, second_type&,
                         const second_type&>;

  using std_pair_type = std::pair<first_type, second_type>;
  using key_std_pair_type = std::pair<const first_type, second_type>;

  const first_type& first;
  second_reference_type second;

  mapped_reference(const FirstT& fst, second_reference_type snd)
      : first(fst), second(snd) {}

  template <typename FstT, typename SndT>
  explicit mapped_reference(const mapped_reference<FstT, SndT>& source)
      : first(source.first), second(source.second) {}

  template <typename FstT, typename SndT>
  explicit operator std::pair<FstT, SndT>() {
    return std::pair<FstT, SndT>(first, second);
  }

  mapped_reference& operator=(const mapped_reference&) = delete;

  template <typename Comparand>
    requires is_mapped_reference_or_combinable<Comparand>::value
  bool operator==(const Comparand& right) const {
    return first == right.first && second == right.second;
  }

  template <typename Comparand>
    requires is_mapped_reference_or_combinable<Comparand>::value
  bool operator!=(const Comparand& right) const {
    return !(*this == right);
  }

  template <typename Comparand>
    requires is_mapped_reference_or_combinable<Comparand>::value
  bool operator<(const Comparand& right) const {
    return first < right.first ||
           (!(right.first < first) && second < right.second);
  }

  template <typename Comparand>
    requires is_mapped_reference_or_combinable<Comparand>::value
  bool operator>(const Comparand& right) const {
    return first > right.first ||
           (!(right.first > first) && second > right.second);
  }

  template <typename Comparand>
    requires is_mapped_reference_or_combinable<Comparand>::value
  bool operator<=(const Comparand& right) const {
    return !(*this > right);
  }

  template <typename Comparand>
    requires is_mapped_reference_or_combinable<Comparand>::value
  bool operator>=(const Comparand& right) const {
    return !(*this < right);
  }
};

//------------------------------------------------------------------------------
template <typename FirstT, typename SecondT, typename StdPairT>
  requires is_mapped_reference_combinable<StdPairT>::value
bool operator==(const StdPairT& left,
                const mapped_reference<FirstT, SecondT>& right) {
  return right == left;
}

template <typename FirstT, typename SecondT, typename StdPairT>
  requires is_mapped_reference_combinable<StdPairT>::value
bool operator!=(const StdPairT& left,
                const mapped_reference<FirstT, SecondT>& right) {
  return !(right == left);
}

//------------------------------------------------------------------------------
template <typename FirstT, typename SecondT, typename StdPairT>
  requires is_mapped_reference_combinable<StdPairT>::value
bool operator<(const StdPairT& left,
               const mapped_reference<FirstT, SecondT>& right) {
  return right > left;
}

//------------------------------------------------------------------------------
template <typename FirstT, typename SecondT, typename StdPairT>
  requires is_mapped_reference_combinable<StdPairT>::value
bool operator>(const StdPairT& left,
               const mapped_reference<FirstT, SecondT>& right) {
  return right < left;
}

//------------------------------------------------------------------------------
template <typename FirstT, typename SecondT, typename StdPairT>
  requires is_mapped_reference_combinable<StdPairT>::value
bool operator<=(const StdPairT& left,
                const mapped_reference<FirstT, SecondT>& right) {
  return !(right < left);
}

//------------------------------------------------------------------------------
template <typename FirstT, typename SecondT, typename StdPairT>
  requires is_mapped_reference_combinable<StdPairT>::value
bool operator>=(const StdPairT& left,
                const mapped_reference<FirstT, SecondT>& right) {
  return !(left < right);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
template <typename FirstT, typename SecondT>
mapped_reference<FirstT, SecondT> make_mapped_reference(const FirstT& left,
                                                        SecondT& right) {
  return mapped_reference<FirstT, SecondT>(left, right);
}

} // namespace icl
