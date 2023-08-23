#include <iostream>
#include <cmath>
#include <iterator>

size_t DivCeil(size_t num1, size_t num2) {
  return static_cast<size_t>(ceil((double)num1 / (double)num2));
}

template <typename T, typename Allocator = std::allocator<T>>
class Deque {
  private:
  using alloc_traits = std::allocator_traits<Allocator>;
  using alloc_external = typename std::allocator_traits<Allocator>::template rebind_alloc<T*>;
  using alloc_external_traits = std::allocator_traits<alloc_external>;


  //size of internal array
  static const size_t kSzInternalArr = 8;
  //size of external array
  size_t size_ = 0;
  //real size of external array
  size_t real_sz_ = 0;
  //container size for users
  size_t size_deque_ = 0;
  //beginning of the deque
  size_t begin_deque_ = 0;
  //end of the deque
  size_t end_deque_ = 0;
  //pointer to the beginning of the external array
  //external array != deque
  T** external_arr_ = nullptr;
  //allocator for T
  Allocator alloc_;
  //allocator for T*
  alloc_external alloc_external_;

  void swap(Deque& object);
  void copy(T**& new_external_array);
  template <typename... Args>
  void helper(Args&&... args);

  public:
  
  template <bool IsConst>
  class CommonIterator {
    private:
    //pointer to an external array
    T** ptr_ = nullptr;
    //Position in the internal array
    std::ptrdiff_t pos_  = 0;
    public:

    using iterator_category = std::random_access_iterator_tag;
    using value_type = std::conditional_t<IsConst, const T, T>;
    using difference_type = std::ptrdiff_t;
    using pointer = value_type*;
    using reference	= value_type&;

    CommonIterator() noexcept = default;
    CommonIterator(T** ptr, size_t pos) noexcept : ptr_(ptr), pos_(pos) {}
    CommonIterator(const CommonIterator& obj) noexcept = default;
    ~CommonIterator() noexcept = default;
    CommonIterator& operator=(const CommonIterator& obj) noexcept = default;

    CommonIterator& operator++() noexcept {
      if (pos_ == kSzInternalArr - 1) {
        ++ptr_;
        pos_ = 0;
        return *this;
      }
      ++pos_;
      return *this;
    }
    CommonIterator& operator--() noexcept {
      if (pos_ == 0) {
        --ptr_;
        pos_ = kSzInternalArr - 1;
        return *this;
      }
      --pos_;
      return *this;
    }
    CommonIterator operator++(int) noexcept {
      CommonIterator tmp = *this;
      
      if (pos_ == kSzInternalArr - 1) {
        ++ptr_;
        pos_ = 0;
        return tmp;
      }
      ++pos_;
      return tmp;
    }

    CommonIterator operator--(int) noexcept {
      CommonIterator tmp = *this;
      if (pos_ == 0) {
        --ptr_; 
        pos_ = kSzInternalArr - 1;
        return tmp;
      }
      --pos_;
      return tmp;
    }

    CommonIterator<IsConst>& operator+=(difference_type number) noexcept {
      ptr_ += (number + pos_) / kSzInternalArr;
      pos_ = (number + pos_) % kSzInternalArr;
      return *this;
    }
    CommonIterator<IsConst>& operator-=(difference_type number) noexcept {
      if (pos_ <= number) {
        ptr_ -= DivCeil((number - pos_), kSzInternalArr);
      }
      pos_ = (pos_ - number) % kSzInternalArr;
      return *this;
    }

    reference operator*() const noexcept {
      return (*ptr_)[pos_];
    }

    pointer operator->() const noexcept {
      return (*ptr_ + pos_);
    }

    template <bool IsBool>
    difference_type operator-(//
        const CommonIterator<IsBool>& iter) const noexcept {
      return std::abs((int)((ptr_ - iter.ptr_) * kSzInternalArr + (pos_ - iter.pos_)));
    }

