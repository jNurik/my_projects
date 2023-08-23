#include "big_integer.hpp"

#include <cmath>

BigInt::BigInt() = default;

void DeleteZerosStr(std::string& str) {
  if (str.size() == 1) {
    return;
  }
  while (str[0] == '0') {
    str.erase(str.begin());
  }
}

int ToInt(const std::string& str) {
  int ans = 0;
  const int kMagicNum = 10;
  const int kCurr = 48;
  for (size_t i = 0; i < str.size(); ++i) {
    ans += pow(kMagicNum, i) * (str[str.size() - 1 - i] - kCurr);
  }
  return ans;
}

BigInt::BigInt(std::string str) {
  if (str.empty() || (str.size() == 1 && str[0] == '-')) {
    throw std::invalid_argument("invalid argument");
  }
  DeleteZerosStr(str);
  if (str.size() == 2 && str[0] == '-' && str[1] == '0') {
    size_ = 1;
    sign_ = true;
    number_.push_back(0);
  } else {
    if (str[0] == '-') {
      sign_ = false;
      str.erase(str.begin());
    }
    size_ = str.size() / kLengthBase +
        static_cast<unsigned long>(str.size() % kLengthBase != 0);
    for (long long i = str.size(); i > 0; i -= kLengthBase) {
      if (i < kLengthBase) {
      number_.push_back(ToInt(str.substr(0, i)));
      } else {
      number_.push_back(ToInt(str.substr(i - kLengthBase, kLengthBase)));
      }
    }
  }
}

BigInt::BigInt(int64_t num) : sign_(num > 0) {
  sign_ = (num >= 0);
  if (num == 0) {
    number_.push_back(0);
    size_ = 1;
  } else {
    while (num != 0) {
      number_.push_back(std::abs(num % kBase));
      num /= kBase;
      ++size_;
    }
  }
}

BigInt::BigInt(const BigInt& num) : sign_(num.sign_), size_(num.size_), number_(size_) {
  for (size_t i = 0; i < size_; ++i) {
  number_[i] = num.number_[i];
  }
}

BigInt& BigInt::operator=(const BigInt& num) {
  sign_ = num.sign_;
  size_ = num.size_;
  number_.resize(size_);
  for (size_t i = 0; i < size_; ++i) {
    number_[i] = num.number_[i];
  }
  return *this;
}

BigInt::BigInt(BigInt&& num) : sign_(num.sign_), size_(num.size_), number_(std::move(num.number_)) {
  num.sign_ = true;
  num.size_ = 0;
}

BigInt& BigInt::operator=(BigInt&& num) {
  sign_ = num.sign_;
  size_ = num.size_;
  number_ = std::move(num.number_);
  num.sign_ = true;
  num.size_ = 0;
  return *this;
}

bool operator<(const BigInt& num1, const BigInt& num2) {
  if (static_cast<int>(num1.sign_) < static_cast<int>(num2.sign_)) {
    return true;
  }
  if (static_cast<int>(num1.sign_) > static_cast<int>(num2.sign_)) {
    return false;
  }

  if (num1.number_.size() < num2.number_.size()) {
    return num1.sign_;
  }
  if (num1.number_.size() > num2.number_.size()) {
    return !(num1.sign_);
  }

  for (size_t i = num1.number_.size(); i > 0; --i) {
    if (num1.number_[i - 1] < num2.number_[i - 1]) {
      return num1.sign_;
    }
    if (num1.number_[i - 1] > num2.number_[i - 1]) {
      return !(num1.sign_);
    }
  }
  return false;
}

bool operator>(const BigInt& num1, const BigInt& num2) { return num2 < num1; }

bool operator==(const BigInt& num1, const BigInt& num2) {
  return (num1 <= num2) && (num1 >= num2);
}

bool operator!=(const BigInt& num1, const BigInt& num2) {
  return !(num1 == num2);
}

bool operator>=(const BigInt& num1, const BigInt& num2) {
  return !(num1 < num2);
}

bool operator<=(const BigInt& num1, const BigInt& num2) {
  return !(num1 > num2);
}

BigInt BigInt::operator-() const {
  BigInt ans = *this;
  if (number_[0] == 0 && number_.size() == 1) {
    return ans;
  }
  ans.sign_ = !ans.sign_;
  return ans;
}

BigInt operator+(const BigInt& num1, const BigInt& num2) {
  if (num1.sign_ && num2.sign_) {
    if (num1 < num2) {
      return num2 + num1;
    }
    BigInt ans = num1.Plus(num2);
    return ans;
  }
  if (num1.sign_ && !num2.sign_) {
    return num1 - (-(num2));
  }
  if (!num1.sign_ && num2.sign_) {
    return num2 - (-(num1));
  }
  return -(-(num2) + (-(num1)));
}

BigInt BigInt::Diff(const BigInt& num2) const {
  BigInt ans = *this;
  bool transfer = false;
  for (size_t i = 0; i < ans.number_.size(); ++i) {
  if (i < num2.number_.size()) {
    int tmp = ans.number_[i];
    ans.number_[i] = (ans.kBase + ans.number_[i] - num2.number_[i] -
            static_cast<int>(transfer)) %
           ans.kBase;
    transfer = tmp < (num2.number_[i] + static_cast<int>(transfer));
  }
  if (i >= num2.number_.size() && transfer) {
    int tmp = ans.number_[i];
    ans.number_[i] =
      (ans.kBase + ans.number_[i] - static_cast<int>(transfer)) % ans.kBase;
    transfer = (tmp == 0);
  }
  }
  ans.DeleteZeros();
  return ans;
}

