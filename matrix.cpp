#include <iostream>
#include <vector>
#include "rational.cpp"
#include "requires.hpp"
#include <type_traits>


//! defenitions
template <size_t N>
class Field;

template <size_t M, size_t N, class Field = Rational>
class Matrix;

template <typename T>
concept is_powebal = 
    requires(T x, T y) { x * y; };


//! some usefull function
template <typename T> requires is_powebal<T>
T pow(T number, int deg) {
  if (deg == 0) {
    return 1;
  }
  else if (deg % 2 == 1) {
    return pow(number, deg - 1) * number;
  }
  T tmp = pow(number, deg / 2);
  return tmp * tmp;
}

enum error {
  ok = true,
  error = false
};

template <typename T>
std::pair<size_t, bool> first_not_zero_elements(size_t begin, const std::vector<T>& vector) {
  std::pair ans{0, ok};
  for (size_t i = begin; i < vector.size(); ++i)  {
    if (vector[i] != 0) return ans;
    ++ans.first;
  }
  ans.second = error;
  return ans;
}

//! operators on Field
template <size_t N>
bool operator<(const Field<N>& object1, const Field<N>& object2) {
  return object1.elem_ < object2.elem_;
}
template <size_t N>
bool operator>(const Field<N>& object1, const Field<N>& object2) {
  return object2 < object1;
}
template <size_t N>
bool operator<=(const Field<N>& object1, const Field<N>& object2) {
  return !(object2 < object1);
}
template <size_t N>
bool operator>=(const Field<N>& object1, const Field<N>& object2) {
  return !(object1 < object2);
}
template <size_t N>
bool operator==(const Field<N>& object1, const Field<N>& object2) {
  return object1.elem_ == object2.elem_;
}
template <size_t N>
bool operator!=(const Field<N>& object1, const Field<N>& object2) {
  return !(object1 == object2);
}

template <size_t N>
Field<N> operator+(Field<N> elem1, const Field<N>& elem2) {
  return (elem1 += elem2);
}
template <size_t N>
Field<N> operator-(Field<N> elem1, const Field<N>& elem2) {
  return (elem1 -= elem2);
}
template <size_t N>
Field<N> operator*(Field<N> elem1, const Field<N>& elem2) {
  return (elem1 *= elem2);
}
template <size_t N>
Field<N> operator/(Field<N> elem1, const Field<N>& elem2) {
  static_assert(is_prime_v<N>, "N isn't prime");
  return (elem1 /= elem2);
}

//! class field
template <size_t N>
class Field {
  private:
  size_t elem_;

  size_t mod_N(long long x) const noexcept {
    return ((x % N) + static_cast<int>(N)) % N;
  }

  size_t get_inverse_mod_N() const noexcept {
    static_assert(is_prime_v<N>, "N isn't prime");
    return mod_N(pow(elem_, N - 2));
  }

  public:
  Field() = default;
  Field(const Field&) = default;
  Field(Field&& other) : elem_(other.elem_) { other.elem_ = 0; }
  Field& operator=(const Field&) = default;
  Field& operator=(Field&& other) {
    elem_ = other.elem_;
    other.elem_ = 0;
    return *this;
  }
  ~Field() = default;

  Field(long long number) : elem_(mod_N(number)) {}

  Field& operator+=(const Field& other) noexcept {
    elem_ = mod_N(elem_ + other.elem_);
    return *this;
  }

  Field& operator-=(const Field& other) noexcept {
    elem_ = mod_N(elem_ - other.elem_);
    return *this;
  }

  Field& operator*=(const Field& other) noexcept {
    elem_ = mod_N(elem_ * other.elem_);
    return *this;
  }

  Field& operator/=(const Field& other) noexcept {
  static_assert(is_prime_v<N>, "N isn't prime");
  elem_ = mod_N(elem_ * other.get_inverse_mod_N());
    return *this;
  }

  Field& operator-() noexcept { 
    elem_ = N - elem_;
    return *this;
  }

  operator int() const noexcept {
    return elem_;
  }

  // auto operator<=>(const Field&) const noexcept = default;
  template <size_t M>
  friend bool operator<(const Field<M>&, const Field<M>&);
  template <size_t M>
  friend bool operator==(const Field<M>&, const Field<M>&);
};