    template <bool IsBool>
    bool operator<(const CommonIterator<IsBool>& iter) const noexcept {
      if (ptr_ < iter.ptr_) {
        return true;
      }
      if (ptr_ > iter.ptr_) {
        return false;
      }
      return (pos_ < iter.pos_);
    }

    template <bool IsBool>
    bool operator>(const CommonIterator<IsBool>& iter) const noexcept {
      return iter < *this;
    }

    template <bool IsBool>
    bool operator==(const CommonIterator<IsBool>& iter) const noexcept {
      return !(*this < iter) && !(iter < *this);
    }

    template <bool IsBool>
    bool operator!=(const CommonIterator<IsBool>& iter) const noexcept {
       return !(*this == iter);
    }

    template <bool IsBool>
    bool operator>=(const CommonIterator<IsBool>& iter) const noexcept {
      return !(*this < iter);
    }

    template <bool IsBool>
    bool operator<=(const CommonIterator<IsBool>& iter) const noexcept {
      return !(*this > iter);
    }

    CommonIterator operator+(size_t num) const noexcept {
      CommonIterator iter = *this;
      iter += num;
      return iter;
    }

    CommonIterator operator-(size_t num) const noexcept {
      CommonIterator iter = *this;
      iter -= num;
      return iter;
    }

    operator CommonIterator<true>() {
      return CommonIterator<true>(ptr_, pos_);
    }
  };

  using reverse_iterator = std::reverse_iterator<CommonIterator<false>>;
  using const_reverse_iterator = std::reverse_iterator<CommonIterator<true>>;
  using const_iterator = CommonIterator<true>;
  using iterator = CommonIterator<false>;

  Deque() noexcept;
  Deque(const Allocator& alloc);
  Deque(size_t count, const Allocator& alloc = Allocator());
  Deque(size_t count, const T& value, const Allocator& alloc = Allocator());
  Deque(std::initializer_list<T> init, const Allocator& alloc = Allocator());
  Deque(const Deque& object);
  Deque(Deque&& object);
  ~Deque() noexcept;
  Deque& operator=(const Deque& object);
  Deque& operator=(Deque&& object);


  bool empty() const noexcept;
  size_t size() const noexcept;

  T& operator[](size_t index);
  const T& operator[](size_t index) const;
  T& at(size_t index);
  const T& at(size_t index) const;

  void push_back(const T& value);
  void push_back(T&& value);
  void pop_back();
  void push_front(const T& value);
  void push_front(T&& value);
  void pop_front();

  template <typename... Args>
  void emplace_back(Args&&... args);
  template <typename... Args>
  void emplace_front(Args&&... args);

  iterator begin() noexcept;
  const_iterator begin() const noexcept;

  iterator end() noexcept;
  const_iterator end() const noexcept;

  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;


  reverse_iterator rbegin() noexcept;
  const_reverse_iterator rbegin() const noexcept;

  reverse_iterator rend() noexcept;
  const_reverse_iterator rend() const noexcept;

  const_reverse_iterator rcbegin() const noexcept;
  const_reverse_iterator rcend() const noexcept;

  void insert(iterator iter, const T& value);
  void insert(iterator iter, T&& value);
  void erase(iterator iter);

  template <typename... Args>
  void emplace(iterator iter, Args&&... args);

  Allocator get_allocator() const noexcept;

};

//------------definitions_Deque--------------//