BigInt BigInt::Plus(const BigInt& num2) const {
  BigInt ans = *this;
  bool transfer = false;
  for (size_t i = 0; i < ans.number_.size(); ++i) {
  if (i < num2.number_.size()) {
    int tmp = number_[i] + num2.number_[i] + static_cast<int>(transfer);
    ans.number_[i] = tmp % ans.kBase;
    transfer = tmp >= ans.kBase;
  }
  if (i >= num2.number_.size() && transfer) {
    int tmp = ans.number_[i];
    ans.number_[i] =
      (ans.number_[i] + static_cast<int>(transfer)) % ans.kBase;
    transfer = (static_cast<int>(transfer) + tmp) >= ans.kBase;
  }
  }
  if (transfer) {
  ans.number_.push_back(1);
  ++ans.size_;
  }
  return ans;
}

BigInt operator-(const BigInt& num1, const BigInt& num2) {
  if (num1.sign_ && num2.sign_) {
  if (num1 < num2) {
    return -(num2 - num1);
  }
  BigInt ans = num1.Diff(num2);
  return ans;
  }
  if (num1.sign_ && !num2.sign_) {
    return num1 + (-(num2));
  }
  if (!num1.sign_ && num2.sign_) {
    return -(num2 + (-(num1)));
  }
  return -(num2) - (-(num1));
}

void BigInt::DeleteZeros() {
  while (number_.size() > 1 && number_.back() == 0) {
    number_.pop_back();
    --size_;
  }
}

BigInt& BigInt::operator+=(const BigInt& num) {
  *this = *this + num;
  return (*this);
}

BigInt& BigInt::operator-=(const BigInt& num) {
  *this = *this - num;
  return *this;
}

BigInt BigInt::operator++(void) { return (*this += 1); }

BigInt BigInt::operator++(int) {
  *this += 1;
  return *this - 1;
}

BigInt BigInt::operator--(void) { return (*this -= 1); }

BigInt BigInt::operator--(int) {
  *this -= 1;
  return *this + 1;
}

std::ostream& operator<<(std::ostream& out, const BigInt& num) {
  if (num.number_.empty()) {
    throw("empty");
  }
  if (!num.sign_) {
    out << '-';
  }
  out << num.number_.back();
  for (size_t i = num.size_; i > 1; --i) {
    for (int j = 1; num.number_[i - 2] < num.kBase / pow(num.kMagicNum, j);
      ++j) {
      out << 0;
    }
    out << num.number_[i - 2];
  }
  return out;
}

std::istream& operator>>(std::istream& is, BigInt& num) {
  std::string str;
  is >> str;
  num = BigInt(str);
  return is;
}

BigInt& BigInt::operator*=(const BigInt& num) {
  std::vector<long long> ans;
  ans.resize(size_ + num.size_);
  sign_ = (sign_ == num.sign_);
  if ((number_[0] == 0 && size_ == 1) ||
    (num.number_[0] == 0 && num.size_ == 1)) {
    sign_ = true;
  }
  for (size_t i = 0; i < size_; ++i) {
    for (size_t j = 0; j < num.number_.size(); ++j) {
      long long tmp = 0;
      tmp = static_cast<long long>(number_[i]) *
        static_cast<long long>(num.number_[j]);
      ans[i + j] += tmp;
    }
  }
  size_ += num.size_;
  number_.resize(size_);
  long long transfer = 0;
  for (size_t j = 0; j < ans.size(); ++j) {
    number_[j] = (ans[j] + transfer) % kBase;
    transfer = (ans[j] + transfer) / kBase;
  }
  DeleteZeros();
  return *this;
}

BigInt operator*(const BigInt& num1, const BigInt& num2) {
  BigInt ans = num1;
  ans *= num2;
  return ans;
}

BigInt& BigInt::DivIoTwo() {
  const int kHalfBase = 500000000;
  for (size_t i = 0; i < number_.size(); ++i) {
    number_[i] /= 2;
    if (i + 1 < number_.size() && (number_[i + 1] % 2 == 1)) {
      number_[i] += kHalfBase;
    }
  }

  DeleteZeros();
  return *this;
}

BigInt BinSearch(BigInt first, BigInt last, const BigInt& num,
         const BigInt& value) {
  BigInt middle = value;
  middle.DivIoTwo();
  while (last - 1 > first) {
    if (middle * num <= value) {
      first = middle;
    } else if (middle * num > value) {
      last = middle;
    }
    middle = first + last;
    middle.DivIoTwo();
    middle.DeleteZeros();
  }
  return first;
}

BigInt& BigInt::operator/=(const BigInt& num) {
  bool sign = num.sign_ == sign_;
  sign_ = true;
  if ((num.sign_ && num > *this) || (!num.sign_ && -(num) > *this)) {
    number_.resize(1);
    number_[0] = 0;
    sign_ = true;
    size_ = 1;
    return *this;
  }
  BigInt first(-1);
  BigInt last = *this + 1;
  if (num.sign_) {
    *this = BinSearch(first, last, num, *this);
  } else {
    *this = BinSearch(first, last, -(num), *this);
  }
  sign_ = sign;
  return *this;
}

BigInt operator/(const BigInt& num1, const BigInt& num2) {
  BigInt ans = num1;
  ans /= num2;
  return ans;
}

BigInt operator%(const BigInt& num1, const BigInt& num2) {
  return (num1 - (num1 / num2) * num2);
}

BigInt& BigInt::operator%=(const BigInt& num) {
  *this = *this % num;
  return *this;
}

// BigInt::operator int() const {

// }