//! class matrix
template <size_t M, size_t N, class Field>
class Matrix {
  std::vector<std::vector<Field>> matrix_ = 
      std::vector<std::vector<Field>>(M, std::vector<Field>(N, 0));

  void row_addition(size_t to, size_t from, const Field& multip_factor) {
    for (size_t i = 0; i < N; ++i)  {
      matrix_[i][from] += multip_factor * matrix_[i][to];
    }
  }

  void row_swap(size_t i, size_t j) {
    for (size_t k = 0; k < N; ++k)  {
      std::swap(matrix_[k][i], matrix_[k][j]);
    }
  }

  public:
  Matrix() = default;
  Matrix(const Matrix&) = default;
  Matrix& operator=(const Matrix&) = default;
  Matrix(Matrix&&) = default;
  Matrix& operator=(Matrix&&) = default;
  ~Matrix() = default;

  template <typename T> 
      requires std::is_convertible_v<T, int>
  Matrix(const std::vector<std::vector<T>>& vec) : matrix_(vec) {}
  template <typename T> 
      requires std::is_convertible_v<T, int>
  Matrix(std::vector<std::vector<T>>&& vec) : matrix_(std::move(vec)) {}

  std::vector<Field> getRow(size_t index) const noexcept { return matrix_[index]; }
  std::vector<Field> getColumn(size_t index) const noexcept {
    std::vector<Field> ans(M);
    for (size_t i = 0; i < M; ++i)  {
      ans[i] = matrix_[i][index];
    }
    return ans;
  }

  std::vector<Field>& operator[](size_t index) noexcept { return matrix_[index]; }
  Field& operator()(size_t i, size_t j) noexcept { return matrix_[i][j]; }

  const std::vector<Field>& operator[](size_t index) 
      const noexcept { return matrix_[index]; }
  const Field& operator()(size_t i, size_t j) 
      const noexcept { return matrix_[i][j]; }

  Matrix& operator+=(const Matrix& other) noexcept {
    for (size_t i = 0; i < M; ++i)  {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] += other[i][j];
      }
    }
    return *this;
  }
  Matrix& operator-=(const Matrix& other) noexcept {
    for (size_t i = 0; i < M; ++i)  {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] -= other[i][j];
      }

    }
    return *this;
  }
  Matrix& operator*=(const Field& num) noexcept {
    for (size_t i = 0; i < M; ++i)  {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] *= num;
      }
    }
    return *this;
  }

  Matrix<N, M, Field> transposed() const noexcept {
    Matrix<N, M, Field> ans;
    for (size_t i = 0; i < N; ++i)  {
      for (size_t j = 0; j < M; ++j) {
        ans[i][j] = matrix_[j][i];
      }
    }
    return ans;
  }

  size_t rank() const noexcept {
    Matrix matrix(matrix_);
    size_t rank = 0;
    std::pair<size_t, bool> first_not_zero = 
        first_not_zero_elements(0, matrix.getColumn(0));
    auto tmp = first_not_zero;
    for (size_t j = 1; j < N; ++j)  {
      first_not_zero = first_not_zero_elements(first_not_zero.second 
          ? first_not_zero.first : 0, matrix.getColumn(j));
      if (first_not_zero.second) {
        tmp = first_not_zero;
        ++rank;
        for (size_t i = first_not_zero.first + 1; i < M; ++i) {
          matrix.row_addition(i, j,
              -(matrix.matrix_[i][j] / matrix.matrix_[first_not_zero.first][j]));
        }
      }
    }
    return rank;
  }
   
};

//! class square matrix
template <size_t N, class Field>
class Matrix<N, N, Field> {
  std::vector<std::vector<Field>> matrix_ = 
      std::vector<std::vector<Field>>(N, std::vector<Field>(N, 0));

  void row_addition(size_t to, size_t from, const Field& multip_factor) {
    for (size_t i = 0; i < N; ++i)  {
      matrix_[i][from] += multip_factor * matrix_[i][to];
    }
  }

  void row_swap(size_t i, size_t j) {
    for (size_t k = 0; k < N; ++k)  {
      std::swap(matrix_[k][i], matrix_[k][j]);
    }
  }

  public:
  Matrix() {
    for (size_t i = 0; i < N; ++i)  {
      matrix_[i][i] = 1;
    }
  }
  Matrix(const Matrix&) = default;
  Matrix& operator=(const Matrix&) = default;
  Matrix(Matrix&&) = default;
  Matrix& operator=(Matrix&&) = default;
  ~Matrix() = default;

