#pragma once

#include "icl/type_traits/is_map.hpp"
#include "icl/type_traits/is_set.hpp"
#include <type_traits>

namespace icl {

//--------------------------------------------------------------------------
template <typename Type>
concept has_key_object_type = requires { typename Type::key_object_type; };

template <typename> struct key_container_type_of {
  using type = no_type;
};

template <has_key_object_type Type> struct key_container_type_of<Type> {
  using type = Type::key_object_type;
};

template <typename Type>
  requires(!has_key_object_type<Type> &&
           (is_set<Type>::value || is_map<Type>::value))
struct key_container_type_of<Type> {
  using type = Type;
};

//--------------------------------------------------------------------------
template <typename KeyT, typename ObjectT>
struct is_strict_key_container_of // set is_strict_key_container_of map
{
  using type = is_strict_key_container_of;
  static constexpr bool value = std::conjunction_v<
      is_map<ObjectT>,
      std::is_same<KeyT, typename key_container_type_of<ObjectT>::type>>;
};

template <typename KeyT, typename ObjectT>
struct is_key_container_of // set is_key_container_of (set or map)
{
  using type = is_key_container_of;
  static constexpr bool value = std::disjunction_v<
      is_strict_key_container_of<KeyT, ObjectT>,
      std::conjunction<std::disjunction<is_set<ObjectT>, is_map<ObjectT>>,
                       std::is_same<ObjectT, KeyT>>>;
};

} // namespace icl
