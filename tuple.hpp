#include <iostream>
#include <utility>
#include <type_traits>
#include <tuple>

// struct Empty {};


template <std::size_t I, typename... Ts>
struct nth_type_impl;

template <std::size_t I, typename Head, typename... Tail>
struct nth_type_impl<I, Head, Tail...> {
    using type = typename nth_type_impl<I - 1, Tail...>::type;
};

template <typename Head, typename... Tail>
struct nth_type_impl<0, Head, Tail...> {
    using type = Head;
};

template <std::size_t I, typename... Ts>
using nth_type = typename nth_type_impl<I, Ts...>::type;



template <std::size_t I, typename T, bool = std::is_empty_v<T> && !std::is_final_v<T>>
struct TupleElement;


template <std::size_t I, typename T>
struct TupleElement<I, T, false> {
    T value;

    TupleElement() = default;

    template <typename U>
    TupleElement(U&& v) : value(std::forward<U>(v)) {}

    T& get() noexcept { return value; }
    const T& get() const noexcept { return value; }
};


template <std::size_t I, typename T>
struct TupleElement<I, T, true> : private T {
    TupleElement() = default;

    template <typename U>
    TupleElement(U&& v) : T(std::forward<U>(v)) {}

    T& get() noexcept { return *this; }
    const T& get() const noexcept { return *this; }
};



template <typename IndexSequence, typename... Ts>
struct TupleImplementation;


template <std::size_t... Is, typename... Ts>
struct TupleImplementation<std::index_sequence<Is...>, Ts...>
    : TupleElement<Is, Ts>...
{
    TupleImplementation() = default;

    template <typename... Args>
    TupleImplementation(Args&&... args)
    : TupleElement<Is, Ts>(std::forward<Args>(args))... {}

    template <std::size_t I>
    auto& get() noexcept {
        using T = nth_type<I, Ts...>;
        return TupleElement<I, T>::get();
    }

    template <std::size_t I>
    const auto& get() const noexcept {
        using T = nth_type<I, Ts...>;
        return TupleElement<I, T>::get();
    }
};



template <typename... Ts>
struct my_tuple : TupleImplementation<std::index_sequence_for<Ts...>, Ts...> {
    using Base = TupleImplementation<std::index_sequence_for<Ts...>, Ts...>;
    using Base::Base;
};


template <std::size_t I, typename... Ts>
auto& get(my_tuple<Ts...>& t) noexcept {
    return t.template get<I>();
}

template <std::size_t I, typename... Ts>
const auto& get(const my_tuple<Ts...>& t) noexcept {
    return t.template get<I>();
}



template <typename... Args>
auto make_my_tuple(Args&&... args) {
    return my_tuple<std::decay_t<Args>...>(std::forward<Args>(args)...);
}



template <typename Tuple>
struct tuple_size;

template <typename... Types>
struct tuple_size<my_tuple<Types...>>
    : std::integral_constant<std::size_t, sizeof...(Types)>
{};



template <typename... Args>
auto my_tie(Args&... args) noexcept {
    return my_tuple<Args&...>(args...);
}

namespace std {

template <typename... Ts>
struct tuple_size<my_tuple<Ts...>>
    : std::integral_constant<std::size_t, sizeof...(Ts)> {};

template <size_t I, typename... Ts>
struct tuple_element<I, my_tuple<Ts...>> {
    using type = nth_type<I, Ts...>;
};

}
