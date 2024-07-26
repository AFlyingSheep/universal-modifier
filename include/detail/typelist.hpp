#ifndef DETAIL_TYPELIST_HPP
#define DETAIL_TYPELIST_HPP

#include <cstddef>
#include <type_traits>
template <typename... Elements>
class Typelist {};

// Typelist
template <typename List>
class FrontT {
 public:
  using Type = typename List::Head;
};
template <typename Head, typename... Tail>
class FrontT<Typelist<Head, Tail...>> {
 public:
  using Type = Head;
};

template <typename List>
using Front = typename FrontT<List>::Type;

template <typename List>
class PopFrontT {
 public:
  using Type = typename List::Tail;
};
template <typename Head, typename... Tail>
class PopFrontT<Typelist<Head, Tail...>> {
 public:
  using Type = Typelist<Tail...>;
};

template <typename List>
using PopFront = typename PopFrontT<List>::Type;

template <typename List, typename NewElement>
class PushFrontT;
template <typename... Elements, typename NewElement>
class PushFrontT<Typelist<Elements...>, NewElement> {
 public:
  using Type = Typelist<NewElement, Elements...>;
};
template <typename List, typename NewElement>
using PushFront = typename PushFrontT<List, NewElement>::Type;

// Typelist checking
template <typename List>
class IsEmpty {
 public:
  constexpr static bool value = false;
};

template <>
class IsEmpty<Typelist<>> {
 public:
  constexpr static bool value = true;
};

template <typename List>
class LengthT;

template <>
class LengthT<Typelist<>> {
  static constexpr std::size_t value = 0;
};

template <typename Head, typename... Tail>
class LengthT<Typelist<Head, Tail...>> {
 public:
  static constexpr std::size_t value = 1 + LengthT<Typelist<Tail...>>::value;
};

template <typename List>
using Length = LengthT<List>;

#endif  // DETAIL_TYPELIST_HPP