template <typename T, typename Allocator>
Allocator Deque<T, Allocator>::get_allocator() const noexcept {
  return alloc_;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque() noexcept = default;

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Allocator& alloc) : Deque() {
  alloc_ = alloc;
  alloc_external_ = alloc;
};

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const Allocator& alloc) : size_(DivCeil(count, kSzInternalArr)), real_sz_(size_), size_deque_(count), begin_deque_(0), end_deque_(size_deque_), alloc_(alloc), alloc_external_(alloc) {
  external_arr_ =  alloc_external_traits::allocate(alloc_external_, real_sz_);
  for (size_t i = 0; i < size_; ++i) {
    external_arr_[i] = alloc_traits::allocate(alloc_, kSzInternalArr);
  }
  auto iter = begin();
  while (iter != end()) {
    try {
      alloc_traits::construct(alloc_, &(*iter));
    } catch(...) {
      auto iter_destroy = begin();
      while (iter_destroy != iter) {
        alloc_traits::destroy(alloc_, &(*iter_destroy));
        ++iter_destroy;
      }
      for (size_t i = 0; i < size_; ++i) {
        alloc_traits::deallocate(alloc_, external_arr_[i], kSzInternalArr);
      }
      alloc_external_traits::deallocate(alloc_external_, external_arr_, real_sz_);
      throw;
    }
    ++iter;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(size_t count, const T& value, const Allocator& alloc) : size_(DivCeil(count, kSzInternalArr)), real_sz_(size_), size_deque_(count), begin_deque_(0), end_deque_(size_deque_), alloc_(alloc), alloc_external_(alloc) {
  external_arr_ =  alloc_external_traits::allocate(alloc_external_, real_sz_);
  for (size_t i = 0; i < size_; ++i) {
    external_arr_[i] = alloc_traits::allocate(alloc_, kSzInternalArr);
  }
  auto iter = begin();
  while (iter != end()) {
    try {
      alloc_traits::construct(alloc_, &(*iter), value);
    } catch(...) {
      auto iter_destroy = begin();
      while (iter_destroy != iter) {
        alloc_traits::destroy(alloc_, &(*iter_destroy));
        ++iter_destroy;
      }
      for (size_t i = 0; i < size_; ++i) {
        alloc_traits::deallocate(alloc_, external_arr_[i], kSzInternalArr);
      }
      alloc_external_traits::deallocate(alloc_external_, external_arr_, real_sz_);
      throw;
    }
    ++iter;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(std::initializer_list<T> init, const Allocator& alloc) : size_(DivCeil(init.size() , kSzInternalArr)), real_sz_(size_), size_deque_(init.size()), begin_deque_(0), end_deque_(size_deque_), alloc_(alloc) {
  external_arr_ =  alloc_external_traits::allocate(alloc_external_, real_sz_);
  for (size_t i = 0; i < size_; ++i) {
    external_arr_[i] = alloc_traits::allocate(alloc_, kSzInternalArr);
  }
  auto iter = begin();
  auto iter_init = init.begin();
  while (iter != end()) {
    try {
      alloc_traits::construct(alloc_, &(*iter), *iter_init);
    } catch(...) {
      auto iter_destroy = begin();
      while (iter_destroy != iter) {
        alloc_traits::destroy(alloc_, &(*iter_destroy));
        ++iter_destroy;
      }
      for (size_t i = 0; i < size_; ++i) {
        alloc_traits::deallocate(alloc_, external_arr_[i], kSzInternalArr);
      }
      alloc_external_traits::deallocate(alloc_external_, external_arr_, real_sz_);
      throw;
    }
    ++iter;
    ++iter_init;
  }
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(Deque<T, Allocator>&& object) : size_(object.size_), real_sz_(object.real_sz_), size_deque_(object.size_deque_), begin_deque_(object.begin_deque_), end_deque_(object.end_deque_), external_arr_(object.external_arr_), alloc_(std::move(object.alloc_)), alloc_external_(std::move(object.alloc_external_)) {
  object.size_ = 0;
  object.real_sz_ = 0;
  object.size_deque_ = 0;
  object.begin_deque_ = 0;
  object.end_deque_ = 0;
  object.external_arr_ = nullptr;
  object.alloc_ = Allocator();
  object.alloc_external_ = alloc_external();
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(Deque<T, Allocator>&& object) {
  this -> ~Deque();
  size_ = object.size_;
  real_sz_ = object.real_sz_;
  size_deque_ = object.size_deque_;
  begin_deque_ = object.begin_deque_;
  end_deque_ = object.end_deque_;
  external_arr_ = object.external_arr_;
  alloc_ = std::move(object.alloc_);
  alloc_external_ = std::move(object.alloc_external_);

  object.size_ = 0;
  object.real_sz_ = 0;
  object.size_deque_ = 0;
  object.begin_deque_ = 0;
  object.end_deque_ = 0;
  object.external_arr_ = nullptr;
  object.alloc_ = Allocator();
  object.alloc_external_ = alloc_external();
  return *this;
}

template <typename T, typename Allocator>
Deque<T, Allocator>::~Deque() noexcept {
  auto iter = begin();
  while (iter != end()) {
    alloc_traits::destroy(alloc_, &(*iter));
    ++iter;
  }
  for (size_t i = begin_deque_ / kSzInternalArr; i < DivCeil(end_deque_, kSzInternalArr); ++i)  {
    alloc_traits::deallocate(alloc_, external_arr_[i], kSzInternalArr);
  }
  alloc_external_traits::deallocate(alloc_external_, external_arr_, real_sz_);
}

template <typename T, typename Allocator>
Deque<T, Allocator>::Deque(const Deque<T, Allocator>& object) : size_(object.size_), real_sz_(object.real_sz_), size_deque_(object.size_deque_), begin_deque_(object.begin_deque_), end_deque_(object.end_deque_), alloc_(alloc_traits::select_on_container_copy_construction(object.get_allocator())), alloc_external_(alloc_) {
  external_arr_ = alloc_external_traits::allocate(alloc_external_, real_sz_);
  for (size_t i = begin_deque_ / kSzInternalArr; i < DivCeil(end_deque_, kSzInternalArr); ++i)  {
    external_arr_[i] = alloc_traits::allocate(alloc_, kSzInternalArr);
  }
  auto iter = begin();
  auto iter_other = object.begin();
  while (iter_other != object.end()) {
    try {
      alloc_traits::construct(alloc_, &(*iter), *iter_other);
    } catch (...) {
      auto iter_destroy = begin();
      while (iter_destroy != iter) {
        alloc_traits::destroy(alloc_, &(*iter_destroy));
        ++iter_destroy;
      }
      for (size_t i = begin_deque_ / kSzInternalArr; i < DivCeil(end_deque_, kSzInternalArr); ++i)  {
        alloc_traits::deallocate(alloc_, external_arr_[i], kSzInternalArr);
      }
      alloc_external_traits::deallocate(alloc_external_, external_arr_, real_sz_);
      throw;
    }
    ++iter;
    ++iter_other;
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::swap(Deque<T, Allocator>& object) {
  std::swap(size_, object.size_);
  std::swap(real_sz_, object.real_sz_);
  std::swap(size_deque_, object.size_deque_);
  std::swap(begin_deque_, object.begin_deque_);
  std::swap(end_deque_, object.end_deque_);
  std::swap(external_arr_, object.external_arr_);
}

template <typename T, typename Allocator>
Deque<T, Allocator>& Deque<T, Allocator>::operator=(const Deque<T, Allocator>& object) {
  Deque tmp = object;
  tmp.alloc_ = alloc_;
  if constexpr (typename alloc_traits::propagate_on_container_copy_assignment::value) {
    alloc_ = object.get_allocator();
  }
  alloc_external_ = alloc_;
  swap(tmp);
  return *this;
}

template <typename T, typename Allocator>
bool Deque<T, Allocator>::empty() const noexcept {
  return size_deque_ == 0;
}
template <typename T, typename Allocator>
size_t Deque<T, Allocator>::size() const noexcept {
  return size_deque_;
}
template <typename T, typename Allocator>
T& Deque<T, Allocator>::operator[](size_t index) {
  return external_arr_[(begin_deque_ + index) / kSzInternalArr][(begin_deque_ + index) % kSzInternalArr];
}
template <typename T, typename Allocator>
const T& Deque<T, Allocator>::operator[](size_t index) const {
  return external_arr_[(begin_deque_ + index) / kSzInternalArr][(begin_deque_ + index) % kSzInternalArr];
}
template <typename T, typename Allocator>
T& Deque<T, Allocator>::at(size_t index) {
  if (begin_deque_ + index < end_deque_) {
    return external_arr_[(begin_deque_ + index)/ kSzInternalArr][(begin_deque_ + index) % kSzInternalArr];
  }
  throw std::out_of_range("oops");
}
template <typename T, typename Allocator>
const T& Deque<T, Allocator>::at(size_t index) const {
  if (begin_deque_ + index < end_deque_) {
  return external_arr_[(begin_deque_ + index)/ kSzInternalArr][(begin_deque_ + index) % kSzInternalArr];
  }
  throw std::out_of_range("oops");
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::copy(T**& new_external_array) {
  size_t begin = 3 * DivCeil(real_sz_, 2);
  size_t end =  3 * DivCeil(real_sz_, 2) + size_;
  for (size_t i = begin; i < end; ++i)  {
    new_external_array[i] = external_arr_[i - begin + begin_deque_ / kSzInternalArr];
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::helper(Args&&... args) {
  size_ = 1;
  real_sz_ = 1;
  begin_deque_ = 0;
  end_deque_ = 1;
  size_deque_ = 1;
  external_arr_ = alloc_external_traits::allocate(alloc_external_, 1);
  external_arr_[0] = alloc_traits::allocate(alloc_, kSzInternalArr);
  try {
    alloc_traits::construct(alloc_, external_arr_[0], std::forward<Args>(args)...);
  } catch (...) {
    alloc_traits::deallocate(alloc_, external_arr_[0], kSzInternalArr);
    alloc_external_traits::deallocate(alloc_external_, external_arr_, 1);
    throw;
  }
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_front(Args&&... args) {
  if (real_sz_ == 0) {
    helper(std::forward<Args>(args)...);
    return;
  }
  if (begin_deque_ == 0) {
    const size_t kSix = 6;
    T** new_exter_arr = alloc_external_traits::allocate(alloc_external_, size_ + kSix * DivCeil(real_sz_,2));
    copy(new_exter_arr);
    size_t begin = 3 * DivCeil(real_sz_, 2) - 1;
    new_exter_arr[begin] = alloc_traits::allocate(alloc_, kSzInternalArr);
    try {
      alloc_traits::construct(alloc_, new_exter_arr[begin] + kSzInternalArr - 1, std::forward<Args>(args)...);
    } catch(...) {
      alloc_traits::deallocate(alloc_, new_exter_arr[begin], kSzInternalArr);
      alloc_external_traits::deallocate(alloc_external_, new_exter_arr, size_ + kSix * DivCeil(real_sz_,2));
      throw;
    }
    alloc_external_traits::deallocate(alloc_external_, external_arr_, real_sz_);
    begin_deque_ = (3 * DivCeil(real_sz_, 2) - 1) * kSzInternalArr + kSzInternalArr - 1;
    end_deque_ = (3 * DivCeil(real_sz_, 2) + size_ - 1) * kSzInternalArr + (end_deque_ % kSzInternalArr) + kSzInternalArr * (int)(end_deque_ % kSzInternalArr == 0);
    real_sz_ =  kSix * DivCeil(real_sz_, 2) + size_;
    ++size_;
    external_arr_ = new_exter_arr;
  } else if (begin_deque_ % kSzInternalArr == 0) {
    external_arr_[begin_deque_ / kSzInternalArr - 1] = alloc_traits::allocate(alloc_, kSzInternalArr);
    try {
      alloc_traits::construct(alloc_, external_arr_[begin_deque_ / kSzInternalArr - 1] + kSzInternalArr - 1, std::forward<Args>(args)...);
    } catch(...) {
      alloc_traits::deallocate(alloc_, external_arr_[begin_deque_ / kSzInternalArr - 1], kSzInternalArr);
      throw;
    }
    --begin_deque_;
    ++size_;
  } else {
    try {
      alloc_traits::construct(alloc_, external_arr_[begin_deque_ / kSzInternalArr] + begin_deque_ % kSzInternalArr - 1, std::forward<Args>(args)...);
    } catch(...) {
      throw;
    }
    --begin_deque_;
  }
  ++size_deque_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(const T& value) { emplace_front(value); }

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_front(T&& value) { emplace_front(std::move(value)); }


template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_front() {
  if (empty()) {
    return;
  }
  alloc_traits::destroy(alloc_, external_arr_[begin_deque_ / kSzInternalArr] + begin_deque_ % kSzInternalArr);
  if (begin_deque_ % kSzInternalArr == kSzInternalArr - 1) {
    alloc_traits::deallocate(alloc_, external_arr_[begin_deque_ / kSzInternalArr], kSzInternalArr);
    --size_;
  }
  ++begin_deque_;
  --size_deque_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::pop_back() {
  if (empty()) {
    return;
  }
  alloc_traits::destroy(alloc_, external_arr_[(end_deque_ - 1) / kSzInternalArr] + (end_deque_ - 1) % kSzInternalArr);
  if ((end_deque_ - 1) % kSzInternalArr == 0) {
    alloc_traits::deallocate(alloc_, external_arr_[(end_deque_ - 1) / kSzInternalArr], kSzInternalArr);
    --size_;
  }
  --end_deque_;
  --size_deque_;
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace_back(Args&&... args) {
  if (real_sz_ == 0) {
    helper(std::forward<Args>(args)...);
    return;
  }
  if (end_deque_ / kSzInternalArr == real_sz_) {
    const size_t kSix = 6;
    T** new_external_array = alloc_external_traits::allocate(alloc_external_, size_ + kSix * DivCeil(real_sz_, 2));
    copy(new_external_array);
    size_t end =  3 * DivCeil(real_sz_, 2) + size_ + 1;
    new_external_array[end - 1] = alloc_traits::allocate(alloc_, kSzInternalArr);
    try {
      alloc_traits::construct(alloc_, new_external_array[end - 1], std::forward<Args...>(args...));
    } catch(...) {
      alloc_traits::deallocate(alloc_, new_external_array[end - 1], kSzInternalArr);
      alloc_external_traits::deallocate(alloc_external_, new_external_array, size_ + kSix * DivCeil(real_sz_, 2));
      throw;
    }
    alloc_external_traits::deallocate(alloc_external_, external_arr_, real_sz_);
    begin_deque_ = (3 * DivCeil(real_sz_, 2)) * kSzInternalArr + begin_deque_ % kSzInternalArr;
    end_deque_ = (3 * DivCeil(real_sz_, 2) + size_) * kSzInternalArr + 1;
    real_sz_ =  kSix * DivCeil(real_sz_, 2) + size_;
    ++size_;
    external_arr_ = new_external_array;
  } else if (end_deque_ % kSzInternalArr == 0) {
    external_arr_[end_deque_ / kSzInternalArr] = alloc_traits::allocate(alloc_, kSzInternalArr);
    try {
      alloc_traits::construct(alloc_, external_arr_[end_deque_ / kSzInternalArr], std::forward<Args...>(args...));
    } catch(...) {
      alloc_traits::deallocate(alloc_, external_arr_[end_deque_ / kSzInternalArr], kSzInternalArr);
      throw;
    }
    ++end_deque_;
    ++size_;
  } else {
    try {
      alloc_traits::construct(alloc_, external_arr_[end_deque_ / kSzInternalArr] + end_deque_ % kSzInternalArr, std::forward<Args...>(args...));
    } catch(...) {
      throw;
    }
    ++end_deque_;
  }
  ++size_deque_;
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(const T& value) { emplace_back(value); }

template <typename T, typename Allocator>
void Deque<T, Allocator>::push_back(T&& value) { emplace_back(std::move(value)); }


template <typename T, typename Allocator>
typename Deque<T, Allocator>::iterator Deque<T, Allocator>::begin() noexcept {
  return iterator(external_arr_ + begin_deque_ / kSzInternalArr, begin_deque_ % kSzInternalArr);
}
template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::begin()//
    const noexcept {
  return const_iterator(external_arr_ + begin_deque_ / kSzInternalArr, begin_deque_ % kSzInternalArr);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::end()//
    const noexcept {
  return const_iterator(external_arr_ + end_deque_ / kSzInternalArr, end_deque_ % kSzInternalArr);
}
template <typename T, typename Allocator>
typename Deque<T, Allocator>::iterator Deque<T, Allocator>::end() noexcept {
  return iterator(external_arr_ + end_deque_ / kSzInternalArr, end_deque_ % kSzInternalArr);
}
template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::cbegin()//
    const noexcept {
  return const_iterator(external_arr_ + begin_deque_ / kSzInternalArr, begin_deque_ % kSzInternalArr);
}
template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_iterator Deque<T, Allocator>::cend()//
    const noexcept {
  return const_iterator(external_arr_ + end_deque_ / kSzInternalArr, end_deque_ % kSzInternalArr);
}


template <typename T, typename Allocator>
typename Deque<T, Allocator>::reverse_iterator Deque<T, Allocator>::rbegin() noexcept {
  iterator iter = end();
  reverse_iterator riter = reverse_iterator(iter);
  return riter;
}
template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator Deque<T, Allocator>::rbegin() const noexcept {
  const_iterator iter = end();
  return const_reverse_iterator(iter);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator Deque<T, Allocator>::rend() const noexcept {
  const_iterator iter = begin();
  return const_reverse_iterator(iter);
}
template <typename T, typename Allocator>
typename Deque<T, Allocator>::reverse_iterator Deque<T, Allocator>::rend() noexcept {
  iterator iter = begin();
  return reverse_iterator(iter);
}

template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator Deque<T, Allocator>::rcbegin() const noexcept {
  const_iterator iter = end();
  return const_reverse_iterator(iter);
}
template <typename T, typename Allocator>
typename Deque<T, Allocator>::const_reverse_iterator Deque<T, Allocator>::rcend() const noexcept {
  const_iterator iter = begin();
  return const_reverse_iterator(iter);
}

template <typename T, typename Allocator>
template <typename... Args>
void Deque<T, Allocator>::emplace(iterator iter, Args&&... args) {
  Deque copy = *this;
  auto iter_copy = copy.begin();
  auto iter_insert = copy.begin();
  iter_insert += iter - begin();
  while (iter_copy != iter_insert) {
    pop_front();
    ++iter_copy;
  }
  try {
    emplace_front(args...);
  } catch (...) {
    --iter_copy;
    while (copy.begin() <= iter_copy) {
      push_front(*iter_copy);
      --iter_copy;
    }
  }
  while (copy.begin() != iter_copy) {
    --iter_copy;
    try {
      push_front(*iter_copy);
    } catch (...) {
      while (copy.begin() != iter_copy) {
        --iter_copy;
        pop_front();
      }
      pop_front();
      while (copy.begin() != iter_copy) {
        --iter_copy;
        push_front(*iter_copy);
      }
      throw;
    }
  }
}

template <typename T, typename Allocator>
void Deque<T, Allocator>::insert(iterator iter, const T& value) { emplace(iter, value); }

template <typename T, typename Allocator>
void Deque<T, Allocator>::insert(iterator iter, T&& value) { emplace(iter, std::move(value)); }

template <typename T, typename Allocator>
void Deque<T, Allocator>::erase(iterator iter) {
  Deque copy = *this;
  auto iter_copy = copy.begin();
  auto iter_insert = copy.begin();
  iter_insert += iter - begin();
  while (iter_copy <= iter_insert) {
    pop_front();
    ++iter_copy;
  }
  --iter_copy;
  while (copy.begin() != iter_copy) {
    --iter_copy;
    try {
      push_front(*iter_copy);
    } catch (...) {
      while (copy.begin() != iter_copy) {
        --iter_copy;
        pop_front();
      }
      pop_front();
      while (copy.begin() != iter_copy) {
        --iter_copy;
        push_front(*iter_copy);
      }
      throw;
    }
  }
}
