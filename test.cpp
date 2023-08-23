#include "big_integer.hpp"
#include <gtest/gtest.h>
#include <numeric>
#include <type_traits>
#include <sstream>
#include <string>

TEST(BASIC, CONSTRUCTORS) {
    BigInt from_empty;

    BigInt from_int(123'456);
    BigInt from_neg_int(-123456);

    BigInt from_string("12345678901234567890");
    BigInt from_neg_string("-123123123123123123123123132132132131231");

    BigInt copy_constr(from_string);

    bool value = std::is_constructible_v<BigInt, std::string>;
    EXPECT_TRUE(value);
    EXPECT_TRUE(std::is_copy_constructible_v<BigInt>);
    EXPECT_TRUE(std::is_copy_assignable_v<BigInt>);
}


template <typename T, typename U>
void TestEqual(T num1, U num2, bool ans = true) {
    BigInt a(num1);
    BigInt b(num2);

    EXPECT_EQ(a == b, ans) << "Failed on " << a << " and " << b;
}

TEST(COMPARE, EQUAL) {
    TestEqual(987'654, 987'654);
    TestEqual(-45678, -45678);
    TestEqual(0, 0);
    TestEqual("123456789123456789", "123456789123456789");
    TestEqual("-12345678901234567890", "-12345678901234567890");
    TestEqual(0, -0);
    TestEqual("0", "-0");

    TestEqual(987'654, 987'653, false);
    TestEqual(-45678, 45678, false);
    TestEqual(45678, -45678, false);
    TestEqual("123456779123456789", "123456789123456789", false);
    TestEqual("-12345678901234567890", "12345678901234567890", false);
    TestEqual("12345678901234567890", "-12345678901234567890", false);
}

template <typename T, typename U>
void TestNotEqual(T num1, U num2, bool ans = true) {
    BigInt a(num1);
    BigInt b(num2);

    EXPECT_EQ(a != b, ans) << "Failed on " << a << " and " << b;
}

TEST(COMPARE, NOT_EQUAL) {
    TestNotEqual(987'654, 987'653);
    TestNotEqual(-45678, 45678);
    TestNotEqual(45678, -45678);
    TestNotEqual("123456779123456789", "123456789123456789");
    TestNotEqual("-12345678901234567890", "12345678901234567890");
    TestNotEqual("12345678901234567890", "-12345678901234567890");
    TestNotEqual(987'654, 987'654, false);
    TestNotEqual(-45678, -45678, false);
    TestNotEqual(0, 0, false);
    TestNotEqual("123456789123456789", "123456789123456789", false);
    TestNotEqual("-12345678901234567890", "-12345678901234567890", false);
    TestNotEqual(0, -0, false);
    TestNotEqual("0", "-0", false);
}

template <typename T, typename U>
void TestLess(T num1, U num2, bool ans = true) {
    BigInt a(num1);
    BigInt b(num2);

    EXPECT_EQ(a < b, ans) << "Failed on " << a << " and " << b;
}

TEST(COMPARE, LESS) {
    TestLess(1, 5);
    TestLess(-100, 5);
    TestLess(1, -5, false);
    TestLess(-100, -5);
    TestLess(1, 1, false);
    TestLess("123456789012345678901234567890", "123456789012545678901234567890");
    TestLess("123456789012345678901234567890", "123456789012345678901234567890", false);
}

TEST(OPERATORS_SUM, BASIC) {
    BigInt a(123'123);
    BigInt b(456'456);

    BigInt c = a + b;

    EXPECT_EQ(a, 123'123);
    EXPECT_EQ(b, 456'456);
    EXPECT_EQ(c, (123'123 + 456'456));
}

TEST(OPERATORS_SUM, OVERFLOW_BASE) {
    BigInt a(999'999'999);
    BigInt b(999'999'999);

    BigInt c = a + b;

    EXPECT_EQ(a, 999'999'999);
    EXPECT_EQ(b, 999'999'999);
    EXPECT_EQ(c, 1'999'999'998);
}

TEST(OPERATORS_SUM, OVERFLOW_INT) {
    BigInt a(std::numeric_limits<int>::max());
    BigInt b(std::numeric_limits<int>::max());

    BigInt c = a + b;

    BigInt ans("4294967294");

    EXPECT_EQ(a, std::numeric_limits<int>::max());
    EXPECT_EQ(b, std::numeric_limits<int>::max());
    EXPECT_EQ(c, ans);
}


TEST(OPERATORS_MINUS, BASIC) {
    BigInt a(456'456);
    BigInt b(123'123);

    BigInt c = a - b;

    EXPECT_EQ(a, 456'456);
    EXPECT_EQ(b, 123'123);
    EXPECT_EQ(c, (456'456 - 123'123));
}

TEST(OPERATORS_MINUS, NEGATIVE_RES) {
    BigInt a(123'123);
    BigInt b(456'456);

    BigInt c = a - b;

    EXPECT_EQ(a, 123'123);
    EXPECT_EQ(b, 456'456);
    EXPECT_TRUE(c < 0);
    EXPECT_EQ(c, (123'123 - 456'456));
}

TEST(OPERATORS_MINUS, NEGATIVE_A) {
    BigInt a(-456'456);
    BigInt b(123'123);

    BigInt c = a - b;

    EXPECT_EQ(a, -456'456);
    EXPECT_EQ(b, 123'123);
    EXPECT_EQ(c, -(456'456 + 123'123));
}

TEST(OPERATORS_MINUS, UNDER_ZERO) {
    BigInt a(0);
    BigInt b(123'123'123'123);

    BigInt c = a - b;

    EXPECT_TRUE(c < 0);
    EXPECT_TRUE(c == -123'123'123'123);
}

TEST(OPERATORS_MINUS, UNDERFLOW) {
    BigInt a(std::numeric_limits<int>::min());
    BigInt b(std::numeric_limits<int>::max());

    BigInt c = a - b;

    int64_t ans = (int64_t)std::numeric_limits<int>::min() - std::numeric_limits<int>::max();

    EXPECT_EQ(a, std::numeric_limits<int>::min());
    EXPECT_EQ(b, std::numeric_limits<int>::max());
    EXPECT_EQ(c, ans);
}

TEST(OPERATORS_MINUS, GIGA_UNDERFLOW) {
    BigInt a(std::numeric_limits<int64_t>::min());
    BigInt b(std::numeric_limits<int64_t>::max());

    BigInt ans("-18446744073709551615");

    BigInt c = a - b;

    EXPECT_EQ(a, std::numeric_limits<int64_t>::min());
    EXPECT_EQ(b, std::numeric_limits<int64_t>::max());
    EXPECT_EQ(c, ans);
}

template <typename T, typename U>
void TestCombo(T num1, U num2) {
    BigInt a(num1);
    BigInt b(num2);

    EXPECT_EQ(a + b, num1 + num2) << " with " << a << " " << b;
    EXPECT_EQ(a - b, num1 - num2) << " with " << a << " " << b;
}

TEST(COMBOS, PLUS_MINUS) {
    TestCombo(2, 4);
    TestCombo(2, -4);
    TestCombo(-2, 4);
    TestCombo(-2, -4);
    TestCombo(0, 4);
    TestCombo(0, -4);
    TestCombo(4, 0);
    TestCombo(-4, 0);
}

template <typename T, typename U>
void TestMul(T num1, U num2) {
    BigInt a(num1);
    BigInt b(num2);

    EXPECT_EQ(a * b, num1 * num2) << " with " << a << " " << b;
}

TEST(MUL, BASIC) {
    TestMul(42, 228);
    TestMul(-42, 228);
    TestMul(42, -228);
    TestMul(-42, -228);

    TestMul(1, 1234567);
    TestMul(1234567, 1);
    TestMul(0, 1234567890123456789);
    TestMul(1234567890123456789, 0);
    TestMul(0, -123);
}

TEST(MUL, BIG_NUMS) {
    {
        BigInt a(1234567890123456789);
        BigInt b(1234567890123456789);

        BigInt c("1524157875323883675019051998750190521");

        EXPECT_EQ(a * b, c);
    }

    {
        BigInt a(-1234567890123456789);
        BigInt b(1234567890123456789);

        BigInt c("-1524157875323883675019051998750190521");

        EXPECT_EQ(a * b, c);
    }

    {
        BigInt a(1234567890123456789);
        BigInt b(-1234567890123456789);

        BigInt c("-1524157875323883675019051998750190521");

        EXPECT_EQ(a * b, c);
    }

    {
        BigInt a(-1234567890123456789);
        BigInt b(-1234567890123456789);

        BigInt c("1524157875323883675019051998750190521");

        EXPECT_EQ(a * b, c);
    }
}

template <typename T, typename U>
void TestDiv(T num1, U num2) {
    BigInt a(num1);
    BigInt b(num2);

    EXPECT_EQ(a / b, num1 / num2) << " with " << a << " " << b;
}

TEST(DIV, BASIC) {
    TestDiv(4, 2);
    TestDiv(7876521, 123);

    TestDiv(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min());
    TestDiv(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());

    TestDiv(123456789, 1);
    TestDiv(1, 123456789);
}

TEST(DIV, BIG) {
    BigInt a("12345123456789012345678923456789123534645723452363465473643423");
    BigInt b("12568432423758325345984738557347237543");

    BigInt c("982232552203790490610772");
    EXPECT_EQ(a / b, c);
}

template <typename T, typename U>
void TestMod(T num1, U num2) {
    BigInt a(num1);
    BigInt b(num2);

    EXPECT_EQ(a % b, num1 % num2) << " with " << a << " " << b;
}

TEST(MOD, DISCORD) {
    TestMod(4, 2);
    TestMod(7876521, 123);

    TestMod(std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::min());
    TestMod(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());

    TestMod(123456789, 1);
    TestMod(1, 123456789);
}

TEST(MOD, BIG) {
    BigInt a("12345123456789012345678923456789123534645723452363465473643423");
    BigInt b("12568432423758325345984738557347237543");

    BigInt c("7378391778761293146339181012435030227");
    EXPECT_EQ(a % b, c);
}

TEST(IO, BASIC) {
    std::istringstream iss("1234567890123456789012345 -1234567890123456789012");
    std::ostringstream oss;

    BigInt a;
    BigInt b;
    iss >> a >> b;

    oss << a << ' ' << b << '\n';

    EXPECT_EQ(oss.str(), "1234567890123456789012345 -1234567890123456789012\n");
}

TEST(UNARY, MINUS) {
    BigInt a(123);

    EXPECT_EQ(a, 123);
    EXPECT_EQ(-a, -123);

    BigInt zero(0);

    EXPECT_EQ(zero, 0);
    EXPECT_EQ(-zero, 0);
    EXPECT_EQ(-(-(-zero)), 0);
}

template <typename T>
void TestIncrement(T num1) {
    BigInt a(num1);

    EXPECT_EQ(++a, ++num1);
    EXPECT_EQ(a++, num1++);
    EXPECT_EQ(a, num1);
}

TEST(INCREMENT, BASIC) {
    TestIncrement(123);
    TestIncrement(-1);
    TestIncrement(1e9 - 1);
}

template <typename T>
void TestDecrement(T num1) {
    BigInt a(num1);

    EXPECT_EQ(--a, --num1);
    EXPECT_EQ(a--, num1--);
    EXPECT_EQ(a, num1);
}

TEST(DECREMENT, BASIC) {
    TestDecrement(123);
    TestDecrement(1);
    TestDecrement(1e9 + 1);
}


int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