  template <typename T>
  Matrix(const std::vector<std::vector<T>>& vec) : matrix_(vec) {}
  template <typename T>
  Matrix(std::vector<std::vector<T>>&& vec) : matrix_(std::move(vec)) {}

  std::vector<Field> getRow(size_t index) const noexcept { return matrix_[index]; }
  std::vector<Field> getColumn(size_t index) const noexcept {
    std::vector<Field> ans(N);
    for (size_t i = 0; i < N; ++i)  {
      ans[i] = matrix_[i][index];
    }
    return ans; //!!! copy-elison
  }

  std::vector<Field>& operator[](size_t index) { return matrix_[index]; }
  Field& operator()(size_t i, size_t j) { return matrix_[i][j]; }

  const std::vector<Field>& operator[](size_t index) 
      const { return matrix_[index]; }
  const Field& operator()(size_t i, size_t j) 
      const { return matrix_[i][j]; }

  Matrix& operator+=(const Matrix& other) noexcept {
    for (size_t i = 0; i < N; ++i)  {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] += other[i][j];
      }
    }
    return *this;
  }
  Matrix& operator-=(const Matrix& other) noexcept {
    for (size_t i = 0; i < N; ++i)  {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] -= other[i][j];
      }
    }
    return *this;
  }
  Matrix& operator*=(const Field& num) noexcept {
    for (size_t i = 0; i < N; ++i)  {
      for (size_t j = 0; j < N; ++j) {
        matrix_[i][j] *= num;
      }
    }
    return *this;
  }

  Matrix<N, N, Field> transposed() const noexcept {
    Matrix<N, N, Field> ans;
    for (size_t i = 0; i < N; ++i)  {
      for (size_t j = 0; j < N; ++j) {
        ans[i][j] = matrix_[j][i];
      }
    }
    return ans; //!!! copy-elison
  }

  Field trace() const noexcept {
    Field ans = static_cast<Field>(0);
    for (size_t i = 0; i < N; ++i)  {
      ans += matrix_[i][i];
    }
    return ans; //!!! copy-elison
  }

  Field det() const noexcept {
    
  }
};

template <size_t N, class Field = Rational>
using SquareMatrix = Matrix<N, N, Field>;

//! operators on matrix
template <size_t M, size_t N, class Field>
bool operator==(const Matrix<M, N, Field>& obj1, 
    const Matrix<M, N, Field>& obj2) {
  for (size_t i = 0; i < M; ++i)  {
    for (size_t j = 0; j < N; ++j) {
      if (obj1[i][j] != obj2[i][j]) return false;
    }
  }
  return true;
}

template <size_t M, size_t N, class Field>
bool operator!=(const Matrix<M, N, Field>& obj1, 
    const Matrix<M, N, Field>& obj2) {
  return !(obj1 == obj2);
}

template <size_t M, size_t N, class Field>
Matrix<M, N, Field> operator+(Matrix<M, N, Field> obj1, 
    const Matrix<M, N, Field>& obj2) { return (obj1 += obj2); }

template <size_t M, size_t N, class Field>
Matrix<M, N, Field> operator-(Matrix<M, N, Field> obj1, 
    const Matrix<M, N, Field>& obj2) { return (obj1 -= obj2); }

template <size_t M, size_t N, class Field>
Matrix<M, N, Field> operator*(Matrix<M, N, Field> matrix, 
    const Field& number) { return (matrix *= number); }
template <size_t M, size_t N, class Field>
Matrix<M, N, Field> operator*(const Field& number, 
    Matrix<M, N, Field> matrix) { return (matrix *= number); }

template <size_t M, size_t K, size_t N, class Field>
Matrix<M, N, Field> operator*(Matrix<M, K, Field> obj1, 
    const Matrix<K, N, Field>& obj2) {
  Matrix<M, N, Field> ans(std::vector<std::vector<Field>>(M,
      std::vector<Field>(N, 0)));
  for (size_t i = 0; i < M; ++i)  {
    for (size_t j = 0; j < N; ++j) {
      for (size_t k = 0; k < K; ++k) {
        ans[i][j] += obj1[i][k] * obj2[k][j];
      }
    }
  }
  return ans; //! copy-elision
}
