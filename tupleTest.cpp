#include "gtest/gtest.h"
#include "../src/tuple.hpp"
#include <iostream>

struct Empty {};
struct NonEmpty { int x; };

struct MoveOnly {
    int value;
    MoveOnly(int v) : value(v) {}
    MoveOnly(const MoveOnly&) = delete;
    MoveOnly(MoveOnly&& other) noexcept : value(other.value) { other.value = -1; }
};

TEST(MyTupleTest, EBOSingleEmpty) {
    my_tuple<Empty> t(Empty{});
    EXPECT_EQ(sizeof(t), 1); 
}

TEST(MyTupleTest, CompareSizes1_Basic) {
    using A = std::tuple<int, Empty, double>;
    using B = my_tuple<int, Empty, double>;
    
    std::cout << "  std::tuple<int, Empty, double>: " << sizeof(A) << " bytes" << std::endl;
    std::cout << "  my_tuple<int, Empty, double>:  " << sizeof(B) << " bytes" << std::endl;
    
    EXPECT_LE(sizeof(B), sizeof(A));
}


TEST(MyTupleTest, CompareSizes2_MultipleEmpty) {
    using A = std::tuple<Empty, Empty, Empty, int>;
    using B = my_tuple<Empty, Empty, Empty, int>;
    
    std::cout << "  std::tuple<Empty, Empty, Empty, int>: " << sizeof(A) << " bytes" << std::endl;
    std::cout << "  my_tuple<Empty, Empty, Empty, int>:  " << sizeof(B) << " bytes" << std::endl;
    
    EXPECT_LE(sizeof(B) + 4, sizeof(A));
}

TEST(MyTupleTest, CompareSizes4_MixedAlignment) {
    using A = std::tuple<char, Empty, int, Empty, double>;
    using B = my_tuple<char, Empty, int, Empty, double>;
    
    std::cout << "  std::tuple<char, Empty, int, Empty, double>: " << sizeof(A) << " bytes" << std::endl;
    std::cout << "  my_tuple<char, Empty, int, Empty, double>:  " << sizeof(B) << " bytes" << std::endl;
    EXPECT_LE(sizeof(B), sizeof(A));
}

TEST(MyTupleTest, CompareSizes5_OnlyEmpty) {
    using A = std::tuple<Empty, Empty>;
    using B = my_tuple<Empty, Empty>;
    
    std::cout << "  std::tuple<Empty, Empty>: " << sizeof(A) << " bytes" << std::endl;
    std::cout << "  my_tuple<Empty, Empty>:  " << sizeof(B) << " bytes" << std::endl;
    
    EXPECT_LE(sizeof(B), sizeof(A));
}

TEST(MyTupleTest, CompareSizes6_SingleEmpty) {
    using A = std::tuple<Empty>;
    using B = my_tuple<Empty>;
    
    std::cout << "  std::tuple<Empty>: " << sizeof(A) << " bytes" << std::endl;
    std::cout << "  my_tuple<Empty>:  " << sizeof(B) << " bytes" << std::endl;
    
    EXPECT_LE(sizeof(B), sizeof(A));
}

TEST(MyTupleTest, CompareSizes7_WithLargeTypes) {
    struct Large { char data[32]; };
    using A = std::tuple<Empty, Large, Empty>;
    using B = my_tuple<Empty, Large, Empty>;
    
    std::cout << "  std::tuple<Empty, Large, Empty>: " << sizeof(A) << " bytes" << std::endl;
    std::cout << "  my_tuple<Empty, Large, Empty>:  " << sizeof(B) << " bytes" << std::endl;
    
    EXPECT_LE(sizeof(B), sizeof(A));
}

TEST(MyTupleTest, CompareSizes8_ManyMixedTypes) {
    using A = std::tuple<char, short, Empty, int, Empty, long, Empty, double>;
    using B = my_tuple<char, short, Empty, int, Empty, long, Empty, double>;
    
    std::cout << "  std::tuple<char, short, Empty, int, Empty, long, Empty, double>: " << sizeof(A) << " bytes" << std::endl;
    std::cout << "  my_tuple<char, short, Empty, int, Empty, long, Empty, double>:  " << sizeof(B) << " bytes" << std::endl;
    
    EXPECT_LE(sizeof(B), sizeof(A));
}

