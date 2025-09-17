#pragma once

#include "icl/type_traits/is_interval_container.hpp"
#include <type_traits>

namespace icl {

template <typename Type> struct is_overloadable {
  using type = is_overloadable;

  static constexpr bool value =
      std::is_same_v<Type, typename Type::overloadable_type>;
};

//------------------------------------------------------------------------------
template <typename LeftT, typename RightT> struct is_codomain_equal {
  using type = is_codomain_equal;
  static constexpr bool value = std::is_same_v<typename LeftT::codomain_type,
                                               typename RightT::codomain_type>;
};

// NOTE: Equality of compare order implies the equality of the domain_types
template <typename LeftT, typename RightT> struct is_key_compare_equal {
  using type = is_key_compare_equal;
  static constexpr bool value =
      std::is_same_v<typename LeftT::key_compare, typename RightT::key_compare>;
};

template <typename LeftT, typename RightT> struct is_codomain_type_equal {
  using type = is_codomain_type_equal;
  static constexpr bool value =
      std::conjunction_v<is_key_compare_equal<LeftT, RightT>,
                         is_codomain_equal<LeftT, RightT>>;
};

// For equal containers concepts, domain order and codomain type must match.
template <template <typename> typename IsConcept, typename LeftT,
          typename RightT>
struct is_concept_compatible {
  using type = is_concept_compatible;
  static constexpr bool value =
      std::conjunction_v<IsConcept<LeftT>, IsConcept<RightT>,
                         is_codomain_type_equal<LeftT, RightT>>;
};

template <template <typename> typename LeftConcept,
          template <typename> typename RightConcept, typename LeftT,
          typename RightT>
struct is_concept_combinable {
  using type = is_concept_combinable;
  static constexpr bool value =
      std::conjunction_v<LeftConcept<LeftT>, RightConcept<RightT>,
                         is_key_compare_equal<LeftT, RightT>>;
};

template <typename LeftT, typename RightT> struct is_intra_combinable {
  using type = is_intra_combinable;
  static constexpr bool value =
      std::disjunction_v<is_concept_compatible<is_interval_set, LeftT, RightT>,
                         is_concept_compatible<is_interval_map, LeftT, RightT>>;
};

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
template <typename LeftT, typename RightT> struct is_cross_combinable {
  using type = is_cross_combinable;
  static constexpr bool value = std::disjunction_v<
      is_concept_combinable<is_interval_set, is_interval_map, LeftT, RightT>,
      is_concept_combinable<is_interval_map, is_interval_set, LeftT, RightT>>;
};

template <typename LeftT, typename RightT> struct is_inter_combinable {
  using type = is_inter_combinable;

  static constexpr bool value =
      std::disjunction_v<is_intra_combinable<LeftT, RightT>,
                         is_cross_combinable<LeftT, RightT>>;
};

//------------------------------------------------------------------------------
// is_fragment_of
//------------------------------------------------------------------------------
template <typename, typename> struct is_fragment_of {
  using type = is_fragment_of;
  static constexpr bool value = false;
};

template <typename Type>
struct is_fragment_of<typename Type::element_type, Type> {
  using type = is_fragment_of;
  static constexpr bool value = true;
};

template <typename Type>
struct is_fragment_of<typename Type::segment_type, Type> {
  using type = is_fragment_of;
  static constexpr bool value = true;
};

//------------------------------------------------------------------------------
// is_key_of
//------------------------------------------------------------------------------
template <typename, typename> struct is_key_of {
  using type = is_key_of;
  static constexpr bool value = false;
};

template <typename Type> struct is_key_of<typename Type::domain_type, Type> {
  using type = is_key_of;
  static constexpr bool value = true;
};

template <typename Type> struct is_key_of<typename Type::interval_type, Type> {
  using type = is_key_of;
  static constexpr bool value = true;
};

//------------------------------------------------------------------------------
// is_interval_set_derivative
//------------------------------------------------------------------------------
template <typename Type, typename AssociateT> struct is_interval_set_derivative;

template <typename Type>
struct is_interval_set_derivative<Type, typename Type::domain_type> {
  using type = is_interval_set_derivative;
  static constexpr bool value = is_interval_container<Type>::value;
};

template <typename Type>
struct is_interval_set_derivative<Type, typename Type::interval_type> {
  using type = is_interval_set_derivative;
  static constexpr bool value = is_interval_container<Type>::value;
};

template <typename, typename> struct is_interval_set_derivative {
  using type = is_interval_set_derivative;
  static constexpr bool value = false;
};

//------------------------------------------------------------------------------
// is_interval_map_derivative
//------------------------------------------------------------------------------
template <typename Type, typename AssociateT> struct is_interval_map_derivative;

template <typename Type>
struct is_interval_map_derivative<Type, typename Type::domain_mapping_type> {
  using type = is_interval_map_derivative;
  static constexpr bool value = is_interval_container<Type>::value;
};

template <typename Type>
struct is_interval_map_derivative<Type, typename Type::interval_mapping_type> {
  using type = is_interval_map_derivative;
  static constexpr bool value = is_interval_container<Type>::value;
};

template <typename Type>
struct is_interval_map_derivative<Type, typename Type::value_type> {
  using type = is_interval_map_derivative;
  static constexpr bool value = is_interval_container<Type>::value;
};

template <typename, typename> struct is_interval_map_derivative {
  using type = is_interval_map_derivative;
  static constexpr bool value = false;
};

//------------------------------------------------------------------------------
// is_intra_derivative
//------------------------------------------------------------------------------
template <typename Type, typename AssociateT> struct is_intra_derivative {
  using type = is_intra_derivative;
  static constexpr bool value = std::disjunction_v<
      std::conjunction<is_interval_set<Type>,
                       is_interval_set_derivative<Type, AssociateT>>,
      std::conjunction<is_interval_map<Type>,
                       is_interval_map_derivative<Type, AssociateT>>>;
};

template <typename Type, typename AssociateT> struct is_cross_derivative {
  using type = is_cross_derivative;
  static constexpr bool value =
      std::conjunction_v<is_interval_map<Type>,
                         is_interval_set_derivative<Type, AssociateT>>;
};

template <typename Type, typename AssociateT> struct is_inter_derivative {
  using type = is_inter_derivative;
  static constexpr bool value =
      std::disjunction_v<is_intra_derivative<Type, AssociateT>,
                         is_cross_derivative<Type, AssociateT>>;
};

//------------------------------------------------------------------------------
//- right combinable
//------------------------------------------------------------------------------

template <typename GuideT, typename CompanionT>
struct is_interval_set_right_combinable {
  using type = is_interval_set_right_combinable;
  static constexpr bool value = std::conjunction_v<
      is_interval_set<GuideT>,
      std::disjunction<
          is_interval_set_derivative<GuideT, CompanionT>,
          is_concept_compatible<is_interval_set, GuideT, CompanionT>>>;
};

template <typename GuideT, typename CompanionT>
struct is_interval_map_right_intra_combinable // NOTE equivalent to
                                              // is_fragment_type_of
{
  using type = is_interval_map_right_intra_combinable;
  static constexpr bool value = std::conjunction_v<
      is_interval_map<GuideT>,
      std::disjunction<
          is_interval_map_derivative<GuideT, CompanionT>,
          is_concept_compatible<is_interval_map, GuideT, CompanionT>>>;
};

template <typename GuideT, typename CompanionT>
struct is_interval_map_right_cross_combinable // NOTE equivalent to
                                              // key_type_of<Comp, Guide>
{
  using type = is_interval_map_right_cross_combinable;
  static constexpr bool value = std::conjunction_v<
      is_interval_map<GuideT>,
      std::disjunction<is_cross_derivative<GuideT, CompanionT>,
                       is_concept_combinable<is_interval_map, is_interval_set,
                                             GuideT, CompanionT>>>;
};

template <typename GuideT, typename CompanionT>
struct is_interval_map_right_inter_combinable {
  using type = is_interval_map_right_inter_combinable;
  static constexpr bool value = std::disjunction_v<
      is_interval_map_right_intra_combinable<GuideT, CompanionT>,
      is_interval_map_right_cross_combinable<GuideT, CompanionT>>;
};

template <typename GuideT, typename CompanionT>
struct is_right_intra_combinable {
  using type = is_right_intra_combinable;
  static constexpr bool value = std::disjunction_v<
      is_interval_set_right_combinable<GuideT, CompanionT>,
      is_interval_map_right_intra_combinable<GuideT, CompanionT>>;
};

template <typename GuideT, typename CompanionT>
struct is_right_inter_combinable {
  using type = is_right_inter_combinable;
  static constexpr bool value = std::disjunction_v<
      is_interval_set_right_combinable<GuideT, CompanionT>,
      is_interval_map_right_inter_combinable<GuideT, CompanionT>>;
};

template <typename GuideT, typename IntervalSetT>
struct combines_right_to_interval_set {
  using type = combines_right_to_interval_set;
  static constexpr bool value =
      is_concept_combinable<is_interval_container, is_interval_set, GuideT,
                            IntervalSetT>::value;
};

template <typename GuideT, typename IntervalMapT>
struct combines_right_to_interval_map {
  using type = combines_right_to_interval_map;
  static constexpr bool value =
      is_concept_compatible<is_interval_map, GuideT, IntervalMapT>::value;
};

template <typename GuideT, typename IntervalContainerT>
struct combines_right_to_interval_container {
  using type = combines_right_to_interval_container;
  static constexpr bool value = std::disjunction_v<
      combines_right_to_interval_set<GuideT, IntervalContainerT>,
      combines_right_to_interval_map<GuideT, IntervalContainerT>>;
};

//------------------------------------------------------------------------------
//- segmentational_fineness
//------------------------------------------------------------------------------
template <typename> struct unknown_fineness {
  using type = unknown_fineness;
  static constexpr int value = 0;
};

template <typename Type> struct known_fineness {
  using type = known_fineness;
  static const int value = Type::fineness;
};

template <typename Type> struct segmentational_fineness {
  using type = segmentational_fineness;
  static const int value =
      std::conditional_t<is_interval_container<Type>::value,
                         known_fineness<Type>, unknown_fineness<Type>>::value;
};

//------------------------------------------------------------------------------
// is_interval_set_companion
//------------------------------------------------------------------------------

// CompanionT is either an interval_set or a derivative of set level:
// element_type=domain_type, segment_type=interval_type
template <typename GuideT, typename CompanionT>
struct is_interval_set_companion {
  using type = is_interval_set_companion;
  static constexpr bool value =
      std::disjunction_v<combines_right_to_interval_set<GuideT, CompanionT>,
                         is_interval_set_derivative<GuideT, CompanionT>>;
};

//------------------------------------------------------------------------------
// is_interval_map_companion
//------------------------------------------------------------------------------

template <typename GuideT, typename CompanionT>
struct is_interval_map_companion {
  using type = is_interval_map_companion;
  static constexpr bool value =
      std::disjunction_v<combines_right_to_interval_map<GuideT, CompanionT>,
                         is_interval_map_derivative<GuideT, CompanionT>>;
};

//------------------------------------------------------------------------------
//- is_coarser_interval_{set,map}_companion
//------------------------------------------------------------------------------
template <typename GuideT, typename CompanionT>
struct is_coarser_interval_set_companion {
  using type = is_coarser_interval_set_companion;
  static constexpr bool value = std::conjunction_v<
      is_interval_set_companion<GuideT, CompanionT>,
      std::bool_constant<(segmentational_fineness<GuideT>::value >
                          segmentational_fineness<CompanionT>::value)>>;
};

template <typename GuideT, typename CompanionT>
struct is_coarser_interval_map_companion {
  using type = is_coarser_interval_map_companion;
  static constexpr bool value = std::conjunction_v<
      is_interval_map_companion<GuideT, CompanionT>,
      std::bool_constant<(segmentational_fineness<GuideT>::value >
                          segmentational_fineness<CompanionT>::value)>>;
};

//------------------------------------------------------------------------------
// is_binary_interval_{set,map}_combinable
//------------------------------------------------------------------------------
template <typename GuideT, typename CompanionT>
struct is_binary_interval_set_combinable {
  using type = is_binary_interval_set_combinable;
  static const int value =
      std::conjunction_v<is_interval_set<GuideT>,
                         is_coarser_interval_set_companion<GuideT, CompanionT>>;
};

template <typename GuideT, typename CompanionT>
struct is_binary_interval_map_combinable {
  using type = is_binary_interval_map_combinable;
  static const int value =
      std::conjunction_v<is_interval_map<GuideT>,
                         is_coarser_interval_map_companion<GuideT, CompanionT>>;
};

template <typename GuideT, typename CompanionT>
struct is_binary_intra_combinable {
  using type = is_binary_intra_combinable;
  static constexpr bool value =
      std::disjunction_v<is_binary_interval_set_combinable<GuideT, CompanionT>,
                         is_binary_interval_map_combinable<GuideT, CompanionT>>;
};

template <typename GuideT, typename CompanionT>
struct is_binary_cross_combinable {
  using type = is_binary_cross_combinable;
  static constexpr bool value = std::conjunction_v<
      is_interval_map<GuideT>,
      std::disjunction<is_coarser_interval_map_companion<GuideT, CompanionT>,
                       is_interval_set_companion<GuideT, CompanionT>>>;
};

template <typename GuideT, typename CompanionT>
struct is_binary_inter_combinable {
  using type = is_binary_inter_combinable;
  static constexpr bool value = std::disjunction_v<
      std::conjunction<is_interval_map<GuideT>,
                       is_binary_cross_combinable<GuideT, CompanionT>>,
      std::conjunction<is_interval_set<GuideT>,
                       is_binary_intra_combinable<GuideT, CompanionT>>>;
};

} // namespace icl
