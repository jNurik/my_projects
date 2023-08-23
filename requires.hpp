//! is equal
template <size_t N, size_t M>
struct is_equal {
  static const bool value = false;
};

template <size_t N>
struct is_equal<N, N> {
  static const bool value = true;
};

template <size_t N, size_t M>
const bool is_equal_v = is_equal<N, M>::value;

//! helper for is_shares
template <size_t N, size_t M, size_t Iteration>
struct helper {
  static const bool value = is_equal_v<N * Iteration, M> || helper<N, M, Iteration - 1>::value;
};

template <size_t N, size_t M>
struct helper<N, M, 1> {
  static const bool value = is_equal_v<N, M>;
};

//! is shares n | M
template <size_t N, size_t M>
const bool is_shares_v = helper<N, M, M>::value;

//! is prime helper
template <size_t N, size_t Iteration>
struct helper_is_prime {
  static const bool value = !(is_shares_v<Iteration, N> || helper_is_prime<Iteration - 1, N>::value);
};

template <size_t N>
struct helper_is_prime<N, 2> {
  static const bool value = !is_shares_v<2, N>;
};

//! is prime
template <size_t N>
const bool is_prime_v = helper_is_prime<N, N - 1>::value;

// template <typename T, typename U>
// concept 