TEST(MyTupleTest, EmptyTupleSizeof) {
    my_tuple<> t;
    EXPECT_EQ(sizeof(t), 1);
    EXPECT_EQ(tuple_size<my_tuple<>>::value, 0u);
}


TEST(MyTupleTest, ManyElements) {
    my_tuple<int, double, char, long, short, float> t(1, 2.5, 'c', 100L, (short)3, 4.5f);

    EXPECT_EQ(get<0>(t), 1);
    EXPECT_DOUBLE_EQ(get<1>(t), 2.5);
    EXPECT_EQ(get<2>(t), 'c');
    EXPECT_EQ(get<3>(t), 100L);
    EXPECT_EQ(get<4>(t), 3);
    EXPECT_FLOAT_EQ(get<5>(t), 4.5f);
}

TEST(MyTupleTest, Range) {
    std::vector<my_tuple<int, std::string>> a = {{1, "abc"}, {2, "fdad"}};
    for (auto&& [val, second]: a) {
        std::cerr << second;
    }
}

TEST(MyTupleTest, CopyConstructor) {
    my_tuple<int, double> a(7, 9.0);
    my_tuple<int, double> b(a);

    EXPECT_EQ(get<0>(b), 7);
    EXPECT_DOUBLE_EQ(get<1>(b), 9.0);
}

TEST(MyTupleTest, MoveConstructorOfMoveOnlyType) {
    my_tuple<MoveOnly> t1(MoveOnly(10));
    my_tuple<MoveOnly> t2(std::move(t1));

    EXPECT_EQ(get<0>(t2).value, 10);
    EXPECT_EQ(get<0>(t1).value, -1);
}


TEST(MyTupleTest, GetFromConstTuple) {
    const my_tuple<int, char> t(5, 'a');
    EXPECT_EQ(get<0>(t), 5);
    EXPECT_EQ(get<1>(t), 'a');
}

TEST(MyTupleTest, TieResultsInReferenceTypes) {
    int a = 1;
    int b = 2;

    auto t = my_tie(a, b);

    static_assert(std::is_same_v<decltype(t), my_tuple<int&, int&>>);

    get<0>(t) = 10;
    EXPECT_EQ(a, 10);
}

TEST(MyTupleTest, NestedTuples) {
    my_tuple<int, my_tuple<char, double>> t(1, my_tuple<char, double>('x', 3.14));

    EXPECT_EQ(get<0>(t), 1);
    EXPECT_EQ(get<0>(get<1>(t)), 'x');
    EXPECT_DOUBLE_EQ(get<1>(get<1>(t)), 3.14);
}

struct Big {
    int data[100];
};

TEST(MyTupleTest, BigStructs) {
    my_tuple<Big, Big> t(Big{}, Big{});
    EXPECT_EQ(sizeof(t), sizeof(Big) * 2);
}

TEST(MyTupleTest, DoesNotThrow) {
    EXPECT_NO_THROW((my_tuple<int, double>(1, 2.0) ));
}

TEST(MyTupleTest, OrderCheck) {
    my_tuple<int, int, int> t(1, 2, 3);
    EXPECT_EQ(get<0>(t), 1);
    EXPECT_EQ(get<1>(t), 2);
    EXPECT_EQ(get<2>(t), 3);
}

TEST(MyTupleTest, ConstTupleAccess) {
    const my_tuple<int, double> t(10, 2.5);

    EXPECT_EQ(get<0>(t), 10);
    EXPECT_DOUBLE_EQ(get<1>(t), 2.5);
}

TEST(MyTupleTest, MoveOnlyMultiple) {
    my_tuple<int, MoveOnly, char> t(1, MoveOnly(7), 'x');

    EXPECT_EQ(get<1>(t).value, 7);
}

TEST(MyTupleTest, FromTemporary) {
    auto t = my_tuple<int, double>(5, 6.7);
    EXPECT_EQ(get<0>(t), 5);
    EXPECT_DOUBLE_EQ(get<1>(t), 6.7);
}