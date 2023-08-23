#pragma once
#include <iostream>
#include <string>
#include <vector>

class BigInt {
 private:
  static const int kBase = 1'000'000'000;
  static const int kMagicNum = 10;
  static const int kLengthBase = 9;
  bool sign_ = true;
  size_t size_ = 0;
  std::vector<int> number_;

 public:
  BigInt();
  BigInt(std::string num_str);
  BigInt(int64_t num);
  BigInt(const BigInt& num);

  BigInt(BigInt&& num);
  BigInt& operator=(BigInt&& num);

  BigInt& operator=(const BigInt& num);
  BigInt& operator+=(const BigInt& num);
  BigInt& operator-=(const BigInt& num);
  BigInt& operator*=(const BigInt& num);
  BigInt& operator/=(const BigInt& num);
  BigInt& operator%=(const BigInt& num);
  BigInt& DivIoTwo();
  BigInt operator-() const;
  BigInt operator++();
  BigInt operator++(int);
  BigInt operator--();
  BigInt operator--(int);
  void DeleteZeros();
  BigInt Diff(const BigInt& num2) const;
  BigInt Plus(const BigInt& num2) const;
  friend bool operator<(const BigInt& num1, const BigInt& num2);
  friend BigInt operator+(const BigInt& num1, const BigInt& num2);
  friend BigInt operator-(const BigInt& num1, const BigInt& num2);
  friend std::ostream& operator<<(std::ostream& out, const BigInt& num);
  friend std::istream& operator>>(std::istream& is, BigInt& num);
  // operator int() const;
};

BigInt operator%(const BigInt& num1, const BigInt& num2);
int ToInt(const std::string& str);
void DeleteZerosStr(std::string& str);
bool operator==(const BigInt& num1, const BigInt& num2);
bool operator!=(const BigInt& num1, const BigInt& num2);
bool operator>(const BigInt& num1, const BigInt& num2);
bool operator>=(const BigInt& num1, const BigInt& num2);
bool operator<=(const BigInt& num1, const BigInt& num2);
BigInt operator*(const BigInt& num1, const BigInt& num2);
BigInt operator/(const BigInt& num1, const BigInt& num2);
