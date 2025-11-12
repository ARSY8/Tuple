#include <iostream>
#include <tuple>
#include <utility>
#include <type_traits>

struct Empty {};

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
struct TupleImplementation<std::index_sequence<Is...>, Ts...> : TupleElement<Is, Ts>... {
    TupleImplementation() = default;

    
    TupleImplementation(const Ts&... args)
        : TupleElement<Is, Ts>(args)... {}

    TupleImplementation(Ts&&... args)
        : TupleElement<Is, Ts>(std::move(args))... {}

    template <std::size_t I>
    auto& get() noexcept {
        return TupleElement<I, std::tuple_element_t<I, std::tuple<Ts...>>>::get();
    }

    template <std::size_t I>
    const auto& get() const noexcept {
        return TupleElement<I, std::tuple_element_t<I, std::tuple<Ts...>>>::get();
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
struct tuple_size<my_tuple<Types...>> : std::integral_constant<std::size_t, sizeof...(Types)> {};

//tie

int main() {
    my_tuple<int, Empty, double, Empty, int> t(1, {}, 3.14, {}, 42);

    std::cout << "sizeof(my_tuple<int, Empty, double, Empty, int>) = "
        << sizeof(t) << '\n';
    std::cout << get<0>(t) << ", " << get<2>(t) << ", " << get<4>(t) << '\n';


    std::cout << "sizeof(Empty) = " << sizeof(Empty) << '\n';

    std::cout << "sizeof(std::tuple<int>) = " << sizeof(std::tuple<int>) << '\n';
    std::cout << "sizeof(my_tuple<int>) = " << sizeof(my_tuple<int>) << '\n';

    std::cout << "sizeof(std::tuple<int, Empty>) = " << sizeof(std::tuple<int, Empty>) << '\n';
    std::cout << "sizeof(my_tuple<int, Empty>) = " << sizeof(my_tuple<int, Empty>) << '\n';

    std::cout << "sizeof(std::tuple<int, Empty, double>) = " << sizeof(std::tuple<int, Empty, double>) << '\n';
    std::cout << "sizeof(my_tuple<int, Empty, double>) = " << sizeof(my_tuple<int, Empty, double>) << '\n';

    std::cout << "sizeof(std::tuple<int, Empty, double, Empty, int>) = "
        << sizeof(std::tuple<int, Empty, double, Empty, int>) << '\n';
    std::cout << "sizeof(my_tuple<int, Empty, double, Empty, int>) = "
        << sizeof(my_tuple<int, Empty, double, Empty, int>) << '\n';

    std::cout << "sizeof(std::tuple<int, int, double, Empty, Empty>) = "
        << sizeof(std::tuple<int, int, double, Empty, Empty>) << '\n';
    std::cout << "sizeof(my_tuple<int, int, double, Empty, Empty>) = "
        << sizeof(my_tuple<int, int, double, Empty, Empty>) << '\n';
}