/******************************************************************************
* djinterp [container]                                           stl_traits.hpp
*
* Comprehensive STL container traits organized for modularity and extensibility.
* Traits are arranged by type (alias, method, operator) then by C++ version.
*
* ORGANIZATION:
*   0.   Foundation (detection idiom, helpers)
*   I.   Type Alias Detectors
*        - I.A   C++11 Common Container Aliases
*        - I.B   C++11 Iterator Aliases
*        - I.C   C++11 Associative Container Aliases
*        - I.D   C++11 Container Adaptor Aliases
*        - I.E   C++11 String Aliases
*        - I.F   C++11 Smart Pointer Aliases
*        - I.G   C++11 Allocator Aliases
*        - I.H   C++11 Iterator Traits Aliases
*        - I.I   C++17 Node Handle Aliases
*        - I.J   C++17 Smart Pointer Aliases
*        - I.K   C++20 Iterator Concept Aliases
*        - I.L   C++20/23 Span/MDSpan Aliases
*   II.  Method Detectors
*        - II.A  C++11 Element Access Methods
*        - II.B  C++11 Iterator Methods
*        - II.C  C++11 Capacity Methods
*        - II.D  C++11 Modifier Methods
*        - II.E  C++11 List/Forward_List Specific Methods
*        - II.F  C++11 Associative Container Methods
*        - II.G  C++11 Unordered Container Methods
*   III. Operator Detectors
*        - III.A C++11 Comparison Operators
*        - III.B C++20 Three-Way Comparison
*        - III.C C++11 Arithmetic Operators
*        - III.D C++11 Logical Operators
*        - III.E C++11 Bitwise Operators
*        - III.F C++11 Stream Operators
*   IV.  Return Type Validators
*   V.   Tuple Protocol Detectors
*   VI.  Type Relationship Validators
*   VII. Composite Traits (Container Classification)
*        - VII.A Alias Group Traits
*        - VII.B Method Group Traits
*        - VII.C Container Type Traits (array, vector, deque, etc.)
*   VIII.Diagnostic Traits
*
* MODULARITY:
*   Each section is designed for easy extraction to child modules. Traits
*   within a section share dependencies only on previous sections.
*
* PORTABILITY:
*   Uses env.h for version detection and cpp_features.h for feature detection.
*   Variable templates (_v) require D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES.
*
* path:      \inc\container\stl_traits.hpp
* link(s):   TBA
* author(s): Samuel 'teer' Neal-Blim                          date: 2026.01.30
******************************************************************************/

#ifndef DJINTERP_STL_TRAITS_
#define DJINTERP_STL_TRAITS_ 1

#include <cstddef>
#include <iosfwd>
#include <type_traits>
#include <utility>
#include "..\env.h"
#include "..\cpp_features.h"
#include "..\djinterp.h"


NS_DJINTERP
NS_CONTAINER
NS_STL


// #############################################################################
// 0.   FOUNDATION
// #############################################################################
// Detection idiom and helper utilities. Required by all subsequent sections.
// DEPENDENCIES: None (self-contained)
// EXTRACTABLE:  Yes - can be moved to a separate detection.hpp
// #############################################################################


NS_INTERNAL
    // void_t
    //   alias template: maps any type sequence to void.
    template<typename... _Ts>
    using void_t = typename make_void<_Ts...>::type;
NS_END  // internal
    // void_t
    //   alias template: maps any type sequence to void.
    template<typename... _Ts>
    using void_t = typename make_void<_Ts...>::type;

// -----------------------------------------------------------------------------
// 0.B  Detection Idiom
// -----------------------------------------------------------------------------

NS_INTERNAL

    // detector
    //   trait: primary template for detection idiom (failure case).
    //   Returns false_type and _Default when _Op<_Args...> is ill-formed.
    template<typename                       _Default,
             typename                       _AlwaysVoid,
             template<typename...> typename _Op,
             typename...                    _Args>
    struct detector
    {
        using value_t = std::false_type;
        using type    = _Default;
    };

    // detector
    //   trait: specialization for detection idiom (success case).
    //   Returns true_type and _Op<_Args...> when well-formed.
    template<typename                       _Default,
             template<typename...> typename _Op,
             typename...                    _Args>
    struct detector<_Default, void_t<_Op<_Args...>>, _Op, _Args...>
    {
        using value_t = std::true_type;
        using type    = _Op<_Args...>;
    };

NS_END  // internal


// nonesuch
//   type: placeholder type for detection idiom representing "no such type".
//   Cannot be constructed, destructed, or copied.
struct nonesuch
{
    nonesuch()                      = delete;
    ~nonesuch()                     = delete;
    nonesuch(const nonesuch&)       = delete;
    void operator=(const nonesuch&) = delete;
};

// is_detected
//   alias template: detects if _Op<_Args...> is well-formed.
//   Yields true_type if well-formed, false_type otherwise.
template<template<typename...> typename _Op,
         typename...                    _Args>
using is_detected = typename internal::detector<nonesuch, void, _Op, _Args...>::value_t;

// detected_t
//   alias template: yields _Op<_Args...> if well-formed, else nonesuch.
template<template<typename...> typename _Op,
         typename...                    _Args>
using detected_t = typename internal::detector<nonesuch, void, _Op, _Args...>::type;

// detected_or
//   trait: detector with custom default type.
//   Provides value_t and type members.
template<typename                       _Default,
         template<typename...> typename _Op,
         typename...                    _Args>
using detected_or = internal::detector<_Default, void, _Op, _Args...>;

// detected_or_t
//   alias template: yields _Op<_Args...> if well-formed, else _Default.
template<typename                       _Default,
         template<typename...> typename _Op,
         typename...                    _Args>
using detected_or_t = typename detected_or<_Default, _Op, _Args...>::type;

// is_detected_exact
//   trait: checks if _Op<_Args...> is well-formed and exactly _Expected.
template<typename                       _Expected,
         template<typename...> typename _Op,
         typename...                    _Args>
using is_detected_exact = std::is_same<_Expected, detected_t<_Op, _Args...>>;

// is_detected_convertible
//   trait: checks if _Op<_Args...> is well-formed and convertible to _To.
template<typename                       _To,
         template<typename...> typename _Op,
         typename...                    _Args>
using is_detected_convertible = std::is_convertible<detected_t<_Op, _Args...>, _To>;

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // is_detected_v
    //   variable template: value of is_detected<_Op, _Args...>.
    template<template<typename...> typename _Op,
             typename...                    _Args>
    constexpr bool is_detected_v = is_detected<_Op, _Args...>::value;

    // is_detected_exact_v
    //   variable template: value of is_detected_exact<_Expected, _Op, _Args...>.
    template<typename                       _Expected,
             template<typename...> typename _Op,
             typename...                    _Args>
    constexpr bool is_detected_exact_v = is_detected_exact<_Expected, _Op, _Args...>::value;

    // is_detected_convertible_v
    //   variable template: value of is_detected_convertible<_To, _Op, _Args...>.
    template<typename                       _To,
             template<typename...> typename _Op,
             typename...                    _Args>
    constexpr bool is_detected_convertible_v =
        is_detected_convertible<_To, _Op, _Args...>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// -----------------------------------------------------------------------------
// 0.C  Trait Definition Macros
// -----------------------------------------------------------------------------
// These macros simplify trait definitions and ensure consistency.
// Using macros makes it trivial to move traits between modules.

// D_IF_VAR_TEMPLATES(code)
//   macro: conditionally includes code if variable templates are supported.
#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    #define D_IF_VAR_TEMPLATES(...) __VA_ARGS__
#else
    #define D_IF_VAR_TEMPLATES(...)
#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

// D_STL_DEFINE_ALIAS_DETECTOR(name)
//   macro: defines detector, trait, variable template, and type alias for a
//   member type alias. Creates: detect_##name (internal), has_##name,
//   has_##name##_v (if variable templates supported), and name##_t.
#define D_STL_DEFINE_ALIAS_DETECTOR(name)                                       \
    NS_INTERNAL                                                                 \
        template<typename _Type>                                                \
        using detect_##name = typename _Type::name;                             \
    NS_END                                                                      \
                                                                                \
    template<typename _Type>                                                    \
    struct has_##name : is_detected<internal::detect_##name, _Type>             \
    {};                                                                         \
                                                                                \
    D_IF_VAR_TEMPLATES(                                                         \
        template<typename _Type>                                                \
        constexpr bool has_##name##_v = has_##name<_Type>::value;               \
    )                                                                           \
                                                                                \
    template<typename _Type>                                                    \
    using name##_t = detected_t<internal::detect_##name, _Type>;


// #############################################################################
// I.   TYPE ALIAS DETECTORS
// #############################################################################
// Detectors for container member type aliases.
// DEPENDENCIES: Section 0 (detection idiom)
// EXTRACTABLE:  Yes - can be moved to stl_alias_traits.hpp
// #############################################################################

// =============================================================================
// I.A  C++11 Common Container Aliases
// =============================================================================
// Type aliases found in most/all standard containers since C++11.
// Containers: array, vector, deque, list, forward_list, set, map, etc.

// has_value_type / value_type_t
//   trait/alias: detects and extracts _Type::value_type.
D_STL_DEFINE_ALIAS_DETECTOR(value_type)

// has_size_type / size_type_t
//   trait/alias: detects and extracts _Type::size_type.
D_STL_DEFINE_ALIAS_DETECTOR(size_type)

// has_difference_type / difference_type_t
//   trait/alias: detects and extracts _Type::difference_type.
D_STL_DEFINE_ALIAS_DETECTOR(difference_type)

// has_reference / reference_t
//   trait/alias: detects and extracts _Type::reference.
D_STL_DEFINE_ALIAS_DETECTOR(reference)

// has_const_reference / const_reference_t
//   trait/alias: detects and extracts _Type::const_reference.
D_STL_DEFINE_ALIAS_DETECTOR(const_reference)

// has_pointer / pointer_t
//   trait/alias: detects and extracts _Type::pointer.
D_STL_DEFINE_ALIAS_DETECTOR(pointer)

// has_const_pointer / const_pointer_t
//   trait/alias: detects and extracts _Type::const_pointer.
D_STL_DEFINE_ALIAS_DETECTOR(const_pointer)

// has_allocator_type / allocator_type_t
//   trait/alias: detects and extracts _Type::allocator_type.
D_STL_DEFINE_ALIAS_DETECTOR(allocator_type)


// =============================================================================
// I.B  C++11 Iterator Aliases
// =============================================================================
// Iterator type aliases for container traversal.
// Note: forward_list lacks reverse iterators.

// has_iterator / iterator_t
//   trait/alias: detects and extracts _Type::iterator.
D_STL_DEFINE_ALIAS_DETECTOR(iterator)

// has_const_iterator / const_iterator_t
//   trait/alias: detects and extracts _Type::const_iterator.
D_STL_DEFINE_ALIAS_DETECTOR(const_iterator)

// has_reverse_iterator / reverse_iterator_t
//   trait/alias: detects and extracts _Type::reverse_iterator.
D_STL_DEFINE_ALIAS_DETECTOR(reverse_iterator)

// has_const_reverse_iterator / const_reverse_iterator_t
//   trait/alias: detects and extracts _Type::const_reverse_iterator.
D_STL_DEFINE_ALIAS_DETECTOR(const_reverse_iterator)

// has_local_iterator / local_iterator_t
//   trait/alias: detects and extracts _Type::local_iterator (unordered containers).
D_STL_DEFINE_ALIAS_DETECTOR(local_iterator)

// has_const_local_iterator / const_local_iterator_t
//   trait/alias: detects and extracts _Type::const_local_iterator (unordered containers).
D_STL_DEFINE_ALIAS_DETECTOR(const_local_iterator)


// =============================================================================
// I.C  C++11 Associative Container Aliases
// =============================================================================
// Type aliases specific to associative containers.

// has_key_type / key_type_t
//   trait/alias: detects and extracts _Type::key_type.
D_STL_DEFINE_ALIAS_DETECTOR(key_type)

// has_mapped_type / mapped_type_t
//   trait/alias: detects and extracts _Type::mapped_type (map, unordered_map only).
D_STL_DEFINE_ALIAS_DETECTOR(mapped_type)

// has_key_compare / key_compare_t
//   trait/alias: detects and extracts _Type::key_compare (ordered containers).
D_STL_DEFINE_ALIAS_DETECTOR(key_compare)

// has_value_compare / value_compare_t
//   trait/alias: detects and extracts _Type::value_compare (ordered containers).
D_STL_DEFINE_ALIAS_DETECTOR(value_compare)

// has_hasher / hasher_t
//   trait/alias: detects and extracts _Type::hasher (unordered containers).
D_STL_DEFINE_ALIAS_DETECTOR(hasher)

// has_key_equal / key_equal_t
//   trait/alias: detects and extracts _Type::key_equal (unordered containers).
D_STL_DEFINE_ALIAS_DETECTOR(key_equal)


// =============================================================================
// I.D  C++11 Container Adaptor Aliases
// =============================================================================
// Type aliases for container adaptors (stack, queue, priority_queue).

// has_container_type / container_type_t
//   trait/alias: detects and extracts _Type::container_type.
D_STL_DEFINE_ALIAS_DETECTOR(container_type)


// =============================================================================
// I.E  C++11 String Aliases
// =============================================================================
// Type aliases specific to string types.

// has_traits_type / traits_type_t
//   trait/alias: detects and extracts _Type::traits_type.
D_STL_DEFINE_ALIAS_DETECTOR(traits_type)

NS_INTERNAL

    // detect_npos
    //   detector: checks for static member _Type::npos.
    template<typename _Type>
    using detect_npos = decltype(_Type::npos);

NS_END  // internal

// has_npos
//   trait: checks if `_Type` has a static npos member.
template<typename _Type>
struct has_npos : is_detected<internal::detect_npos, _Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_npos_v
    //   variable template: value of has_npos<_Type>.
    template<typename _Type>
    constexpr bool has_npos_v = has_npos<_Type>::value;
#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// I.F  C++11 Smart Pointer Aliases
// =============================================================================
// Type aliases for smart pointers.

// has_element_type / element_type_t
//   trait/alias: detects and extracts _Type::element_type.
D_STL_DEFINE_ALIAS_DETECTOR(element_type)

// has_deleter_type / deleter_type_t
//   trait/alias: detects and extracts _Type::deleter_type.
D_STL_DEFINE_ALIAS_DETECTOR(deleter_type)


// =============================================================================
// I.G  C++11 Allocator Aliases
// =============================================================================
// Allocator-related type aliases and rebind detection.

NS_INTERNAL

    // detect_allocator_rebind
    //   detector: checks for _Allocator::rebind<_Other>::other.
    template<typename _Allocator,
             typename _Other>
    using detect_allocator_rebind = typename _Allocator::template rebind<_Other>::other;

    // detect_propagate_on_container_copy_assignment
    //   detector: checks for propagate_on_container_copy_assignment typedef.
    template<typename _Type>
    using detect_propagate_on_container_copy_assignment =
        typename _Type::propagate_on_container_copy_assignment;

    // detect_propagate_on_container_move_assignment
    //   detector: checks for propagate_on_container_move_assignment typedef.
    template<typename _Type>
    using detect_propagate_on_container_move_assignment =
        typename _Type::propagate_on_container_move_assignment;

    // detect_propagate_on_container_swap
    //   detector: checks for propagate_on_container_swap typedef.
    template<typename _Type>
    using detect_propagate_on_container_swap =
        typename _Type::propagate_on_container_swap;

    // detect_is_always_equal
    //   detector: checks for is_always_equal typedef.
    template<typename _Type>
    using detect_is_always_equal = typename _Type::is_always_equal;

NS_END  // internal
// has_allocator_rebind
//   trait: checks if _Allocator has rebind<_Other>::other.
template<typename _Allocator,
         typename _Other = int>
struct has_allocator_rebind
    : is_detected<internal::detect_allocator_rebind, _Allocator, _Other>
{};

// has_propagate_on_container_copy_assignment
//   trait: checks for propagate_on_container_copy_assignment typedef.
template<typename _Type>
struct has_propagate_on_container_copy_assignment
    : is_detected<internal::detect_propagate_on_container_copy_assignment, _Type>
{};

// has_propagate_on_container_move_assignment
//   trait: checks for propagate_on_container_move_assignment typedef.
template<typename _Type>
struct has_propagate_on_container_move_assignment
    : is_detected<internal::detect_propagate_on_container_move_assignment, _Type>
{};

// has_propagate_on_container_swap
//   trait: checks for propagate_on_container_swap typedef.
template<typename _Type>
struct has_propagate_on_container_swap
    : is_detected<internal::detect_propagate_on_container_swap, _Type>
{};

// has_is_always_equal
//   trait: checks for is_always_equal typedef (C++17 allocator trait).
template<typename _Type>
struct has_is_always_equal
    : is_detected<internal::detect_is_always_equal, _Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_allocator_rebind_v
    //   variable template: value of has_allocator_rebind<_Allocator, _Other>.
    template<typename _Allocator,
             typename _Other = int>
    constexpr bool has_allocator_rebind_v =
        has_allocator_rebind<_Allocator, _Other>::value;

    // has_propagate_on_container_copy_assignment_v
    //   variable template: value of has_propagate_on_container_copy_assignment<_Type>.
    template<typename _Type>
    constexpr bool has_propagate_on_container_copy_assignment_v =
        has_propagate_on_container_copy_assignment<_Type>::value;

    // has_propagate_on_container_move_assignment_v
    //   variable template: value of has_propagate_on_container_move_assignment<_Type>.
    template<typename _Type>
    constexpr bool has_propagate_on_container_move_assignment_v =
        has_propagate_on_container_move_assignment<_Type>::value;

    // has_propagate_on_container_swap_v
    //   variable template: value of has_propagate_on_container_swap<_Type>.
    template<typename _Type>
    constexpr bool has_propagate_on_container_swap_v =
        has_propagate_on_container_swap<_Type>::value;

    // has_is_always_equal_v
    //   variable template: value of has_is_always_equal<_Type>.
    template<typename _Type>
    constexpr bool has_is_always_equal_v = has_is_always_equal<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// I.H  C++11 Iterator Traits Aliases
// =============================================================================
// Iterator classification aliases.

// has_iterator_category / iterator_category_t
//   trait/alias: detects and extracts _Type::iterator_category.
D_STL_DEFINE_ALIAS_DETECTOR(iterator_category)


// =============================================================================
// I.I  C++17 Node Handle Aliases
// =============================================================================
// Node extraction support (C++17).

#if D_ENV_LANG_IS_CPP17_OR_HIGHER

// has_node_type / node_type_t
    //   trait/alias: detects and extracts _Type::node_type (C++17).
    D_STL_DEFINE_ALIAS_DETECTOR(node_type)

    // has_insert_return_type / insert_return_type_t
    //   trait/alias: detects and extracts _Type::insert_return_type (C++17).
    D_STL_DEFINE_ALIAS_DETECTOR(insert_return_type)

    #else  // pre-C++17 stubs

    // has_node_type
    //   trait: stub for pre-C++17, always false.
    template<typename _Type>
    struct has_node_type : std::false_type
    {};

    // has_insert_return_type
    //   trait: stub for pre-C++17, always false.
    template<typename _Type>
    struct has_insert_return_type : std::false_type
    {};

    #if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
        template<typename _Type>
        constexpr bool has_node_type_v = false;

        template<typename _Type>
        constexpr bool has_insert_return_type_v = false;
    #endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

#endif  // D_ENV_LANG_IS_CPP17_OR_HIGHER


// =============================================================================
// I.J  C++17 Smart Pointer Aliases
// =============================================================================

#if D_ENV_LANG_IS_CPP17_OR_HIGHER

    // has_weak_type / weak_type_t
    //   trait/alias: detects and extracts _Type::weak_type (C++17 shared_ptr).
    D_STL_DEFINE_ALIAS_DETECTOR(weak_type)

#else  // pre-C++17 stub

    // has_weak_type
    //   trait: stub for pre-C++17, always false.
    template<typename _Type>
    struct has_weak_type : std::false_type
    {};

    #if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
        template<typename _Type>
        constexpr bool has_weak_type_v = false;
    #endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

#endif  // D_ENV_LANG_IS_CPP17_OR_HIGHER


// =============================================================================
// I.K  C++20 Iterator Concept Aliases
// =============================================================================

#if D_ENV_LANG_IS_CPP20_OR_HIGHER

    // has_iterator_concept / iterator_concept_t
    //   trait/alias: detects and extracts _Type::iterator_concept (C++20).
    D_STL_DEFINE_ALIAS_DETECTOR(iterator_concept)

#else  // pre-C++20 stub

    // has_iterator_concept
    //   trait: stub for pre-C++20, always false.
    template<typename _Type>
    struct has_iterator_concept : std::false_type
    {};

    #if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
        template<typename _Type>
        constexpr bool has_iterator_concept_v = false;
    #endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

#endif  // D_ENV_LANG_IS_CPP20_OR_HIGHER


// =============================================================================
// I.L  C++20/23 Span/MDSpan Aliases
// =============================================================================

#if D_ENV_LANG_IS_CPP20_OR_HIGHER
    // has_index_type / index_type_t
    //   trait/alias: detects and extracts _Type::index_type (C++20 span/mdspan).
    D_STL_DEFINE_ALIAS_DETECTOR(index_type)

    #else  // pre-C++20 stub

    // has_index_type
    //   trait: stub for pre-C++20, always false.
    template<typename _Type>
    struct has_index_type : std::false_type
    {};

    #if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
        template<typename _Type>
        constexpr bool has_index_type_v = false;
    #endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

#endif  // D_ENV_LANG_IS_CPP20_OR_HIGHER

#if D_ENV_LANG_IS_CPP23_OR_HIGHER

    // has_rank_type / rank_type_t
    //   trait/alias: detects and extracts _Type::rank_type (C++23 mdspan).
    D_STL_DEFINE_ALIAS_DETECTOR(rank_type)

    // has_extents_type / extents_type_t
    //   trait/alias: detects and extracts _Type::extents_type (C++23 mdspan).
    D_STL_DEFINE_ALIAS_DETECTOR(extents_type)

    // has_layout_type / layout_type_t
    //   trait/alias: detects and extracts _Type::layout_type (C++23 mdspan).
    D_STL_DEFINE_ALIAS_DETECTOR(layout_type)

    // has_accessor_type / accessor_type_t
    //   trait/alias: detects and extracts _Type::accessor_type (C++23 mdspan).
    D_STL_DEFINE_ALIAS_DETECTOR(accessor_type)

    // has_mapping_type / mapping_type_t
    //   trait/alias: detects and extracts _Type::mapping_type (C++23 mdspan).
    D_STL_DEFINE_ALIAS_DETECTOR(mapping_type)

    // has_data_handle_type / data_handle_type_t
    //   trait/alias: detects and extracts _Type::data_handle_type (C++23 mdspan).
    D_STL_DEFINE_ALIAS_DETECTOR(data_handle_type)

    #else  // pre-C++23 stubs

    // has_rank_type
    //   trait: stub for pre-C++23, always false.
    template<typename _Type>
    struct has_rank_type : std::false_type
    {};

    // has_extents_type
    //   trait: stub for pre-C++23, always false.
    template<typename _Type>
    struct has_extents_type : std::false_type
    {};

    // has_layout_type
    //   trait: stub for pre-C++23, always false.
    template<typename _Type>
    struct has_layout_type : std::false_type
    {};

    // has_accessor_type
    //   trait: stub for pre-C++23, always false.
    template<typename _Type>
    struct has_accessor_type : std::false_type
    {};

    // has_mapping_type
    //   trait: stub for pre-C++23, always false.
    template<typename _Type>
    struct has_mapping_type : std::false_type
    {};

    // has_data_handle_type
    //   trait: stub for pre-C++23, always false.
    template<typename _Type>
    struct has_data_handle_type : std::false_type
    {};

    #if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
        template<typename _Type>
        constexpr bool has_rank_type_v = false;

        template<typename _Type>
        constexpr bool has_extents_type_v = false;

        template<typename _Type>
        constexpr bool has_layout_type_v = false;

        template<typename _Type>
        constexpr bool has_accessor_type_v = false;

        template<typename _Type>
        constexpr bool has_mapping_type_v = false;

        template<typename _Type>
        constexpr bool has_data_handle_type_v = false;
    #endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

#endif  // D_ENV_LANG_IS_CPP23_OR_HIGHER


// #############################################################################
// II.  METHOD DETECTORS
// #############################################################################
// Detectors for container member functions.
// DEPENDENCIES: Section 0 (detection idiom), Section I (for size_type, etc.)
// EXTRACTABLE:  Yes - can be moved to stl_method_traits.hpp
// #############################################################################

// =============================================================================
// II.A  C++11 Element Access Methods
// =============================================================================
// Methods for accessing container elements.

NS_INTERNAL

    // detect_at
    //   detector: checks for _Type::at(size_type) non-const.
    template<typename _Type>
    using detect_at = decltype(std::declval<_Type&>().at(
        std::declval<typename _Type::size_type>()));

    // detect_at_const
    //   detector: checks for _Type::at(size_type) const.
    template<typename _Type>
    using detect_at_const = decltype(std::declval<const _Type&>().at(
        std::declval<typename _Type::size_type>()));

    // detect_subscript
    //   detector: checks for _Type::operator[](size_type) non-const.
    template<typename _Type>
    using detect_subscript = decltype(std::declval<_Type&>()[
        std::declval<typename _Type::size_type>()]);

    // detect_subscript_const
    //   detector: checks for _Type::operator[](size_type) const.
    template<typename _Type>
    using detect_subscript_const = decltype(std::declval<const _Type&>()[
        std::declval<typename _Type::size_type>()]);

    // detect_front
    //   detector: checks for _Type::front() non-const.
    template<typename _Type>
    using detect_front = decltype(std::declval<_Type&>().front());

    // detect_front_const
    //   detector: checks for _Type::front() const.
    template<typename _Type>
    using detect_front_const = decltype(std::declval<const _Type&>().front());

    // detect_back
    //   detector: checks for _Type::back() non-const.
    template<typename _Type>
    using detect_back = decltype(std::declval<_Type&>().back());

    // detect_back_const
    //   detector: checks for _Type::back() const.
    template<typename _Type>
    using detect_back_const = decltype(std::declval<const _Type&>().back());

    // detect_data
    //   detector: checks for _Type::data() non-const.
    template<typename _Type>
    using detect_data = decltype(std::declval<_Type&>().data());

    // detect_data_const
    //   detector: checks for _Type::data() const.
    template<typename _Type>
    using detect_data_const = decltype(std::declval<const _Type&>().data());

NS_END  // internal
// has_at
//   trait: checks if `_Type` has at(size_type) with const overload.
template<typename _Type>
struct has_at : std::integral_constant<bool,
    ( is_detected<internal::detect_at, _Type>::value &&
      is_detected<internal::detect_at_const, _Type>::value )>
{};

// has_subscript_operator
//   trait: checks if `_Type` has operator[](size_type) with const overload.
template<typename _Type>
struct has_subscript_operator : std::integral_constant<bool,
    ( is_detected<internal::detect_subscript, _Type>::value &&
      is_detected<internal::detect_subscript_const, _Type>::value )>
{};

// has_front
//   trait: checks if `_Type` has front() with const overload.
template<typename _Type>
struct has_front : std::integral_constant<bool,
    ( is_detected<internal::detect_front, _Type>::value &&
      is_detected<internal::detect_front_const, _Type>::value )>
{};

// has_back
//   trait: checks if `_Type` has back() with const overload.
template<typename _Type>
struct has_back : std::integral_constant<bool,
    ( is_detected<internal::detect_back, _Type>::value &&
      is_detected<internal::detect_back_const, _Type>::value )>
{};

// has_data
//   trait: checks if `_Type` has data() with const overload.
template<typename _Type>
struct has_data : std::integral_constant<bool,
    ( is_detected<internal::detect_data, _Type>::value &&
      is_detected<internal::detect_data_const, _Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_at_v
    //   variable template: value of has_at<_Type>.
    template<typename _Type>
    constexpr bool has_at_v = has_at<_Type>::value;

    // has_subscript_operator_v
    //   variable template: value of has_subscript_operator<_Type>.
    template<typename _Type>
    constexpr bool has_subscript_operator_v = has_subscript_operator<_Type>::value;

    // has_front_v
    //   variable template: value of has_front<_Type>.
    template<typename _Type>
    constexpr bool has_front_v = has_front<_Type>::value;

    // has_back_v
    //   variable template: value of has_back<_Type>.
    template<typename _Type>
    constexpr bool has_back_v = has_back<_Type>::value;

    // has_data_v
    //   variable template: value of has_data<_Type>.
    template<typename _Type>
    constexpr bool has_data_v = has_data<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// II.B  C++11 Iterator Methods
// =============================================================================
// Methods returning iterators.

NS_INTERNAL

    // detect_begin
    //   detector: checks for _Type::begin() non-const.
    template<typename _Type>
    using detect_begin = decltype(std::declval<_Type&>().begin());

    // detect_begin_const
    //   detector: checks for _Type::begin() const.
    template<typename _Type>
    using detect_begin_const = decltype(std::declval<const _Type&>().begin());

    // detect_end
    //   detector: checks for _Type::end() non-const.
    template<typename _Type>
    using detect_end = decltype(std::declval<_Type&>().end());

    // detect_end_const
    //   detector: checks for _Type::end() const.
    template<typename _Type>
    using detect_end_const = decltype(std::declval<const _Type&>().end());

    // detect_cbegin
    //   detector: checks for _Type::cbegin() const.
    template<typename _Type>
    using detect_cbegin = decltype(std::declval<const _Type&>().cbegin());

    // detect_cend
    //   detector: checks for _Type::cend() const.
    template<typename _Type>
    using detect_cend = decltype(std::declval<const _Type&>().cend());

    // detect_rbegin
    //   detector: checks for _Type::rbegin() non-const.
    template<typename _Type>
    using detect_rbegin = decltype(std::declval<_Type&>().rbegin());

    // detect_rbegin_const
    //   detector: checks for _Type::rbegin() const.
    template<typename _Type>
    using detect_rbegin_const = decltype(std::declval<const _Type&>().rbegin());

    // detect_rend
    //   detector: checks for _Type::rend() non-const.
    template<typename _Type>
    using detect_rend = decltype(std::declval<_Type&>().rend());

    // detect_rend_const
    //   detector: checks for _Type::rend() const.
    template<typename _Type>
    using detect_rend_const = decltype(std::declval<const _Type&>().rend());

    // detect_crbegin
    //   detector: checks for _Type::crbegin() const.
    template<typename _Type>
    using detect_crbegin = decltype(std::declval<const _Type&>().crbegin());

    // detect_crend
    //   detector: checks for _Type::crend() const.
    template<typename _Type>
    using detect_crend = decltype(std::declval<const _Type&>().crend());

    // detect_before_begin (forward_list specific)
    //   detector: checks for _Type::before_begin() non-const.
    template<typename _Type>
    using detect_before_begin = decltype(std::declval<_Type&>().before_begin());

    // detect_before_begin_const (forward_list specific)
    //   detector: checks for _Type::before_begin() const.
    template<typename _Type>
    using detect_before_begin_const = decltype(std::declval<const _Type&>().before_begin());

    // detect_cbefore_begin (forward_list specific)
    //   detector: checks for _Type::cbefore_begin() const.
    template<typename _Type>
    using detect_cbefore_begin = decltype(std::declval<const _Type&>().cbefore_begin());

NS_END  // internal

// has_begin_end
//   trait: checks if `_Type` has begin()/end() with const overloads.
template<typename _Type>
struct has_begin_end : std::integral_constant<bool,
    ( is_detected<internal::detect_begin, _Type>::value       &&
      is_detected<internal::detect_begin_const, _Type>::value &&
      is_detected<internal::detect_end, _Type>::value         &&
      is_detected<internal::detect_end_const, _Type>::value )>
{};

// has_cbegin_cend
//   trait: checks if `_Type` has cbegin()/cend() const methods.
template<typename _Type>
struct has_cbegin_cend : std::integral_constant<bool,
    ( is_detected<internal::detect_cbegin, _Type>::value &&
      is_detected<internal::detect_cend, _Type>::value )>
{};

// has_rbegin_rend
//   trait: checks if `_Type` has rbegin()/rend() with const overloads.
template<typename _Type>
struct has_rbegin_rend : std::integral_constant<bool,
    ( is_detected<internal::detect_rbegin, _Type>::value       &&
      is_detected<internal::detect_rbegin_const, _Type>::value &&
      is_detected<internal::detect_rend, _Type>::value         &&
      is_detected<internal::detect_rend_const, _Type>::value )>
{};

// has_crbegin_crend
//   trait: checks if `_Type` has crbegin()/crend() const methods.
template<typename _Type>
struct has_crbegin_crend : std::integral_constant<bool,
    ( is_detected<internal::detect_crbegin, _Type>::value &&
      is_detected<internal::detect_crend, _Type>::value )>
{};

// has_before_begin
//   trait: checks if `_Type` has before_begin() (forward_list specific).
template<typename _Type>
struct has_before_begin : std::integral_constant<bool,
    ( is_detected<internal::detect_before_begin, _Type>::value       &&
      is_detected<internal::detect_before_begin_const, _Type>::value &&
      is_detected<internal::detect_cbefore_begin, _Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

// has_begin_end_v
//   variable template: value of has_begin_end<_Type>.
template<typename _Type>
constexpr bool has_begin_end_v = has_begin_end<_Type>::value;

// has_cbegin_cend_v
//   variable template: value of has_cbegin_cend<_Type>.
template<typename _Type>
constexpr bool has_cbegin_cend_v = has_cbegin_cend<_Type>::value;

// has_rbegin_rend_v
//   variable template: value of has_rbegin_rend<_Type>.
template<typename _Type>
constexpr bool has_rbegin_rend_v = has_rbegin_rend<_Type>::value;

// has_crbegin_crend_v
//   variable template: value of has_crbegin_crend<_Type>.
template<typename _Type>
constexpr bool has_crbegin_crend_v = has_crbegin_crend<_Type>::value;

// has_before_begin_v
//   variable template: value of has_before_begin<_Type>.
template<typename _Type>
constexpr bool has_before_begin_v = has_before_begin<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// =============================================================================
// II.C  C++11 Capacity Methods
// =============================================================================
// Methods for container size/capacity queries.

NS_INTERNAL

    // detect_empty
    //   detector: checks for _Type::empty() const.
    template<typename _Type>
    using detect_empty = decltype(std::declval<const _Type&>().empty());

    // detect_size
    //   detector: checks for _Type::size() const.
    template<typename _Type>
    using detect_size = decltype(std::declval<const _Type&>().size());

    // detect_max_size
    //   detector: checks for _Type::max_size() const.
    template<typename _Type>
    using detect_max_size = decltype(std::declval<const _Type&>().max_size());

    // detect_capacity
    //   detector: checks for _Type::capacity() const.
    template<typename _Type>
    using detect_capacity = decltype(std::declval<const _Type&>().capacity());

    // detect_reserve
    //   detector: checks for _Type::reserve(size_type).
    template<typename _Type>
    using detect_reserve = decltype(std::declval<_Type&>().reserve(
        std::declval<typename _Type::size_type>()));

    // detect_shrink_to_fit
    //   detector: checks for _Type::shrink_to_fit().
    template<typename _Type>
    using detect_shrink_to_fit = decltype(std::declval<_Type&>().shrink_to_fit());

NS_END  // internal

// has_empty
//   trait: checks if `_Type` has empty() const method.
template<typename _Type>
struct has_empty : is_detected<internal::detect_empty, _Type>
{};

// has_size
//   trait: checks if `_Type` has size() const method.
template<typename _Type>
struct has_size : is_detected<internal::detect_size, _Type>
{};

// has_max_size
//   trait: checks if `_Type` has max_size() const method.
template<typename _Type>
struct has_max_size : is_detected<internal::detect_max_size, _Type>
{};

// has_capacity
//   trait: checks if `_Type` has capacity() const method.
template<typename _Type>
struct has_capacity : is_detected<internal::detect_capacity, _Type>
{};

// has_reserve
//   trait: checks if `_Type` has reserve(size_type) method.
template<typename _Type>
struct has_reserve : is_detected<internal::detect_reserve, _Type>
{};

// has_shrink_to_fit
//   trait: checks if `_Type` has shrink_to_fit() method.
template<typename _Type>
struct has_shrink_to_fit : is_detected<internal::detect_shrink_to_fit, _Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_empty_v
    //   variable template: value of has_empty<_Type>.
    template<typename _Type>
    constexpr bool has_empty_v = has_empty<_Type>::value;

    // has_size_v
    //   variable template: value of has_size<_Type>.
    template<typename _Type>
    constexpr bool has_size_v = has_size<_Type>::value;

    // has_max_size_v
    //   variable template: value of has_max_size<_Type>.
    template<typename _Type>
    constexpr bool has_max_size_v = has_max_size<_Type>::value;

    // has_capacity_v
    //   variable template: value of has_capacity<_Type>.
    template<typename _Type>
    constexpr bool has_capacity_v = has_capacity<_Type>::value;

    // has_reserve_v
    //   variable template: value of has_reserve<_Type>.
    template<typename _Type>
    constexpr bool has_reserve_v = has_reserve<_Type>::value;

    // has_shrink_to_fit_v
    //   variable template: value of has_shrink_to_fit<_Type>.
    template<typename _Type>
    constexpr bool has_shrink_to_fit_v = has_shrink_to_fit<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// II.D  C++11 Modifier Methods
// =============================================================================
// Methods that modify container contents.

NS_INTERNAL

    // detect_fill
    //   detector: checks for _Type::fill(const value_type&) (array-specific).
    template<typename _Type>
    using detect_fill = decltype(std::declval<_Type&>().fill(
        std::declval<const typename _Type::value_type&>()));

    // detect_swap
    //   detector: checks for _Type::swap(_Type&).
    template<typename _Type>
    using detect_swap = decltype(std::declval<_Type&>().swap(std::declval<_Type&>()));

    // detect_clear
    //   detector: checks for _Type::clear().
    template<typename _Type>
    using detect_clear = decltype(std::declval<_Type&>().clear());

    // detect_insert
    //   detector: checks for _Type::insert(const_iterator, const value_type&).
    template<typename _Type>
    using detect_insert = decltype(std::declval<_Type&>().insert(
        std::declval<typename _Type::const_iterator>(),
        std::declval<const typename _Type::value_type&>()));

    // detect_emplace
    //   detector: checks for _Type::emplace(const_iterator, Args...).
    template<typename _Type>
    using detect_emplace = decltype(std::declval<_Type&>().emplace(
        std::declval<typename _Type::const_iterator>()));

    // detect_erase
    //   detector: checks for _Type::erase(const_iterator).
    template<typename _Type>
    using detect_erase = decltype(std::declval<_Type&>().erase(
        std::declval<typename _Type::const_iterator>()));

    // detect_push_back
    //   detector: checks for _Type::push_back(const value_type&).
    template<typename _Type>
    using detect_push_back = decltype(std::declval<_Type&>().push_back(
        std::declval<const typename _Type::value_type&>()));

    // detect_emplace_back
    //   detector: checks for _Type::emplace_back().
    template<typename _Type>
    using detect_emplace_back = decltype(std::declval<_Type&>().emplace_back());

    // detect_pop_back
    //   detector: checks for _Type::pop_back().
    template<typename _Type>
    using detect_pop_back = decltype(std::declval<_Type&>().pop_back());

    // detect_push_front
    //   detector: checks for _Type::push_front(const value_type&).
    template<typename _Type>
    using detect_push_front = decltype(std::declval<_Type&>().push_front(
        std::declval<const typename _Type::value_type&>()));

    // detect_emplace_front
    //   detector: checks for _Type::emplace_front().
    template<typename _Type>
    using detect_emplace_front = decltype(std::declval<_Type&>().emplace_front());

    // detect_pop_front
    //   detector: checks for _Type::pop_front().
    template<typename _Type>
    using detect_pop_front = decltype(std::declval<_Type&>().pop_front());

    // detect_resize
    //   detector: checks for _Type::resize(size_type).
    template<typename _Type>
    using detect_resize = decltype(std::declval<_Type&>().resize(
        std::declval<typename _Type::size_type>()));

    // detect_assign
    //   detector: checks for _Type::assign(size_type, const value_type&).
    template<typename _Type>
    using detect_assign = decltype(std::declval<_Type&>().assign(
        std::declval<typename _Type::size_type>(),
        std::declval<const typename _Type::value_type&>()));

NS_END  // internal

// has_fill
//   trait: checks if `_Type` has fill(const value_type&) method.
template<typename _Type>
struct has_fill : is_detected<internal::detect_fill, _Type>
{};

// has_swap
//   trait: checks if `_Type` has swap(_Type&) method.
template<typename _Type>
struct has_swap : is_detected<internal::detect_swap, _Type>
{};

// has_clear
//   trait: checks if `_Type` has clear() method.
template<typename _Type>
struct has_clear : is_detected<internal::detect_clear, _Type>
{};

// has_insert
//   trait: checks if `_Type` has insert(const_iterator, const value_type&).
template<typename _Type>
struct has_insert : is_detected<internal::detect_insert, _Type>
{};

// has_emplace
//   trait: checks if `_Type` has emplace(const_iterator, Args...).
template<typename _Type>
struct has_emplace : is_detected<internal::detect_emplace, _Type>
{};

// has_erase
//   trait: checks if `_Type` has erase(const_iterator).
template<typename _Type>
struct has_erase : is_detected<internal::detect_erase, _Type>
{};

// has_push_back
//   trait: checks if `_Type` has push_back(const value_type&) method.
template<typename _Type>
struct has_push_back : is_detected<internal::detect_push_back, _Type>
{};

// has_emplace_back
//   trait: checks if `_Type` has emplace_back() method.
template<typename _Type>
struct has_emplace_back : is_detected<internal::detect_emplace_back, _Type>
{};

// has_pop_back
//   trait: checks if `_Type` has pop_back() method.
template<typename _Type>
struct has_pop_back : is_detected<internal::detect_pop_back, _Type>
{};

// has_push_front
//   trait: checks if `_Type` has push_front(const value_type&) method.
template<typename _Type>
struct has_push_front : is_detected<internal::detect_push_front, _Type>
{};

// has_emplace_front
//   trait: checks if `_Type` has emplace_front() method.
template<typename _Type>
struct has_emplace_front : is_detected<internal::detect_emplace_front, _Type>
{};

// has_pop_front
//   trait: checks if `_Type` has pop_front() method.
template<typename _Type>
struct has_pop_front : is_detected<internal::detect_pop_front, _Type>
{};

// has_resize
//   trait: checks if `_Type` has resize(size_type) method.
template<typename _Type>
struct has_resize : is_detected<internal::detect_resize, _Type>
{};

// has_assign
//   trait: checks if `_Type` has assign(size_type, const value_type&) method.
template<typename _Type>
struct has_assign : is_detected<internal::detect_assign, _Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_fill_v
    //   variable template: value of has_fill<_Type>.
    template<typename _Type>
    constexpr bool has_fill_v = has_fill<_Type>::value;

    // has_swap_v
    //   variable template: value of has_swap<_Type>.
    template<typename _Type>
    constexpr bool has_swap_v = has_swap<_Type>::value;

    // has_clear_v
    //   variable template: value of has_clear<_Type>.
    template<typename _Type>
    constexpr bool has_clear_v = has_clear<_Type>::value;

    // has_insert_v
    //   variable template: value of has_insert<_Type>.
    template<typename _Type>
    constexpr bool has_insert_v = has_insert<_Type>::value;

    // has_emplace_v
    //   variable template: value of has_emplace<_Type>.
    template<typename _Type>
    constexpr bool has_emplace_v = has_emplace<_Type>::value;

    // has_erase_v
    //   variable template: value of has_erase<_Type>.
    template<typename _Type>
    constexpr bool has_erase_v = has_erase<_Type>::value;

    // has_push_back_v
    //   variable template: value of has_push_back<_Type>.
    template<typename _Type>
    constexpr bool has_push_back_v = has_push_back<_Type>::value;

    // has_emplace_back_v
    //   variable template: value of has_emplace_back<_Type>.
    template<typename _Type>
    constexpr bool has_emplace_back_v = has_emplace_back<_Type>::value;

    // has_pop_back_v
    //   variable template: value of has_pop_back<_Type>.
    template<typename _Type>
    constexpr bool has_pop_back_v = has_pop_back<_Type>::value;

    // has_push_front_v
    //   variable template: value of has_push_front<_Type>.
    template<typename _Type>
    constexpr bool has_push_front_v = has_push_front<_Type>::value;

    // has_emplace_front_v
    //   variable template: value of has_emplace_front<_Type>.
    template<typename _Type>
    constexpr bool has_emplace_front_v = has_emplace_front<_Type>::value;

    // has_pop_front_v
    //   variable template: value of has_pop_front<_Type>.
    template<typename _Type>
    constexpr bool has_pop_front_v = has_pop_front<_Type>::value;

    // has_resize_v
    //   variable template: value of has_resize<_Type>.
    template<typename _Type>
    constexpr bool has_resize_v = has_resize<_Type>::value;

    // has_assign_v
    //   variable template: value of has_assign<_Type>.
    template<typename _Type>
    constexpr bool has_assign_v = has_assign<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// II.E  C++11 List/Forward_List Specific Methods
// =============================================================================
// Methods specific to list and forward_list containers.

NS_INTERNAL
    // detect_splice (list)
    //   detector: checks for _Type::splice(const_iterator, _Type&).
    template<typename _Type>
    using detect_splice = decltype(std::declval<_Type&>().splice(
        std::declval<typename _Type::const_iterator>(),
        std::declval<_Type&>()));

    // detect_splice_after (forward_list)
    //   detector: checks for _Type::splice_after(const_iterator, _Type&).
    template<typename _Type>
    using detect_splice_after = decltype(std::declval<_Type&>().splice_after(
        std::declval<typename _Type::const_iterator>(),
        std::declval<_Type&>()));

    // detect_remove
    //   detector: checks for _Type::remove(const value_type&).
    template<typename _Type>
    using detect_remove = decltype(std::declval<_Type&>().remove(
        std::declval<const typename _Type::value_type&>()));

    // detect_remove_if
    //   detector: checks for _Type::remove_if(Predicate).
    template<typename _Type>
    using detect_remove_if = decltype(std::declval<_Type&>().remove_if(
        std::declval<bool(*)(const typename _Type::value_type&)>()));

    // detect_unique
    //   detector: checks for _Type::unique().
    template<typename _Type>
    using detect_unique = decltype(std::declval<_Type&>().unique());

    // detect_merge
    //   detector: checks for _Type::merge(_Type&).
    template<typename _Type>
    using detect_merge = decltype(std::declval<_Type&>().merge(std::declval<_Type&>()));

    // detect_sort
    //   detector: checks for _Type::sort().
    template<typename _Type>
    using detect_sort = decltype(std::declval<_Type&>().sort());

    // detect_reverse
    //   detector: checks for _Type::reverse().
    template<typename _Type>
    using detect_reverse = decltype(std::declval<_Type&>().reverse());

    // detect_insert_after (forward_list)
    //   detector: checks for _Type::insert_after(const_iterator, const value_type&).
    template<typename _Type>
    using detect_insert_after = decltype(std::declval<_Type&>().insert_after(
        std::declval<typename _Type::const_iterator>(),
        std::declval<const typename _Type::value_type&>()));

    // detect_emplace_after (forward_list)
    //   detector: checks for _Type::emplace_after(const_iterator).
    template<typename _Type>
    using detect_emplace_after = decltype(std::declval<_Type&>().emplace_after(
        std::declval<typename _Type::const_iterator>()));

    // detect_erase_after (forward_list)
    //   detector: checks for _Type::erase_after(const_iterator).
    template<typename _Type>
    using detect_erase_after = decltype(std::declval<_Type&>().erase_after(
        std::declval<typename _Type::const_iterator>()));

NS_END  // internal

// has_splice
//   trait: checks if `_Type` has splice(const_iterator, _Type&) (list).
template<typename _Type>
struct has_splice : is_detected<internal::detect_splice, _Type>
{};

// has_splice_after
//   trait: checks if `_Type` has splice_after (forward_list).
template<typename _Type>
struct has_splice_after : is_detected<internal::detect_splice_after, _Type>
{};

// has_remove
//   trait: checks if `_Type` has remove(const value_type&).
template<typename _Type>
struct has_remove : is_detected<internal::detect_remove, _Type>
{};

// has_remove_if
//   trait: checks if `_Type` has remove_if(Predicate).
template<typename _Type>
struct has_remove_if : is_detected<internal::detect_remove_if, _Type>
{};

// has_unique
//   trait: checks if `_Type` has unique().
template<typename _Type>
struct has_unique : is_detected<internal::detect_unique, _Type>
{};

// has_merge
//   trait: checks if `_Type` has merge(_Type&).
template<typename _Type>
struct has_merge : is_detected<internal::detect_merge, _Type>
{};

// has_sort
//   trait: checks if `_Type` has sort().
template<typename _Type>
struct has_sort : is_detected<internal::detect_sort, _Type>
{};

// has_reverse
//   trait: checks if `_Type` has reverse().
template<typename _Type>
struct has_reverse : is_detected<internal::detect_reverse, _Type>
{};

// has_insert_after
//   trait: checks if `_Type` has insert_after (forward_list).
template<typename _Type>
struct has_insert_after : is_detected<internal::detect_insert_after, _Type>
{};

// has_emplace_after
//   trait: checks if `_Type` has emplace_after (forward_list).
template<typename _Type>
struct has_emplace_after : is_detected<internal::detect_emplace_after, _Type>
{};

// has_erase_after
//   trait: checks if `_Type` has erase_after (forward_list).
template<typename _Type>
struct has_erase_after : is_detected<internal::detect_erase_after, _Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_splice_v
    //   variable template: value of has_splice<_Type>.
    template<typename _Type>
    constexpr bool has_splice_v = has_splice<_Type>::value;

    // has_splice_after_v
    //   variable template: value of has_splice_after<_Type>.
    template<typename _Type>
    constexpr bool has_splice_after_v = has_splice_after<_Type>::value;

    // has_remove_v
    //   variable template: value of has_remove<_Type>.
    template<typename _Type>
    constexpr bool has_remove_v = has_remove<_Type>::value;

    // has_remove_if_v
    //   variable template: value of has_remove_if<_Type>.
    template<typename _Type>
    constexpr bool has_remove_if_v = has_remove_if<_Type>::value;

    // has_unique_v
    //   variable template: value of has_unique<_Type>.
    template<typename _Type>
    constexpr bool has_unique_v = has_unique<_Type>::value;

    // has_merge_v
    //   variable template: value of has_merge<_Type>.
    template<typename _Type>
    constexpr bool has_merge_v = has_merge<_Type>::value;

    // has_sort_v
    //   variable template: value of has_sort<_Type>.
    template<typename _Type>
    constexpr bool has_sort_v = has_sort<_Type>::value;

    // has_reverse_v
    //   variable template: value of has_reverse<_Type>.
    template<typename _Type>
    constexpr bool has_reverse_v = has_reverse<_Type>::value;

    // has_insert_after_v
    //   variable template: value of has_insert_after<_Type>.
    template<typename _Type>
    constexpr bool has_insert_after_v = has_insert_after<_Type>::value;

    // has_emplace_after_v
    //   variable template: value of has_emplace_after<_Type>.
    template<typename _Type>
    constexpr bool has_emplace_after_v = has_emplace_after<_Type>::value;

    // has_erase_after_v
    //   variable template: value of has_erase_after<_Type>.
    template<typename _Type>
    constexpr bool has_erase_after_v = has_erase_after<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// =============================================================================
// II.F  C++11 Associative Container Methods
// =============================================================================
// Methods specific to associative containers (set, map, multiset, multimap).

NS_INTERNAL

    // detect_find
    //   detector: checks for _Type::find(const key_type&).
    template<typename _Type>
    using detect_find = decltype(std::declval<_Type&>().find(
        std::declval<const typename _Type::key_type&>()));

    // detect_count
    //   detector: checks for _Type::count(const key_type&).
    template<typename _Type>
    using detect_count = decltype(std::declval<const _Type&>().count(
        std::declval<const typename _Type::key_type&>()));

    // detect_lower_bound
    //   detector: checks for _Type::lower_bound(const key_type&).
    template<typename _Type>
    using detect_lower_bound = decltype(std::declval<_Type&>().lower_bound(
        std::declval<const typename _Type::key_type&>()));

    // detect_upper_bound
    //   detector: checks for _Type::upper_bound(const key_type&).
    template<typename _Type>
    using detect_upper_bound = decltype(std::declval<_Type&>().upper_bound(
        std::declval<const typename _Type::key_type&>()));

    // detect_equal_range
    //   detector: checks for _Type::equal_range(const key_type&).
    template<typename _Type>
    using detect_equal_range = decltype(std::declval<_Type&>().equal_range(
        std::declval<const typename _Type::key_type&>()));

    // detect_key_comp
    //   detector: checks for _Type::key_comp().
    template<typename _Type>
    using detect_key_comp = decltype(std::declval<const _Type&>().key_comp());

    // detect_value_comp
    //   detector: checks for _Type::value_comp().
    template<typename _Type>
    using detect_value_comp = decltype(std::declval<const _Type&>().value_comp());

    // detect_extract_key (C++17)
    //   detector: checks for _Type::extract(const key_type&).
    template<typename _Type>
    using detect_extract_key = decltype(std::declval<_Type&>().extract(
        std::declval<const typename _Type::key_type&>()));

NS_END  // internal

// has_find
//   trait: checks if `_Type` has find(const key_type&).
template<typename _Type>
struct has_find : is_detected<internal::detect_find, _Type>
{};

// has_count
//   trait: checks if `_Type` has count(const key_type&).
template<typename _Type>
struct has_count : is_detected<internal::detect_count, _Type>
{};

// has_lower_bound
//   trait: checks if `_Type` has lower_bound(const key_type&).
template<typename _Type>
struct has_lower_bound : is_detected<internal::detect_lower_bound, _Type>
{};

// has_upper_bound
//   trait: checks if `_Type` has upper_bound(const key_type&).
template<typename _Type>
struct has_upper_bound : is_detected<internal::detect_upper_bound, _Type>
{};

// has_equal_range
//   trait: checks if `_Type` has equal_range(const key_type&).
template<typename _Type>
struct has_equal_range : is_detected<internal::detect_equal_range, _Type>
{
};

// has_key_comp
//   trait: checks if `_Type` has key_comp() (ordered associative).
template<typename _Type>
struct has_key_comp : is_detected<internal::detect_key_comp, _Type>
{};

// has_value_comp
//   trait: checks if `_Type` has value_comp() (ordered associative).
template<typename _Type>
struct has_value_comp : is_detected<internal::detect_value_comp, _Type>
{};

// has_extract
//   trait: checks if `_Type` has extract(const key_type&) (C++17).
template<typename _Type>
struct has_extract : is_detected<internal::detect_extract_key, _Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_find_v
    //   variable template: value of has_find<_Type>.
    template<typename _Type>
    constexpr bool has_find_v = has_find<_Type>::value;

    // has_count_v
    //   variable template: value of has_count<_Type>.
    template<typename _Type>
    constexpr bool has_count_v = has_count<_Type>::value;

    // has_lower_bound_v
    //   variable template: value of has_lower_bound<_Type>.
    template<typename _Type>
    constexpr bool has_lower_bound_v = has_lower_bound<_Type>::value;

    // has_upper_bound_v
    //   variable template: value of has_upper_bound<_Type>.
    template<typename _Type>
    constexpr bool has_upper_bound_v = has_upper_bound<_Type>::value;

    // has_equal_range_v
    //   variable template: value of has_equal_range<_Type>.
    template<typename _Type>
    constexpr bool has_equal_range_v = has_equal_range<_Type>::value;

    // has_key_comp_v
    //   variable template: value of has_key_comp<_Type>.
    template<typename _Type>
    constexpr bool has_key_comp_v = has_key_comp<_Type>::value;

    // has_value_comp_v
    //   variable template: value of has_value_comp<_Type>.
    template<typename _Type>
    constexpr bool has_value_comp_v = has_value_comp<_Type>::value;

    // has_extract_v
    //   variable template: value of has_extract<_Type>.
    template<typename _Type>
    constexpr bool has_extract_v = has_extract<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// II.G  C++11 Unordered Container Methods
// =============================================================================
// Methods specific to unordered containers.

NS_INTERNAL

    // detect_bucket_count
    //   detector: checks for _Type::bucket_count().
    template<typename _Type>
    using detect_bucket_count = decltype(std::declval<const _Type&>().bucket_count());

    // detect_max_bucket_count
    //   detector: checks for _Type::max_bucket_count().
    template<typename _Type>
    using detect_max_bucket_count = decltype(std::declval<const _Type&>().max_bucket_count());

    // detect_bucket_size
    //   detector: checks for _Type::bucket_size(size_type).
    template<typename _Type>
    using detect_bucket_size = decltype(std::declval<const _Type&>().bucket_size(
        std::declval<typename _Type::size_type>()));

    // detect_bucket
    //   detector: checks for _Type::bucket(const key_type&).
    template<typename _Type>
    using detect_bucket = decltype(std::declval<const _Type&>().bucket(
        std::declval<const typename _Type::key_type&>()));

    // detect_load_factor
    //   detector: checks for _Type::load_factor().
    template<typename _Type>
    using detect_load_factor = decltype(std::declval<const _Type&>().load_factor());

    // detect_max_load_factor
    //   detector: checks for _Type::max_load_factor().
    template<typename _Type>
    using detect_max_load_factor = decltype(std::declval<const _Type&>().max_load_factor());

    // detect_rehash
    //   detector: checks for _Type::rehash(size_type).
    template<typename _Type>
    using detect_rehash = decltype(std::declval<_Type&>().rehash(
        std::declval<typename _Type::size_type>()));

    // detect_hash_function
    //   detector: checks for _Type::hash_function().
    template<typename _Type>
    using detect_hash_function = decltype(std::declval<const _Type&>().hash_function());

    // detect_key_eq
    //   detector: checks for _Type::key_eq().
    template<typename _Type>
    using detect_key_eq = decltype(std::declval<const _Type&>().key_eq());

NS_END  // internal

// has_bucket_count
//   trait: checks if `_Type` has bucket_count() (unordered containers).
template<typename _Type>
struct has_bucket_count : is_detected<internal::detect_bucket_count, _Type>
{};

// has_max_bucket_count
//   trait: checks if `_Type` has max_bucket_count() (unordered containers).
template<typename _Type>
struct has_max_bucket_count : is_detected<internal::detect_max_bucket_count, _Type>
{};

// has_bucket_size
//   trait: checks if `_Type` has bucket_size(size_type) (unordered containers).
template<typename _Type>
struct has_bucket_size : is_detected<internal::detect_bucket_size, _Type>
{};

// has_bucket
//   trait: checks if `_Type` has bucket(const key_type&) (unordered containers).
template<typename _Type>
struct has_bucket : is_detected<internal::detect_bucket, _Type>
{};

// has_load_factor
//   trait: checks if `_Type` has load_factor() (unordered containers).
template<typename _Type>
struct has_load_factor : is_detected<internal::detect_load_factor, _Type>
{};

// has_max_load_factor
//   trait: checks if `_Type` has max_load_factor() (unordered containers).
template<typename _Type>
struct has_max_load_factor : is_detected<internal::detect_max_load_factor, _Type>
{};

// has_rehash
//   trait: checks if `_Type` has rehash(size_type) (unordered containers).
template<typename _Type>
struct has_rehash : is_detected<internal::detect_rehash, _Type>
{};

// has_hash_function
//   trait: checks if `_Type` has hash_function() (unordered containers).
template<typename _Type>
struct has_hash_function : is_detected<internal::detect_hash_function, _Type>
{};

// has_key_eq
//   trait: checks if `_Type` has key_eq() (unordered containers).
template<typename _Type>
struct has_key_eq : is_detected<internal::detect_key_eq, _Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_bucket_count_v
    //   variable template: value of has_bucket_count<_Type>.
    template<typename _Type>
    constexpr bool has_bucket_count_v = has_bucket_count<_Type>::value;

    // has_max_bucket_count_v
    //   variable template: value of has_max_bucket_count<_Type>.
    template<typename _Type>
    constexpr bool has_max_bucket_count_v = has_max_bucket_count<_Type>::value;

    // has_bucket_size_v
    //   variable template: value of has_bucket_size<_Type>.
    template<typename _Type>
    constexpr bool has_bucket_size_v = has_bucket_size<_Type>::value;

    // has_bucket_v
    //   variable template: value of has_bucket<_Type>.
    template<typename _Type>
    constexpr bool has_bucket_v = has_bucket<_Type>::value;

    // has_load_factor_v
    //   variable template: value of has_load_factor<_Type>.
    template<typename _Type>
    constexpr bool has_load_factor_v = has_load_factor<_Type>::value;

    // has_max_load_factor_v
    //   variable template: value of has_max_load_factor<_Type>.
    template<typename _Type>
    constexpr bool has_max_load_factor_v = has_max_load_factor<_Type>::value;

    // has_rehash_v
    //   variable template: value of has_rehash<_Type>.
    template<typename _Type>
    constexpr bool has_rehash_v = has_rehash<_Type>::value;

    // has_hash_function_v
    //   variable template: value of has_hash_function<_Type>.
    template<typename _Type>
    constexpr bool has_hash_function_v = has_hash_function<_Type>::value;

    // has_key_eq_v
    //   variable template: value of has_key_eq<_Type>.
    template<typename _Type>
    constexpr bool has_key_eq_v = has_key_eq<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// #############################################################################
// III. OPERATOR DETECTORS
// #############################################################################
// Detectors for operators supported by types.
// DEPENDENCIES: Section 0 (detection idiom)
// EXTRACTABLE:  Yes - can be moved to stl_operator_traits.hpp
// #############################################################################

// =============================================================================
// III.A  C++11 Comparison Operators
// =============================================================================
// Equality and relational comparison operators.

NS_INTERNAL

    // detect_equality_operator
    //   detector: checks for operator==(T, T).
    template<typename _Type>
    using detect_equality_operator = decltype(
        std::declval<const _Type&>() == std::declval<const _Type&>());

    // detect_inequality_operator
    //   detector: checks for operator!=(T, T).
    template<typename _Type>
    using detect_inequality_operator = decltype(
        std::declval<const _Type&>() != std::declval<const _Type&>());

    // detect_less_operator
    //   detector: checks for operator<(T, T).
    template<typename _Type>
    using detect_less_operator = decltype(
        std::declval<const _Type&>() < std::declval<const _Type&>());

    // detect_less_equal_operator
    //   detector: checks for operator<=(T, T).
    template<typename _Type>
    using detect_less_equal_operator = decltype(
        std::declval<const _Type&>() <= std::declval<const _Type&>());

    // detect_greater_operator
    //   detector: checks for operator>(T, T).
    template<typename _Type>
    using detect_greater_operator = decltype(
        std::declval<const _Type&>() > std::declval<const _Type&>());

    // detect_greater_equal_operator
    //   detector: checks for operator>=(T, T).
    template<typename _Type>
    using detect_greater_equal_operator = decltype(
        std::declval<const _Type&>() >= std::declval<const _Type&>());

NS_END  // internal

// has_equality_operator
//   trait: checks if `_Type` supports operator==.
template<typename _Type>
struct has_equality_operator : is_detected<internal::detect_equality_operator, _Type>
{};

// has_inequality_operator
//   trait: checks if `_Type` supports operator!=.
template<typename _Type>
struct has_inequality_operator : is_detected<internal::detect_inequality_operator, _Type>
{};

// has_less_operator
//   trait: checks if `_Type` supports operator<.
template<typename _Type>
struct has_less_operator : is_detected<internal::detect_less_operator, _Type>
{};

// has_less_equal_operator
//   trait: checks if `_Type` supports operator<=.
template<typename _Type>
struct has_less_equal_operator : is_detected<internal::detect_less_equal_operator, _Type>
{};

// has_greater_operator
//   trait: checks if `_Type` supports operator>.
template<typename _Type>
struct has_greater_operator : is_detected<internal::detect_greater_operator, _Type>
{};

// has_greater_equal_operator
//   trait: checks if `_Type` supports operator>=.
template<typename _Type>
struct has_greater_equal_operator : is_detected<internal::detect_greater_equal_operator, _Type>
{};

// has_equality_operators
//   trait: checks if `_Type` supports both == and !=.
template<typename _Type>
struct has_equality_operators : std::integral_constant<bool,
    ( has_equality_operator<_Type>::value &&
      has_inequality_operator<_Type>::value )
>
{};

// has_relational_operators
//   trait: checks if `_Type` supports <, <=, >, >=.
template<typename _Type>
struct has_relational_operators : std::integral_constant<bool,
    ( has_less_operator<_Type>::value         &&
      has_less_equal_operator<_Type>::value   &&
      has_greater_operator<_Type>::value      &&
      has_greater_equal_operator<_Type>::value )
>
{};

// has_all_comparison_operators
//   trait: checks if `_Type` supports all six comparison operators.
template<typename _Type>
struct has_all_comparison_operators : std::integral_constant<bool,
    ( has_equality_operators<_Type>::value &&
      has_relational_operators<_Type>::value )
>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_equality_operator_v
    //   variable template: value of has_equality_operator<_Type>.
    template<typename _Type>
    constexpr bool has_equality_operator_v = has_equality_operator<_Type>::value;

    // has_inequality_operator_v
    //   variable template: value of has_inequality_operator<_Type>.
    template<typename _Type>
    constexpr bool has_inequality_operator_v = has_inequality_operator<_Type>::value;

    // has_less_operator_v
    //   variable template: value of has_less_operator<_Type>.
    template<typename _Type>
    constexpr bool has_less_operator_v = has_less_operator<_Type>::value;

    // has_less_equal_operator_v
    //   variable template: value of has_less_equal_operator<_Type>.
    template<typename _Type>
    constexpr bool has_less_equal_operator_v = has_less_equal_operator<_Type>::value;

    // has_greater_operator_v
    //   variable template: value of has_greater_operator<_Type>.
    template<typename _Type>
    constexpr bool has_greater_operator_v = has_greater_operator<_Type>::value;

    // has_greater_equal_operator_v
    //   variable template: value of has_greater_equal_operator<_Type>.
    template<typename _Type>
    constexpr bool has_greater_equal_operator_v = has_greater_equal_operator<_Type>::value;

    // has_equality_operators_v
    //   variable template: value of has_equality_operators<_Type>.
    template<typename _Type>
    constexpr bool has_equality_operators_v = has_equality_operators<_Type>::value;

    // has_relational_operators_v
    //   variable template: value of has_relational_operators<_Type>.
    template<typename _Type>
    constexpr bool has_relational_operators_v = has_relational_operators<_Type>::value;

    // has_all_comparison_operators_v
    //   variable template: value of has_all_comparison_operators<_Type>.
    template<typename _Type>
    constexpr bool has_all_comparison_operators_v = has_all_comparison_operators<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// III.B  C++20 Three-Way Comparison
// =============================================================================
// Spaceship operator (operator<=>).

#if D_ENV_LANG_IS_CPP20_OR_HIGHER

    NS_INTERNAL

        // detect_spaceship_operator
        //   detector: checks for operator<=>(T, T).
        template<typename _Type>
        using detect_spaceship_operator = decltype(
            std::declval<const _Type&>() <=> std::declval<const _Type&>());

    NS_END  // internal

    // has_spaceship_operator
    //   trait: checks if `_Type` supports operator<=> (C++20).
    template<typename _Type>
    struct has_spaceship_operator : is_detected<internal::detect_spaceship_operator, _Type>
    {};

    #if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_spaceship_operator_v
    //   variable template: value of has_spaceship_operator<_Type>.
    template<typename _Type>
    constexpr bool has_spaceship_operator_v = has_spaceship_operator<_Type>::value;

    #endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

#else  // pre-C++20 stub
    // has_spaceship_operator
    //   trait: stub for pre-C++20, always false.
    template<typename _Type>
    struct has_spaceship_operator : std::false_type
    {};

    #if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

        // has_spaceship_operator_v
        //   variable template: value of has_spaceship_operator<_Type>.
        template<typename _Type>
        constexpr bool has_spaceship_operator_v = false;

    #endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

#endif  // D_ENV_LANG_IS_CPP20_OR_HIGHER


// =============================================================================
// III.C  C++11 Arithmetic Operators
// =============================================================================
// Arithmetic operators (+, -, *, /, %).

NS_INTERNAL
    // detect_plus_operator
    //   detector: checks for operator+(T, T).
    template<typename _Type>
    using detect_plus_operator = decltype(
        std::declval<const _Type&>() + std::declval<const _Type&>()
    );

    // detect_minus_operator
    //   detector: checks for operator-(T, T).
    template<typename _Type>
    using detect_minus_operator = decltype(
        std::declval<const _Type&>() - std::declval<const _Type&>()
    );

    // detect_multiply_operator
    //   detector: checks for operator*(T, T).
    template<typename _Type>
    using detect_multiply_operator = decltype(
        std::declval<const _Type&>() * std::declval<const _Type&>()
    );

    // detect_divide_operator
    //   detector: checks for operator/(T, T).
    template<typename _Type>
    using detect_divide_operator = decltype(
        std::declval<const _Type&>() / std::declval<const _Type&>()
    );

    // detect_modulo_operator
    //   detector: checks for operator%(T, T).
    template<typename _Type>
    using detect_modulo_operator = decltype(
        std::declval<const _Type&>() % std::declval<const _Type&>()
    );

    // detect_negate_operator
    //   detector: checks for unary operator-(T).
    template<typename _Type>
    using detect_negate_operator = decltype(-std::declval<const _Type&>());

    // detect_positive_operator
    //   detector: checks for unary operator+(T).
    template<typename _Type>
    using detect_positive_operator = decltype(+std::declval<const _Type&>());

NS_END  // internal

// has_plus_operator
//   trait: checks if `_Type` supports binary operator+.
template<typename _Type>
struct has_plus_operator : is_detected<internal::detect_plus_operator, _Type>
{};

// has_minus_operator
//   trait: checks if `_Type` supports binary operator-.
template<typename _Type>
struct has_minus_operator : is_detected<internal::detect_minus_operator, _Type>
{};

// has_multiply_operator
//   trait: checks if `_Type` supports operator*.
template<typename _Type>
struct has_multiply_operator : is_detected<internal::detect_multiply_operator, _Type>
{};

// has_divide_operator
//   trait: checks if `_Type` supports operator/.
template<typename _Type>
struct has_divide_operator : is_detected<internal::detect_divide_operator, _Type>
{};

// has_modulo_operator
//   trait: checks if `_Type` supports operator%.
template<typename _Type>
struct has_modulo_operator : is_detected<internal::detect_modulo_operator, _Type>
{};

// has_negate_operator
//   trait: checks if `_Type` supports unary operator-.
template<typename _Type>
struct has_negate_operator : is_detected<internal::detect_negate_operator, _Type>
{};

// has_positive_operator
//   trait: checks if `_Type` supports unary operator+.
template<typename _Type>
struct has_positive_operator : is_detected<internal::detect_positive_operator, _Type>
{};

// has_arithmetic_operators
//   trait: checks if `_Type` supports +, -, *, /.
template<typename _Type>
struct has_arithmetic_operators : std::integral_constant<bool,
    ( has_plus_operator<_Type>::value     &&
      has_minus_operator<_Type>::value    &&
      has_multiply_operator<_Type>::value &&
      has_divide_operator<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_plus_operator_v
    //   variable template: value of has_plus_operator<_Type>.
    template<typename _Type>
    constexpr bool has_plus_operator_v = has_plus_operator<_Type>::value;

    // has_minus_operator_v
    //   variable template: value of has_minus_operator<_Type>.
    template<typename _Type>
    constexpr bool has_minus_operator_v = has_minus_operator<_Type>::value;

    // has_multiply_operator_v
    //   variable template: value of has_multiply_operator<_Type>.
    template<typename _Type>
    constexpr bool has_multiply_operator_v = has_multiply_operator<_Type>::value;

    // has_divide_operator_v
    //   variable template: value of has_divide_operator<_Type>.
    template<typename _Type>
    constexpr bool has_divide_operator_v = has_divide_operator<_Type>::value;

    // has_modulo_operator_v
    //   variable template: value of has_modulo_operator<_Type>.
    template<typename _Type>
    constexpr bool has_modulo_operator_v = has_modulo_operator<_Type>::value;

    // has_negate_operator_v
    //   variable template: value of has_negate_operator<_Type>.
    template<typename _Type>
    constexpr bool has_negate_operator_v = has_negate_operator<_Type>::value;

    // has_positive_operator_v
    //   variable template: value of has_positive_operator<_Type>.
    template<typename _Type>
    constexpr bool has_positive_operator_v = has_positive_operator<_Type>::value;

    // has_arithmetic_operators_v
    //   variable template: value of has_arithmetic_operators<_Type>.
    template<typename _Type>
    constexpr bool has_arithmetic_operators_v = has_arithmetic_operators<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// III.D  C++11 Increment/Decrement Operators
// =============================================================================
// Prefix and postfix increment/decrement.

NS_INTERNAL

    // detect_pre_increment_operator
    //   detector: checks for prefix operator++.
    template<typename _Type>
    using detect_pre_increment_operator = decltype(++std::declval<_Type&>());

    // detect_post_increment_operator
    //   detector: checks for postfix operator++.
    template<typename _Type>
    using detect_post_increment_operator = decltype(std::declval<_Type&>()++);

    // detect_pre_decrement_operator
    //   detector: checks for prefix operator--.
    template<typename _Type>
    using detect_pre_decrement_operator = decltype(--std::declval<_Type&>());

    // detect_post_decrement_operator
    //   detector: checks for postfix operator--.
    template<typename _Type>
    using detect_post_decrement_operator = decltype(std::declval<_Type&>()--);

NS_END  // internal

// has_pre_increment_operator
//   trait: checks if `_Type` supports prefix ++.
template<typename _Type>
struct has_pre_increment_operator : is_detected<internal::detect_pre_increment_operator, _Type>
{};

// has_post_increment_operator
//   trait: checks if `_Type` supports postfix ++.
template<typename _Type>
struct has_post_increment_operator : is_detected<internal::detect_post_increment_operator, _Type>
{};

// has_pre_decrement_operator
//   trait: checks if `_Type` supports prefix --.
template<typename _Type>
struct has_pre_decrement_operator : is_detected<internal::detect_pre_decrement_operator, _Type>
{};

// has_post_decrement_operator
//   trait: checks if `_Type` supports postfix --.
template<typename _Type>
struct has_post_decrement_operator : is_detected<internal::detect_post_decrement_operator, _Type>
{};

// has_increment_operators
//   trait: checks if `_Type` supports both prefix and postfix ++.
template<typename _Type>
struct has_increment_operators : std::integral_constant<bool,
    ( has_pre_increment_operator<_Type>::value &&
      has_post_increment_operator<_Type>::value )>
{};

// has_decrement_operators
//   trait: checks if `_Type` supports both prefix and postfix --.
template<typename _Type>
struct has_decrement_operators : std::integral_constant<bool,
    ( has_pre_decrement_operator<_Type>::value &&
      has_post_decrement_operator<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_pre_increment_operator_v
    //   variable template: value of has_pre_increment_operator<_Type>.
    template<typename _Type>
    constexpr bool has_pre_increment_operator_v = has_pre_increment_operator<_Type>::value;

    // has_post_increment_operator_v
    //   variable template: value of has_post_increment_operator<_Type>.
    template<typename _Type>
    constexpr bool has_post_increment_operator_v = has_post_increment_operator<_Type>::value;

    // has_pre_decrement_operator_v
    //   variable template: value of has_pre_decrement_operator<_Type>.
    template<typename _Type>
    constexpr bool has_pre_decrement_operator_v = has_pre_decrement_operator<_Type>::value;

    // has_post_decrement_operator_v
    //   variable template: value of has_post_decrement_operator<_Type>.
    template<typename _Type>
    constexpr bool has_post_decrement_operator_v = has_post_decrement_operator<_Type>::value;

    // has_increment_operators_v
    //   variable template: value of has_increment_operators<_Type>.
    template<typename _Type>
    constexpr bool has_increment_operators_v = has_increment_operators<_Type>::value;

    // has_decrement_operators_v
    //   variable template: value of has_decrement_operators<_Type>.
    template<typename _Type>
    constexpr bool has_decrement_operators_v = has_decrement_operators<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// III.E  C++11 Logical Operators
// =============================================================================
// Logical operators (&&, ||, !).

NS_INTERNAL

    // detect_logical_and_operator
    //   detector: checks for operator&&(T, T).
    template<typename _Type>
    using detect_logical_and_operator = decltype(
        std::declval<const _Type&>() && std::declval<const _Type&>());

    // detect_logical_or_operator
    //   detector: checks for operator||(T, T).
    template<typename _Type>
    using detect_logical_or_operator = decltype(
        std::declval<const _Type&>() || std::declval<const _Type&>());

    // detect_logical_not_operator
    //   detector: checks for operator!(T).
    template<typename _Type>
    using detect_logical_not_operator = decltype(!std::declval<const _Type&>());

NS_END  // internal

// has_logical_and_operator
//   trait: checks if `_Type` supports operator&&.
template<typename _Type>
struct has_logical_and_operator : is_detected<internal::detect_logical_and_operator, _Type>
{};

// has_logical_or_operator
//   trait: checks if `_Type` supports operator||.
template<typename _Type>
struct has_logical_or_operator : is_detected<internal::detect_logical_or_operator, _Type>
{};

// has_logical_not_operator
//   trait: checks if `_Type` supports operator!.
template<typename _Type>
struct has_logical_not_operator : is_detected<internal::detect_logical_not_operator, _Type>
{};

// has_logical_operators
//   trait: checks if `_Type` supports &&, ||, !.
template<typename _Type>
struct has_logical_operators : std::integral_constant<bool,
    ( has_logical_and_operator<_Type>::value &&
      has_logical_or_operator<_Type>::value  &&
      has_logical_not_operator<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_logical_and_operator_v
    //   variable template: value of has_logical_and_operator<_Type>.
    template<typename _Type>
    constexpr bool has_logical_and_operator_v = has_logical_and_operator<_Type>::value;

    // has_logical_or_operator_v
    //   variable template: value of has_logical_or_operator<_Type>.
    template<typename _Type>
    constexpr bool has_logical_or_operator_v = has_logical_or_operator<_Type>::value;

    // has_logical_not_operator_v
    //   variable template: value of has_logical_not_operator<_Type>.
    template<typename _Type>
    constexpr bool has_logical_not_operator_v = has_logical_not_operator<_Type>::value;

    // has_logical_operators_v
    //   variable template: value of has_logical_operators<_Type>.
    template<typename _Type>
    constexpr bool has_logical_operators_v = has_logical_operators<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// =============================================================================
// III.F  C++11 Bitwise Operators
// =============================================================================
// Bitwise operators (&, |, ^, ~, <<, >>).

NS_INTERNAL

    // detect_bitwise_and_operator
    //   detector: checks for operator&(T, T).
    template<typename _Type>
    using detect_bitwise_and_operator = decltype(
        std::declval<const _Type&>() & std::declval<const _Type&>());

    // detect_bitwise_or_operator
    //   detector: checks for operator|(T, T).
    template<typename _Type>
    using detect_bitwise_or_operator = decltype(
        std::declval<const _Type&>() | std::declval<const _Type&>());

    // detect_bitwise_xor_operator
    //   detector: checks for operator^(T, T).
    template<typename _Type>
    using detect_bitwise_xor_operator = decltype(
        std::declval<const _Type&>() ^ std::declval<const _Type&>());

    // detect_bitwise_not_operator
    //   detector: checks for operator~(T).
    template<typename _Type>
    using detect_bitwise_not_operator = decltype(~std::declval<const _Type&>());

    // detect_left_shift_operator
    //   detector: checks for operator<<(T, int).
    template<typename _Type>
    using detect_left_shift_operator = decltype(
        std::declval<const _Type&>() << std::declval<int>());

    // detect_right_shift_operator
    //   detector: checks for operator>>(T, int).
    template<typename _Type>
    using detect_right_shift_operator = decltype(
        std::declval<const _Type&>() >> std::declval<int>());

NS_END  // internal

// has_bitwise_and_operator
//   trait: checks if `_Type` supports operator&.
template<typename _Type>
struct has_bitwise_and_operator : is_detected<internal::detect_bitwise_and_operator, _Type>
{};

// has_bitwise_or_operator
//   trait: checks if `_Type` supports operator|.
template<typename _Type>
struct has_bitwise_or_operator : is_detected<internal::detect_bitwise_or_operator, _Type>
{};

// has_bitwise_xor_operator
//   trait: checks if `_Type` supports operator^.
template<typename _Type>
struct has_bitwise_xor_operator : is_detected<internal::detect_bitwise_xor_operator, _Type>
{};

// has_bitwise_not_operator
//   trait: checks if `_Type` supports operator~.
template<typename _Type>
struct has_bitwise_not_operator : is_detected<internal::detect_bitwise_not_operator, _Type>
{};

// has_left_shift_operator
//   trait: checks if `_Type` supports operator<<.
template<typename _Type>
struct has_left_shift_operator : is_detected<internal::detect_left_shift_operator, _Type>
{};

// has_right_shift_operator
//   trait: checks if `_Type` supports operator>>.
template<typename _Type>
struct has_right_shift_operator : is_detected<internal::detect_right_shift_operator, _Type>
{};

// has_bitwise_operators
//   trait: checks if `_Type` supports &, |, ^, ~.
template<typename _Type>
struct has_bitwise_operators : std::integral_constant<bool,
    ( has_bitwise_and_operator<_Type>::value &&
      has_bitwise_or_operator<_Type>::value  &&
      has_bitwise_xor_operator<_Type>::value &&
      has_bitwise_not_operator<_Type>::value )>
{};

// has_shift_operators
//   trait: checks if `_Type` supports << and >>.
template<typename _Type>
struct has_shift_operators : std::integral_constant<bool,
    ( has_left_shift_operator<_Type>::value &&
      has_right_shift_operator<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_bitwise_and_operator_v
    //   variable template: value of has_bitwise_and_operator<_Type>.
    template<typename _Type>
    constexpr bool has_bitwise_and_operator_v = has_bitwise_and_operator<_Type>::value;

    // has_bitwise_or_operator_v
    //   variable template: value of has_bitwise_or_operator<_Type>.
    template<typename _Type>
    constexpr bool has_bitwise_or_operator_v = has_bitwise_or_operator<_Type>::value;

    // has_bitwise_xor_operator_v
    //   variable template: value of has_bitwise_xor_operator<_Type>.
    template<typename _Type>
    constexpr bool has_bitwise_xor_operator_v = has_bitwise_xor_operator<_Type>::value;

    // has_bitwise_not_operator_v
    //   variable template: value of has_bitwise_not_operator<_Type>.
    template<typename _Type>
    constexpr bool has_bitwise_not_operator_v = has_bitwise_not_operator<_Type>::value;

    // has_left_shift_operator_v
    //   variable template: value of has_left_shift_operator<_Type>.
    template<typename _Type>
    constexpr bool has_left_shift_operator_v = has_left_shift_operator<_Type>::value;

    // has_right_shift_operator_v
    //   variable template: value of has_right_shift_operator<_Type>.
    template<typename _Type>
    constexpr bool has_right_shift_operator_v = has_right_shift_operator<_Type>::value;

    // has_bitwise_operators_v
    //   variable template: value of has_bitwise_operators<_Type>.
    template<typename _Type>
    constexpr bool has_bitwise_operators_v = has_bitwise_operators<_Type>::value;

    // has_shift_operators_v
    //   variable template: value of has_shift_operators<_Type>.
    template<typename _Type>
    constexpr bool has_shift_operators_v = has_shift_operators<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// =============================================================================
// III.G  C++11 Access Operators
// =============================================================================
// Dereference and member access operators.

NS_INTERNAL

    // detect_dereference_operator
    //   detector: checks for operator*(T).
    template<typename _Type>
    using detect_dereference_operator = decltype(*std::declval<_Type&>());

    // detect_arrow_operator
    //   detector: checks for operator->(T).
    template<typename _Type>
    using detect_arrow_operator = decltype(std::declval<_Type&>().operator->());

    // detect_call_operator
    //   detector: checks for operator()(T).
    template<typename _Type>
    using detect_call_operator = decltype(std::declval<_Type&>()());

    // detect_subscript_generic
    //   detector: checks for operator[](Index) with generic index.
    template<typename _Type,
             typename _Index>
    using detect_subscript_generic = decltype(std::declval<_Type&>()[std::declval<_Index>()]);

NS_END  // internal

// has_dereference_operator
//   trait: checks if `_Type` supports operator*.
template<typename _Type>
struct has_dereference_operator : is_detected<internal::detect_dereference_operator, _Type>
{};

// has_arrow_operator
//   trait: checks if `_Type` supports operator->.
template<typename _Type>
struct has_arrow_operator : is_detected<internal::detect_arrow_operator, _Type>
{};

// has_call_operator
//   trait: checks if `_Type` supports operator() (callable).
template<typename _Type>
struct has_call_operator : is_detected<internal::detect_call_operator, _Type>
{};

// has_subscript_with
//   trait: checks if `_Type` supports operator[](_Index).
template<typename _Type,
         typename _Index>
struct has_subscript_with : is_detected<internal::detect_subscript_generic, _Type, _Index>
{};

// has_pointer_operators
//   trait: checks if `_Type` supports * and -> (pointer-like).
template<typename _Type>
struct has_pointer_operators : std::integral_constant<bool,
    ( has_dereference_operator<_Type>::value &&
      has_arrow_operator<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_dereference_operator_v
    //   variable template: value of has_dereference_operator<_Type>.
    template<typename _Type>
    constexpr bool has_dereference_operator_v = has_dereference_operator<_Type>::value;

    // has_arrow_operator_v
    //   variable template: value of has_arrow_operator<_Type>.
    template<typename _Type>
    constexpr bool has_arrow_operator_v = has_arrow_operator<_Type>::value;

    // has_call_operator_v
    //   variable template: value of has_call_operator<_Type>.
    template<typename _Type>
    constexpr bool has_call_operator_v = has_call_operator<_Type>::value;

    // has_subscript_with_v
    //   variable template: value of has_subscript_with<_Type, _Index>.
    template<typename _Type,
             typename _Index>
    constexpr bool has_subscript_with_v = has_subscript_with<_Type, _Index>::value;

    // has_pointer_operators_v
    //   variable template: value of has_pointer_operators<_Type>.
    template<typename _Type>
    constexpr bool has_pointer_operators_v = has_pointer_operators<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// =============================================================================
// III.H  C++11 Stream Operators
// =============================================================================
// Stream insertion and extraction operators.

NS_INTERNAL

    // detect_stream_insert_operator
    //   detector: checks for operator<<(ostream, T).
    template<typename _Type>
    using detect_stream_insert_operator = decltype(
        std::declval<std::ostream&>() << std::declval<const _Type&>());

    // detect_stream_extract_operator
    //   detector: checks for operator>>(istream, T).
    template<typename _Type>
    using detect_stream_extract_operator = decltype(
        std::declval<std::istream&>() >> std::declval<_Type&>());

NS_END  // internal

// has_stream_insert_operator
//   trait: checks if `_Type` supports operator<< with ostream.
template<typename _Type>
struct has_stream_insert_operator : is_detected<internal::detect_stream_insert_operator, _Type>
{};

// has_stream_extract_operator
//   trait: checks if `_Type` supports operator>> with istream.
template<typename _Type>
struct has_stream_extract_operator : is_detected<internal::detect_stream_extract_operator, _Type>
{};

// has_stream_operators
//   trait: checks if `_Type` supports both << and >> with streams.
template<typename _Type>
struct has_stream_operators : std::integral_constant<bool,
    ( has_stream_insert_operator<_Type>::value &&
      has_stream_extract_operator<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

    // has_stream_insert_operator_v
    //   variable template: value of has_stream_insert_operator<_Type>.
    template<typename _Type>
    constexpr bool has_stream_insert_operator_v = has_stream_insert_operator<_Type>::value;

    // has_stream_extract_operator_v
    //   variable template: value of has_stream_extract_operator<_Type>.
    template<typename _Type>
    constexpr bool has_stream_extract_operator_v = has_stream_extract_operator<_Type>::value;

    // has_stream_operators_v
    //   variable template: value of has_stream_operators<_Type>.
    template<typename _Type>
    constexpr bool has_stream_operators_v = has_stream_operators<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// #############################################################################
// IV.  RETURN TYPE VALIDATORS
// #############################################################################
// Validates that methods return the correct types.
// DEPENDENCIES: Sections 0, I, II
// EXTRACTABLE:  Yes - can be moved to stl_return_type_traits.hpp
// #############################################################################

NS_INTERNAL

    // at_returns_reference
    //   validator: checks if at() returns reference/const_reference.
    template<typename _Type,
             bool     = has_at<_Type>::value>
    struct at_returns_reference : std::false_type
    {};

    template<typename _Type>
    struct at_returns_reference<_Type, true> : std::integral_constant<bool,
        ( std::is_same<detected_t<detect_at, _Type>,
                       typename _Type::reference>::value &&
          std::is_same<detected_t<detect_at_const, _Type>,
                       typename _Type::const_reference>::value )>
    {};

    // subscript_returns_reference
    //   validator: checks if operator[] returns reference/const_reference.
    template<typename _Type,
             bool     = has_subscript_operator<_Type>::value>
    struct subscript_returns_reference : std::false_type
    {};

    template<typename _Type>
    struct subscript_returns_reference<_Type, true> : std::integral_constant<bool,
        ( std::is_same<detected_t<detect_subscript, _Type>,
                       typename _Type::reference>::value &&
          std::is_same<detected_t<detect_subscript_const, _Type>,
                       typename _Type::const_reference>::value )>
    {};

    // front_returns_reference
    //   validator: checks if front() returns reference/const_reference.
    template<typename _Type,
             bool     = has_front<_Type>::value>
    struct front_returns_reference : std::false_type
    {};

    template<typename _Type>
    struct front_returns_reference<_Type, true> : std::integral_constant<bool,
        ( std::is_same<detected_t<detect_front, _Type>,
                       typename _Type::reference>::value &&
          std::is_same<detected_t<detect_front_const, _Type>,
                       typename _Type::const_reference>::value )>
    {};

    // back_returns_reference
    //   validator: checks if back() returns reference/const_reference.
    template<typename _Type,
             bool     = has_back<_Type>::value>
    struct back_returns_reference : std::false_type
    {};

    template<typename _Type>
    struct back_returns_reference<_Type, true> : std::integral_constant<bool,
        ( std::is_same<detected_t<detect_back, _Type>,
                       typename _Type::reference>::value &&
          std::is_same<detected_t<detect_back_const, _Type>,
                       typename _Type::const_reference>::value )>
    {};

    // data_returns_pointer
    //   validator: checks if data() returns pointer/const_pointer.
    template<typename _Type,
             bool     = has_data<_Type>::value>
    struct data_returns_pointer : std::false_type
    {};

    template<typename _Type>
    struct data_returns_pointer<_Type, true> : std::integral_constant<bool,
        ( std::is_same<detected_t<detect_data, _Type>,
                       typename _Type::pointer>::value &&
          std::is_same<detected_t<detect_data_const, _Type>,
                       typename _Type::const_pointer>::value )>
    {};

    // begin_end_return_iterator
    //   validator: checks if begin/end return iterator/const_iterator.
    template<typename _Type,
             bool     = has_begin_end<_Type>::value>
    struct begin_end_return_iterator : std::false_type
    {};

    template<typename _Type>
    struct begin_end_return_iterator<_Type, true> : std::integral_constant<bool,
        ( std::is_same<detected_t<detect_begin, _Type>,
                       typename _Type::iterator>::value       &&
          std::is_same<detected_t<detect_begin_const, _Type>,
                       typename _Type::const_iterator>::value &&
          std::is_same<detected_t<detect_end, _Type>,
                       typename _Type::iterator>::value       &&
          std::is_same<detected_t<detect_end_const, _Type>,
                       typename _Type::const_iterator>::value )>
    {};

    // empty_returns_bool
    //   validator: checks if empty() returns bool-convertible type.
    template<typename _Type,
             bool     = has_empty<_Type>::value>
    struct empty_returns_bool : std::false_type
    {};

    template<typename _Type>
    struct empty_returns_bool<_Type, true>
        : std::is_convertible<detected_t<detect_empty, _Type>, bool>
    {};

    // size_returns_size_type
    //   validator: checks if size() returns size_type.
    template<typename _Type,
             bool     = has_size<_Type>::value>
    struct size_returns_size_type : std::false_type
    {};

    template<typename _Type>
    struct size_returns_size_type<_Type, true>
        : std::is_same<detected_t<detect_size, _Type>, typename _Type::size_type>
    {};

NS_END  // internal

// has_correct_element_access_return_types
//   trait: validates element access methods return correct types.
template<typename _Type>
struct has_correct_element_access_return_types : std::integral_constant<bool,
    ( internal::at_returns_reference<_Type>::value        &&
      internal::subscript_returns_reference<_Type>::value &&
      internal::front_returns_reference<_Type>::value     &&
      internal::back_returns_reference<_Type>::value      &&
      internal::data_returns_pointer<_Type>::value )>
{};

// has_correct_iterator_return_types
//   trait: validates iterator methods return correct types.
template<typename _Type>
struct has_correct_iterator_return_types : std::integral_constant<bool,
    ( internal::begin_end_return_iterator<_Type>::value )>
{};

// has_correct_capacity_return_types
//   trait: validates capacity methods return correct types.
template<typename _Type>
struct has_correct_capacity_return_types : std::integral_constant<bool,
    ( internal::empty_returns_bool<_Type>::value &&
      internal::size_returns_size_type<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_correct_element_access_return_types_v
    //   variable template: value of has_correct_element_access_return_types<_Type>.
    template<typename _Type>
    constexpr bool has_correct_element_access_return_types_v =
        has_correct_element_access_return_types<_Type>::value;

    // has_correct_iterator_return_types_v
    //   variable template: value of has_correct_iterator_return_types<_Type>.
    template<typename _Type>
    constexpr bool has_correct_iterator_return_types_v =
        has_correct_iterator_return_types<_Type>::value;

    // has_correct_capacity_return_types_v
    //   variable template: value of has_correct_capacity_return_types<_Type>.
    template<typename _Type>
    constexpr bool has_correct_capacity_return_types_v =
        has_correct_capacity_return_types<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// #############################################################################
// V.   TUPLE PROTOCOL DETECTORS
// #############################################################################
// Detectors for tuple-like interface (std::get, std::tuple_size, etc.)
// DEPENDENCIES: Section 0
// EXTRACTABLE:  Yes - can be moved to stl_tuple_protocol_traits.hpp
// #############################################################################

NS_INTERNAL

    // detect_tuple_size
    //   detector: checks for std::tuple_size<_Type>::value.
    template<typename _Type>
    using detect_tuple_size = decltype(std::tuple_size<_Type>::value);

    // detect_tuple_element
    //   detector: checks for std::tuple_element<0, _Type>::type.
    template<typename _Type>
    using detect_tuple_element = typename std::tuple_element<0, _Type>::type;

    // detect_std_get
    //   detector: checks for std::get<0>(_Type&).
    template<typename _Type>
    using detect_std_get = decltype(std::get<0>(std::declval<_Type&>()));

NS_END  // internal

// has_tuple_size
//   trait: checks if std::tuple_size<_Type> is well-formed.
template<typename _Type>
struct has_tuple_size : is_detected<internal::detect_tuple_size, _Type>
{};

// has_tuple_element
//   trait: checks if std::tuple_element<0, _Type> is well-formed.
template<typename _Type>
struct has_tuple_element : is_detected<internal::detect_tuple_element, _Type>
{};

// has_std_get
//   trait: checks if std::get<0>(_Type&) is well-formed.
template<typename _Type>
struct has_std_get : is_detected<internal::detect_std_get, _Type>
{};

// has_tuple_protocol
//   trait: checks if `_Type` supports complete tuple-like interface.
template<typename _Type>
struct has_tuple_protocol : std::integral_constant<bool,
    ( has_tuple_size<_Type>::value    &&
      has_tuple_element<_Type>::value &&
      has_std_get<_Type>::value )>
{};

// has_constexpr_size
//   trait: checks if `_Type` has compile-time known size via tuple_size.
template<typename _Type>
struct has_constexpr_size : has_tuple_size<_Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_tuple_size_v
    //   variable template: value of has_tuple_size<_Type>.
    template<typename _Type>
    constexpr bool has_tuple_size_v = has_tuple_size<_Type>::value;

    // has_tuple_element_v
    //   variable template: value of has_tuple_element<_Type>.
    template<typename _Type>
    constexpr bool has_tuple_element_v = has_tuple_element<_Type>::value;

    // has_std_get_v
    //   variable template: value of has_std_get<_Type>.
    template<typename _Type>
    constexpr bool has_std_get_v = has_std_get<_Type>::value;

    // has_tuple_protocol_v
    //   variable template: value of has_tuple_protocol<_Type>.
    template<typename _Type>
    constexpr bool has_tuple_protocol_v = has_tuple_protocol<_Type>::value;

    // has_constexpr_size_v
    //   variable template: value of has_constexpr_size<_Type>.
    template<typename _Type>
    constexpr bool has_constexpr_size_v = has_constexpr_size<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// #############################################################################
// VI.  TYPE RELATIONSHIP VALIDATORS
// #############################################################################
// Validates relationships between type aliases.
// DEPENDENCIES: Section I
// EXTRACTABLE:  Yes - can be moved to stl_type_relationship_traits.hpp
// #############################################################################

NS_INTERNAL

    // type_aliases_consistent
    //   validator: checks standard type alias relationships.
    //   reference == value_type&, const_reference == const value_type&, etc.
    template<typename _Type,
             bool     = ( has_value_type<_Type>::value      &&
                          has_reference<_Type>::value       &&
                          has_const_reference<_Type>::value &&
                          has_pointer<_Type>::value         &&
                          has_const_pointer<_Type>::value )>
    struct type_aliases_consistent : std::false_type
    {};

    template<typename _Type>
    struct type_aliases_consistent<_Type, true> : std::integral_constant<bool,
        ( std::is_same<typename _Type::reference,
                       typename _Type::value_type&>::value       &&
          std::is_same<typename _Type::const_reference,
                       const typename _Type::value_type&>::value &&
          std::is_same<typename _Type::pointer,
                       typename _Type::value_type*>::value       &&
          std::is_same<typename _Type::const_pointer,
                       const typename _Type::value_type*>::value )>
    {};

NS_END  // internal

// has_consistent_type_aliases
//   trait: validates type alias relationships match standard conventions.
template<typename _Type>
struct has_consistent_type_aliases : internal::type_aliases_consistent<_Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_consistent_type_aliases_v
    //   variable template: value of has_consistent_type_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_consistent_type_aliases_v = has_consistent_type_aliases<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// #############################################################################
// VII. COMPOSITE TRAITS (Container Classification)
// #############################################################################
// Composite traits for classifying container types.
// DEPENDENCIES: All previous sections
// EXTRACTABLE:  Yes - can be moved to stl_container_traits.hpp
// #############################################################################

// =============================================================================
// VII.A  Alias Group Traits
// =============================================================================

// has_basic_container_aliases
//   trait: checks for value_type, size_type, difference_type.
template<typename _Type>
struct has_basic_container_aliases : std::integral_constant<bool,
    ( has_value_type<_Type>::value      &&
      has_size_type<_Type>::value       &&
      has_difference_type<_Type>::value )>
{};

// has_reference_aliases
//   trait: checks for reference, const_reference, pointer, const_pointer.
template<typename _Type>
struct has_reference_aliases : std::integral_constant<bool,
    ( has_reference<_Type>::value       &&
      has_const_reference<_Type>::value &&
      has_pointer<_Type>::value         &&
      has_const_pointer<_Type>::value )>
{};

// has_iterator_aliases
//   trait: checks for iterator, const_iterator.
template<typename _Type>
struct has_iterator_aliases : std::integral_constant<bool,
    ( has_iterator<_Type>::value &&
      has_const_iterator<_Type>::value )>
{};

// has_reverse_iterator_aliases
//   trait: checks for reverse_iterator, const_reverse_iterator.
template<typename _Type>
struct has_reverse_iterator_aliases : std::integral_constant<bool,
    ( has_reverse_iterator<_Type>::value &&
      has_const_reverse_iterator<_Type>::value )>
{};

// has_local_iterator_aliases
//   trait: checks for local_iterator, const_local_iterator.
template<typename _Type>
struct has_local_iterator_aliases : std::integral_constant<bool,
    ( has_local_iterator<_Type>::value &&
      has_const_local_iterator<_Type>::value )>
{};

// has_associative_aliases
//   trait: checks for key_type (all associative containers have this).
template<typename _Type>
struct has_associative_aliases : std::integral_constant<bool,
    ( has_key_type<_Type>::value )>
{};

// has_ordered_associative_aliases
//   trait: checks for key_compare, value_compare (ordered associative).
template<typename _Type>
struct has_ordered_associative_aliases : std::integral_constant<bool,
    ( has_key_type<_Type>::value      &&
      has_key_compare<_Type>::value   &&
      has_value_compare<_Type>::value )>
{};

// has_unordered_associative_aliases
//   trait: checks for hasher, key_equal (unordered associative).
template<typename _Type>
struct has_unordered_associative_aliases : std::integral_constant<bool,
    ( has_key_type<_Type>::value  &&
      has_hasher<_Type>::value    &&
      has_key_equal<_Type>::value )>
{};

// has_map_aliases
//   trait: checks for mapped_type (map containers only).
template<typename _Type>
struct has_map_aliases : std::integral_constant<bool,
    ( has_key_type<_Type>::value    &&
      has_mapped_type<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_basic_container_aliases_v
    //   variable template: value of has_basic_container_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_basic_container_aliases_v = has_basic_container_aliases<_Type>::value;

    // has_reference_aliases_v
    //   variable template: value of has_reference_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_reference_aliases_v = has_reference_aliases<_Type>::value;

    // has_iterator_aliases_v
    //   variable template: value of has_iterator_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_iterator_aliases_v = has_iterator_aliases<_Type>::value;

    // has_reverse_iterator_aliases_v
    //   variable template: value of has_reverse_iterator_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_reverse_iterator_aliases_v = has_reverse_iterator_aliases<_Type>::value;

    // has_local_iterator_aliases_v
    //   variable template: value of has_local_iterator_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_local_iterator_aliases_v = has_local_iterator_aliases<_Type>::value;

    // has_associative_aliases_v
    //   variable template: value of has_associative_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_associative_aliases_v = has_associative_aliases<_Type>::value;

    // has_ordered_associative_aliases_v
    //   variable template: value of has_ordered_associative_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_ordered_associative_aliases_v = has_ordered_associative_aliases<_Type>::value;

    // has_unordered_associative_aliases_v
    //   variable template: value of has_unordered_associative_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_unordered_associative_aliases_v = has_unordered_associative_aliases<_Type>::value;

    // has_map_aliases_v
    //   variable template: value of has_map_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_map_aliases_v = has_map_aliases<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// =============================================================================
// VII.B  Method Group Traits
// =============================================================================

// has_element_access_methods
//   trait: checks for at, operator[], front, back, data.
template<typename _Type>
struct has_element_access_methods : std::integral_constant<bool,
    ( has_at<_Type>::value                 &&
      has_subscript_operator<_Type>::value &&
      has_front<_Type>::value              &&
      has_back<_Type>::value               &&
      has_data<_Type>::value )>
{};

// has_iterator_methods
//   trait: checks for begin, end, cbegin, cend.
template<typename _Type>
struct has_iterator_methods : std::integral_constant<bool,
    ( has_begin_end<_Type>::value &&
      has_cbegin_cend<_Type>::value )>
{};

// has_reverse_iterator_methods
//   trait: checks for rbegin, rend, crbegin, crend.
template<typename _Type>
struct has_reverse_iterator_methods : std::integral_constant<bool,
    ( has_rbegin_rend<_Type>::value &&
      has_crbegin_crend<_Type>::value )>
{};

// has_capacity_methods
//   trait: checks for empty, size, max_size.
template<typename _Type>
struct has_capacity_methods : std::integral_constant<bool,
    ( has_empty<_Type>::value    &&
      has_size<_Type>::value     &&
      has_max_size<_Type>::value )>
{};

// has_list_operations
//   trait: checks for list-specific operations (splice, merge, sort, etc).
template<typename _Type>
struct has_list_operations : std::integral_constant<bool,
    ( has_splice<_Type>::value  &&
      has_remove<_Type>::value  &&
      has_unique<_Type>::value  &&
      has_merge<_Type>::value   &&
      has_sort<_Type>::value    &&
      has_reverse<_Type>::value )>
{};

// has_forward_list_operations
//   trait: checks for forward_list-specific operations.
template<typename _Type>
struct has_forward_list_operations : std::integral_constant<bool,
    ( has_before_begin<_Type>::value   &&
      has_insert_after<_Type>::value   &&
      has_emplace_after<_Type>::value  &&
      has_erase_after<_Type>::value    &&
      has_splice_after<_Type>::value )>
{};

// has_associative_lookup_methods
//   trait: checks for find, count, equal_range (all associative).
template<typename _Type>
struct has_associative_lookup_methods : std::integral_constant<bool,
    ( has_find<_Type>::value        &&
      has_count<_Type>::value       &&
      has_equal_range<_Type>::value )>
{};

// has_ordered_lookup_methods
//   trait: checks for lower_bound, upper_bound (ordered only).
template<typename _Type>
struct has_ordered_lookup_methods : std::integral_constant<bool,
    ( has_lower_bound<_Type>::value &&
      has_upper_bound<_Type>::value )>
{};

// has_bucket_interface
//   trait: checks for bucket_count, bucket_size, bucket (unordered).
template<typename _Type>
struct has_bucket_interface : std::integral_constant<bool,
    ( has_bucket_count<_Type>::value     &&
      has_max_bucket_count<_Type>::value &&
      has_bucket_size<_Type>::value      &&
      has_bucket<_Type>::value )>
{};

// has_hash_policy
//   trait: checks for load_factor, max_load_factor, rehash (unordered).
template<typename _Type>
struct has_hash_policy : std::integral_constant<bool,
    ( has_load_factor<_Type>::value     &&
      has_max_load_factor<_Type>::value &&
      has_rehash<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_element_access_methods_v
    //   variable template: value of has_element_access_methods<_Type>.
    template<typename _Type>
    constexpr bool has_element_access_methods_v = has_element_access_methods<_Type>::value;

    // has_iterator_methods_v
    //   variable template: value of has_iterator_methods<_Type>.
    template<typename _Type>
    constexpr bool has_iterator_methods_v = has_iterator_methods<_Type>::value;

    // has_reverse_iterator_methods_v
    //   variable template: value of has_reverse_iterator_methods<_Type>.
    template<typename _Type>
    constexpr bool has_reverse_iterator_methods_v = has_reverse_iterator_methods<_Type>::value;

    // has_capacity_methods_v
    //   variable template: value of has_capacity_methods<_Type>.
    template<typename _Type>
    constexpr bool has_capacity_methods_v = has_capacity_methods<_Type>::value;

    // has_list_operations_v
    //   variable template: value of has_list_operations<_Type>.
    template<typename _Type>
    constexpr bool has_list_operations_v = has_list_operations<_Type>::value;

    // has_forward_list_operations_v
    //   variable template: value of has_forward_list_operations<_Type>.
    template<typename _Type>
    constexpr bool has_forward_list_operations_v = has_forward_list_operations<_Type>::value;

    // has_associative_lookup_methods_v
    //   variable template: value of has_associative_lookup_methods<_Type>.
    template<typename _Type>
    constexpr bool has_associative_lookup_methods_v = has_associative_lookup_methods<_Type>::value;

    // has_ordered_lookup_methods_v
    //   variable template: value of has_ordered_lookup_methods<_Type>.
    template<typename _Type>
    constexpr bool has_ordered_lookup_methods_v = has_ordered_lookup_methods<_Type>::value;

    // has_bucket_interface_v
    //   variable template: value of has_bucket_interface<_Type>.
    template<typename _Type>
    constexpr bool has_bucket_interface_v = has_bucket_interface<_Type>::value;

    // has_hash_policy_v
    //   variable template: value of has_hash_policy<_Type>.
    template<typename _Type>
    constexpr bool has_hash_policy_v = has_hash_policy<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// =============================================================================
// VII.C  Container Type Traits
// =============================================================================
// Comprehensive traits for detecting specific container types.

// -----------------------------------------------------------------------------
// VII.C.1  std::array detection
// -----------------------------------------------------------------------------

// has_all_array_type_aliases
//   trait: checks for all type aliases required by std::array.
template<typename _Type>
struct has_all_array_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value   &&
      has_reference_aliases<_Type>::value         &&
      has_iterator_aliases<_Type>::value          &&
      has_reverse_iterator_aliases<_Type>::value )>
{};

// has_all_array_member_functions
//   trait: checks for all member functions required by std::array.
template<typename _Type>
struct has_all_array_member_functions : std::integral_constant<bool,
    ( has_at<_Type>::value                       &&
      has_subscript_operator<_Type>::value       &&
      has_front<_Type>::value                    &&
      has_back<_Type>::value                     &&
      has_data<_Type>::value                     &&
      has_iterator_methods<_Type>::value         &&
      has_reverse_iterator_methods<_Type>::value &&
      has_capacity_methods<_Type>::value         &&
      has_fill<_Type>::value                     &&
      has_swap<_Type>::value )>
{};

// is_array_like
//   trait: checks if `_Type` has the interface of std::array.
//   Requires: fixed size, tuple protocol, no allocator, no dynamic operations.
template<typename _Type>
struct is_array_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_all_array_type_aliases<T>::value     &&
          has_all_array_member_functions<T>::value &&
          has_tuple_protocol<T>::value             &&
          !has_allocator_type<T>::value            &&
          !has_push_back<T>::value                 &&
          !has_resize<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_all_array_type_aliases_v
    //   variable template: value of has_all_array_type_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_all_array_type_aliases_v = has_all_array_type_aliases<_Type>::value;

    // has_all_array_member_functions_v
    //   variable template: value of has_all_array_member_functions<_Type>.
    template<typename _Type>
    constexpr bool has_all_array_member_functions_v = has_all_array_member_functions<_Type>::value;

    // is_array_like_v
    //   variable template: value of is_array_like<_Type>.
    template<typename _Type>
    constexpr bool is_array_like_v = is_array_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// -----------------------------------------------------------------------------
// VII.C.2  std::vector detection
// -----------------------------------------------------------------------------

// has_vector_type_aliases
//   trait: checks for type aliases required by std::vector.
template<typename _Type>
struct has_vector_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value   &&
      has_reference_aliases<_Type>::value         &&
      has_iterator_aliases<_Type>::value          &&
      has_reverse_iterator_aliases<_Type>::value  &&
      has_allocator_type<_Type>::value )>
{};

// has_vector_member_functions
//   trait: checks for member functions characteristic of std::vector.
//   Vector has: at, [], front, back, data, capacity, reserve, shrink_to_fit,
//   push_back, pop_back, resize, but NOT push_front/pop_front.
template<typename _Type>
struct has_vector_member_functions : std::integral_constant<bool,
    ( has_at<_Type>::value                 &&
      has_subscript_operator<_Type>::value &&
      has_front<_Type>::value              &&
      has_back<_Type>::value               &&
      has_data<_Type>::value               &&
      has_capacity<_Type>::value           &&
      has_reserve<_Type>::value            &&
      has_shrink_to_fit<_Type>::value      &&
      has_push_back<_Type>::value          &&
      has_emplace_back<_Type>::value       &&
      has_pop_back<_Type>::value           &&
      has_resize<_Type>::value             &&
      has_insert<_Type>::value             &&
      has_erase<_Type>::value              &&
      has_clear<_Type>::value )>
{};

// is_vector_like
//   trait: checks if `_Type` has the interface of std::vector.
//   Distinguishing features: has data(), capacity, reserve, no push_front.
template<typename _Type>
struct is_vector_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_vector_type_aliases<T>::value       &&
          has_vector_member_functions<T>::value   &&
          has_iterator_methods<T>::value          &&
          has_reverse_iterator_methods<T>::value  &&
          !has_push_front<T>::value               &&
          !has_key_type<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

// has_vector_type_aliases_v
//   variable template: value of has_vector_type_aliases<_Type>.
template<typename _Type>
constexpr bool has_vector_type_aliases_v = has_vector_type_aliases<_Type>::value;

// has_vector_member_functions_v
//   variable template: value of has_vector_member_functions<_Type>.
template<typename _Type>
constexpr bool has_vector_member_functions_v = has_vector_member_functions<_Type>::value;

// is_vector_like_v
//   variable template: value of is_vector_like<_Type>.
template<typename _Type>
constexpr bool is_vector_like_v = is_vector_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// -----------------------------------------------------------------------------
// VII.C.3  std::deque detection
// -----------------------------------------------------------------------------

// has_deque_type_aliases
//   trait: checks for type aliases required by std::deque.
template<typename _Type>
struct has_deque_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value  &&
      has_reference_aliases<_Type>::value        &&
      has_iterator_aliases<_Type>::value         &&
      has_reverse_iterator_aliases<_Type>::value &&
      has_allocator_type<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
// has_deque_type_aliases_v
//   variable template: value of has_deque_type_aliases<_Type>.
template<typename _Type>
constexpr bool has_deque_type_aliases_v = has_deque_type_aliases<_Type>::value;
#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

// has_deque_member_functions
//   trait: checks for member functions characteristic of std::deque.
//   Deque has: at, [], front, back, push_front, pop_front, push_back, pop_back,
//   but NO data(), NO capacity.
template<typename _Type>
struct has_deque_member_functions : std::integral_constant<bool,
    ( has_at<_Type>::value                 &&
      has_subscript_operator<_Type>::value &&
      has_front<_Type>::value              &&
      has_back<_Type>::value               &&
      has_push_front<_Type>::value         &&
      has_emplace_front<_Type>::value      &&
      has_pop_front<_Type>::value          &&
      has_push_back<_Type>::value          &&
      has_emplace_back<_Type>::value       &&
      has_pop_back<_Type>::value           &&
      has_resize<_Type>::value             &&
      has_shrink_to_fit<_Type>::value      &&
      has_insert<_Type>::value             &&
      has_erase<_Type>::value              &&
      has_clear<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
// has_deque_member_functions_v
//   variable template: value of has_deque_member_functions<_Type>.
template<typename _Type>
constexpr bool has_deque_member_functions_v = has_deque_member_functions<_Type>::value;
#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

// is_deque_like
//   trait: checks if `_Type` has the interface of std::deque.
//   Distinguishing features: has both push_front and push_back, has at/[],
//   no data(), no capacity.
template<typename _Type>
struct is_deque_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_deque_type_aliases<T>::value     &&
          has_deque_member_functions<T>::value &&
          has_iterator_methods<T>::value       &&
          has_reverse_iterator_methods<T>::value &&
          !has_data<T>::value                  &&
          !has_capacity<T>::value              &&
          !has_key_type<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
// is_deque_like_v
//   variable template: value of is_deque_like<_Type>.
template<typename _Type>
constexpr bool is_deque_like_v = is_deque_like<_Type>::value;
#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// -----------------------------------------------------------------------------
// VII.C.4  std::list detection
// -----------------------------------------------------------------------------

// has_deque_type_aliases
//   trait: checks for type aliases required by std::deque.
template<typename _Type>
struct has_deque_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value   &&
      has_reference_aliases<_Type>::value         &&
      has_iterator_aliases<_Type>::value          &&
      has_reverse_iterator_aliases<_Type>::value  &&
      has_allocator_type<_Type>::value )>
{};

// has_deque_member_functions
//   trait: checks for member functions characteristic of std::deque.
//   Deque has: at, [], front, back, push_front, pop_front, push_back, pop_back,
//   but NO data(), NO capacity.
template<typename _Type>
struct has_deque_member_functions : std::integral_constant<bool,
    ( has_at<_Type>::value                 &&
      has_subscript_operator<_Type>::value &&
      has_front<_Type>::value              &&
      has_back<_Type>::value               &&
      has_push_front<_Type>::value         &&
      has_emplace_front<_Type>::value      &&
      has_pop_front<_Type>::value          &&
      has_push_back<_Type>::value          &&
      has_emplace_back<_Type>::value       &&
      has_pop_back<_Type>::value           &&
      has_resize<_Type>::value             &&
      has_shrink_to_fit<_Type>::value      &&
      has_insert<_Type>::value             &&
      has_erase<_Type>::value              &&
      has_clear<_Type>::value )>
{};

// is_deque_like
//   trait: checks if `_Type` has the interface of std::deque.
//   Distinguishing features: has both push_front and push_back, has at/[],
//   no data(), no capacity.
template<typename _Type>
struct is_deque_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_deque_type_aliases<T>::value       &&
          has_deque_member_functions<T>::value   &&
          has_iterator_methods<T>::value         &&
          has_reverse_iterator_methods<T>::value &&
          !has_data<T>::value                    &&
          !has_capacity<T>::value                &&
          !has_key_type<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_deque_type_aliases_v
    //   variable template: value of has_deque_type_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_deque_type_aliases_v = has_deque_type_aliases<_Type>::value;

    // has_deque_member_functions_v
    //   variable template: value of has_deque_member_functions<_Type>.
    template<typename _Type>
    constexpr bool has_deque_member_functions_v = has_deque_member_functions<_Type>::value;

    // is_deque_like_v
    //   variable template: value of is_deque_like<_Type>.
    template<typename _Type>
    constexpr bool is_deque_like_v = is_deque_like<_Type>::value;

#endif

// -----------------------------------------------------------------------------
// VII.C.5  std::forward_list detection
// -----------------------------------------------------------------------------

// has_forward_list_type_aliases
//   trait: checks for type aliases required by std::forward_list.
//   Note: forward_list does NOT have reverse_iterator.
template<typename _Type>
struct has_forward_list_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value &&
      has_reference_aliases<_Type>::value       &&
      has_iterator_aliases<_Type>::value        &&
      has_allocator_type<_Type>::value )>
{};

// has_forward_list_member_functions
//   trait: checks for member functions characteristic of std::forward_list.
//   forward_list has: front, push_front, pop_front, before_begin, insert_after,
//   emplace_after, erase_after, splice_after, remove, unique, merge, sort, reverse.
//   forward_list does NOT have: back, push_back, pop_back, at, [], size, rbegin/rend.
template<typename _Type>
struct has_forward_list_member_functions : std::integral_constant<bool,
    ( has_front<_Type>::value          &&
      has_push_front<_Type>::value     &&
      has_emplace_front<_Type>::value  &&
      has_pop_front<_Type>::value      &&
      has_before_begin<_Type>::value   &&
      has_insert_after<_Type>::value   &&
      has_emplace_after<_Type>::value  &&
      has_erase_after<_Type>::value    &&
      has_splice_after<_Type>::value   &&
      has_remove<_Type>::value         &&
      has_remove_if<_Type>::value      &&
      has_unique<_Type>::value         &&
      has_merge<_Type>::value          &&
      has_sort<_Type>::value           &&
      has_reverse<_Type>::value        &&
      has_resize<_Type>::value         &&
      has_clear<_Type>::value )>
{};

// is_forward_list_like
//   trait: checks if `_Type` has the interface of std::forward_list.
//   Distinguishing features: has before_begin, splice_after (not splice),
//   no reverse iterators, no size, no back.
template<typename _Type>
struct is_forward_list_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_forward_list_type_aliases<T>::value      &&
          has_forward_list_member_functions<T>::value  &&
          has_iterator_methods<T>::value               &&
          has_forward_list_operations<T>::value        &&
          !has_back<T>::value                          &&
          !has_push_back<T>::value                     &&
          !has_size<T>::value                          &&
          !has_reverse_iterator_aliases<T>::value      &&
          !has_splice<T>::value                        &&
          !has_key_type<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_forward_list_type_aliases_v
    //   variable template: value of has_forward_list_type_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_forward_list_type_aliases_v =
        has_forward_list_type_aliases<_Type>::value;

    // has_forward_list_member_functions_v
    //   variable template: value of has_forward_list_member_functions<_Type>.
    template<typename _Type>
    constexpr bool has_forward_list_member_functions_v =
        has_forward_list_member_functions<_Type>::value;

    // is_forward_list_like_v
    //   variable template: value of is_forward_list_like<_Type>.
    template<typename _Type>
    constexpr bool is_forward_list_like_v = is_forward_list_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// -----------------------------------------------------------------------------
// VII.C.6  std::set detection
// -----------------------------------------------------------------------------

// has_set_type_aliases
//   trait: checks for type aliases required by std::set.
//   Set has: key_type, value_type (same as key_type), key_compare, value_compare.
//   Set does NOT have: mapped_type.
template<typename _Type>
struct has_set_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value       &&
      has_reference_aliases<_Type>::value             &&
      has_iterator_aliases<_Type>::value              &&
      has_reverse_iterator_aliases<_Type>::value      &&
      has_allocator_type<_Type>::value                &&
      has_ordered_associative_aliases<_Type>::value )>
{};

// has_set_member_functions
//   trait: checks for member functions characteristic of std::set.
//   Set has: find, count, lower_bound, upper_bound, equal_range, key_comp, value_comp,
//   insert, emplace, emplace_hint, erase, clear, swap, merge (C++17), extract (C++17).
template<typename _Type>
struct has_set_member_functions : std::integral_constant<bool,
    ( has_find<_Type>::value        &&
      has_count<_Type>::value       &&
      has_lower_bound<_Type>::value &&
      has_upper_bound<_Type>::value &&
      has_equal_range<_Type>::value &&
      has_key_comp<_Type>::value    &&
      has_value_comp<_Type>::value  &&
      has_insert<_Type>::value      &&
      has_emplace<_Type>::value     &&
      has_erase<_Type>::value       &&
      has_clear<_Type>::value       &&
      has_swap<_Type>::value )>
{};

// is_set_like
//   trait: checks if `_Type` has the interface of std::set.
//   Distinguishing features: ordered (has lower_bound, upper_bound), no mapped_type,
//   no bucket interface.
template<typename _Type>
struct is_set_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_set_type_aliases<T>::value            &&
          has_set_member_functions<T>::value        &&
          has_iterator_methods<T>::value            &&
          has_reverse_iterator_methods<T>::value    &&
          has_associative_lookup_methods<T>::value  &&
          has_ordered_lookup_methods<T>::value      &&
          !has_mapped_type<T>::value                &&
          !has_bucket_interface<T>::value           &&
          !has_subscript_operator<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES

// has_set_type_aliases_v
//   variable template: value of has_set_type_aliases<_Type>.
template<typename _Type>
constexpr bool has_set_type_aliases_v = has_set_type_aliases<_Type>::value;

// has_set_member_functions_v
//   variable template: value of has_set_member_functions<_Type>.
template<typename _Type>
constexpr bool has_set_member_functions_v = has_set_member_functions<_Type>::value;

// is_set_like_v
//   variable template: value of is_set_like<_Type>.
template<typename _Type>
constexpr bool is_set_like_v = is_set_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// -----------------------------------------------------------------------------
// VII.C.7  std::multiset detection
// -----------------------------------------------------------------------------

// is_multiset_like
//   trait: checks if `_Type` has the interface of std::multiset.
//   Same as set but count() can return > 1. We can't distinguish at compile time
//   so this is essentially the same as is_set_like.
template<typename _Type>
struct is_multiset_like : is_set_like<_Type>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // is_multiset_like_v
    //   variable template: value of is_multiset_like<_Type>.
    template<typename _Type>
    constexpr bool is_multiset_like_v = is_multiset_like<_Type>::value;
#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// -----------------------------------------------------------------------------
// VII.C.8  std::map detection
// -----------------------------------------------------------------------------

// has_map_type_aliases
//   trait: checks for type aliases required by std::map.
//   Map has: key_type, mapped_type, value_type (pair), key_compare, value_compare.
template<typename _Type>
struct has_map_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value       &&
      has_reference_aliases<_Type>::value             &&
      has_iterator_aliases<_Type>::value              &&
      has_reverse_iterator_aliases<_Type>::value      &&
      has_allocator_type<_Type>::value                &&
      has_ordered_associative_aliases<_Type>::value   &&
      has_mapped_type<_Type>::value )>
{};

NS_INTERNAL

// detect_subscript_key
//   detector: checks for operator[](const key_type&).
template<typename _Type>
using detect_subscript_key = decltype(std::declval<_Type&>()[
    std::declval<const typename _Type::key_type&>()]);

// detect_at_key
//   detector: checks for at(const key_type&).
template<typename _Type>
using detect_at_key = decltype(std::declval<_Type&>().at(
    std::declval<const typename _Type::key_type&>()));

NS_END  // internal

// has_subscript_key
//   trait: checks if `_Type` has operator[](const key_type&).
template<typename _Type>
struct has_subscript_key : is_detected<internal::detect_subscript_key, _Type>
{};

// has_at_key
//   trait: checks if `_Type` has at(const key_type&).
template<typename _Type>
struct has_at_key : is_detected<internal::detect_at_key, _Type>
{};

// has_map_member_functions
//   trait: checks for member functions characteristic of std::map.
//   Map has everything set has plus: operator[], at (with key_type).
template<typename _Type>
struct has_map_member_functions : std::integral_constant<bool,
    ( has_set_member_functions<_Type>::value &&
      has_subscript_key<_Type>::value        &&
      has_at_key<_Type>::value )>
{};

// is_map_like
//   trait: checks if `_Type` has the interface of std::map.
//   Distinguishing features: ordered, has mapped_type, has operator[] and at with key.
template<typename _Type>
struct is_map_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_map_type_aliases<T>::value            &&
          has_map_member_functions<T>::value        &&
          has_iterator_methods<T>::value            &&
          has_reverse_iterator_methods<T>::value    &&
          has_associative_lookup_methods<T>::value  &&
          has_ordered_lookup_methods<T>::value      &&
          !has_bucket_interface<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_map_type_aliases_v
    //   variable template: value of has_map_type_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_map_type_aliases_v = has_map_type_aliases<_Type>::value;

    // has_subscript_key_v
    //   variable template: value of has_subscript_key<_Type>.
    template<typename _Type>
    constexpr bool has_subscript_key_v = has_subscript_key<_Type>::value;

    // has_at_key_v
    //   variable template: value of has_at_key<_Type>.
    template<typename _Type>
    constexpr bool has_at_key_v = has_at_key<_Type>::value;

    // has_map_member_functions_v
    //   variable template: value of has_map_member_functions<_Type>.
    template<typename _Type>
    constexpr bool has_map_member_functions_v = has_map_member_functions<_Type>::value;

    // is_map_like_v
    //   variable template: value of is_map_like<_Type>.
    template<typename _Type>
    constexpr bool is_map_like_v = is_map_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// -----------------------------------------------------------------------------
// VII.C.9  std::multimap detection
// -----------------------------------------------------------------------------

// has_multimap_member_functions
//   trait: checks for member functions characteristic of std::multimap.
//   Multimap does NOT have: operator[], at.
template<typename _Type>
struct has_multimap_member_functions : std::integral_constant<bool,
    ( has_set_member_functions<_Type>::value &&
      !has_subscript_key<_Type>::value       &&
      !has_at_key<_Type>::value )>
{};

// is_multimap_like
//   trait: checks if `_Type` has the interface of std::multimap.
//   Distinguishing features: ordered, has mapped_type, but NO operator[] or at.
template<typename _Type>
struct is_multimap_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_map_type_aliases<T>::value              &&
          has_multimap_member_functions<T>::value     &&
          has_iterator_methods<T>::value              &&
          has_reverse_iterator_methods<T>::value      &&
          has_associative_lookup_methods<T>::value    &&
          has_ordered_lookup_methods<T>::value        &&
          !has_bucket_interface<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_multimap_member_functions_v
    //   variable template: value of has_multimap_member_functions<_Type>.
    template<typename _Type>
    constexpr bool has_multimap_member_functions_v =
        has_multimap_member_functions<_Type>::value;

    // is_multimap_like_v
    //   variable template: value of is_multimap_like<_Type>.
    template<typename _Type>
    constexpr bool is_multimap_like_v = is_multimap_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// -----------------------------------------------------------------------------
// VII.C.10  std::unordered_set detection
// -----------------------------------------------------------------------------

// has_unordered_set_type_aliases
//   trait: checks for type aliases required by std::unordered_set.
template<typename _Type>
struct has_unordered_set_type_aliases : std::integral_constant<bool,
    ( has_basic_container_aliases<_Type>::value         &&
      has_reference_aliases<_Type>::value               &&
      has_iterator_aliases<_Type>::value                &&
      has_local_iterator_aliases<_Type>::value          &&
      has_allocator_type<_Type>::value                  &&
      has_unordered_associative_aliases<_Type>::value )>
{};

// has_unordered_set_member_functions
//   trait: checks for member functions characteristic of std::unordered_set.
template<typename _Type>
struct has_unordered_set_member_functions : std::integral_constant<bool,
    ( has_find<_Type>::value             &&
      has_count<_Type>::value            &&
      has_equal_range<_Type>::value      &&
      has_bucket_count<_Type>::value     &&
      has_max_bucket_count<_Type>::value &&
      has_bucket_size<_Type>::value      &&
      has_bucket<_Type>::value           &&
      has_load_factor<_Type>::value      &&
      has_max_load_factor<_Type>::value  &&
      has_rehash<_Type>::value           &&
      has_reserve<_Type>::value          &&
      has_hash_function<_Type>::value    &&
      has_key_eq<_Type>::value           &&
      has_insert<_Type>::value           &&
      has_emplace<_Type>::value          &&
      has_erase<_Type>::value            &&
      has_clear<_Type>::value            &&
      has_swap<_Type>::value )>
{};

// is_unordered_set_like
//   trait: checks if `_Type` has the interface of std::unordered_set.
//   Distinguishing features: has bucket interface, no lower_bound/upper_bound,
//   no mapped_type, no reverse iterators.
template<typename _Type>
struct is_unordered_set_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_unordered_set_type_aliases<T>::value      &&
          has_unordered_set_member_functions<T>::value  &&
          has_iterator_methods<T>::value                &&
          has_bucket_interface<T>::value                &&
          has_hash_policy<T>::value                     &&
          !has_mapped_type<T>::value                    &&
          !has_lower_bound<T>::value                    &&
          !has_reverse_iterator_aliases<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_unordered_set_type_aliases_v
    //   variable template: value of has_unordered_set_type_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_unordered_set_type_aliases_v =
        has_unordered_set_type_aliases<_Type>::value;

    // has_unordered_set_member_functions_v
    //   variable template: value of has_unordered_set_member_functions<_Type>.
    template<typename _Type>
    constexpr bool has_unordered_set_member_functions_v =
        has_unordered_set_member_functions<_Type>::value;

    // is_unordered_set_like_v
    //   variable template: value of is_unordered_set_like<_Type>.
    template<typename _Type>
    constexpr bool is_unordered_set_like_v = is_unordered_set_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// -----------------------------------------------------------------------------
// VII.C.11  std::unordered_map detection
// -----------------------------------------------------------------------------

// has_unordered_map_type_aliases
//   trait: checks for type aliases required by std::unordered_map.
template<typename _Type>
struct has_unordered_map_type_aliases : std::integral_constant<bool,
    ( has_unordered_set_type_aliases<_Type>::value &&
      has_mapped_type<_Type>::value )>
{};

// has_unordered_map_member_functions
//   trait: checks for member functions characteristic of std::unordered_map.
//   unordered_map has everything unordered_set has plus: operator[], at.
template<typename _Type>
struct has_unordered_map_member_functions : std::integral_constant<bool,
    ( has_unordered_set_member_functions<_Type>::value &&
      has_subscript_key<_Type>::value                  &&
      has_at_key<_Type>::value )>
{};

// is_unordered_map_like
//   trait: checks if `_Type` has the interface of std::unordered_map.
template<typename _Type>
struct is_unordered_map_like
{
private:
    using T = clean_t<_Type>;

public:
    static constexpr bool value =
        ( has_unordered_map_type_aliases<T>::value      &&
          has_unordered_map_member_functions<T>::value  &&
          has_iterator_methods<T>::value                &&
          has_bucket_interface<T>::value                &&
          has_hash_policy<T>::value                     &&
          !has_lower_bound<T>::value                    &&
          !has_reverse_iterator_aliases<T>::value );
};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // has_unordered_map_type_aliases_v
    //   variable template: value of has_unordered_map_type_aliases<_Type>.
    template<typename _Type>
    constexpr bool has_unordered_map_type_aliases_v =
        has_unordered_map_type_aliases<_Type>::value;

    // has_unordered_map_member_functions_v
    //   variable template: value of has_unordered_map_member_functions<_Type>.
    template<typename _Type>
    constexpr bool has_unordered_map_member_functions_v =
        has_unordered_map_member_functions<_Type>::value;

    // is_unordered_map_like_v
    //   variable template: value of is_unordered_map_like<_Type>.
    template<typename _Type>
    constexpr bool is_unordered_map_like_v = is_unordered_map_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES


// -----------------------------------------------------------------------------
// VII.C.12  Container Category Traits
// -----------------------------------------------------------------------------
    // is_sequence_container_like
//   trait: checks if `_Type` is a sequence container (`vector`,`deque`, list, forward_list).
template<typename _Type>
struct is_sequence_container_like : std::integral_constant<bool,
    ( is_vector_like<_Type>::value        ||
      is_deque_like<_Type>::value         ||
      is_list_like<_Type>::value          ||
      is_forward_list_like<_Type>::value )>
{};

// is_ordered_associative_like
//   trait: checks if `_Type` is an ordered associative container (set, map, etc).
template<typename _Type>
struct is_ordered_associative_like : std::integral_constant<bool,
    ( is_set_like<_Type>::value       ||
      is_map_like<_Type>::value       ||
      is_multimap_like<_Type>::value )>
{};

// is_unordered_associative_like
//   trait: checks if `_Type` is an unordered associative container.
template<typename _Type>
struct is_unordered_associative_like : std::integral_constant<bool,
    ( is_unordered_set_like<_Type>::value ||
      is_unordered_map_like<_Type>::value )>
{};

// is_associative_container_like
//   trait: checks if `_Type` is any associative container.
template<typename _Type>
struct is_associative_container_like : std::integral_constant<bool,
    ( is_ordered_associative_like<_Type>::value   ||
      is_unordered_associative_like<_Type>::value )>
{};

// is_stl_container_like
//   trait: checks if `_Type` resembles any standard container.
template<typename _Type>
struct is_stl_container_like : std::integral_constant<bool,
    ( is_array_like<_Type>::value               ||
      is_sequence_container_like<_Type>::value  ||
      is_associative_container_like<_Type>::value )>
{};

#if D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES
    // is_sequence_container_like_v
    //   variable template: value of is_sequence_container_like<_Type>.
    template<typename _Type>
    constexpr bool is_sequence_container_like_v =
        is_sequence_container_like<_Type>::value;

    // is_ordered_associative_like_v
    //   variable template: value of is_ordered_associative_like<_Type>.
    template<typename _Type>
    constexpr bool is_ordered_associative_like_v =
        is_ordered_associative_like<_Type>::value;

    // is_unordered_associative_like_v
    //   variable template: value of is_unordered_associative_like<_Type>.
    template<typename _Type>
    constexpr bool is_unordered_associative_like_v =
        is_unordered_associative_like<_Type>::value;

    // is_associative_container_like_v
    //   variable template: value of is_associative_container_like<_Type>.
    template<typename _Type>
    constexpr bool is_associative_container_like_v =
        is_associative_container_like<_Type>::value;

    // is_stl_container_like_v
    //   variable template: value of is_stl_container_like<_Type>.
    template<typename _Type>
    constexpr bool is_stl_container_like_v =
        is_stl_container_like<_Type>::value;

#endif  // D_ENV_CPP_FEATURE_LANG_VARIABLE_TEMPLATES



// #############################################################################
// VIII. DIAGNOSTIC TRAITS
// #############################################################################
// Comprehensive diagnostic information about a type.
// DEPENDENCIES: All previous sections
// EXTRACTABLE:  Yes - can be moved to stl_diagnostic_traits.hpp
// #############################################################################

// alias_diagnostic
//   diagnostic: all type alias presence flags for a type.
template<typename _Type>
struct alias_diagnostic
{
private:
    using T = clean_t<_Type>;

public:
    // C++11 common
    static constexpr bool value_type       = has_value_type<T>::value;
    static constexpr bool size_type        = has_size_type<T>::value;
    static constexpr bool difference_type  = has_difference_type<T>::value;
    static constexpr bool reference        = has_reference<T>::value;
    static constexpr bool const_reference  = has_const_reference<T>::value;
    static constexpr bool pointer          = has_pointer<T>::value;
    static constexpr bool const_pointer    = has_const_pointer<T>::value;
    static constexpr bool allocator_type   = has_allocator_type<T>::value;

    // C++11 iterators
    static constexpr bool iterator_alias         = has_iterator<T>::value;
    static constexpr bool const_iterator         = has_const_iterator<T>::value;
    static constexpr bool reverse_iterator       = has_reverse_iterator<T>::value;
    static constexpr bool const_reverse_iterator = has_const_reverse_iterator<T>::value;
    static constexpr bool local_iterator         = has_local_iterator<T>::value;
    static constexpr bool const_local_iterator   = has_const_local_iterator<T>::value;

    // C++11 associative
    static constexpr bool key_type      = has_key_type<T>::value;
    static constexpr bool mapped_type   = has_mapped_type<T>::value;
    static constexpr bool key_compare   = has_key_compare<T>::value;
    static constexpr bool value_compare = has_value_compare<T>::value;
    static constexpr bool hasher        = has_hasher<T>::value;
    static constexpr bool key_equal     = has_key_equal<T>::value;

    // C++17
    static constexpr bool node_type          = has_node_type<T>::value;
    static constexpr bool insert_return_type = has_insert_return_type<T>::value;

    // C++20
    static constexpr bool iterator_concept = has_iterator_concept<T>::value;
};

// method_diagnostic
//   diagnostic: all method presence flags for a type.
template<typename _Type>
struct method_diagnostic
{
private:
    using T = clean_t<_Type>;

public:
    // element access
    static constexpr bool at            = has_at<T>::value;
    static constexpr bool subscript     = has_subscript_operator<T>::value;
    static constexpr bool front         = has_front<T>::value;
    static constexpr bool back          = has_back<T>::value;
    static constexpr bool data          = has_data<T>::value;

    // iterators
    static constexpr bool begin_end     = has_begin_end<T>::value;
    static constexpr bool cbegin_cend   = has_cbegin_cend<T>::value;
    static constexpr bool rbegin_rend   = has_rbegin_rend<T>::value;
    static constexpr bool crbegin_crend = has_crbegin_crend<T>::value;
    static constexpr bool before_begin  = has_before_begin<T>::value;

    // capacity
    static constexpr bool empty         = has_empty<T>::value;
    static constexpr bool size          = has_size<T>::value;
    static constexpr bool max_size      = has_max_size<T>::value;
    static constexpr bool capacity      = has_capacity<T>::value;
    static constexpr bool reserve       = has_reserve<T>::value;
    static constexpr bool shrink_to_fit = has_shrink_to_fit<T>::value;

    // modifiers
    static constexpr bool fill          = has_fill<T>::value;
    static constexpr bool swap          = has_swap<T>::value;
    static constexpr bool clear         = has_clear<T>::value;
    static constexpr bool insert        = has_insert<T>::value;
    static constexpr bool emplace       = has_emplace<T>::value;
    static constexpr bool erase         = has_erase<T>::value;
    static constexpr bool push_back     = has_push_back<T>::value;
    static constexpr bool emplace_back  = has_emplace_back<T>::value;
    static constexpr bool pop_back      = has_pop_back<T>::value;
    static constexpr bool push_front    = has_push_front<T>::value;
    static constexpr bool emplace_front = has_emplace_front<T>::value;
    static constexpr bool pop_front     = has_pop_front<T>::value;
    static constexpr bool resize        = has_resize<T>::value;
    static constexpr bool assign        = has_assign<T>::value;

    // list operations
    static constexpr bool splice        = has_splice<T>::value;
    static constexpr bool splice_after  = has_splice_after<T>::value;
    static constexpr bool remove        = has_remove<T>::value;
    static constexpr bool remove_if     = has_remove_if<T>::value;
    static constexpr bool unique        = has_unique<T>::value;
    static constexpr bool merge         = has_merge<T>::value;
    static constexpr bool sort          = has_sort<T>::value;
    static constexpr bool reverse       = has_reverse<T>::value;

    // associative
    static constexpr bool find          = has_find<T>::value;
    static constexpr bool count         = has_count<T>::value;
    static constexpr bool lower_bound   = has_lower_bound<T>::value;
    static constexpr bool upper_bound   = has_upper_bound<T>::value;
    static constexpr bool equal_range   = has_equal_range<T>::value;

    // unordered
    static constexpr bool bucket_count  = has_bucket_count<T>::value;
    static constexpr bool load_factor   = has_load_factor<T>::value;
    static constexpr bool rehash        = has_rehash<T>::value;
};

// operator_diagnostic
//   diagnostic: all operator presence flags for a type.
template<typename _Type>
struct operator_diagnostic
{
private:
    using T = clean_t<_Type>;

public:
    // comparison
    static constexpr bool equality         = has_equality_operator<T>::value;
    static constexpr bool inequality       = has_inequality_operator<T>::value;
    static constexpr bool less             = has_less_operator<T>::value;
    static constexpr bool less_equal       = has_less_equal_operator<T>::value;
    static constexpr bool greater          = has_greater_operator<T>::value;
    static constexpr bool greater_equal    = has_greater_equal_operator<T>::value;
    static constexpr bool spaceship        = has_spaceship_operator<T>::value;

    // arithmetic
    static constexpr bool plus             = has_plus_operator<T>::value;
    static constexpr bool minus            = has_minus_operator<T>::value;
    static constexpr bool multiply         = has_multiply_operator<T>::value;
    static constexpr bool divide           = has_divide_operator<T>::value;
    static constexpr bool modulo           = has_modulo_operator<T>::value;
    static constexpr bool negate           = has_negate_operator<T>::value;

    // increment/decrement
    static constexpr bool pre_increment    = has_pre_increment_operator<T>::value;
    static constexpr bool post_increment   = has_post_increment_operator<T>::value;
    static constexpr bool pre_decrement    = has_pre_decrement_operator<T>::value;
    static constexpr bool post_decrement   = has_post_decrement_operator<T>::value;

    // logical
    static constexpr bool logical_and      = has_logical_and_operator<T>::value;
    static constexpr bool logical_or       = has_logical_or_operator<T>::value;
    static constexpr bool logical_not      = has_logical_not_operator<T>::value;

    // bitwise
    static constexpr bool bitwise_and      = has_bitwise_and_operator<T>::value;
    static constexpr bool bitwise_or       = has_bitwise_or_operator<T>::value;
    static constexpr bool bitwise_xor      = has_bitwise_xor_operator<T>::value;
    static constexpr bool bitwise_not      = has_bitwise_not_operator<T>::value;
    static constexpr bool left_shift       = has_left_shift_operator<T>::value;
    static constexpr bool right_shift      = has_right_shift_operator<T>::value;

    // access
    static constexpr bool dereference      = has_dereference_operator<T>::value;
    static constexpr bool arrow            = has_arrow_operator<T>::value;
    static constexpr bool call             = has_call_operator<T>::value;

    // stream
    static constexpr bool stream_insert    = has_stream_insert_operator<T>::value;
    static constexpr bool stream_extract   = has_stream_extract_operator<T>::value;
};

// container_diagnostic
//   diagnostic: comprehensive container analysis.
template<typename _Type>
struct container_diagnostic
{
private:
    using T = clean_t<_Type>;

public:
    // container type detection
    static constexpr bool is_array              = is_array_like<T>::value;
    static constexpr bool is_vector             = is_vector_like<T>::value;
    static constexpr bool is_deque              = is_deque_like<T>::value;
    static constexpr bool is_list               = is_list_like<T>::value;
    static constexpr bool is_forward_list       = is_forward_list_like<T>::value;
    static constexpr bool is_set                = is_set_like<T>::value;
    static constexpr bool is_map                = is_map_like<T>::value;
    static constexpr bool is_multimap           = is_multimap_like<T>::value;
    static constexpr bool is_unordered_set      = is_unordered_set_like<T>::value;
    static constexpr bool is_unordered_map      = is_unordered_map_like<T>::value;

    // container categories
    static constexpr bool is_sequence           = is_sequence_container_like<T>::value;
    static constexpr bool is_ordered_assoc      = is_ordered_associative_like<T>::value;
    static constexpr bool is_unordered_assoc    = is_unordered_associative_like<T>::value;
    static constexpr bool is_associative        = is_associative_container_like<T>::value;
    static constexpr bool is_stl_container      = is_stl_container_like<T>::value;

    // alias groups
    static constexpr bool basic_aliases         = has_basic_container_aliases<T>::value;
    static constexpr bool reference_aliases     = has_reference_aliases<T>::value;
    static constexpr bool iterator_aliases      = has_iterator_aliases<T>::value;
    static constexpr bool reverse_iter_aliases  = has_reverse_iterator_aliases<T>::value;
    static constexpr bool local_iter_aliases    = has_local_iterator_aliases<T>::value;

    // method groups
    static constexpr bool element_access        = has_at<T>::value || has_subscript_operator<T>::value;
    static constexpr bool iterator_methods      = has_iterator_methods<T>::value;
    static constexpr bool reverse_iter_methods  = has_reverse_iterator_methods<T>::value;
    static constexpr bool capacity_methods      = has_capacity_methods<T>::value;
    static constexpr bool list_operations       = has_list_operations<T>::value;
    static constexpr bool bucket_interface      = has_bucket_interface<T>::value;

    // tuple protocol
    static constexpr bool tuple_protocol        = has_tuple_protocol<T>::value;

    // consistent types
    static constexpr bool consistent_aliases    = has_consistent_type_aliases<T>::value;
};


NS_END  // stl
NS_END  // container
NS_END  // djinterp


#endif  // DJINTERP_STL_TRAITS_